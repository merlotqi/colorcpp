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

#if defined(__has_include)
#if __has_include(<version>)
#include <version>
#endif
#endif

#include <cassert>
#include <cstdint>
#include <ratio>
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
template <typename T, typename ScaleH = std::ratio<1, 360>, typename ScaleSVA = std::ratio<1, 100>>
struct basic_hsva {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");
  static constexpr T h_full = static_cast<T>(ScaleH::den) / static_cast<T>(ScaleH::num);
  static constexpr T sva_full = static_cast<T>(ScaleSVA::den) / static_cast<T>(ScaleSVA::num);

  using value_type = T;

  T h, s, v, a;

  constexpr basic_hsva() : h(0), s(0), v(0), a(sva_full) {}

  constexpr basic_hsva(T hue, T saturation, T value, T alpha) : h(hue), s(saturation), v(value), a(alpha) {
    if (!is_valid_val(hue, saturation, value, alpha)) {
      assert(false && "HSVA values out of range!");
    }
  }

  template <intptr_t H_raw, intptr_t S_raw, intptr_t V_raw, intptr_t A_raw = sva_full>
  static constexpr basic_hsva make() {
    constexpr T static_h = static_cast<T>(H_raw);
    constexpr T static_s = static_cast<T>(S_raw);
    constexpr T static_v = static_cast<T>(V_raw);
    constexpr T static_a = static_cast<T>(A_raw);
    static_assert(is_valid_val(static_h, static_s, static_v, static_a), "HSVA value out of range!");
    return {static_h, static_s, static_v, static_a};
  }

  static constexpr bool is_valid_val(T hv, T sv, T vv, T av) {
    return (hv >= 0 && hv < h_full) && (sv >= 0 && sv <= sva_full) && (vv >= 0 && vv <= sva_full) &&
           (av >= 0 && av <= sva_full);
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
 * Alpha channel defaults to 100 (fully opaque).
 *
 * @tparam H Hue component (0-359)
 * @tparam S Saturation component (0-100)
 * @tparam V Value component (0-100)
 * @tparam A Alpha component (0-100, default 100)
 */
using hsva_int_t = basic_hsva<int, std::ratio<1, 360>, std::ratio<1, 100>>;
template <int H, int S, int V, int A = 100>
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
using hsva_float_t = basic_hsva<float, std::ratio<1>, std::ratio<1>>;
#if defined(__cpp_nontype_template_args) && __cpp_nontype_template_args >= 201907L
template <float H, float S, float V, float A = 1.0f>
inline constexpr hsva_float_t hsva_float = hsva_float_t(H, S, V, A);
#else
template <int H_deg, int S_per, int V_per, int A_per = 100>
inline constexpr hsva_float_t hsva_float =
    hsva_float_t(static_cast<float>(H_deg) / 360.0f, static_cast<float>(S_per) / 100.0f,
                 static_cast<float>(V_per) / 100.0f, static_cast<float>(A_per) / 100.0f);
#endif

/** @} */

}  // namespace color::core
