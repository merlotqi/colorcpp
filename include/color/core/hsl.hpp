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

#include <cassert>
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
 * @tparam Scale Scaling factor for value conversion
 */
template <typename T, intptr_t Scale = 1>
struct basic_hsl {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

  using value_type = T;
  T h, s, l;
  constexpr basic_hsl() : h(0), s(0), l(0) {}
  constexpr basic_hsl(T hue, T saturation, T lightness) : h(hue), s(saturation), l(lightness) {
    if (!is_valid_val(hue, saturation, lightness)) {
      assert(false && "HSL values out of range!");
    }
  }

  template <intptr_t H_raw, intptr_t S_raw, intptr_t L_raw>
  static constexpr basic_hsl make() {
    constexpr T static_h = static_cast<T>(H_raw) / static_cast<T>(Scale);
    constexpr T static_s = static_cast<T>(S_raw) / static_cast<T>(Scale);
    constexpr T static_l = static_cast<T>(L_raw) / static_cast<T>(Scale);
    static_assert(is_valid_val(static_h, static_s, static_l), "HSL value out of range!");
    return {static_h, static_s, static_l};
  }

  static constexpr bool is_valid_val(T hv, T sv, T lv) {
    if constexpr (std::is_floating_point_v<T>) {
      return hv >= 0.0 && hv < 360.0 && sv >= 0.0 && sv <= 1.0 && lv >= 0.0 && lv <= 1.0;
    } else {
      return hv >= 0 && hv < 360 && sv >= 0 && sv <= 100 && lv >= 0 && lv <= 100;
    }
  }

  constexpr bool is_valid() const { return is_valid_val(h, s, l); }
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
using hsl_int_t = basic_hsl<int, 1>;
template <int H, int S, int L>
inline constexpr hsl_int_t hsl_int = hsl_int_t::make<H, S, L>();

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
using hsl_float_t = basic_hsl<double, 1000>;
template <int H, int S, int L>
inline constexpr hsl_float_t hsl_float = hsl_float_t::make<H, S, L>();

/** @} */

namespace colors {
// Basic HSL colors
inline constexpr hsl_int_t red_hsl = hsl_int<0, 100, 50>;
inline constexpr hsl_int_t orange_hsl = hsl_int<30, 100, 50>;
inline constexpr hsl_int_t yellow_hsl = hsl_int<60, 100, 50>;
inline constexpr hsl_int_t chartreuse_hsl = hsl_int<90, 100, 50>;
inline constexpr hsl_int_t green_hsl = hsl_int<120, 100, 50>;
inline constexpr hsl_int_t springgreen_hsl = hsl_int<150, 100, 50>;
inline constexpr hsl_int_t cyan_hsl = hsl_int<180, 100, 50>;
inline constexpr hsl_int_t azure_hsl = hsl_int<210, 100, 50>;
inline constexpr hsl_int_t blue_hsl = hsl_int<240, 100, 50>;
inline constexpr hsl_int_t violet_hsl = hsl_int<270, 100, 50>;
inline constexpr hsl_int_t magenta_hsl = hsl_int<300, 100, 50>;
inline constexpr hsl_int_t rose_hsl = hsl_int<330, 100, 50>;

// Grayscale HSL
inline constexpr hsl_int_t black_hsl = hsl_int<0, 0, 0>;
inline constexpr hsl_int_t gray_hsl = hsl_int<0, 0, 50>;
inline constexpr hsl_int_t silver_hsl = hsl_int<0, 0, 75>;
inline constexpr hsl_int_t white_hsl = hsl_int<0, 0, 100>;

// W3C standard colors in HSL
inline constexpr hsl_int_t maroon_hsl = hsl_int<0, 100, 25>;
inline constexpr hsl_int_t darkred_hsl = hsl_int<0, 100, 27>;
inline constexpr hsl_int_t brown_hsl = hsl_int<0, 59, 41>;
inline constexpr hsl_int_t olive_hsl = hsl_int<60, 100, 25>;
inline constexpr hsl_int_t darkgreen_hsl = hsl_int<120, 100, 20>;
inline constexpr hsl_int_t teal_hsl = hsl_int<180, 100, 25>;
inline constexpr hsl_int_t navy_hsl = hsl_int<240, 100, 25>;
inline constexpr hsl_int_t purple_hsl = hsl_int<300, 100, 25>;
inline constexpr hsl_int_t fuchsia_hsl = hsl_int<300, 100, 50>;
inline constexpr hsl_int_t aqua_hsl = hsl_int<180, 100, 50>;
inline constexpr hsl_int_t lime_hsl = hsl_int<120, 100, 50>;

// Light variants
inline constexpr hsl_int_t lightred_hsl = hsl_int<0, 100, 75>;
inline constexpr hsl_int_t lightgreen_hsl = hsl_int<120, 100, 75>;
inline constexpr hsl_int_t lightblue_hsl = hsl_int<210, 100, 75>;
inline constexpr hsl_int_t lightyellow_hsl = hsl_int<60, 100, 75>;
inline constexpr hsl_int_t lightcyan_hsl = hsl_int<180, 100, 75>;
inline constexpr hsl_int_t lightmagenta_hsl = hsl_int<300, 100, 75>;

// Dark variants
inline constexpr hsl_int_t darkorange_hsl = hsl_int<30, 100, 40>;
inline constexpr hsl_int_t darkcyan_hsl = hsl_int<180, 100, 30>;
inline constexpr hsl_int_t darkblue_hsl = hsl_int<240, 100, 30>;
inline constexpr hsl_int_t darkpurple_hsl = hsl_int<280, 100, 30>;

// Pastel variants (lower saturation)
inline constexpr hsl_int_t pink_hsl = hsl_int<350, 70, 75>;
inline constexpr hsl_int_t lavender_hsl = hsl_int<240, 70, 85>;
inline constexpr hsl_int_t mint_hsl = hsl_int<150, 60, 80>;
inline constexpr hsl_int_t peach_hsl = hsl_int<30, 70, 80>;
}  // namespace colors

}  // namespace color::core
