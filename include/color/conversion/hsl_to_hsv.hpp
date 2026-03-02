/**
 * @file hsl_to_hsv.hpp
 * @brief HSL to HSV color space conversion
 *
 * Provides compile-time conversion from HSL color space to HSV color space.
 * Uses direct mathematical transformation between the two color models.
 *
 * @author Merlot.Qi
 * 
 */

#pragma once

#include <cmath>

#include "../core/hsv.hpp"

namespace color::conversion {

/**
 * @brief HSL to HSV color space converter
 *
 * Template struct for converting HSL colors to HSV colors at compile time.
 * Uses direct mathematical transformation between the HSL and HSV color models.
 *
 * @tparam HSLType Source HSL color type
 */
template <typename HSLType>
struct hsl_to_hsv {
  using hsl_type = HSLType;
  using value_type = typename HSLType::value_type;

  /// @brief Hue component from HSL input
  static constexpr value_type h = HSLType::h;
  /// @brief Saturation component from HSL input
  static constexpr value_type s = HSLType::s;
  /// @brief Lightness component from HSL input
  static constexpr value_type l = HSLType::l;

  /// @brief Normalized hue component (0.0-360.0)
  static constexpr value_type h_norm = h / (std::is_integral_v<value_type> ? 1.0 : 1.0);
  /// @brief Normalized saturation component (0.0-1.0)
  static constexpr value_type s_norm = s / (std::is_integral_v<value_type> ? 100.0 : 1.0);
  /// @brief Normalized lightness component (0.0-1.0)
  static constexpr value_type l_norm = l / (std::is_integral_v<value_type> ? 100.0 : 1.0);

  /// @brief Calculated value (brightness) component (0.0-1.0)
  static constexpr value_type v = l_norm + s_norm * std::min(l_norm, 1.0 - l_norm);
  /// @brief Calculated saturation component for HSV (0.0-1.0)
  static constexpr value_type s_hsv = (v == 0.0) ? 0.0 : (2.0 * (1.0 - l_norm / v));

  /**
   * @brief Resulting HSV type
   *
   * Converts the calculated HSV values to integer format and creates
   * an hsv_int type with the computed hue, saturation, and value components.
   */
  using type = core::hsv_int<static_cast<int>(std::round(h_norm)), static_cast<int>(std::round(s_hsv * 100.0)),
                             static_cast<int>(std::round(v * 100.0))>;
};

/**
 * @brief Type alias for HSL to HSV conversion result
 *
 * @tparam HSLType Source HSL color type
 * @return HSV color type resulting from the conversion
 */
template <typename HSLType>
using hsl_to_hsv_t = typename hsl_to_hsv<HSLType>::type;

}  // namespace color::conversion
