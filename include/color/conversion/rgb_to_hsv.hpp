/**
 * @file rgb_to_hsv.hpp
 * @brief RGB to HSV color space conversion
 *
 * Provides compile-time conversion from RGB color space to HSV color space.
 * Uses the standard RGB to HSV transformation algorithm.
 *
 * @author Merlot.Qi
 * 
 */

#pragma once

#include <cmath>

#include "../core/hsv.hpp"

namespace color::conversion {

/**
 * @brief RGB to HSV color space converter
 *
 * Template struct for converting RGB colors to HSV colors at compile time.
 * Uses the standard RGB to HSV transformation algorithm.
 *
 * @tparam RGBType Source RGB color type
 */
template <typename RGBType>
struct rgb_to_hsv {
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
  /// @brief Minimum RGB component value
  static constexpr value_type min_val =
      (r_norm < g_norm) ? ((r_norm < b_norm) ? r_norm : b_norm) : ((g_norm < b_norm) ? g_norm : b_norm);
  /// @brief Difference between max and min RGB values
  static constexpr value_type delta = max_val - min_val;

  /**
   * @brief Compute hue value from RGB components
   *
   * Calculates the hue angle in degrees (0-360) based on the RGB color model.
   * Uses the standard HSV hue calculation algorithm.
   *
   * @return Hue value in degrees (0.0-360.0)
   */
  static constexpr value_type compute_hue() {
    if (delta == 0) return 0.0;

    value_type hue = 0.0;
    if (max_val == r_norm) {
      hue = 60.0 * std::fmod((g_norm - b_norm) / delta, 6.0);
    } else if (max_val == g_norm) {
      hue = 60.0 * ((b_norm - r_norm) / delta + 2.0);
    } else {
      hue = 60.0 * ((r_norm - g_norm) / delta + 4.0);
    }

    return hue < 0.0 ? hue + 360.0 : hue;
  }

  /// @brief Calculated hue value (0.0-360.0)
  static constexpr value_type h = compute_hue();

  /// @brief Calculated saturation value (0.0-1.0)
  static constexpr value_type s = (max_val == 0.0) ? 0.0 : (delta / max_val);

  /// @brief Calculated value (brightness) (0.0-1.0)
  static constexpr value_type v = max_val;

  /**
   * @brief Resulting HSV type
   *
   * Converts the calculated HSV values to integer format and creates
   * an hsv_int type with the computed hue, saturation, and value components.
   */
  using type = core::hsv_int<static_cast<int>(std::round(h)), static_cast<int>(std::round(s * 100.0)),
                             static_cast<int>(std::round(v * 100.0))>;
};

/**
 * @brief Type alias for RGB to HSV conversion result
 *
 * @tparam RGBType Source RGB color type
 * @return HSV color type resulting from the conversion
 */
template <typename RGBType>
using rgb_to_hsv_t = typename rgb_to_hsv<RGBType>::type;

}  // namespace color::conversion
