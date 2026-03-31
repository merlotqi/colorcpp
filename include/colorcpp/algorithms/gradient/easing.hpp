/**
 * @file easing.hpp
 * @brief Easing functions for gradient interpolation control.
 *
 * Easing functions control the rate of change along a gradient,
 * allowing for non-linear interpolation between color stops.
 */

#pragma once

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
 * @brief Create a custom easing function from a cubic bezier.
 * @tparam T Value type.
 * @param x1 First control point x coordinate.
 * @param y1 First control point y coordinate.
 * @param x2 Second control point x coordinate.
 * @param y2 Second control point y coordinate.
 * @return Easing function.
 */
template <typename T = float>
easing_function<T> cubic_bezier(T x1, T y1, T x2, T y2) {
  // Simplified cubic bezier implementation
  // For production, you'd want a more robust implementation
  return [x1, y1, x2, y2](T t) -> T {
    // This is a simplified version; a full implementation would solve the bezier equation
    // For now, we'll use a polynomial approximation
    T t2 = t * t;
    T t3 = t2 * t;
    T mt = 1.0f - t;
    T mt2 = mt * mt;
    T mt3 = mt2 * mt;

    // Bezier curve approximation
    T x = mt3 * 0.0f + 3.0f * mt2 * t * x1 + 3.0f * mt * t2 * x2 + t3 * 1.0f;
    T y = mt3 * 0.0f + 3.0f * mt2 * t * y1 + 3.0f * mt * t2 * y2 + t3 * 1.0f;

    return y;
  };
}

}  // namespace colorcpp::algorithms::gradient::easing
