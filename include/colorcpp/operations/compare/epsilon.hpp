/**
 * @file epsilon.hpp
 * @brief Epsilon-based color comparison policy.
 */

#pragma once

#include <colorcpp/operations/compare/detail/channel_compare.hpp>
#include <limits>

namespace colorcpp::operations::compare {

/**
 * @brief Epsilon comparison policy.
 *
 * Compares colors channel-by-channel within a fixed epsilon tolerance.
 */
struct epsilon_policy {
  /**
   * @brief Compare two colors within epsilon tolerance.
   * @tparam Color Color type.
   * @param a First color.
   * @param b Second color.
   * @param eps Epsilon tolerance (default: epsilon of value_type).
   * @return True if all channels are within epsilon.
   */
  template <typename Color>
  static constexpr bool compare(
      const Color& a, const Color& b,
      typename Color::value_type eps = std::numeric_limits<typename Color::value_type>::epsilon()) noexcept {
    return detail::compare_channels(a, b, [eps](auto va, auto vb) { return detail::channel_epsilon(va, vb, eps); });
  }
};

/**
 * @brief Compare two colors within epsilon tolerance.
 * @tparam Color Color type.
 * @param a First color.
 * @param b Second color.
 * @param eps Epsilon tolerance.
 * @return True if all channels are within epsilon.
 */
template <typename Color>
constexpr bool epsilon_equal(
    const Color& a, const Color& b,
    typename Color::value_type eps = std::numeric_limits<typename Color::value_type>::epsilon()) noexcept {
  return epsilon_policy::compare(a, b, eps);
}

}  // namespace colorcpp::operations::compare
