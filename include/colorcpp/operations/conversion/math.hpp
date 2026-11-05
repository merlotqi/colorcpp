/**
 * @file math.hpp
 * @brief Constexpr-safe math shims for conversion paths (issue #75).
 *
 * std::pow and std::cbrt are not constexpr in C++17/20 (P0533R3 adopted
 * them for C++26). GCC folds the libm calls as builtins during constant
 * evaluation; Clang and MSVC do not, so `constexpr color_cast` through the
 * Adobe gamma and CIELAB/CIELUV/OKLab cbrt paths failed to compile there
 * while CI stayed green (nothing constant-evaluated them).
 *
 * Design:
 *  - The portable_* wrappers branch on is_constant_evaluated(): at runtime
 *    they call std::pow/std::cbrt (libm speed and exact semantics — zero
 *    runtime change); in a constant expression they call the pure shims.
 *  - The shims are pure C++17 arithmetic (Newton iterations in double, so
 *    results match libm to float precision) and need no compiler builtins.
 *  - Branch selection needs __builtin_is_constant_evaluated (GCC 9+,
 *    Clang 8+, MSVC 2019 16.4+). Older compilers fall back to "never
 *    constant-evaluated" and keep today's behavior: std::pow/cbrt folds
 *    where the compiler provides them (GCC), fails to constant-evaluate
 *    elsewhere (documented in README).
 */

#pragma once

#include <cmath>
#include <limits>

// Detect __builtin_is_constant_evaluated without breaking older compilers.
#if defined(__has_builtin)
#if __has_builtin(__builtin_is_constant_evaluated)
#define COLORCPP_HAS_IS_CE 1
#endif
#endif
#if !defined(COLORCPP_HAS_IS_CE) && defined(_MSC_VER) && _MSC_VER >= 1924
#define COLORCPP_HAS_IS_CE 1
#endif
// GCC 9 introduced the builtin but not __has_builtin (GCC 10+).
#if !defined(COLORCPP_HAS_IS_CE) && defined(__GNUC__) && !defined(__clang__) && __GNUC__ >= 9
#define COLORCPP_HAS_IS_CE 1
#endif

