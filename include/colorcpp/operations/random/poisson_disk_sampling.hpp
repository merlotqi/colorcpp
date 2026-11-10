/**
 * @file poisson_disk_sampling.hpp
 * @brief Poisson Disk Sampling for color palette generation.
 *
 * Generates color palettes where all colors are guaranteed to have at least
 * a minimum perceptual distance from each other. Eliminates similar colors
 * and produces visually balanced and distinguishable sets.
 *
 * @note Uses OkLab Euclidean distance (not CIEDE2000) for efficiency.
 *       The guarantee holds for the colors the caller actually receives:
 *       candidates are converted to the output type (which may clamp to a
 *       gamut and quantize) *before* the distance check, and the received
 *       values are what gets stored and returned (issue #69).
 * @note Candidates are drawn from the OkLab working box but only accepted
 *       when sRGB-reachable (issue #94): most of that box lies outside sRGB,
 *       and clipping those at conversion time collapses distinct colors onto
 *       the same boundary value.
 */

#pragma once

#include <algorithm>
#include <colorcpp/algorithms/delta_e.hpp>
#include <colorcpp/algorithms/gamut/details.hpp>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/random/basic.hpp>
#include <random>
#include <vector>

namespace colorcpp::operations::random {

/**
 * @brief Poisson Disk Sampling color generator.
 *
 * Generates a palette of colors where each pair of *received* colors has a
 * minimum perceptual distance ΔE between them. Uses OkLab color space and
 * Euclidean distance in OkLab for fast perceptual distance measurement;
 * candidates are converted to Color (gamut clamp + quantization) before the
 * check, so the guarantee holds for the values the caller gets (issue #69).
 *
 * @tparam Color Target output color type.
 * @tparam Engine Random engine type.
 */
template <typename Color, typename Engine = std::mt19937>
class poisson_disk_generator {
 public:
  using color_type = Color;
  using engine_type = Engine;
  using intermediate_type = core::oklab_t;

  /// Default minimum OkLab ΔE distance (OkLab scale: JND ≈ 0.02, box diagonal = 3).
  static constexpr float kDefaultMinDistance = 0.1f;

  /**
   * @brief Construct generator with seed.
   * @param seed Random engine seed.
   * @param min_distance Minimum OkLab ΔE distance between colors (default: 0.1).
   *                     OkLab scale (not CIE ΔE): 0.05 - subtle difference,
   *                     0.1 - clearly different, 0.15 - very distinct.
   *                     Non-positive values fall back to the default: a
   *                     non-positive minimum accepts every candidate, which
   *                     would make @ref generate_max loop forever.
   */
  explicit poisson_disk_generator(typename Engine::result_type seed, float min_distance = kDefaultMinDistance)
      : rng_(seed), min_distance_(sanitize_min_distance(min_distance)) {}

  /**
   * @brief Construct generator with existing engine.
   * @param e Random engine instance.
   * @param min_distance Minimum OkLab ΔE distance between colors.
   *                     Non-positive values fall back to the default (see above).
   */
  explicit poisson_disk_generator(Engine&& e, float min_distance = kDefaultMinDistance)
      : rng_(std::move(e)), min_distance_(sanitize_min_distance(min_distance)) {}

  /**
   * @brief Generate a palette with specified number of colors.
   * @param count Number of colors to generate.
   * @param max_attempts Maximum number of attempts per color before giving up (default: 500).
   * @return Vector of generated colors.
   *
   * @note If the requested number of colors cannot be generated within the
   *       minimum distance constraint, the returned vector may be smaller than requested.
   */
  std::vector<Color> generate(std::size_t count, std::size_t max_attempts = 500) {
    std::vector<intermediate_type> samples;
    samples.reserve(count);

    // The sampler keeps its own engine copy; write the advanced state back so
    // consecutive generate() calls produce different palettes.
    basic_random_generator<intermediate_type, Engine> gen(rng_);

    for (std::size_t i = 0; i < count; ++i) {
      bool found = false;

      for (std::size_t attempt = 0; attempt < max_attempts; ++attempt) {
        intermediate_type raw;
        if (!draw_reachable(gen, raw)) continue;  // pathological: no reachable draw
        auto candidate = to_received(raw);

        if (is_valid_candidate(candidate, samples)) {
          samples.push_back(candidate);
          found = true;
          break;
        }
      }

      if (!found) {
        // Cannot generate more distinct colors within constraints
        break;
      }
    }

    rng_ = gen.engine();

    // Convert back to target color space
    std::vector<Color> result;
    result.reserve(samples.size());

    for (const auto& sample : samples) {
      result.push_back(conversion::color_cast<Color>(sample));
    }

    return result;
  }

  /**
   * @brief Generate colors until no more can be added within the distance constraint.
   * @param max_attempts Maximum attempts before stopping.
   * @return Vector of all possible distinct colors.
   */
  std::vector<Color> generate_max(std::size_t max_attempts = 1000) {
    std::vector<intermediate_type> samples;

    // The sampler keeps its own engine copy; write the advanced state back so
    // consecutive calls produce different palettes.
    basic_random_generator<intermediate_type, Engine> gen(rng_);

    std::size_t consecutive_failures = 0;

    while (consecutive_failures < max_attempts) {
      intermediate_type raw;
      if (!draw_reachable(gen, raw)) {
        consecutive_failures++;
        continue;
      }
      auto candidate = to_received(raw);

      if (is_valid_candidate(candidate, samples)) {
        samples.push_back(candidate);
        consecutive_failures = 0;
      } else {
        consecutive_failures++;
      }
    }

    rng_ = gen.engine();

    // Convert back to target color space
    std::vector<Color> result;
    result.reserve(samples.size());

    for (const auto& sample : samples) {
      result.push_back(conversion::color_cast<Color>(sample));
    }

    return result;
  }

