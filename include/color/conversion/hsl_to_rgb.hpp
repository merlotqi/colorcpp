/**
 * @file hsl_to_rgb.hpp
 * @brief HSL to RGB color space conversion
 *
 * Provides compile-time conversion from HSL color space to RGB color space.
 * Uses the standard HSL to RGB transformation algorithm.
 *
 * @author Merlot.Qi
 * 
 */

#pragma once

#include <cmath>

#include "../core/rgb.hpp"

namespace color::conversion {

/**
 * @brief HSL to RGB color space converter
 *
 * Template struct for converting HSL colors to RGB colors at compile time.
 * Uses the standard HSL to RGB transformation algorithm.
 *
 * @tparam HSLType Source HSL color type
 */
template <typename HSLType>
struct hsl_to_rgb {
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

  /// @brief Chroma value for HSL to RGB conversion
  static constexpr value_type c = (1.0 - std::abs(2.0 * l_norm - 1.0)) * s_norm;
  /// @brief Intermediate value for HSL to RGB conversion
  static constexpr value_type x = c * (1.0 - std::abs(std::fmod(h_norm / 60.0, 2.0) - 1.0));
  /// @brief Match value for HSL to RGB conversion
  static constexpr value_type m = l_norm - c / 2.0;

  /// @brief Temporary red component during HSL to RGB conversion
  static constexpr value_type r_temp = (h_norm < 60.0)    ? c
                                       : (h_norm < 120.0) ? x
                                       : (h_norm < 240.0) ? 0.0
                                       : (h_norm < 300.0) ? x
                                                          : c;

  /// @brief Temporary green component during HSL to RGB conversion
  static constexpr value_type g_temp = (h_norm < 60.0) ? x : (h_norm < 180.0) ? c : (h_norm < 240.0) ? x : 0.0;

  /// @brief Temporary blue component during HSL to RGB conversion
  static constexpr value_type b_temp = (h_norm < 120.0) ? 0.0 : (h_norm < 180.0) ? x : (h_norm < 300.0) ? c : x;

  /// @brief Final red component (0.0-1.0)
  static constexpr value_type r = r_temp + m;
  /// @brief Final green component (0.0-1.0)
  static constexpr value_type g = g_temp + m;
  /// @brief Final blue component (0.0-1.0)
  static constexpr value_type b = b_temp + m;

  /**
   * @brief Resulting RGB type
   *
   * Converts the calculated RGB values to 8-bit format and creates
   * an rgb8 type with the computed red, green, and blue components.
   */
  using type = core::rgb8<static_cast<uint8_t>(std::round(r * 255.0)), static_cast<uint8_t>(std::round(g * 255.0)),
                          static_cast<uint8_t>(std::round(b * 255.0))>;
};

/**
 * @brief Type alias for HSL to RGB conversion result
 *
 * @tparam HSLType Source HSL color type
 * @return RGB color type resulting from the conversion
 */
template <typename HSLType>
using hsl_to_rgb_t = typename hsl_to_rgb<HSLType>::type;

}  // namespace color::conversion
