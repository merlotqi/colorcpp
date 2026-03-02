/**
 * @file hsl.hpp
 * @brief HSL color space core implementation
 *
 * Provides basic HSL color template class and common color aliases.
 * Supports integer and floating-point HSL representations.
 *
 * @author Merlot.Qi
 * 
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
// Basic HSL colors
using red_hsl = hsl_int<0, 100, 50>;
using orange_hsl = hsl_int<30, 100, 50>;
using yellow_hsl = hsl_int<60, 100, 50>;
using chartreuse_hsl = hsl_int<90, 100, 50>;
using green_hsl = hsl_int<120, 100, 50>;
using springgreen_hsl = hsl_int<150, 100, 50>;
using cyan_hsl = hsl_int<180, 100, 50>;
using azure_hsl = hsl_int<210, 100, 50>;
using blue_hsl = hsl_int<240, 100, 50>;
using violet_hsl = hsl_int<270, 100, 50>;
using magenta_hsl = hsl_int<300, 100, 50>;
using rose_hsl = hsl_int<330, 100, 50>;

// Grayscale HSL
using black_hsl = hsl_int<0, 0, 0>;
using gray_hsl = hsl_int<0, 0, 50>;
using silver_hsl = hsl_int<0, 0, 75>;
using white_hsl = hsl_int<0, 0, 100>;

// W3C standard colors in HSL
using maroon_hsl = hsl_int<0, 100, 25>;
using darkred_hsl = hsl_int<0, 100, 27>;
using brown_hsl = hsl_int<0, 59, 41>;
using olive_hsl = hsl_int<60, 100, 25>;
using darkgreen_hsl = hsl_int<120, 100, 20>;
using teal_hsl = hsl_int<180, 100, 25>;
using navy_hsl = hsl_int<240, 100, 25>;
using purple_hsl = hsl_int<300, 100, 25>;
using fuchsia_hsl = hsl_int<300, 100, 50>;
using aqua_hsl = hsl_int<180, 100, 50>;
using lime_hsl = hsl_int<120, 100, 50>;

// Light variants
using lightred_hsl = hsl_int<0, 100, 75>;
using lightgreen_hsl = hsl_int<120, 100, 75>;
using lightblue_hsl = hsl_int<210, 100, 75>;
using lightyellow_hsl = hsl_int<60, 100, 75>;
using lightcyan_hsl = hsl_int<180, 100, 75>;
using lightmagenta_hsl = hsl_int<300, 100, 75>;

// Dark variants
using darkorange_hsl = hsl_int<30, 100, 40>;
using darkcyan_hsl = hsl_int<180, 100, 30>;
using darkblue_hsl = hsl_int<240, 100, 30>;
using darkpurple_hsl = hsl_int<280, 100, 30>;

// Pastel variants (lower saturation)
using pink_hsl = hsl_int<350, 70, 75>;
using lavender_hsl = hsl_int<240, 70, 85>;
using mint_hsl = hsl_int<150, 60, 80>;
using peach_hsl = hsl_int<30, 70, 80>;
}  // namespace colors

}  // namespace color::core
