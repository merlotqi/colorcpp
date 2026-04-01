/**
 * @file exact.hpp
 * @brief Exact color comparison policy.
 */

#pragma once

#include <colorcpp/operations/compare/detail/channel_compare.hpp>

namespace colorcpp::operations::compare {

/**
 * @brief Exact comparison policy.
 *
 * Compares colors channel-by-channel using exact equality.
 * This is the default comparison strategy.
 */
struct exact_policy {
  /**
   * @brief Compare two colors exactly.
   * @tparam Color Color type.
   * @param a First color.
   * @param b Second color.
   * @return True if all channels are exactly equal.
   */
  template <typename Color>
  static constexpr bool compare(const Color& a, const Color& b) noexcept {
    return detail::compare_channels(a, b, detail::channel_exact<typename Color::value_type>);
  }
};

/**
 * @brief Compare two colors exactly.
 * @tparam Color Color type.
 * @param a First color.
 * @param b Second color.
 * @return True if all channels are exactly equal.
 */
template <typename Color>
constexpr bool exact_equal(const Color& a, const Color& b) noexcept {
  return exact_policy::compare(a, b);
}

}  // namespace colorcpp::operations::compare
