/**
 * @file cmyk.hpp
 * @brief CMYK color space core implementation
 *
 * Provides basic CMYK color template class and common color aliases.
 * Supports integer and floating-point CMYK representations.
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
 * @brief Basic CMYK color template class
 *
 * Template class for representing CMYK colors with compile-time validation.
 * Supports both integer (0-100) and floating-point (0.0-1.0) value ranges.
 *
 * @tparam T Value type (must be arithmetic)
 * @tparam Scale Scaling factor for value conversion
 */
template <typename T, intptr_t Scale = 1>
struct basic_cmyk {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

  using value_type = T;

  T c, m, y, k;

  constexpr basic_cmyk() : c(0), m(0), y(0), k(0) {}

  constexpr basic_cmyk(T cyan, T magenta, T yellow, T key) : c(cyan), m(magenta), y(yellow), k(key) {
    if (!is_valid_val(cyan, magenta, yellow, key)) {
      assert(false && "CMYK values out of range!");
    }
  }

  template <intptr_t C_raw, intptr_t M_raw, intptr_t Y_raw, intptr_t K_raw>
  static constexpr basic_cmyk make() {
    constexpr T static_c = static_cast<T>(C_raw) / static_cast<T>(Scale);
    constexpr T static_m = static_cast<T>(M_raw) / static_cast<T>(Scale);
    constexpr T static_y = static_cast<T>(Y_raw) / static_cast<T>(Scale);
    constexpr T static_k = static_cast<T>(K_raw) / static_cast<T>(Scale);
    static_assert(is_valid_val(static_c, static_m, static_y, static_k), "CMYK value out of range!");
    return {static_c, static_m, static_y, static_k};
  }

  static constexpr bool is_valid_val(T cv, T mv, T yv, T kv) {
    if constexpr (std::is_floating_point_v<T>) {
      return cv >= 0.0 && cv <= 1.0 && mv >= 0.0 && mv <= 1.0 && yv >= 0.0 && yv <= 1.0 && kv >= 0.0 && kv <= 1.0;
    } else {
      return cv >= 0 && cv <= 100 && mv >= 0 && mv <= 100 && yv >= 0 && yv <= 100 && kv >= 0 && kv <= 100;
    }
  }

  constexpr bool is_valid() const { return is_valid_val(c, m, y, k); }
};

/**
 * @name CMYK Color Type Aliases
 * @{
 */

/**
 * @brief Integer CMYK color type
 *
 * Represents CMYK colors using integer values (0-100 range).
 *
 * @tparam C Cyan component (0-100)
 * @tparam M Magenta component (0-100)
 * @tparam Y Yellow component (0-100)
 * @tparam K Key (black) component (0-100)
 */
using cmyk_int_t = basic_cmyk<int, 1>;
template <int C, int M, int Y, int K>
inline constexpr cmyk_int_t cmyk_int = cmyk_int_t::make<C, M, Y, K>();

/**
 * @brief Floating-point CMYK color type
 *
 * Represents CMYK colors using float precision floating-point values (0.0-1.0 range).
 * Scale is set to 1000, mapping integer inputs to floating-point range.
 *
 * @tparam C Cyan component (scaled from integer to 0.0-1.0)
 * @tparam M Magenta component (scaled from integer to 0.0-1.0)
 * @tparam Y Yellow component (scaled from integer to 0.0-1.0)
 * @tparam K Key (black) component (scaled from integer to 0.0-1.0)
 */
using cmyk_float_t = basic_cmyk<float, 1000>;
template <int C, int M, int Y, int K>
inline constexpr cmyk_float_t cmyk_float = cmyk_float_t::make<C, M, Y, K>();

/** @} */

}  // namespace color::core
