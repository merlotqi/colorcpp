/**
 * @file maths.hpp
 * @brief Mathematical utility functions
 *
 * Provides compile-time mathematical utility functions used throughout
 * the color library. Includes absolute value, floating-point modulo,
 * and rounding operations optimized for constexpr evaluation.
 *
 * @author Merlot.Qi
 *
 */

#pragma once

namespace color {
namespace maths {

/**
 * @brief Compile-time absolute value function
 *
 * Calculates the absolute value of a number at compile time.
 * Works with any arithmetic type.
 *
 * @tparam T Arithmetic type
 * @param v Input value
 * @return Absolute value of the input
 */
template <typename T>
constexpr T abs(T v) noexcept {
  return v < 0 ? -v : v;
}

/**
 * @brief Compile-time floating-point modulo function
 *
 * Calculates the floating-point modulo of two numbers at compile time.
 * Handles division by zero by returning 0.0.
 *
 * @param x Dividend
 * @param y Divisor
 * @return Floating-point modulo result (x - floor(x/y) * y)
 */
constexpr float fmod(float x, float y) noexcept {
  if (y == 0.0) return 0.0;
  return x - static_cast<long long>(x / y) * y;
}

/**
 * @brief Compile-time rounding function
 *
 * Rounds a floating-point number to the nearest integer at compile time.
 * Supports custom return types and handles both positive and negative values.
 *
 * @tparam Ret Return type (defaults to int)
 * @tparam T Input type
 * @param v Value to round
 * @return Rounded value converted to the specified return type
 */
template <typename Ret = int, typename T>
constexpr Ret round(T v) noexcept {
  return static_cast<Ret>(v >= 0 ? v + 0.5 : v - 0.5);
}

}  // namespace maths
}  // namespace color
