/**
 * @file hsl.hpp
 * @brief HSL color space core implementation
 *
 * Provides basic HSL color template class and common color aliases.
 * Supports integer and floating-point HSL representations.
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
 * @brief Basic HSL color template class
 *
 * Template class for representing HSL colors with compile-time validation.
 * Supports both integer (H: 0-359, S/L: 0-100) and floating-point (H: 0-360, S/L: 0.0-1.0) value ranges.
 *
 * @tparam T Value type (must be arithmetic)
 * @tparam H_raw Raw hue component value
 * @tparam S_raw Raw saturation component value
 * @tparam L_raw Raw lightness component value
 * @tparam Scale Scaling factor for value conversion
 */
template <typename T, intptr_t H_raw, intptr_t S_raw, intptr_t L_raw, intptr_t Scale = 1>
struct basic_hsl {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

  using value_type = T;

  /// @brief Hue component value (converted from raw value using scale)
  static constexpr T h = static_cast<T>(H_raw) / static_cast<T>(Scale);
  /// @brief Saturation component value (converted from raw value using scale)
  static constexpr T s = static_cast<T>(S_raw) / static_cast<T>(Scale);
  /// @brief Lightness component value (converted from raw value using scale)
  static constexpr T l = static_cast<T>(L_raw) / static_cast<T>(Scale);

  /**
   * @brief Validate that HSL values are within acceptable ranges
   * @return true if values are valid, false otherwise
   */
  static constexpr bool is_valid() {
    if constexpr (std::is_floating_point_v<T>) {
      return h >= 0.0 && h < 360.0 && s >= 0.0 && s <= 1.0 && l >= 0.0 && l <= 1.0;
    } else {
      return h >= 0 && h < 360 && s >= 0 && s <= 100 && l >= 0 && l <= 100;
    }
  }

  static_assert(is_valid(),
                "HSL values must be in valid range (0-360 for H, 0-100 for S/L for int, 0.0-1.0 for float)");
};

/**
 * @name HSL Color Type Aliases
 * @{
 */

/**
 * @brief Integer HSL color type
 *
 * Represents HSL colors using integer values.
 * Hue range: 0-359 degrees, Saturation/Lightness range: 0-100.
 *
 * @tparam H Hue component (0-359)
 * @tparam S Saturation component (0-100)
 * @tparam L Lightness component (0-100)
 */
template <int H, int S, int L>
using hsl_int = basic_hsl<int, H, S, L, 1>;

/**
 * @brief Floating-point HSL color type
 *
 * Represents HSL colors using double precision floating-point values.
 * Hue range: 0-360 degrees, Saturation/Lightness range: 0.0-1.0.
 * Scale is set to 1000, mapping integer inputs to floating-point range.
 *
 * @tparam H Hue component (scaled from integer to 0-360)
 * @tparam S Saturation component (scaled from integer to 0.0-1.0)
 * @tparam L Lightness component (scaled from integer to 0.0-1.0)
 */
template <int H, int S, int L>
using hsl_float = basic_hsl<double, H, S, L, 1000>;

/** @} */

namespace colors {
using red_hsl = hsl_int<0, 100, 50>;
using green_hsl = hsl_int<120, 100, 50>;
using blue_hsl = hsl_int<240, 100, 50>;
using yellow_hsl = hsl_int<60, 100, 50>;
using cyan_hsl = hsl_int<180, 100, 50>;
using magenta_hsl = hsl_int<300, 100, 50>;
using black_hsl = hsl_int<0, 0, 0>;
using white_hsl = hsl_int<0, 0, 100>;
using gray_hsl = hsl_int<0, 0, 50>;
}  // namespace colors

}  // namespace color::core
