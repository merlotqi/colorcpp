/**
 * @file float_utils.hpp
 * @brief Floating-point comparison utilities for color comparison.
 */

#pragma once

#include <cmath>
#include <limits>
#include <type_traits>

namespace colorcpp::operations::compare::detail {

/**
 * @brief Check if two floating-point values are equal within an absolute epsilon.
 *
 * Pure absolute tolerance in the values' own units: @c |a - b| <= eps.
 * The same @c eps means the same thing on every channel scale (a Lab lightness
 * of 0.01 stays 0.01, not 0.01 scaled by the magnitude of L*).
 * NaN compares unequal; a negative tolerance never matches (including for
 * identical values, matching the integer channel path).
 * Arithmetic only (no @c std::abs) so the function stays constexpr in C++17.
 *
 * @tparam T Floating-point type.
 * @param a First value.
 * @param b Second value.
 * @param eps Absolute tolerance (default: epsilon of T).
 * @return True if the values are within eps of each other.
 */
template <typename T>
constexpr bool float_equal(T a, T b, T eps = std::numeric_limits<T>::epsilon()) noexcept {
  static_assert(std::is_floating_point_v<T>, "T must be a floating-point type");

  // Branchless abs so NaN propagates (both comparisons false -> NaN diff).
  const T diff = (a > b) ? (a - b) : (b - a);
  return diff <= eps;
}

/**
 * @brief Check if two floating-point values are equal within relative tolerance
 *        with an absolute floor (isclose-style: @c diff <= max(rel * max(|a|,|b|), abs_floor)).
 *
 * Pure relative tolerance fails near zero (rel * max cannot cover a fixed-size
 * difference when both magnitudes are tiny); @p abs_floor supplies that missing
 * absolute half. Arithmetic only (no @c std::abs) so the function stays constexpr
 * in C++17. NaN compares unequal; a negative tolerance or floor never matches
 * (including for identical values), matching @ref float_equal.
 *
 * @tparam T Floating-point type.
 * @param a First value.
 * @param b Second value.
 * @param rel Relative tolerance (as fraction of the larger magnitude).
 * @param abs_floor Absolute tolerance that dominates when larger (default 0: pure relative).
 * @return True if values are within the combined tolerance.
 */
template <typename T>
constexpr bool float_relative(T a, T b, T rel = static_cast<T>(1e-5), T abs_floor = T(0)) noexcept {
  static_assert(std::is_floating_point_v<T>, "T must be a floating-point type");

  // Negative rel or floor never matches, including for identical values —
  // consistent with float_equal's `diff <= eps` on a negative eps.
  if (rel < T(0) || abs_floor < T(0)) return false;

  // Branchless abs so NaN propagates (both comparisons false -> NaN diff).
  const T diff = (a > b) ? (a - b) : (b - a);
  const T abs_a = (a > T(0)) ? a : -a;
  const T abs_b = (b > T(0)) ? b : -b;
  const T max_val = (abs_a > abs_b) ? abs_a : abs_b;

  const T rel_bound = rel * max_val;
  const T tol = (rel_bound > abs_floor) ? rel_bound : abs_floor;
  return diff <= tol;
}

/**
 * @brief Check if a floating-point value is approximately zero.
 * @tparam T Floating-point type.
 * @param value Value to check.
 * @param eps Epsilon tolerance.
 * @return True if value is approximately zero.
 */
template <typename T>
constexpr bool is_near_zero(T value, T eps = std::numeric_limits<T>::epsilon()) noexcept {
  static_assert(std::is_floating_point_v<T>, "T must be a floating-point type");
  return std::abs(value) <= eps;
}

}  // namespace colorcpp::operations::compare::detail
