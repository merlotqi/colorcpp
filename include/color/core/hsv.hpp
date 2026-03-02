/**
 * @file hsv.hpp
 * @brief HSV color space core implementation
 *
 * Provides basic HSV color template class and common color aliases.
 * Supports integer and floating-point HSV representations.
 *
 * @author Color Library Team
 * @date 2025
 * @version 1.0
 */

#pragma once

#include <cstdint>
#include <type_traits>

namespace color::core {

/**
 * @brief Basic HSV color template class
 *
 * Template class for representing HSV colors with compile-time validation.
 * Supports both integer (H: 0-359, S/V: 0-100) and floating-point (H: 0-360, S/V: 0.0-1.0) value ranges.
 *
 * @tparam T Value type (must be arithmetic)
 * @tparam H_raw Raw hue component value
 * @tparam S_raw Raw saturation component value
 * @tparam V_raw Raw value component value
 * @tparam Scale Scaling factor for value conversion
 */
template <typename T, intptr_t H_raw, intptr_t S_raw, intptr_t V_raw, intptr_t Scale = 1>
struct basic_hsv {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

  using value_type = T;

  /// @brief Hue component value (converted from raw value using scale)
  static constexpr T h = static_cast<T>(H_raw) / static_cast<T>(Scale);
  /// @brief Saturation component value (converted from raw value using scale)
  static constexpr T s = static_cast<T>(S_raw) / static_cast<T>(Scale);
  /// @brief Value component value (converted from raw value using scale)
  static constexpr T v = static_cast<T>(V_raw) / static_cast<T>(Scale);

  /**
   * @brief Validate that HSV values are within acceptable ranges
   * @return true if values are valid, false otherwise
   */
  static constexpr bool is_valid() {
    if constexpr (std::is_floating_point_v<T>) {
      return h >= 0.0 && h < 360.0 && s >= 0.0 && s <= 1.0 && v >= 0.0 && v <= 1.0;
    } else {
      return h >= 0 && h < 360 && s >= 0 && s <= 100 && v >= 0 && v <= 100;
    }
  }

  static_assert(is_valid(),
                "HSV values must be in valid range (0-360 for H, 0-100 for S/V for int, 0.0-1.0 for float)");
};

/**
 * @name HSV Color Type Aliases
 * @{
 */

/**
 * @brief Integer HSV color type
 *
 * Represents HSV colors using integer values.
 * Hue range: 0-359 degrees, Saturation/Value range: 0-100.
 *
 * @tparam H Hue component (0-359)
 * @tparam S Saturation component (0-100)
 * @tparam V Value component (0-100)
 */
template <int H, int S, int V>
using hsv_int = basic_hsv<int, H, S, V, 1>;

/**
 * @brief Floating-point HSV color type
 *
 * Represents HSV colors using double precision floating-point values.
 * Hue range: 0-360 degrees, Saturation/Value range: 0.0-1.0.
 * Scale is set to 1000, mapping integer inputs to floating-point range.
 *
 * @tparam H Hue component (scaled from integer to 0-360)
 * @tparam S Saturation component (scaled from integer to 0.0-1.0)
 * @tparam V Value component (scaled from integer to 0.0-1.0)
 */
template <int H, int S, int V>
using hsv_float = basic_hsv<double, H, S, V, 1000>;

/** @} */

namespace colors {
using red_hsv = hsv_int<0, 100, 100>;
using green_hsv = hsv_int<120, 100, 100>;
using blue_hsv = hsv_int<240, 100, 100>;
using yellow_hsv = hsv_int<60, 100, 100>;
using cyan_hsv = hsv_int<180, 100, 100>;
using magenta_hsv = hsv_int<300, 100, 100>;
using black_hsv = hsv_int<0, 0, 0>;
using white_hsv = hsv_int<0, 0, 100>;
using gray_hsv = hsv_int<0, 0, 50>;
}  // namespace colors

}  // namespace color::core
