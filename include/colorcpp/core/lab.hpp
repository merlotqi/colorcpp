#pragma once

#include <cassert>
#include <colorcpp/core/color_base.hpp>
#include <colorcpp/traits/concepts.hpp>
#include <ratio>
#include <type_traits>

namespace color::core {

/**
 * @brief Basic CIELAB color template class
 *
 * Template class for representing CIELAB colors with compile-time validation.
 * Supports both integer and floating-point value ranges with separate scaling
 * for L* (lightness) and a* / b* (chromaticity) components.
 *
 * @tparam T Value type (must be arithmetic)
 * @tparam ScaleL Scaling factor for L* component (lightness, 0-100)
 * @tparam ScaleAB Scaling factor for a* and b* components (chromaticity, -128 to +127)
 * @tparam OffsetAB Offset for a* and b* components to handle negative values
 */
template <typename T, typename ScaleL = std::ratio<1, 100>, typename ScaleAB = std::ratio<1, 255>>
struct basic_lab : color_base<T, ScaleL, category::lab> {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

  static constexpr T max_l = static_cast<T>(ScaleL::den) / static_cast<T>(ScaleL::num);
  static constexpr T max_ab = static_cast<T>(ScaleAB::den) / static_cast<T>(ScaleAB::num);
  static constexpr T offset_ab = max_ab / T(2);

  T l, a, b;

  constexpr basic_lab() : l(0), a(offset_ab), b(offset_ab) {}

  constexpr basic_lab(T lightness, T chroma_a, T chroma_b) : l(lightness), a(chroma_a), b(chroma_b) {
    if (!is_valid_val(lightness, chroma_a, chroma_b)) {
      assert(false && "LAB values out of range!");
    }
  }

  template <long long L_raw, long long A_raw, long long B_raw>
  static constexpr basic_lab make() {
    constexpr T static_l = static_cast<T>(L_raw);
    constexpr T static_a = static_cast<T>(A_raw);
    constexpr T static_b = static_cast<T>(B_raw);

    static_assert(is_valid_val(static_l, static_a, static_b), "LAB value out of range!");

    return {static_l, static_a, static_b};
  }

  static constexpr bool is_valid_val(T lv, T av, T bv) {
    if (lv < T(0) || lv > max_l) return false;

    if (av < T(0) || av > max_ab) return false;
    if (bv < T(0) || bv > max_ab) return false;

    return true;
  }

  constexpr bool is_valid() const { return is_valid_val(l, a, b); }
};

/**
 * @name LAB Color Type Aliases
 * @{
 */

/**
 * @brief Integer LAB color type
 *
 * Represents LAB colors using integer values with separate scaling for
 * L* (lightness) and a* / b* (chromaticity) components.
 * L* component ranges from 0-100, a* / b* components range from 0-255
 * with 128 as the neutral point (representing -128 to +127 in actual LAB space).
 *
 * @tparam L Lightness component (0-100)
 * @tparam A a* chromaticity component (0-255, representing -128 to +127)
 * @tparam B b* chromaticity component (0-255, representing -128 to +127)
 */
using lab_int_t = basic_lab<int>;
template <int L, int A, int B>
inline constexpr lab_int_t lab_int = lab_int_t::make<L, A, B>();
inline lab_int_t make_lab_from_raw(int l_raw, int a_raw, int b_raw) {
  static_assert(lab_int_t::max_ab > 0, "max_ab must be positive");
  constexpr int offset = static_cast<int>(lab_int_t::max_ab / 2);
  int stored_l = l_raw;
  int stored_a = a_raw + offset;
  int stored_b = b_raw + offset;
  return lab_int_t(static_cast<int>(stored_l), static_cast<int>(stored_a), static_cast<int>(stored_b));
}

/**
 * @brief Floating-point LAB color type
 *
 * Represents LAB colors using float precision floating-point values.
 * L* component ranges from 0.0-100.0, a* / b* components range from -128.0 to +127.0.
 *
 * @tparam L Lightness component (0.0-100.0)
 * @tparam A a* chromaticity component (-128.0 to +127.0)
 * @tparam B b* chromaticity component (-128.0 to +127.0)
 */
using lab_float_t = basic_lab<float>;
template <int L, int A, int B>
inline constexpr lab_float_t labf = lab_float_t::make<L, A + 128, B + 128>();
inline lab_float_t make_labf(float l, float a, float b) {
  constexpr float offset = 128.0f;
  return lab_float_t(l, a + offset, b + offset);
}

/** @} */

}  // namespace color::core
