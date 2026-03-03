/**
 * @file hsl.hpp
 * @brief HSLA color space core implementation
 *
 * Provides basic HSLA color template class and common color aliases.
 * Supports integer and floating-point HSLA representations.
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
 * @brief Basic HSLA color template class
 *
 * Template class for representing HSLA colors with compile-time validation.
 * Supports both integer (H: 0-359, S/L: 0-100) and floating-point (H: 0-360, S/L: 0.0-1.0) value ranges.
 *
 * @tparam T Value type (must be arithmetic)
 * @tparam Scale Scaling factor for value conversion
 */
template <typename T, intptr_t Scale = 1>
struct basic_hsla {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

  using value_type = T;

  T h, s, l, a;

  constexpr basic_hsla() : h(0), s(0), l(0), a(1) {}

  constexpr basic_hsla(T hue, T saturation, T lightness, T alpha) : h(hue), s(saturation), l(lightness), a(alpha) {
    if (!is_valid_val(hue, saturation, lightness, alpha)) {
      assert(false && "HSLA values out of range!");
    }
  }

  template <intptr_t H_raw, intptr_t S_raw, intptr_t L_raw, intptr_t A_raw = Scale>
  static constexpr basic_hsla make() {
    constexpr T static_h = static_cast<T>(H_raw) / static_cast<T>(Scale);
    constexpr T static_s = static_cast<T>(S_raw) / static_cast<T>(Scale);
    constexpr T static_l = static_cast<T>(L_raw) / static_cast<T>(Scale);
    constexpr T static_a = static_cast<T>(A_raw) / static_cast<T>(Scale);
    static_assert(is_valid_val(static_h, static_s, static_l, static_a), "HSLA value out of range!");
    return {static_h, static_s, static_l, static_a};
  }

  static constexpr bool is_valid_val(T hv, T sv, T lv, T av) {
    if constexpr (std::is_floating_point_v<T>) {
      return hv >= 0.0 && hv < 360.0 && sv >= 0.0 && sv <= 1.0 && lv >= 0.0 && lv <= 1.0 && av >= 0.0 && av <= 1.0;
    } else {
      return hv >= 0 && hv < 360 && sv >= 0 && sv <= static_cast<T>(Scale) && lv >= 0 && lv <= static_cast<T>(Scale) &&
             av >= 0 && av <= static_cast<T>(Scale);
    }
  }

  constexpr bool is_valid() const { return is_valid_val(h, s, l, a); }
};

/**
 * @name HSLA Color Type Aliases
 * @{
 */

/**
 * @brief Integer HSLA color type
 *
 * Represents HSLA colors using integer values.
 * Hue range: 0-359 degrees, Saturation/Lightness range: 0-100.
 * Alpha channel defaults to 255 (fully opaque).
 *
 * @tparam H Hue component (0-359)
 * @tparam S Saturation component (0-100)
 * @tparam L Lightness component (0-100)
 * @tparam A Alpha component (0-255, default 255)
 */
using hsla_int_t = basic_hsla<int, 1>;
template <int H, int S, int L, int A = 255>
inline constexpr hsla_int_t hsla_int = hsla_int_t::make<H, S, L, A>();

/**
 * @brief Floating-point HSLA color type
 *
 * Represents HSLA colors using float precision floating-point values.
 * Hue range: 0-360 degrees, Saturation/Lightness range: 0.0-1.0.
 * Scale is set to 1000, mapping integer inputs to floating-point range.
 * Alpha channel defaults to 1.0 (fully opaque).
 *
 * @tparam H Hue component (scaled from integer to 0-360)
 * @tparam S Saturation component (scaled from integer to 0.0-1.0)
 * @tparam L Lightness component (scaled from integer to 0.0-1.0)
 * @tparam A Alpha component (scaled from integer to 0.0-1.0, default 1.0)
 */
using hsla_float_t = basic_hsla<float, 1000>;
template <int H, int S, int L, int A = 1000>
inline constexpr hsla_float_t hsla_float = hsla_float_t::make<H, S, L, A>();

/** @} */

}  // namespace color::core
