/**
 * @file rgb_to_cmyk.hpp
 * @brief RGB to CMYK color space conversion
 *
 * Provides compile-time conversion from RGB color space to CMYK color space.
 * Uses the standard RGB to CMYK transformation algorithm.
 *
 * @author Merlot.Qi
 * 
 */

#pragma once

#include <cmath>

#include "../core/cmyk.hpp"

namespace color::conversion {

/**
 * @brief RGB to CMYK color space converter
 *
 * Template struct for converting RGB colors to CMYK colors at compile time.
 * Uses the standard RGB to CMYK transformation algorithm.
 *
 * @tparam RGBType Source RGB color type
 */
template <typename RGBType>
struct rgb_to_cmyk {
  using rgb_type = RGBType;
  using value_type = typename RGBType::value_type;

  /// @brief Red component from RGB input
  static constexpr value_type r = RGBType::r;
  /// @brief Green component from RGB input
  static constexpr value_type g = RGBType::g;
  /// @brief Blue component from RGB input
  static constexpr value_type b = RGBType::b;

  /// @brief Normalized red component (0.0-1.0)
  static constexpr value_type r_norm = r / (std::is_integral_v<value_type> ? 255.0 : 1.0);
  /// @brief Normalized green component (0.0-1.0)
  static constexpr value_type g_norm = g / (std::is_integral_v<value_type> ? 255.0 : 1.0);
  /// @brief Normalized blue component (0.0-1.0)
  static constexpr value_type b_norm = b / (std::is_integral_v<value_type> ? 255.0 : 1.0);

  /// @brief Maximum RGB component value
  static constexpr value_type max_val =
      (r_norm > g_norm) ? ((r_norm > b_norm) ? r_norm : b_norm) : ((g_norm > b_norm) ? g_norm : b_norm);

  /// @brief Cyan component before CMYK adjustment
  static constexpr value_type c = 1.0 - r_norm;
  /// @brief Magenta component before CMYK adjustment
  static constexpr value_type m = 1.0 - g_norm;
  /// @brief Yellow component before CMYK adjustment
  static constexpr value_type y = 1.0 - b_norm;
  /// @brief Key (black) component
  static constexpr value_type k = 1.0 - max_val;

  /// @brief Final cyan component (0.0-1.0)
  static constexpr value_type c_final = (k == 1.0) ? 0.0 : ((c - k) / (1.0 - k));
  /// @brief Final magenta component (0.0-1.0)
  static constexpr value_type m_final = (k == 1.0) ? 0.0 : ((m - k) / (1.0 - k));
  /// @brief Final yellow component (0.0-1.0)
  static constexpr value_type y_final = (k == 1.0) ? 0.0 : ((y - k) / (1.0 - k));

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
 * @brief Type alias for RGB to CMYK conversion result
 *
 * @tparam RGBType Source RGB color type
 * @return CMYK color type resulting from the conversion
 */
template <typename RGBType>
using rgb_to_cmyk_t = typename rgb_to_cmyk<RGBType>::type;

}  // namespace color::conversion
