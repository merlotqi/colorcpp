/**
 * @file hsv_to_cmyk.hpp
 * @brief HSV to CMYK color space conversion
 *
 * Provides compile-time conversion from HSV color space to CMYK color space.
 * The conversion is performed in two steps: HSV -> RGB -> CMYK.
 *
 * @author Color Library Team
 * @date 2025
 * @version 1.0
 */

#pragma once

#include <cmath>

#include "../core/cmyk.hpp"

namespace color::conversion {

/**
 * @brief HSV to CMYK color space converter
 *
 * Template struct for converting HSV colors to CMYK colors at compile time.
 * The conversion follows the standard HSV -> RGB -> CMYK transformation path.
 *
 * @tparam HSVType Source HSV color type
 */
template <typename HSVType>
struct hsv_to_cmyk {
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

  /// @brief Chroma value for HSV to RGB conversion
  static constexpr value_type c = v_norm * s_norm;
  /// @brief Intermediate value for HSV to RGB conversion
  static constexpr value_type x = c * (1.0 - std::abs(std::fmod(h_norm / 60.0, 2.0) - 1.0));
  /// @brief Match value for HSV to RGB conversion
  static constexpr value_type m = v_norm - c;

  /// @brief Temporary red component during HSV to RGB conversion
  static constexpr value_type r_temp = (h_norm < 60.0)    ? c
                                       : (h_norm < 120.0) ? x
                                       : (h_norm < 240.0) ? 0.0
                                       : (h_norm < 300.0) ? x
                                                          : c;

  /// @brief Temporary green component during HSV to RGB conversion
  static constexpr value_type g_temp = (h_norm < 60.0) ? x : (h_norm < 180.0) ? c : (h_norm < 240.0) ? x : 0.0;

  /// @brief Temporary blue component during HSV to RGB conversion
  static constexpr value_type b_temp = (h_norm < 120.0) ? 0.0 : (h_norm < 180.0) ? x : (h_norm < 300.0) ? c : x;

  /// @brief Red component after HSV to RGB conversion
  static constexpr value_type r = r_temp + m;
  /// @brief Green component after HSV to RGB conversion
  static constexpr value_type g = g_temp + m;
  /// @brief Blue component after HSV to RGB conversion
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
 * @brief Type alias for HSV to CMYK conversion result
 *
 * @tparam HSVType Source HSV color type
 * @return CMYK color type resulting from the conversion
 */
template <typename HSVType>
using hsv_to_cmyk_t = typename hsv_to_cmyk<HSVType>::type;

}  // namespace color::conversion
