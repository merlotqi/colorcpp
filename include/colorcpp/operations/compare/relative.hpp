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
    return detail::compare_channels(a, b, [rel](auto va, auto vb) { return detail::channel_relative(va, vb, rel); });
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
