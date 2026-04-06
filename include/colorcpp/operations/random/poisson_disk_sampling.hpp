/**
 * @file poisson_disk_sampling.hpp
 * @brief Poisson Disk Sampling for color palette generation.
 *
 * Generates color palettes where all colors are guaranteed to have at least
 * a minimum perceptual distance from each other. Eliminates similar colors
 * and produces visually balanced and distinguishable sets.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/algorithms/delta_e.hpp>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/random/basic.hpp>
#include <random>
#include <vector>

namespace colorcpp::operations::random {

/**
 * @brief Poisson Disk Sampling color generator.
 *
 * Generates a palette of colors where each pair of colors has a minimum
 * perceptual distance ΔE between them. Uses OkLab color space and CIEDE2000
 * for accurate perceptual distance measurement.
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

  /**
   * @brief Construct generator with seed.
   * @param seed Random engine seed.
   * @param min_distance Minimum ΔE distance between colors (default: 10).
   *                     Typical values: 5 - subtle difference, 10 - clearly different, 20 - very distinct.
   */
  explicit poisson_disk_generator(typename Engine::result_type seed, float min_distance = 10.0f)
      : rng_(seed), min_distance_(min_distance) {}

  /**
   * @brief Construct generator with existing engine.
   * @param e Random engine instance.
   * @param min_distance Minimum ΔE distance between colors.
   */
  explicit poisson_disk_generator(Engine&& e, float min_distance = 10.0f)
      : rng_(std::move(e)), min_distance_(min_distance) {}

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

    basic_random_generator<intermediate_type, Engine> gen(rng_());

    for (std::size_t i = 0; i < count; ++i) {
      bool found = false;

      for (std::size_t attempt = 0; attempt < max_attempts; ++attempt) {
        auto candidate = gen.next();

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

    basic_random_generator<intermediate_type, Engine> gen(rng_());

    std::size_t consecutive_failures = 0;

    while (consecutive_failures < max_attempts) {
      auto candidate = gen.next();

      if (is_valid_candidate(candidate, samples)) {
        samples.push_back(candidate);
        consecutive_failures = 0;
      } else {
        consecutive_failures++;
      }
    }

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
   * @param distance New minimum ΔE distance.
   */
  void set_min_distance(float distance) noexcept { min_distance_ = distance; }

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
 * @param min_distance Minimum perceptual distance between colors.
 * @param seed Optional random seed.
 * @return Vector of generated colors.
 */
template <typename Color>
std::vector<Color> poisson_disk_palette(std::size_t count, float min_distance = 10.0f,
                                        typename std::mt19937::result_type seed = std::random_device{}()) {
  poisson_disk_generator<Color> gen(seed, min_distance);
  return gen.generate(count);
}

/**
 * @brief Generate maximum possible distinct colors within distance constraint.
 *
 * @tparam Color Output color type.
 * @param min_distance Minimum perceptual distance between colors.
 * @param seed Optional random seed.
 * @return Vector of all possible distinct colors.
 */
template <typename Color>
std::vector<Color> poisson_disk_max(float min_distance = 10.0f,
                                    typename std::mt19937::result_type seed = std::random_device{}()) {
  poisson_disk_generator<Color> gen(seed, min_distance);
  return gen.generate_max();
}

}  // namespace colorcpp::operations::random
