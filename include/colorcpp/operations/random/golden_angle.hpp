/**
 * @file golden_angle.hpp
 * @brief Golden angle-based sequential color generator.
 *
 * @par First sample
 * The first `next()` returns the initial random hue set at construction. Each following `next()` advances the hue by
 * the golden angle (137.50776405°) on `[0, 360)`.
 *
 * @par Thread safety
 * Not thread-safe: uses a `mutable` engine and hue state.
 */

#pragma once

#include <cmath>
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/random/basic.hpp>
#include <colorcpp/operations/random/hsl.hpp>
#include <vector>

namespace colorcpp::operations::random {

/**
 * @brief Sequential hues separated by the golden angle on the color wheel.
 *
 * Works with any color space that has a hue channel (HSL, HSV, HWB, OkLCH, CIELCH).
 * Internally converts to HSL for hue manipulation, then converts back.
 *
 * @tparam Color Color type (must have hue channel).
 * @tparam Engine Random engine type (default: std::mt19937).
 */
template <typename Color, typename Engine = std::mt19937>
class golden_angle_generator {
  static_assert(details::color_traits<Color>::has_hue,
                "golden_angle_generator requires a color type with hue channel (HSL, HSV, HWB, OkLCH, CIELCH)");

  using traits = details::color_traits<Color>;

  mutable Engine rng;
  mutable float current_hue;
  mutable bool first_next_{true};
  float saturation;
  float lightness;

  // Golden angle in degrees: 137.5077640500378°
  static constexpr float kGoldenAngle = 137.5077640500378f;

 public:
  /**
   * @brief Construct from engine and saturation/lightness values.
   * @param e Engine to copy.
   * @param sat Saturation value in [0, 1] (default: 1.0).
   * @param light Lightness/value value in [0, 1] (default: 0.5).
   */
  explicit golden_angle_generator(Engine& e, float sat = 1.0f, float light = 0.5f)
      : rng(e), saturation(sat), lightness(light) {
    current_hue = random_hue();
  }

  /**
   * @brief Construct from seed and saturation/lightness values.
   * @param seed Random seed.
   * @param sat Saturation value in [0, 1] (default: 1.0).
   * @param light Lightness/value value in [0, 1] (default: 0.5).
   */
  explicit golden_angle_generator(typename Engine::result_type seed, float sat = 1.0f, float light = 0.5f)
      : rng(seed), saturation(sat), lightness(light) {
    current_hue = random_hue();
  }

  /**
   * @brief Construct from seed and options (for backward compatibility).
   * @param seed Random seed.
   * @param opts Options with h_min, h_max, s_min, s_max, l_min, l_max.
   *
   * Uses s_max as saturation and l_max as lightness.
   */
  explicit golden_angle_generator(typename Engine::result_type seed,
                                  const typename basic_hsl_generator<core::hsl_float_t, Engine>::options& opts)
      : rng(seed), saturation(opts.s_max), lightness(opts.l_max) {
    current_hue = random_hue();
  }

  Color next() const {
    if (!first_next_) {
      current_hue = std::fmod(current_hue + kGoldenAngle, 360.0f);
    } else {
      first_next_ = false;
    }
    core::hsl_float_t hsl{current_hue, saturation, lightness};
    return conversion::color_cast<Color>(hsl);
  }

  std::vector<Color> generate_n(std::size_t count) const {
    std::vector<Color> out;
    out.reserve(count);
    for (std::size_t i = 0; i < count; ++i) out.push_back(next());
    return out;
  }

  float get_saturation() const { return saturation; }
  void set_saturation(float sat) { saturation = sat; }

  float get_lightness() const { return lightness; }
  void set_lightness(float light) { lightness = light; }

 private:
  float random_hue() const {
    std::uniform_real_distribution<float> dist(0.0f, 360.0f);
    return dist(rng);
  }
};

/** @brief Typedef for common HSL model. */
using golden_gen = golden_angle_generator<core::hsl_float_t>;

}  // namespace colorcpp::operations::random
