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
 * tested against @c max(rel * span, abs_floor), where @c span is the hue
 * ring's own range (360 for float hues) — not the raw hue magnitudes, so
 * the same gap gets the same verdict at every ring position (issue #89).
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
  static constexpr bool compare(const Color& a, const Color& b, float rel = 1e-5f, float abs_floor = 0.0f) noexcept {
    using T = typename Color::value_type;
    // Negative tolerance (either half) never matches any channel, hue included.
    if (rel < 0.0f || abs_floor < 0.0f) return false;
    return detail::compare_channels(
        a, b, [rel, abs_floor](T va, T vb) { return detail::channel_relative(va, vb, rel, abs_floor); },
        [rel, abs_floor](T /*va*/, T /*vb*/, T d, T range) {
          // Issue #89: scale by the ring span, not the raw hue magnitudes —
          // hue position on the ring must not change the verdict.
          const double rel_bound = static_cast<double>(rel) * static_cast<double>(range);
          const double tol = (rel_bound > static_cast<double>(abs_floor)) ? rel_bound : static_cast<double>(abs_floor);
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
constexpr bool relative_equal(const Color& a, const Color& b, float rel = 1e-5f, float abs_floor = 0.0f) noexcept {
  return relative_policy::compare(a, b, rel, abs_floor);
}

}  // namespace colorcpp::operations::compare
