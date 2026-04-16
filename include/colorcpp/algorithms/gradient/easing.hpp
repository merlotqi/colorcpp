/**
 * @file easing.hpp
 * @brief Easing functions for gradient interpolation control.
 *
 * Easing functions control the rate of change along a gradient,
 * allowing for non-linear interpolation between color stops.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <functional>

namespace colorcpp::algorithms::gradient::easing {

/**
 * @brief Easing function type.
 * @tparam T Value type (typically float).
 */
template <typename T = float>
using easing_function = std::function<T(T)>;

/**
 * @brief Linear easing (no easing).
 * @tparam T Value type.
 * @param t Input value in [0, 1].
 * @return Output value in [0, 1].
 */
template <typename T = float>
constexpr T linear(T t) noexcept {
  return t;
}

/**
 * @brief Ease in (accelerating from zero velocity).
 * @tparam T Value type.
 * @param t Input value in [0, 1].
 * @return Output value in [0, 1].
 */
template <typename T = float>
constexpr T ease_in(T t) noexcept {
  return t * t;
}

/**
 * @brief Ease out (decelerating to zero velocity).
 * @tparam T Value type.
 * @param t Input value in [0, 1].
 * @return Output value in [0, 1].
 */
template <typename T = float>
constexpr T ease_out(T t) noexcept {
  return t * (2.0f - t);
}

/**
 * @brief Ease in-out (accelerating until halfway, then decelerating).
 * @tparam T Value type.
 * @param t Input value in [0, 1].
 * @return Output value in [0, 1].
 */
template <typename T = float>
constexpr T ease_in_out(T t) noexcept {
  if (t < 0.5f) {
    return 2.0f * t * t;
  } else {
    return -1.0f + (4.0f - 2.0f * t) * t;
  }
}

/**
 * @brief Smooth step easing (Hermite interpolation).
 * @tparam T Value type.
 * @param t Input value in [0, 1].
 * @return Output value in [0, 1].
 */
template <typename T = float>
constexpr T smooth_step(T t) noexcept {
  return t * t * (3.0f - 2.0f * t);
}

/**
 * @brief Smoother step easing (Ken Perlin's improvement).
 * @tparam T Value type.
 * @param t Input value in [0, 1].
 * @return Output value in [0, 1].
 */
