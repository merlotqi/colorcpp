/**
 * @file relative.hpp
 * @brief Relative color comparison policy.
 */

#pragma once

#include <colorcpp/operations/compare/detail/channel_compare.hpp>

namespace colorcpp::operations::compare {

/**
 * @brief Relative comparison policy with an absolute floor (isclose-style).
 *
 * Compares colors channel-by-channel within
 * @c max(rel * max(|a|, |b|), abs_floor). The tolerance parameters are
 * floating-point regardless of the channel type, so integer channels can
 * express sub-100% ratios (0.02f is a real 2%, not a truncated zero).
 * Hue channels compare on the ring: the circular distance (in degrees) is
 * tested against the same combined bound. At zero hue magnitude the relative
 * part contributes nothing; near-zero hue differences pass via @p abs_floor.
 * A negative @p rel or @p abs_floor never matches, including for identical
 * values. Models whose chroma is zero on both sides ignore hue entirely.
 */
struct relative_policy {
  /**
   * @brief Compare two colors within relative tolerance.
   * @tparam Color Color type.
   * @param a First color.
   * @param b Second color.
   * @param rel Relative tolerance (default: 1e-5).
   * @param abs_floor Absolute tolerance that dominates when larger (default: 0).
   * @return True if all channels are within the combined tolerance.
   */
  template <typename Color>
  static constexpr bool compare(const Color& a, const Color& b, float rel = 1e-5f,
                                float abs_floor = 0.0f) noexcept {
    using T = typename Color::value_type;
    // Negative tolerance (either half) never matches any channel, hue included.
    if (rel < 0.0f || abs_floor < 0.0f) return false;
    return detail::compare_channels(
        a, b,
        [rel, abs_floor](T va, T vb) { return detail::channel_relative(va, vb, rel, abs_floor); },
        [rel, abs_floor](T va, T vb, T d) {
          const T abs_a = (va > T(0)) ? va : -va;
          const T abs_b = (vb > T(0)) ? vb : -vb;
          const T max_val = (abs_a > abs_b) ? abs_a : abs_b;
          const double rel_bound = static_cast<double>(rel) * static_cast<double>(max_val);
          const double tol = (rel_bound > static_cast<double>(abs_floor)) ? rel_bound
                                                                          : static_cast<double>(abs_floor);
          return static_cast<double>(d) <= tol;
        });
  }
};

/**
 * @brief Compare two colors within relative tolerance with an absolute floor.
 * @tparam Color Color type.
 * @param a First color.
 * @param b Second color.
 * @param rel Relative tolerance (as a real ratio — works for integer channels).
 * @param abs_floor Absolute tolerance that dominates when larger (default: 0).
 * @return True if all channels are within the combined tolerance.
 */
template <typename Color>
constexpr bool relative_equal(const Color& a, const Color& b, float rel = 1e-5f,
                              float abs_floor = 0.0f) noexcept {
  return relative_policy::compare(a, b, rel, abs_floor);
}

}  // namespace colorcpp::operations::compare
