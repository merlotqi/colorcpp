/**
 * @file adjust.hpp
 * @brief Color adjustment operations
 *
 * Provides compile-time color adjustment operations including lightening,
 * saturating, and hue shifting. All operations work in HSV color space for
 * natural color manipulation.
 *
 * @author Color Library Team
 * @date 2025
 * @version 1.0
 */

#pragma once

#include "../conversion/hsv_to_rgb.hpp"
#include "../conversion/rgb_to_hsv.hpp"
#include "../core/hsv.hpp"

namespace color::operations {

/**
 * @brief Lighten color operation
 *
 * Template struct for lightening a color by a specified percentage.
 * Works by adjusting the value (brightness) component in HSV color space.
 *
 * @tparam RGBType Source RGB color type
 * @tparam Percent Percentage to lighten (-100 to 100)
 */
template <typename RGBType, int Percent>
struct lighten {
  using rgb_type = RGBType;
  static_assert(Percent >= -100 && Percent <= 100, "Percent must be -100 to 100");

  /// @brief HSV representation of the input color
  using hsv_type = conversion::rgb_to_hsv_t<RGBType>;

  /// @brief New value (brightness) after adjustment
  static constexpr int new_v = hsv_type::v + (100 - hsv_type::v) * Percent / 100;
  /// @brief Clamped value to ensure it stays in valid range (0-100)
  static constexpr int clamped_v = (new_v < 0) ? 0 : ((new_v > 100) ? 100 : new_v);

  /// @brief Adjusted HSV type with new brightness
  using adjusted_hsv = core::hsv_int<hsv_type::h, hsv_type::s, clamped_v>;

  /// @brief Resulting RGB type after conversion back from HSV
  using type = conversion::hsv_to_rgb_t<adjusted_hsv>;
};

/**
 * @brief Saturate color operation
 *
 * Template struct for saturating a color by a specified percentage.
 * Works by adjusting the saturation component in HSV color space.
 *
 * @tparam RGBType Source RGB color type
 * @tparam Percent Percentage to saturate (-100 to 100)
 */
template <typename RGBType, int Percent>
struct saturate {
  using rgb_type = RGBType;
  static_assert(Percent >= -100 && Percent <= 100, "Percent must be -100 to 100");

  /// @brief HSV representation of the input color
  using hsv_type = conversion::rgb_to_hsv_t<RGBType>;

  /// @brief New saturation after adjustment
  static constexpr int new_s = hsv_type::s + (100 - hsv_type::s) * Percent / 100;
  /// @brief Clamped saturation to ensure it stays in valid range (0-100)
  static constexpr int clamped_s = (new_s < 0) ? 0 : ((new_s > 100) ? 100 : new_s);

  /// @brief Adjusted HSV type with new saturation
  using adjusted_hsv = core::hsv_int<hsv_type::h, clamped_s, hsv_type::v>;

  /// @brief Resulting RGB type after conversion back from HSV
  using type = conversion::hsv_to_rgb_t<adjusted_hsv>;
};

/**
 * @brief Hue shift color operation
 *
 * Template struct for shifting the hue of a color by a specified number of degrees.
 * Works by adjusting the hue component in HSV color space with proper wrapping.
 *
 * @tparam RGBType Source RGB color type
 * @tparam Degrees Number of degrees to shift the hue
 */
template <typename RGBType, int Degrees>
struct hue_shift {
  using rgb_type = RGBType;

  /// @brief HSV representation of the input color
  using hsv_type = conversion::rgb_to_hsv_t<RGBType>;

  /// @brief New hue after adjustment (before clamping)
  static constexpr int new_h = (hsv_type::h + Degrees) % 360;
  /// @brief Clamped hue to ensure it stays in valid range (0-360)
  static constexpr int clamped_h = (new_h < 0) ? (new_h + 360) : new_h;

  /// @brief Adjusted HSV type with new hue
  using adjusted_hsv = core::hsv_int<clamped_h, hsv_type::s, hsv_type::v>;

  /// @brief Resulting RGB type after conversion back from HSV
  using type = conversion::hsv_to_rgb_t<adjusted_hsv>;
};

/**
 * @brief Type alias for lighten operation result
 *
 * @tparam RGBType Source RGB color type
 * @tparam Percent Percentage to lighten (-100 to 100)
 * @return RGB color type with increased brightness
 */
template <typename RGBType, int Percent>
using lighten_t = typename lighten<RGBType, Percent>::type;

/**
 * @brief Type alias for saturate operation result
 *
 * @tparam RGBType Source RGB color type
 * @tparam Percent Percentage to saturate (-100 to 100)
 * @return RGB color type with increased saturation
 */
template <typename RGBType, int Percent>
using saturate_t = typename saturate<RGBType, Percent>::type;

/**
 * @brief Type alias for hue shift operation result
 *
 * @tparam RGBType Source RGB color type
 * @tparam Degrees Number of degrees to shift the hue
 * @return RGB color type with shifted hue
 */
template <typename RGBType, int Degrees>
using hue_shift_t = typename hue_shift<RGBType, Degrees>::type;

}  // namespace color::operations
