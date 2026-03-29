/**
 * @file channel_compare.hpp
 * @brief Channel-wise comparison utilities for color comparison.
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/operations/compare/detail/float_utils.hpp>
#include <type_traits>

namespace colorcpp::operations::compare::detail {

/**
 * @brief Compare two values exactly.
 * @tparam T Value type.
 * @param a First value.
 * @param b Second value.
 * @return True if values are exactly equal.
 */
template <typename T>
constexpr bool channel_exact(T a, T b) noexcept {
  return a == b;
}

/**
 * @brief Compare two values within epsilon tolerance.
 * @tparam T Value type.
 * @param a First value.
 * @param b Second value.
 * @param eps Epsilon tolerance.
 * @return True if values are within epsilon.
 */
template <typename T>
constexpr bool channel_epsilon(T a, T b, T eps) noexcept {
  if constexpr (std::is_floating_point_v<T>) {
    return float_equal(a, b, eps);
  } else {
    // For integer types, use exact comparison with tolerance
    const T diff = (a > b) ? (a - b) : (b - a);
    return diff <= eps;
  }
}

/**
 * @brief Compare two values within relative tolerance.
 * @tparam T Value type.
 * @param a First value.
 * @param b Second value.
 * @param rel Relative tolerance.
 * @return True if values are within relative tolerance.
 */
template <typename T>
constexpr bool channel_relative(T a, T b, T rel) noexcept {
  if constexpr (std::is_floating_point_v<T>) {
    return float_relative(a, b, rel);
  } else {
    // For integer types, convert to float for relative comparison
    const double diff = static_cast<double>(a > b ? a - b : b - a);
    const double max_val = static_cast<double>(a > b ? a : b);
    return diff <= rel * max_val;
  }
}

/**
 * @brief Helper to compare all channels of two colors.
 * @tparam CompareFunc Comparison function type.
 * @tparam Color Color type.
 * @tparam Is Index sequence.
 * @param a First color.
 * @param b Second color.
 * @param comp Comparison function.
 * @return True if all channels match according to comparison function.
 */
template <typename CompareFunc, typename Color, std::size_t... Is>
constexpr bool compare_channels_impl(const Color& a, const Color& b, CompareFunc comp,
                                     std::index_sequence<Is...>) noexcept {
  return (comp(a.template get_index<Is>(), b.template get_index<Is>()) && ...);
}

/**
 * @brief Compare all channels of two colors.
 * @tparam CompareFunc Comparison function type.
 * @tparam Color Color type.
 * @param a First color.
 * @param b Second color.
 * @param comp Comparison function.
 * @return True if all channels match according to comparison function.
 */
template <typename CompareFunc, typename Color>
constexpr bool compare_channels(const Color& a, const Color& b, CompareFunc comp) noexcept {
  return compare_channels_impl(a, b, comp, std::make_index_sequence<Color::channels>{});
}

}  // namespace colorcpp::operations::compare::detail
