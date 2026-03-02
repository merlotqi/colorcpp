/**
 * @file hsl_to_cmyk.hpp
 * @brief HSL to CMYK color space conversion
 *
 * Provides compile-time conversion from HSL color space to CMYK color space.
 * The conversion is performed in two steps: HSL -> RGB -> CMYK.
 *
 * @author Merlot.Qi
 * 
 */

#pragma once

#include <cmath>

#include "../core/cmyk.hpp"

namespace color::conversion {

/**
 * @brief HSL to CMYK color space converter
 *
 * Template struct for converting HSL colors to CMYK colors at compile time.
 * The conversion follows the standard HSL -> RGB -> CMYK transformation path.
 *
 * @tparam HSLType Source HSL color type
 */
template <typename HSLType>
struct hsl_to_cmyk {
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

  /// @brief Red component after HSL to RGB conversion
  static constexpr value_type r = r_temp + m;
  /// @brief Green component after HSL to RGB conversion
  static constexpr value_type g = g_temp + m;
  /// @brief Blue component after HSL to RGB conversion
  static constexpr value_type b = b_temp + m;

  /// @brief Maximum RGB component value
  static constexpr value_type max_val = (r > g) ? ((r > b) ? r : b) : ((g > b) ? g : b);

  /// @brief Cyan component before CMYK adjustment
  static constexpr value_type c_cmyk = 1.0 - r;
  /// @brief Magenta component before CMYK adjustment
  static constexpr value_type m_cmyk = 1.0 - g;
  /// @brief Yellow component before CMYK adjustment
  static constexpr value_type y_cmyk = 1.0 - b;
  /// @brief Key (black) component
  static constexpr value_type k = 1.0 - max_val;

  /// @brief Final cyan component (0.0-1.0)
  static constexpr value_type c_final = (k == 1.0) ? 0.0 : ((c_cmyk - k) / (1.0 - k));
  /// @brief Final magenta component (0.0-1.0)
  static constexpr value_type m_final = (k == 1.0) ? 0.0 : ((m_cmyk - k) / (1.0 - k));
  /// @brief Final yellow component (0.0-1.0)
  static constexpr value_type y_final = (k == 1.0) ? 0.0 : ((y_cmyk - k) / (1.0 - k));

  /**
   * @brief Resulting CMYK type
   *
   * Converts the calculated CMYK values to integer format and creates
   * a cmyk_int type with the computed cyan, magenta, yellow, and key components.
   */
  using type =
      core::cmyk_int<static_cast<int>(std::round(c_final * 100.0)), static_cast<int>(std::round(m_final * 100.0)),
                     static_cast<int>(std::round(y_final * 100.0)), static_cast<int>(std::round(k * 100.0))>;
};

/**
 * @brief Type alias for HSL to CMYK conversion result
 *
 * @tparam HSLType Source HSL color type
 * @return CMYK color type resulting from the conversion
 */
template <typename HSLType>
using hsl_to_cmyk_t = typename hsl_to_cmyk<HSLType>::type;

}  // namespace color::conversion
