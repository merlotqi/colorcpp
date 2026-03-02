/**
 * @file hsv_to_hsl.hpp
 * @brief HSV to HSL color space conversion
 *
 * Provides compile-time conversion from HSV color space to HSL color space.
 * Uses direct mathematical transformation between the two color models.
 *
 * @author Merlot.Qi
 * 
 */

#pragma once

#include <cmath>
#include <type_traits>

#include "../core/hsl.hpp"

namespace color::conversion {

/**
 * @brief HSV to HSL color space converter
 *
 * Template struct for converting HSV colors to HSL colors at compile time.
 * Uses direct mathematical transformation between the HSV and HSL color models.
 *
 * @tparam HSVType Source HSV color type
 */
template <typename HSVType>
struct hsv_to_hsl {
  using hsv_type = HSVType;
  using value_type = typename HSVType::value_type;

  /// @brief Hue component from HSV input
  static constexpr value_type h = HSVType::h;
  /// @brief Saturation component from HSV input
  static constexpr value_type s = HSVType::s;
  /// @brief Value (brightness) component from HSV input
  static constexpr value_type v = HSVType::v;

  /// @brief Normalized hue component (0.0-360.0)
  static constexpr value_type h_norm = h / (std::is_integral_v<value_type> ? 1.0 : 1.0);
  /// @brief Normalized saturation component (0.0-1.0)
  static constexpr value_type s_norm = s / (std::is_integral_v<value_type> ? 100.0 : 1.0);
  /// @brief Normalized value component (0.0-1.0)
  static constexpr value_type v_norm = v / (std::is_integral_v<value_type> ? 100.0 : 1.0);

  /// @brief Calculated lightness component (0.0-1.0)
  static constexpr value_type l = v_norm * (2.0 - s_norm) / 2.0;
  /// @brief Calculated saturation component for HSL (0.0-1.0)
  static constexpr value_type s_hsl = (l == 0.0 || l == 1.0) ? 0.0 : (v_norm - l) / std::min(l, 1.0 - l);

  /**
   * @brief Resulting HSL type
   *
   * Converts the calculated HSL values to integer format and creates
   * an hsl_int type with the computed hue, saturation, and lightness components.
   */
  using type = core::hsl_int<static_cast<int>(std::round(h_norm)), static_cast<int>(std::round(s_hsl * 100.0)),
                             static_cast<int>(std::round(l * 100.0))>;
};

/**
 * @brief Type alias for HSV to HSL conversion result
 *
 * @tparam HSVType Source HSV color type
 * @return HSL color type resulting from the conversion
 */
template <typename HSVType>
using hsv_to_hsl_t = typename hsv_to_hsl<HSVType>::type;

}  // namespace color::conversion
