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
 * @brief Check if two floating-point values are equal within relative tolerance.
 * @tparam T Floating-point type.
 * @param a First value.
 * @param b Second value.
 * @param rel Relative tolerance (as fraction of the larger value).
 * @return True if values are within relative tolerance.
 */
template <typename T>
constexpr bool float_relative(T a, T b, T rel = static_cast<T>(1e-5)) noexcept {
  static_assert(std::is_floating_point_v<T>, "T must be a floating-point type");

  if (a == b) return true;

  const T diff = std::abs(a - b);
  const T abs_a = std::abs(a);
  const T abs_b = std::abs(b);
  const T max_val = (abs_a > abs_b) ? abs_a : abs_b;

  return diff <= rel * max_val;
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