template <typename T = float>
constexpr T smoother_step(T t) noexcept {
  return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

/**
 * @brief Sine easing in.
 * @tparam T Value type.
 * @param t Input value in [0, 1].
 * @return Output value in [0, 1].
 */
template <typename T = float>
T ease_in_sine(T t) noexcept {
  static constexpr T pi = static_cast<T>(3.14159265358979323846);
  return 1.0f - std::cos(t * pi / 2.0f);
}

/**
 * @brief Sine easing out.
 * @tparam T Value type.
 * @param t Input value in [0, 1].
 * @return Output value in [0, 1].
 */
template <typename T = float>
T ease_out_sine(T t) noexcept {
  static constexpr T pi = static_cast<T>(3.14159265358979323846);
  return std::sin(t * pi / 2.0f);
}

/**
 * @brief Sine easing in-out.
 * @tparam T Value type.
 * @param t Input value in [0, 1].
 * @return Output value in [0, 1].
 */
template <typename T = float>
T ease_in_out_sine(T t) noexcept {
  static constexpr T pi = static_cast<T>(3.14159265358979323846);
  return -(std::cos(pi * t) - 1.0f) / 2.0f;
}

/**
 * @brief Cubic easing in.
 * @tparam T Value type.
 * @param t Input value in [0, 1].
 * @return Output value in [0, 1].
 */
template <typename T = float>
constexpr T ease_in_cubic(T t) noexcept {
  return t * t * t;
}

/**
 * @brief Cubic easing out.
 * @tparam T Value type.
 * @param t Input value in [0, 1].
 * @return Output value in [0, 1].
 */
template <typename T = float>
constexpr T ease_out_cubic(T t) noexcept {
  T t1 = t - 1.0f;
  return t1 * t1 * t1 + 1.0f;
}

/**
 * @brief Cubic easing in-out.
 * @tparam T Value type.
 * @param t Input value in [0, 1].
 * @return Output value in [0, 1].
 */
template <typename T = float>
constexpr T ease_in_out_cubic(T t) noexcept {
  if (t < 0.5f) {
    return 4.0f * t * t * t;
  } else {
    T t1 = 2.0f * t - 2.0f;
    return (t1 * t1 * t1 + 2.0f) / 2.0f;
  }
}

/**
 * @brief Quadratic easing in.
 * @tparam T Value type.
 * @param t Input value in [0, 1].
 * @return Output value in [0, 1].
 */
template <typename T = float>
constexpr T ease_in_quad(T t) noexcept {
  return t * t;
}

/**
 * @brief Quadratic easing out.
 * @tparam T Value type.
 * @param t Input value in [0, 1].
 * @return Output value in [0, 1].
 */
template <typename T = float>
constexpr T ease_out_quad(T t) noexcept {
  return 1.0f - (1.0f - t) * (1.0f - t);
}

/**
 * @brief Quadratic easing in-out.
 * @tparam T Value type.
 * @param t Input value in [0, 1].
 * @return Output value in [0, 1].
 */
template <typename T = float>
constexpr T ease_in_out_quad(T t) noexcept {
  if (t < 0.5f) {
    return 2.0f * t * t;
  } else {
    return 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
  }
}

/**
 * @brief Exponential easing in.
 * @tparam T Value type.
 * @param t Input value in [0, 1].
 * @return Output value in [0, 1].
 */
template <typename T = float>
T ease_in_expo(T t) noexcept {
  return t == 0.0f ? 0.0f : std::pow(2.0f, 10.0f * t - 10.0f);
}

/**
 * @brief Exponential easing out.
 * @tparam T Value type.
 * @param t Input value in [0, 1].
 * @return Output value in [0, 1].
 */
template <typename T = float>
T ease_out_expo(T t) noexcept {
  return t == 1.0f ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t);
}

/**
 * @brief Exponential easing in-out.
 * @tparam T Value type.
 * @param t Input value in [0, 1].
 * @return Output value in [0, 1].
 */
template <typename T = float>
T ease_in_out_expo(T t) noexcept {
  if (t == 0.0f) return 0.0f;
  if (t == 1.0f) return 1.0f;
  if (t < 0.5f) {
    return std::pow(2.0f, 20.0f * t - 10.0f) / 2.0f;
  } else {
    return (2.0f - std::pow(2.0f, -20.0f * t + 10.0f)) / 2.0f;
  }
}

/**
 * @brief Circular easing in.
 * @tparam T Value type.
 * @param t Input value in [0, 1].
 * @return Output value in [0, 1].
 */
template <typename T = float>
T ease_in_circ(T t) noexcept {
  return 1.0f - std::sqrt(1.0f - std::pow(t, 2.0f));
}

/**
 * @brief Circular easing out.
 * @tparam T Value type.
 * @param t Input value in [0, 1].
 * @return Output value in [0, 1].
 */
template <typename T = float>
T ease_out_circ(T t) noexcept {
  T t1 = t - 1.0f;
  return std::sqrt(1.0f - std::pow(t1, 2.0f));
}

/**
 * @brief Circular easing in-out.
 * @tparam T Value type.
 * @param t Input value in [0, 1].
 * @return Output value in [0, 1].
 */
template <typename T = float>
T ease_in_out_circ(T t) noexcept {
  if (t < 0.5f) {
    return (1.0f - std::sqrt(1.0f - std::pow(2.0f * t, 2.0f))) / 2.0f;
  } else {
    T t1 = -2.0f * t + 2.0f;
    return (std::sqrt(1.0f - std::pow(t1, 2.0f)) + 1.0f) / 2.0f;
  }
}

