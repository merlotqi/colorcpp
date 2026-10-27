/**
 * @file relative.hpp
 * @brief Relative color comparison policy.
 */

#pragma once

#include <colorcpp/operations/compare/detail/channel_compare.hpp>

namespace colorcpp::operations::compare {

/**
 * @brief Relative comparison policy.
 *
 * Compares colors channel-by-channel within a relative tolerance.
 * The tolerance is relative to the larger of the two values being compared.
 * Hue channels compare on the ring: the circular distance (in degrees) is
 * tested against @c rel * max(|a|, |b|), so 0 and 360 are the same hue.
 */
struct relative_policy {
  /**
   * @brief Compare two colors within relative tolerance.
   * @tparam Color Color type.
   * @param a First color.
   * @param b Second color.
   * @param rel Relative tolerance (default: 1e-5).
   * @return True if all channels are within relative tolerance.
   */
  template <typename Color>
  static constexpr bool compare(
      const Color& a, const Color& b,
      typename Color::value_type rel = static_cast<typename Color::value_type>(1e-5)) noexcept {
    using T = typename Color::value_type;
    return detail::compare_channels(
        a, b, [rel](T va, T vb) { return detail::channel_relative(va, vb, rel); },
        [rel](T va, T vb, T d) {
          const T abs_a = (va > T(0)) ? va : -va;
          const T abs_b = (vb > T(0)) ? vb : -vb;
          const T max_val = (abs_a > abs_b) ? abs_a : abs_b;
          return d <= rel * max_val;
        });
  }
};

/**
 * @brief Compare two colors within relative tolerance.
 * @tparam Color Color type.
 * @param a First color.
 * @param b Second color.
 * @param rel Relative tolerance.
 * @return True if all channels are within relative tolerance.
 */
template <typename Color>
constexpr bool relative_equal(const Color& a, const Color& b,
                              typename Color::value_type rel = static_cast<typename Color::value_type>(1e-5)) noexcept {
  return relative_policy::compare(a, b, rel);
}

}  // namespace colorcpp::operations::compare
