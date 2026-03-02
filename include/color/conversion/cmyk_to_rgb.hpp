/**
 * @file cmyk_to_rgb.hpp
 * @brief CMYK to RGB color space conversion
 *
 * Provides compile-time conversion from CMYK color space to RGB color space.
 * Uses the standard CMYK to RGB transformation formula.
 *
 * @author Merlot.Qi
 * 
 */

#pragma once

#include <cmath>

#include "../core/rgb.hpp"

namespace color::conversion {

/**
 * @brief CMYK to RGB color space converter
 *
 * Template struct for converting CMYK colors to RGB colors at compile time.
 * Uses the standard CMYK to RGB transformation formula.
 *
 * @tparam CMYKType Source CMYK color type
 */
template <typename CMYKType>
struct cmyk_to_rgb {
  using cmyk_type = CMYKType;
  using value_type = typename CMYKType::value_type;

  /// @brief Cyan component from CMYK input
  static constexpr value_type c = CMYKType::c;
  /// @brief Magenta component from CMYK input
  static constexpr value_type m = CMYKType::m;
  /// @brief Yellow component from CMYK input
  static constexpr value_type y = CMYKType::y;
  /// @brief Key (black) component from CMYK input
  static constexpr value_type k = CMYKType::k;

  /// @brief Normalized cyan component (0.0-1.0)
  static constexpr value_type c_norm = c / (std::is_integral_v<value_type> ? 100.0 : 1.0);
  /// @brief Normalized magenta component (0.0-1.0)
  static constexpr value_type m_norm = m / (std::is_integral_v<value_type> ? 100.0 : 1.0);
  /// @brief Normalized yellow component (0.0-1.0)
  static constexpr value_type y_norm = y / (std::is_integral_v<value_type> ? 100.0 : 1.0);
  /// @brief Normalized key component (0.0-1.0)
  static constexpr value_type k_norm = k / (std::is_integral_v<value_type> ? 100.0 : 1.0);

  /// @brief Calculated red component (0.0-1.0)
  static constexpr value_type r = (1.0 - c_norm) * (1.0 - k_norm);
  /// @brief Calculated green component (0.0-1.0)
  static constexpr value_type g = (1.0 - m_norm) * (1.0 - k_norm);
  /// @brief Calculated blue component (0.0-1.0)
  static constexpr value_type b = (1.0 - y_norm) * (1.0 - k_norm);

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
 * @brief Type alias for CMYK to RGB conversion result
 *
 * @tparam CMYKType Source CMYK color type
 * @return RGB color type resulting from the conversion
 */
template <typename CMYKType>
using cmyk_to_rgb_t = typename cmyk_to_rgb<CMYKType>::type;

}  // namespace color::conversion