namespace colorcpp::operations::conversion::details {

/**
 * @brief True when the current evaluation is a constant expression.
 *
 * C++17 has no std::is_constant_evaluated (C++20); wraps the compiler
 * builtin where available and reports false otherwise.
 */
constexpr bool is_constant_evaluated() noexcept {
#if defined(COLORCPP_HAS_IS_CE)
  return __builtin_is_constant_evaluated();
#else
  return false;
#endif
}

namespace math_shim {

constexpr double kDblMax = 1.7976931348623157e308;

/// IEEE NaN in constant evaluation — quiet_NaN() is constexpr since C++11.
constexpr double nan_d() noexcept { return std::numeric_limits<double>::quiet_NaN(); }
constexpr float nan_f() noexcept { return std::numeric_limits<float>::quiet_NaN(); }

/**
 * @brief Newton square root in double. Pure arithmetic — constant-evaluable
 * on every C++17 compiler. Matches std::sqrt to double rounding.
 */
constexpr double dsqrt(double x) noexcept {
  if (x != x) return x;  // NaN
  if (x < 0.0) return nan_d();
  if (x == 0.0 || x > kDblMax) return x;  // 0 and +inf pass through
  // Scale into [1, 4) by powers of 4 (exact in binary FP): x = a * 4^k
  // implies sqrt(x) = sqrt(a) * 2^k.
  double a = x;
  int k = 0;
  while (a < 1.0) {
    a *= 4.0;
    --k;
  }
  while (a >= 4.0) {
    a /= 4.0;
    ++k;
  }
  double y = 0.5 * (a + 1.0);  // seed; sqrt(a) ∈ [1, 2)
  for (int i = 0; i < 10; ++i) y = 0.5 * (y + a / y);
  while (k > 0) {
    y *= 2.0;
    --k;
  }
  while (k < 0) {
    y *= 0.5;
    ++k;
  }
  return y;
}

/**
 * @brief Newton cube root in double. Pure arithmetic — constant-evaluable
 * on every C++17 compiler. Matches std::cbrt to double rounding.
 */
constexpr double dcbrt(double x) noexcept {
  if (x != x) return x;                                   // NaN
  if (x == 0.0 || x > kDblMax || x < -kDblMax) return x;  // 0, ±inf
  const bool neg = x < 0.0;
  double a = neg ? -x : x;
  // Scale into [1, 8) by powers of 8: x = a * 8^k implies
  // cbrt(x) = cbrt(a) * 2^k (k is a multiple of 3 worth of 2s).
  int k = 0;
  while (a < 1.0) {
    a *= 8.0;
    --k;
  }
  while (a >= 8.0) {
    a /= 8.0;
    ++k;
  }
  double y = 1.5;  // seed; cbrt(a) ∈ [1, 2)
  for (int i = 0; i < 12; ++i) y = (2.0 * y + a / (y * y)) / 3.0;
  while (k > 0) {
    y *= 2.0;
    --k;
  }
  while (k < 0) {
    y *= 0.5;
    ++k;
  }
  return neg ? -y : y;
}

/**
 * @brief Adobe RGB linearization v^(563/256) in double (issue #68 exponent).
 *
 * v^(563/256) = v^2 · (v^(1/256))^51; the 256th root is eight successive
 * square roots, the 51st power is square-and-multiply (51 = 0b110011).
 * Double intermediates keep the composed-root error far below float
 * rounding. Domain: v ≥ 0; callers guard negatives (std::pow of a negative
 * base with a non-integer exponent is a domain error → NaN).
 */
constexpr double adobe_linearize_d(double v) noexcept {
  double r = v;
  for (int i = 0; i < 8; ++i) r = dsqrt(r);
  const double r2 = r * r;
  const double r4 = r2 * r2;
  const double r8 = r4 * r4;
  const double r16 = r8 * r8;
  const double r32 = r16 * r16;
  const double f = r32 * r16 * r2 * r;  // r^51
  return v * v * f;
}

/**
 * @brief Adobe RGB gamma encode v^(256/563) in double.
 *
 * Exact inverse of adobe_linearize_d, located by bisection on the monotone
 * decode over y ∈ [v, 1] (for v ∈ [0,1], v^p ≥ v when p > 1 is inverted
 * here, so y ≥ v). 60 halvings drive the bracket below double resolution.
 */
constexpr double adobe_encode_d(double v) noexcept {
  if (v <= 0.0) return 0.0;
  if (v >= 1.0) return 1.0;
  double lo = v, hi = 1.0;
  for (int i = 0; i < 60; ++i) {
    const double mid = 0.5 * (lo + hi);
    if (adobe_linearize_d(mid) < v)
      lo = mid;
    else
      hi = mid;
  }
  return 0.5 * (lo + hi);
}

}  // namespace math_shim

// ---------------------------------------------------------------------------
// Pure shims (always arithmetic — directly unit-tested against libm)
// ---------------------------------------------------------------------------

/// Constexpr-safe std::cbrt.
constexpr float constexpr_cbrt(float x) noexcept {
  return static_cast<float>(math_shim::dcbrt(static_cast<double>(x)));
}

/// Constexpr-safe Adobe linearization std::pow(v, 563/256).
constexpr float constexpr_adobe_linearize(float v) noexcept {
  if (v != v) return v;                     // NaN
  if (v < 0.0f) return math_shim::nan_f();  // domain error, like std::pow
  if (v == 0.0f) return 0.0f;
  return static_cast<float>(math_shim::adobe_linearize_d(static_cast<double>(v)));
}

/// Constexpr-safe Adobe gamma encode std::pow(v, 256/563) on [0, 1].
constexpr float constexpr_adobe_gamma_encode(float v) noexcept {
  if (v != v) return v;
  if (v <= 0.0f) return 0.0f;
  if (v >= 1.0f) return 1.0f;
  return static_cast<float>(math_shim::adobe_encode_d(static_cast<double>(v)));
}

// ---------------------------------------------------------------------------
// Portable wrappers: libm at runtime, shims in constant evaluation
// ---------------------------------------------------------------------------

/// std::cbrt at runtime; constexpr_cbrt in a constant expression (issue #75).
constexpr float portable_cbrt(float x) noexcept {
  if (is_constant_evaluated()) return constexpr_cbrt(x);
  return std::cbrt(x);
}

/// std::pow(v, 563/256) at runtime; shim in a constant expression (issue #75).
constexpr float portable_adobe_linearize(float v) noexcept {
  if (is_constant_evaluated()) return constexpr_adobe_linearize(v);
  return std::pow(v, 563.0f / 256.0f);
}

/// std::pow(v, 256/563) at runtime; shim in a constant expression (issue #75).
constexpr float portable_adobe_gamma_encode(float v) noexcept {
  if (is_constant_evaluated()) return constexpr_adobe_gamma_encode(v);
  return std::pow(v, 256.0f / 563.0f);
}

}  // namespace colorcpp::operations::conversion::details
