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
#include <colorcpp/core/color_base.hpp>
#include <colorcpp/traits/concepts.hpp>
#include <ratio>

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
template <typename T, typename Scale = std::ratio<1>>
struct basic_cmyk : public color_base<T, Scale, category::cmyk> {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");
  static constexpr T full_range = static_cast<T>(Scale::den) / static_cast<T>(Scale::num);

  using value_type = T;
  using scale_type = Scale;
  using color_tag = category::cmyk;

  T c, m, y, k;

  constexpr basic_cmyk() : c(0), m(0), y(0), k(0) {}

  constexpr basic_cmyk(T cyan, T magenta, T yellow, T key) : c(cyan), m(magenta), y(yellow), k(key) {
    if (!is_valid_val(cyan, magenta, yellow, key)) {
      assert(false && "CMYK values out of range!");
    }
  }

  template <long long C_raw, long long M_raw, long long Y_raw, long long K_raw>
  static constexpr basic_cmyk make() {
    constexpr T static_c = static_cast<T>(C_raw);
    constexpr T static_m = static_cast<T>(M_raw);
    constexpr T static_y = static_cast<T>(Y_raw);
    constexpr T static_k = static_cast<T>(K_raw);
    static_assert(is_valid_val(static_c, static_m, static_y, static_k), "CMYK value out of range!");
    return {static_c, static_m, static_y, static_k};
  }

  static constexpr bool is_valid_val(T cv, T mv, T yv, T kv) {
    auto in_range = [](T v) { return v >= T(0) && v <= full_range; };
    return in_range(cv) && in_range(mv) && in_range(yv) && in_range(kv);
  }

  constexpr bool is_valid() const { return is_valid_val(c, m, y, k); }

  constexpr bool is_printable() const { return (c + m + y + k) <= (full_range * 3); }
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
using cmyk_int_t = basic_cmyk<int, std::ratio<1, 100>>;
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
using cmyk_float_t = basic_cmyk<float, std::ratio<1>>;
template <int R100, int G100, int B100, int A100 = 1000>
inline constexpr cmyk_float_t cmykf =
    cmyk_float_t(static_cast<float>(R100) / 100.0f, static_cast<float>(G100) / 100.0f,
                 static_cast<float>(B100) / 100.0f, static_cast<float>(A100) / 100.0f);

/** @} */

}  // namespace color::core
