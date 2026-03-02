/**
 * @file cmyk_to_hsl.hpp
 * @brief CMYK to HSL color space conversion
 *
 * Provides compile-time conversion from CMYK color space to HSL color space.
 * The conversion is performed in two steps: CMYK -> RGB -> HSL.
 *
 * @author Color Library Team
 * @date 2025
 * @version 1.0
 */

#pragma once

#include <cmath>

#include "../core/hsl.hpp"

namespace color::conversion {

/**
 * @brief CMYK to HSL color space converter
 *
 * Template struct for converting CMYK colors to HSL colors at compile time.
 * The conversion follows the standard CMYK -> RGB -> HSL transformation path.
 *
 * @tparam CMYKType Source CMYK color type
 */
template <typename CMYKType>
struct cmyk_to_hsl {
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

  /// @brief Red component after CMYK to RGB conversion
  static constexpr value_type r = (1.0 - c_norm) * (1.0 - k_norm);
  /// @brief Green component after CMYK to RGB conversion
  static constexpr value_type g = (1.0 - m_norm) * (1.0 - k_norm);
  /// @brief Blue component after CMYK to RGB conversion
  static constexpr value_type b = (1.0 - y_norm) * (1.0 - k_norm);

  /// @brief Maximum RGB component value
  static constexpr value_type max_val = (r > g) ? ((r > b) ? r : b) : ((g > b) ? g : b);
  /// @brief Minimum RGB component value
  static constexpr value_type min_val = (r < g) ? ((r < b) ? r : b) : ((g < b) ? g : b);
  /// @brief Difference between max and min RGB values
  static constexpr value_type delta = max_val - min_val;

  /**
   * @brief Compute hue value from RGB components
   *
   * Calculates the hue angle in degrees (0-360) based on the RGB color model.
   * Uses the standard HSL hue calculation algorithm.
   *
   * @return Hue value in degrees (0.0-360.0)
   */
  static constexpr value_type compute_hue() {
    if (delta == 0) return 0.0;

    value_type hue = 0.0;
    if (max_val == r) {
      hue = 60.0 * std::fmod((g - b) / delta, 6.0);
    } else if (max_val == g) {
      hue = 60.0 * ((b - r) / delta + 2.0);
    } else {
      hue = 60.0 * ((r - g) / delta + 4.0);
    }

    return hue < 0.0 ? hue + 360.0 : hue;
  }

  /// @brief Calculated hue value (0.0-360.0)
  static constexpr value_type h = compute_hue();

  /// @brief Calculated lightness value (0.0-1.0)
  static constexpr value_type l = (max_val + min_val) / 2.0;

  /// @brief Calculated saturation value (0.0-1.0)
  static constexpr value_type s = (delta == 0.0) ? 0.0 : (delta / (1.0 - std::abs(2.0 * l - 1.0)));

  /**
   * @brief Resulting HSL type
   *
   * Converts the calculated HSL values to integer format and creates
   * an hsl_int type with the computed hue, saturation, and lightness values.
   */
  using type = core::hsl_int<static_cast<int>(std::round(h)), static_cast<int>(std::round(s * 100.0)),
                             static_cast<int>(std::round(l * 100.0))>;
};

/**
 * @brief Type alias for CMYK to HSL conversion result
 *
 * @tparam CMYKType Source CMYK color type
 * @return HSL color type resulting from the conversion
 */
template <typename CMYKType>
using cmyk_to_hsl_t = typename cmyk_to_hsl<CMYKType>::type;

}  // namespace color::conversion