/**
 * @brief Create a custom easing function from a cubic bezier curve.
 *
 * @details
 * Cubic Bezier easing curves are defined by four control points with fixed endpoints:
 *   P0 = (0, 0) - Start point (fixed)
 *   P1 = (x1, y1) - First user control point
 *   P2 = (x2, y2) - Second user control point
 *   P3 = (1, 1) - End point (fixed)
 *
 * This implementation follows the W3C CSS Transitions specification for cubic-bezier timing functions.
 * The algorithm solves the inverse problem: given an x coordinate (input progress), find
 * the corresponding y coordinate (eased progress). There is no analytical solution for this,
 * so we use numerical root finding.
 *
 * Algorithm approach:
 *  1. Precompute polynomial coefficients for x(t) and y(t) during function construction
 *  2. First attempt fast convergence with Newton-Raphson method (usually converges in <4 iterations)
 *  3. Fall back to robust bisection method if Newton-Raphson fails
 *  4. Return y value for the found curve parameter
 *
 * References:
 *  - W3C CSS Working Group specification: https://www.w3.org/TR/css-easing-1/#cubic-bezier-easing-functions
 *  - WebKit implementation reference:
 * https://github.com/WebKit/WebKit/blob/main/Source/WebCore/platform/graphics/UnitBezier.h
 *  - Numerical Recipes: Newton-Raphson method for root finding
 *
 * @tparam T Value type, typically float
 * @param x1 X coordinate of first control point [0, 1]
 * @param y1 Y coordinate of first control point
 * @param x2 X coordinate of second control point [0, 1]
 * @param y2 Y coordinate of second control point
 * @return Easing function that maps [0, 1] to [0, 1]
 */
template <typename T = float>
easing_function<T> cubic_bezier(T x1, T y1, T x2, T y2) {
  // Constants (pre-calculated at function construction time, not per sample)
  const T zero = static_cast<T>(0);
  const T one = static_cast<T>(1);
  const T two = static_cast<T>(2);
  const T three = static_cast<T>(3);
  const T epsilon = static_cast<T>(1e-6);  // 1e-6 gives sub-pixel accuracy for 16-bit values

  // Precalculate polynomial coefficients for cubic curve
  // x(t) = ax*t³ + bx*t² + cx*t
  // y(t) = ay*t³ + by*t² + cy*t
  const T cx = three * x1;
  const T bx = three * (x2 - x1) - cx;
  const T ax = one - cx - bx;

  const T cy = three * y1;
  const T by = three * (y2 - y1) - cy;
  const T ay = one - cy - by;

  // Return closure with precomputed coefficients
  return [ax, bx, cx, ay, by, cy, zero, one, two, three, epsilon](T t) -> T {
    // Curve sample functions
    auto sample_curve_x = [ax, bx, cx](T s) { return ((ax * s + bx) * s + cx) * s; };
    auto sample_curve_y = [ay, by, cy](T s) { return ((ay * s + by) * s + cy) * s; };
    auto sample_derivative_x = [ax, bx, cx, two, three](T s) { return (three * ax * s + two * bx) * s + cx; };

    t = std::clamp(t, zero, one);

    // Step 1: Fast Newton-Raphson root finding
    // Converges in 2-4 iterations for most valid bezier curves
    T s = t;  // initial guess
    for (int i = 0; i < 8; ++i) {
      const T x_error = sample_curve_x(s) - t;
      if (std::abs(x_error) <= epsilon) {
        // Found solution within tolerance
        return std::clamp(sample_curve_y(s), zero, one);
      }

      const T derivative = sample_derivative_x(s);
      if (std::abs(derivative) <= epsilon) {
        // Flat region, Newton-Raphson would diverge -> fall back to bisection
        break;
      }

      // Newton step: s = s - f(s)/f'(s)
      s -= x_error / derivative;
    }

    // Step 2: Fallback bisection method (always converges, slower but robust)
    T low = zero;
    T high = one;
    s = t;

    for (int i = 0; i < 24; ++i) {
      const T x_error = sample_curve_x(s) - t;
      if (std::abs(x_error) <= epsilon) {
        break;
      }

      // Narrow search interval
      if (x_error < zero) {
        low = s;
      } else {
        high = s;
      }
      s = (low + high) / two;
    }

    // Final result
    return std::clamp(sample_curve_y(s), zero, one);
  };
}

}  // namespace colorcpp::algorithms::gradient::easing
