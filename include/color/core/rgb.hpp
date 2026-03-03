/**
 * @file rgb.hpp
 * @brief RGBA color space core implementation
 *
 * Provides basic RGBA color template class and common color aliases.
 * Supports 8-bit integer RGBA and floating-point RGBA representations.
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
 * @brief Basic RGBA color template class
 *
 * Template class for representing RGBA colors with compile-time validation.
 * Supports both integer (0-255) and floating-point (0.0-1.0) value ranges.
 *
 * @tparam T Value type (must be arithmetic)
 * @tparam Scale Scaling factor for value conversion
 */
template <typename T, intptr_t Scale = 1>
struct basic_rgba {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

  using value_type = T;

  T r, g, b, a;

  constexpr basic_rgba() : r(0), g(0), b(1) {}

  constexpr basic_rgba(T red, T green, T blue, T alpha) : r(red), g(green), b(blue), a(alpha) {
    if (!is_valid_val(red, green, blue, alpha)) {
      assert(false && "RGBA values out of range!");
    }
  }

  template <intptr_t R_raw, intptr_t G_raw, intptr_t B_raw, intptr_t A_raw = Scale>
  static constexpr basic_rgba make() {
    constexpr T static_r = static_cast<T>(R_raw) / static_cast<T>(Scale);
    constexpr T static_g = static_cast<T>(G_raw) / static_cast<T>(Scale);
    constexpr T static_b = static_cast<T>(B_raw) / static_cast<T>(Scale);
    constexpr T static_a = static_cast<T>(A_raw) / static_cast<T>(Scale);
    static_assert(is_valid_val(static_r, static_g, static_b, static_a), "RGBA value out of range!");
    return {static_r, static_g, static_b, static_a};
  }

  static constexpr bool is_valid_val(T rv, T gv, T bv, T av) {
    if constexpr (std::is_floating_point_v<T>) {
      return rv >= 0.0 && rv <= 1.0 && gv >= 0.0 && gv <= 1.0 && bv >= 0.0 && bv <= 1.0 && av >= 0.0 && av <= 1.0;
    } else {
      return rv >= 0 && rv <= static_cast<T>(Scale) && gv >= 0 && gv <= static_cast<T>(Scale) && bv >= 0 &&
             bv <= static_cast<T>(Scale) && av >= 0 && av <= static_cast<T>(Scale);
    }
  }

  constexpr bool is_valid() const { return is_valid_val(r, g, b, a); }
};

/**
 * @name RGBA Color Type Aliases
 * @{
 */

/**
 * @brief 8-bit integer RGBA color type
 *
 * Represents RGBA colors using 8-bit integer values (0-255 range).
 * Scale is set to 1, so values are stored directly.
 * Alpha channel defaults to 255 (fully opaque).
 *
 * @tparam R Red component (0-255)
 * @tparam G Green component (0-255)
 * @tparam B Blue component (0-255)
 * @tparam A Alpha component (0-255, default 255)
 */
using rgba8_t = basic_rgba<uint8_t, 255>;
template <uint8_t R, uint8_t G, uint8_t B, uint8_t A = 255>
inline constexpr rgba8_t rgba8 = rgba8_t::make<R, G, B, A>();

/**
 * @brief Floating-point RGBA color type
 *
 * Represents RGBA colors using float precision floating-point values (0.0-1.0 range).
 * Scale is set to 1000, mapping integer inputs to floating-point range.
 * Alpha channel defaults to 1.0 (fully opaque).
 *
 * @tparam R Red component (scaled from integer to 0.0-1.0)
 * @tparam G Green component (scaled from integer to 0.0-1.0)
 * @tparam B Blue component (scaled from integer to 0.0-1.0)
 * @tparam A Alpha component (scaled from integer to 0.0-1.0, default 1.0)
 */
using rgbaf_t = basic_rgba<float, 1000>;
template <intptr_t R, intptr_t G, intptr_t B, intptr_t A = 1000>
inline constexpr rgbaf_t rgbaf = rgbaf_t::make<R, G, B, A>();

/**
 * @brief Percentage-based RGBA color type
 *
 * Represents RGBA colors using integer percentage values (0-100 range).
 * Alpha channel defaults to 100% (fully opaque).
 *
 * @tparam R Red component percentage (0-100)
 * @tparam G Green component percentage (0-100)
 * @tparam B Blue component percentage (0-100)
 * @tparam A Alpha component percentage (0-100, default 100)
 */
using rgba_percent_t = basic_rgba<int, 100>;
template <intptr_t R, intptr_t G, intptr_t B, intptr_t A = 100>
inline constexpr rgba_percent_t rgbaper = rgba_percent_t::make<R, G, B, A>();

/** @} */

}  // namespace color::core
