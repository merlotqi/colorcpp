/**
 * @file hsv.hpp
 * @brief HSVA color space core implementation
 *
 * Provides basic HSVA color template class and common color aliases.
 * Supports integer and floating-point HSVA representations.
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
 * @brief Basic HSVA color template class
 *
 * Template class for representing HSVA colors with compile-time validation.
 * Supports both integer (H: 0-359, S/V: 0-100) and floating-point (H: 0-360, S/V: 0.0-1.0) value ranges.
 *
 * @tparam T Value type (must be arithmetic)
 * @tparam Scale Scaling factor for value conversion
 */
template <typename T, intptr_t Scale = 1>
struct basic_hsva {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

  using value_type = T;

  T h, s, v, a;

  constexpr basic_hsva() : h(0), s(0), v(0), a(1) {}

  constexpr basic_hsva(T hue, T saturation, T value, T alpha) : h(hue), s(saturation), v(value), a(alpha) {
    if (!is_valid_val(hue, saturation, value, alpha)) {
      assert(false && "HSVA values out of range!");
    }
  }

  template <intptr_t H_raw, intptr_t S_raw, intptr_t V_raw, intptr_t A_raw = Scale>
  static constexpr basic_hsva make() {
    constexpr T static_h = static_cast<T>(H_raw) / static_cast<T>(Scale);
    constexpr T static_s = static_cast<T>(S_raw) / static_cast<T>(Scale);
    constexpr T static_v = static_cast<T>(V_raw) / static_cast<T>(Scale);
    constexpr T static_a = static_cast<T>(A_raw) / static_cast<T>(Scale);
    static_assert(is_valid_val(static_h, static_s, static_v, static_a), "HSVA value out of range!");
    return {static_h, static_s, static_v, static_a};
  }

  static constexpr bool is_valid_val(T hv, T sv, T vv, T av) {
    if constexpr (std::is_floating_point_v<T>) {
      return hv >= 0.0 && hv < 360.0 && sv >= 0.0 && sv <= 1.0 && vv >= 0.0 && vv <= 1.0 && av >= 0.0 && av <= 1.0;
    } else {
      return hv >= 0 && hv < 360 && sv >= 0 && sv <= 100 && vv >= 0 && vv <= 100 && av >= 0 && av <= 255;
    }
  }

  constexpr bool is_valid() const { return is_valid_val(h, s, v, a); }
};

/**
 * @name HSVA Color Type Aliases
 * @{
 */

/**
 * @brief Integer HSVA color type
 *
 * Represents HSVA colors using integer values.
 * Hue range: 0-359 degrees, Saturation/Value range: 0-100.
 * Alpha channel defaults to 255 (fully opaque).
 *
 * @tparam H Hue component (0-359)
 * @tparam S Saturation component (0-100)
 * @tparam V Value component (0-100)
 * @tparam A Alpha component (0-255, default 255)
 */
using hsva_int_t = basic_hsva<int, 1>;
template <int H, int S, int V, int A = 255>
inline constexpr hsva_int_t hsva_int = hsva_int_t::make<H, S, V, A>();

/**
 * @brief Floating-point HSVA color type
 *
 * Represents HSVA colors using float precision floating-point values.
 * Hue range: 0-360 degrees, Saturation/Value range: 0.0-1.0.
 * Scale is set to 1000, mapping integer inputs to floating-point range.
 * Alpha channel defaults to 1.0 (fully opaque).
 *
 * @tparam H Hue component (scaled from integer to 0-360)
 * @tparam S Saturation component (scaled from integer to 0.0-1.0)
 * @tparam V Value component (scaled from integer to 0.0-1.0)
 * @tparam A Alpha component (scaled from integer to 0.0-1.0, default 1.0)
 */
using hsva_float_t = basic_hsva<float, 1000>;
template <intptr_t H, intptr_t S, intptr_t V, intptr_t A = 1000>
inline constexpr hsva_float_t hsva_float = hsva_float_t::make<H, S, V, A>();

/** @} */

}  // namespace color::core