  /**
   * @brief Get minimum distance setting.
   * @return Minimum ΔE distance.
   */
  float min_distance() const noexcept { return min_distance_; }

  /**
   * @brief Set new minimum distance.
   * @param distance New minimum ΔE distance. Non-positive values fall back to
   *                 the default (see the constructor).
   */
  void set_min_distance(float distance) noexcept { min_distance_ = sanitize_min_distance(distance); }

  /**
   * @brief Access internal random engine.
   * @return Reference to the engine.
   */
  Engine& engine() noexcept { return rng_; }

  /**
   * @brief Access internal random engine (const).
   * @return Const reference to the engine.
   */
  const Engine& engine() const noexcept { return rng_; }

 private:
  /**
   * @brief Non-positive minima are unusable (every candidate would pass and
   *        @ref generate_max would never terminate); fall back to the default.
   */
  static constexpr float sanitize_min_distance(float distance) noexcept {
    return distance > 0.0f ? distance : kDefaultMinDistance;
  }

  /// ~1/30 of uniform OkLab working-box draws land in sRGB (issue #94), so
  /// 256 redraws fail with probability ~(29/30)^256 ≈ 1e-4.
  static constexpr int kMaxGamutDraws = 256;

  /**
   * @brief True if the raw OkLab candidate lies inside the sRGB gamut (issue #94).
   *
   * Unclamped gamut test (oklab_to_linrgb + linrgb_in_gamut): a color_cast
   * through an RGB type would clamp the out-of-range channels and report
   * every candidate as in-gamut.
   */
  static bool is_srgb_reachable(const intermediate_type& c) noexcept {
    float r, g, b;
    algorithms::gamut::details::oklab_to_linrgb(c.template get_index<0>(), c.template get_index<1>(),
                                                c.template get_index<2>(), r, g, b);
    return algorithms::gamut::details::linrgb_in_gamut(r, g, b);
  }

  /**
   * @brief Draw a raw candidate that is sRGB-reachable (issue #94).
   *
   * Out-of-gamut draws are redrawn inside this helper instead of counting
   * against the caller's attempt budget - the budget governs the distance
   * constraint, and burning 29 of every 30 attempts on unreachable draws
   * would starve it.
   *
   * @return false if every draw fell outside sRGB (pathological; the caller
   *         treats it as a failed attempt).
   */
  bool draw_reachable(basic_random_generator<intermediate_type, Engine>& gen, intermediate_type& out) const {
    for (int draw = 0; draw < kMaxGamutDraws; ++draw) {
      out = gen.next();
      if (is_srgb_reachable(out)) return true;
    }
    return false;
  }

  /**
   * @brief Map a raw sampled candidate onto the color the caller would receive.
   *
   * Casting to Color clamps to the output gamut and may quantize (e.g. rgb8):
   * raw candidates that were far apart in OkLab can collapse onto the same
   * received value. The distance guarantee must hold for that received value,
   * so candidates are round-tripped through Color before the check and the
   * stored samples (issue #69).
   */
  intermediate_type to_received(const intermediate_type& candidate) const {
    return conversion::color_cast<intermediate_type>(conversion::color_cast<Color>(candidate));
  }

  /**
   * @brief Check if candidate is at least min_distance away from all existing samples.
   */
  bool is_valid_candidate(const intermediate_type& candidate, const std::vector<intermediate_type>& samples) const {
    for (const auto& existing : samples) {
      float distance = algorithms::delta_e::delta_e_ok(candidate, existing);
      if (distance < min_distance_) {
        return false;
      }
    }
    return true;
  }

  Engine rng_;
  float min_distance_;
};

/**
 * @brief Convenience function to generate Poisson disk sampled color palette.
 *
 * @tparam Color Output color type.
 * @param count Number of colors to generate.
 * @param min_distance Minimum OkLab ΔE distance between colors.
 * @param seed Optional random seed.
 * @return Vector of generated colors.
 */
template <typename Color>
std::vector<Color> poisson_disk_palette(std::size_t count,
                                        float min_distance = poisson_disk_generator<Color>::kDefaultMinDistance,
                                        typename std::mt19937::result_type seed = std::random_device{}()) {
  poisson_disk_generator<Color> gen(seed, min_distance);
  return gen.generate(count);
}

/**
 * @brief Generate maximum possible distinct colors within distance constraint.
 *
 * @tparam Color Output color type.
 * @param min_distance Minimum OkLab ΔE distance between colors.
 * @param seed Optional random seed.
 * @return Vector of all possible distinct colors.
 */
template <typename Color>
std::vector<Color> poisson_disk_max(float min_distance = poisson_disk_generator<Color>::kDefaultMinDistance,
                                    typename std::mt19937::result_type seed = std::random_device{}()) {
  poisson_disk_generator<Color> gen(seed, min_distance);
  return gen.generate_max();
}

}  // namespace colorcpp::operations::random
