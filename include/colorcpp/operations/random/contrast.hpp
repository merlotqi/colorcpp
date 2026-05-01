/**
 * @file contrast.hpp
 * @brief WCAG contrast-constrained random color generator.
 *
 * Generates random colors that meet a minimum WCAG contrast ratio
 * against a reference (typically background) color. Uses rejection
 * sampling in OkLCH space with optional gamut mapping.
 *
 * @par Thread safety
 * Not thread-safe (inherited mutable engine). Same as @ref basic_random_generator.
 */

#pragma once

#include <cmath>
#include <colorcpp/algorithms/accessibility/wcag.hpp>
#include <colorcpp/algorithms/gamut/preserve.hpp>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/random/basic.hpp>
#include <stdexcept>
#include <vector>

namespace colorcpp::operations::random {

/**
 * @brief Random color generator constrained by WCAG contrast ratio.
 *
 * Generates candidates in OkLCH polar coordinates, optionally gamut-maps to sRGB,
 * then accepts only those meeting the minimum contrast ratio against a reference color.
 * Uses rejection sampling with a configurable attempt limit.
 *
 * @tparam Color Target output color type.
 * @tparam Engine Random engine type (default: std::mt19937).
 */
template <typename Color, typename Engine = std::mt19937>
class contrast_generator : public basic_random_generator<Color, Engine> {
  using base = basic_random_generator<Color, Engine>;
  using T = typename base::T;

 public:
  /** @brief Generation options. */
  struct options {
    float min_ratio = 4.5f;          ///< Minimum WCAG contrast ratio (AA normal text).
    float l_min = 0.0f;              ///< OkLab lightness lower bound [0, 1].
    float l_max = 1.0f;              ///< OkLab lightness upper bound [0, 1].
    float chroma_max = 0.4f;         ///< OkLCH chroma upper bound [0, 0.4].
    bool gamut_map = true;           ///< Clip to sRGB gamut via lightness-preserving mapper.
    std::size_t max_attempts = 200;  ///< Maximum rejection sampling attempts.
  };

  explicit contrast_generator(const Engine& e, const options& o = {}) : base(e), opts_(o) { validate(opts_); }

  explicit contrast_generator(typename Engine::result_type seed, const options& o = {}) : base(seed), opts_(o) {
    validate(opts_);
  }

  explicit contrast_generator(Engine&& e, const options& o = {}) : base(std::move(e)), opts_(o) { validate(opts_); }

  /**
   * @brief Generate a color meeting contrast constraint against white.
   * @return Random color, or last candidate if attempts exhausted.
   */
  Color next() const { return generate_constrained(core::rgbf_t{1.0f, 1.0f, 1.0f}); }

  /**
   * @brief Generate a color meeting contrast constraint against a reference.
   * @tparam RefColor Reference color type.
   * @param reference The reference (typically background) color.
   * @return Random color, or last candidate if attempts exhausted.
   */
  template <typename RefColor>
  Color next(const RefColor& reference) const {
    return generate_constrained(reference);
  }

  /**
   * @brief Generate multiple colors meeting contrast constraint against white.
   * @param count Number of colors to generate.
   * @return Vector of random colors.
   */
  std::vector<Color> generate_n(std::size_t count) const {
    const core::rgbf_t white{1.0f, 1.0f, 1.0f};
    std::vector<Color> out;
    out.reserve(count);
    for (std::size_t i = 0; i < count; ++i) out.push_back(generate_constrained(white));
    return out;
  }

  /**
   * @brief Generate multiple colors meeting contrast constraint against a reference.
   * @tparam RefColor Reference color type.
   * @param count Number of colors to generate.
   * @param reference The reference (typically background) color.
   * @return Vector of random colors.
   */
  template <typename RefColor>
  std::vector<Color> generate_n(std::size_t count, const RefColor& reference) const {
    std::vector<Color> out;
    out.reserve(count);
    for (std::size_t i = 0; i < count; ++i) out.push_back(generate_constrained(reference));
    return out;
  }

  const options& get_options() const { return opts_; }

  void set_options(const options& o) {
    validate(o);
    opts_ = o;
  }

 private:
  static constexpr float kPi = 3.14159265358979323846f;

  static void validate(const options& o) {
    if (o.l_min > o.l_max) throw std::invalid_argument("colorcpp: l_min must not exceed l_max");
    if (o.chroma_max < 0.0f) throw std::invalid_argument("colorcpp: chroma_max must be non-negative");
    if (o.min_ratio < 1.0f) throw std::invalid_argument("colorcpp: min_ratio must be >= 1.0");
  }

  template <typename RefColor>
  Color generate_constrained(const RefColor& reference) const {
    Color candidate = make_candidate();

    for (std::size_t i = 0; i < opts_.max_attempts; ++i) {
      if (algorithms::accessibility::contrast_ratio(candidate, reference) >= opts_.min_ratio) {
        return candidate;
      }
      candidate = make_candidate();
    }

    return candidate;
  }

  Color make_candidate() const {
    const float L = this->random_value(opts_.l_min, opts_.l_max);
    const float C = this->random_value(0.0f, opts_.chroma_max);
    const float H = this->random_value(0.0f, 360.0f);

    const float H_rad = H * kPi / 180.0f;
    const float a = C * std::cos(H_rad);
    const float b = C * std::sin(H_rad);

    core::oklab_t lab{L, a, b};
    Color result = conversion::color_cast<Color>(lab);

    if (opts_.gamut_map) {
      result = algorithms::gamut::gamut_clip_preserve_lightness(result);
    }
    return result;
  }

  options opts_;
};

}  // namespace colorcpp::operations::random
