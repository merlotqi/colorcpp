/**
 * @file color_lerp.hpp
 * @brief High-level color interpolation: multi-stop gradients, easing, alpha control, and stop-based gradients.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/interpolate/lerp.hpp>
#include <initializer_list>
#include <stdexcept>
#include <vector>

namespace colorcpp::operations::interpolate {

/**
 * @brief A color stop with position for gradient interpolation.
 * @tparam Color The color type.
 */
template <typename Color>
struct stop {
  Color color;     ///< The color at this stop.
  float position;  ///< Position in [0, 1] along the gradient.
};

/**
 * @brief Easing curves mapping @f$t \in [0,1]@f$ → @f$[0,1]@f$ (back easing may overshoot slightly).
 *        Pass to @ref ease, @ref ease_hsl, @ref ease_oklab, or @ref ease_oklch.
 */
namespace easing {

static constexpr float kPi = 3.14159265358979323846f;

inline float linear(float t) noexcept { return t; }

// Quadratic
inline float in_quad(float t) noexcept { return t * t; }
inline float out_quad(float t) noexcept { return t * (2.0f - t); }
inline float in_out_quad(float t) noexcept { return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t; }

// Cubic
inline float in_cubic(float t) noexcept { return t * t * t; }
inline float out_cubic(float t) noexcept {
  float s = 1.0f - t;
  return 1.0f - s * s * s;
}
inline float in_out_cubic(float t) noexcept {
  return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) * 0.5f;
}

// Smoothstep
inline float smoothstep(float t) noexcept { return t * t * (3.0f - 2.0f * t); }

// Smootherstep
inline float smootherstep(float t) noexcept { return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f); }

// Sine
inline float in_sine(float t) noexcept { return 1.0f - std::cos(t * kPi * 0.5f); }
inline float out_sine(float t) noexcept { return std::sin(t * kPi * 0.5f); }
inline float in_out_sine(float t) noexcept { return -(std::cos(kPi * t) - 1.0f) * 0.5f; }

// Exponential
inline float in_expo(float t) noexcept { return t == 0.0f ? 0.0f : std::pow(2.0f, 10.0f * t - 10.0f); }
inline float out_expo(float t) noexcept { return t == 1.0f ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t); }
inline float in_out_expo(float t) noexcept {
  if (t == 0.0f) return 0.0f;
  if (t == 1.0f) return 1.0f;
  return t < 0.5f ? std::pow(2.0f, 20.0f * t - 10.0f) * 0.5f : (2.0f - std::pow(2.0f, -20.0f * t + 10.0f)) * 0.5f;
}

// Back
inline float in_back(float t) noexcept {
  constexpr float c = 1.70158f;
  return (c + 1.0f) * t * t * t - c * t * t;
}
inline float out_back(float t) noexcept {
  constexpr float c = 1.70158f;
  float s = 1.0f - t;
  return 1.0f - ((c + 1.0f) * s * s * s - c * s * s);
}
inline float in_out_back(float t) noexcept {
  constexpr float c = 1.70158f * 1.525f;
  return t < 0.5f ? (std::pow(2.0f * t, 2.0f) * ((c + 1.0f) * 2.0f * t - c)) * 0.5f
                  : (std::pow(2.0f * t - 2.0f, 2.0f) * ((c + 1.0f) * (2.0f * t - 2.0f) + c) + 2.0f) * 0.5f;
}

}  // namespace easing

enum class alpha_mode {
  lerp,     ///< Interpolate alpha channel linearly between start and end values
  start,    ///< Use the start color's alpha value throughout the interpolation
  end,      ///< Use the end color's alpha value throughout the interpolation
  multiply  ///< Multiply the alpha values of both colors at each interpolation step
};

/**
 * @brief Interpolation with independent alpha channel control.
 * @param a Start color
 * @param b End color
 * @param t Interpolation factor in [0, 1] (clamped).
 * @param mode Alpha interpolation mode (default: lerp).
 */
template <typename Color>
Color lerp_alpha_mode(const Color& a, const Color& b, float t, alpha_mode mode = alpha_mode::lerp) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);

  auto ca = color_cast<core::rgbaf_t>(a);
  auto cb = color_cast<core::rgbaf_t>(b);

  float r = details::lerp_f(ca.template get_index<0>(), cb.template get_index<0>(), t);
  float g = details::lerp_f(ca.template get_index<1>(), cb.template get_index<1>(), t);
  float bl = details::lerp_f(ca.template get_index<2>(), cb.template get_index<2>(), t);

  float alpha;
  switch (mode) {
    case alpha_mode::lerp:
      alpha = details::lerp_f(ca.template get_index<3>(), cb.template get_index<3>(), t);
      break;
    case alpha_mode::start:
      alpha = ca.template get_index<3>();
      break;
    case alpha_mode::end:
      alpha = cb.template get_index<3>();
      break;
    case alpha_mode::multiply:
      alpha = ca.template get_index<3>() * cb.template get_index<3>();
      break;
    default:
      alpha = details::lerp_f(ca.template get_index<3>(), cb.template get_index<3>(), t);
      break;
  }

  core::rgbaf_t out{r, g, bl, alpha};
  return color_cast<Color>(out);
}

/** @brief RGB @ref lerp with @p ef applied to @p t (e.g. @c easing::in_cubic). */
template <typename Color, typename EasingFunc>
Color ease(const Color& a, const Color& b, float t, EasingFunc&& ef) {
  return lerp(a, b, ef(std::clamp(t, 0.0f, 1.0f)));
}

/** @brief HSL @ref lerp_hsl with easing on @p t. */
template <typename Color, typename EasingFunc>
Color ease_hsl(const Color& a, const Color& b, float t, EasingFunc&& ef) {
  return lerp_hsl(a, b, ef(std::clamp(t, 0.0f, 1.0f)));
}

/** @brief OkLab @ref lerp_oklab with easing on @p t. */
template <typename Color, typename EasingFunc>
Color ease_oklab(const Color& a, const Color& b, float t, EasingFunc&& ef) {
  return lerp_oklab(a, b, ef(std::clamp(t, 0.0f, 1.0f)));
}

/** @brief OkLCH @ref lerp_oklch with easing on @p t. */
template <typename Color, typename EasingFunc>
Color ease_oklch(const Color& a, const Color& b, float t, EasingFunc&& ef) {
  return lerp_oklch(a, b, ef(std::clamp(t, 0.0f, 1.0f)));
}

/** @brief Shorthand for @ref ease with @c easing::smoothstep. */
template <typename Color>
Color ease_in_out(const Color& a, const Color& b, float t) {
  return ease(a, b, t, easing::smoothstep);
}

/** @brief Piecewise @ref lerp across evenly spaced stops on @f$t \in [0,1]@f$; at least two stops required. */
template <typename Color>
Color multi_lerp(std::initializer_list<Color> stops, float t) {
  if (stops.size() < 2) throw std::invalid_argument("colorcpp: multi_lerp requires at least 2 colour stops");
  t = std::clamp(t, 0.0f, 1.0f);

  const std::size_t n = stops.size() - 1;
  float scaled = t * static_cast<float>(n);
  std::size_t i = std::min(static_cast<std::size_t>(scaled), n - 1);
  float local_t = scaled - static_cast<float>(i);

  auto it = stops.begin();
  std::advance(it, i);
  const Color& lo = *it++;
  const Color& hi = *it;
  return lerp(lo, hi, local_t);
}

/** @brief Piecewise @ref lerp_hsl between adjacent stops. */
template <typename Color>
Color multi_lerp_hsl(std::initializer_list<Color> stops, float t) {
  if (stops.size() < 2) throw std::invalid_argument("colorcpp: multi_lerp_hsl requires at least 2 colour stops");
  t = std::clamp(t, 0.0f, 1.0f);

  const std::size_t n = stops.size() - 1;
  float scaled = t * static_cast<float>(n);
  std::size_t i = std::min(static_cast<std::size_t>(scaled), n - 1);
  float local_t = scaled - static_cast<float>(i);

  auto it = stops.begin();
  std::advance(it, i);
  const Color& lo = *it++;
  const Color& hi = *it;
  return lerp_hsl(lo, hi, local_t);
}

/** @brief Piecewise @ref lerp_oklab between adjacent stops. */
template <typename Color>
Color multi_lerp_oklab(std::initializer_list<Color> stops, float t) {
  if (stops.size() < 2) throw std::invalid_argument("colorcpp: multi_lerp_oklab requires at least 2 colour stops");
  t = std::clamp(t, 0.0f, 1.0f);

  const std::size_t n = stops.size() - 1;
  float scaled = t * static_cast<float>(n);
  std::size_t i = std::min(static_cast<std::size_t>(scaled), n - 1);
  float local_t = scaled - static_cast<float>(i);

  auto it = stops.begin();
  std::advance(it, i);
  const Color& lo = *it++;
  const Color& hi = *it;
  return lerp_oklab(lo, hi, local_t);
}

/** @brief Piecewise @ref lerp_oklch between adjacent stops. */
template <typename Color>
Color multi_lerp_oklch(std::initializer_list<Color> stops, float t) {
  if (stops.size() < 2) throw std::invalid_argument("colorcpp: multi_lerp_oklch requires at least 2 colour stops");
  t = std::clamp(t, 0.0f, 1.0f);

  const std::size_t n = stops.size() - 1;
  float scaled = t * static_cast<float>(n);
  std::size_t i = std::min(static_cast<std::size_t>(scaled), n - 1);
  float local_t = scaled - static_cast<float>(i);

  auto it = stops.begin();
  std::advance(it, i);
  const Color& lo = *it++;
  const Color& hi = *it;
  return lerp_oklch(lo, hi, local_t);
}

/**
 * @brief Piecewise @ref lerp across stops with explicit positions on @f$t \in [0,1]@f$.
 * @param stops Vector of color stops with positions; at least two required.
 * @param t Interpolation factor in [0, 1] (clamped).
 */
template <typename Color>
Color multi_lerp(const std::vector<stop<Color>>& stops, float t) {
  if (stops.size() < 2) throw std::invalid_argument("colorcpp: multi_lerp requires at least 2 colour stops");
  t = std::clamp(t, 0.0f, 1.0f);

  if (t <= stops.front().position) return stops.front().color;
  if (t >= stops.back().position) return stops.back().color;

  for (std::size_t i = 0; i < stops.size() - 1; ++i) {
    const auto& lo = stops[i];
    const auto& hi = stops[i + 1];
    if (t >= lo.position && t <= hi.position) {
      float range = hi.position - lo.position;
      if (range < 1e-10f) return lo.color;
      float local_t = (t - lo.position) / range;
      return lerp(lo.color, hi.color, local_t);
    }
  }

  return stops.back().color;
}

/**
 * @brief Piecewise @ref lerp_hsl across stops with explicit positions.
 */
template <typename Color>
Color multi_lerp_hsl(const std::vector<stop<Color>>& stops, float t) {
  if (stops.size() < 2) throw std::invalid_argument("colorcpp: multi_lerp_hsl requires at least 2 colour stops");
  t = std::clamp(t, 0.0f, 1.0f);

  if (t <= stops.front().position) return stops.front().color;
  if (t >= stops.back().position) return stops.back().color;

  for (std::size_t i = 0; i < stops.size() - 1; ++i) {
    const auto& lo = stops[i];
    const auto& hi = stops[i + 1];
    if (t >= lo.position && t <= hi.position) {
      float range = hi.position - lo.position;
      if (range < 1e-10f) return lo.color;
      float local_t = (t - lo.position) / range;
      return lerp_hsl(lo.color, hi.color, local_t);
    }
  }

  return stops.back().color;
}

/**
 * @brief Piecewise @ref lerp_oklab across stops with explicit positions.
 */
template <typename Color>
Color multi_lerp_oklab(const std::vector<stop<Color>>& stops, float t) {
  if (stops.size() < 2) throw std::invalid_argument("colorcpp: multi_lerp_oklab requires at least 2 colour stops");
  t = std::clamp(t, 0.0f, 1.0f);

  if (t <= stops.front().position) return stops.front().color;
  if (t >= stops.back().position) return stops.back().color;

  for (std::size_t i = 0; i < stops.size() - 1; ++i) {
    const auto& lo = stops[i];
    const auto& hi = stops[i + 1];
    if (t >= lo.position && t <= hi.position) {
      float range = hi.position - lo.position;
      if (range < 1e-10f) return lo.color;
      float local_t = (t - lo.position) / range;
      return lerp_oklab(lo.color, hi.color, local_t);
    }
  }

  return stops.back().color;
}

/**
 * @brief Piecewise @ref lerp_oklch across stops with explicit positions.
 */
template <typename Color>
Color multi_lerp_oklch(const std::vector<stop<Color>>& stops, float t) {
  if (stops.size() < 2) throw std::invalid_argument("colorcpp: multi_lerp_oklch requires at least 2 colour stops");
  t = std::clamp(t, 0.0f, 1.0f);

  if (t <= stops.front().position) return stops.front().color;
  if (t >= stops.back().position) return stops.back().color;

  for (std::size_t i = 0; i < stops.size() - 1; ++i) {
    const auto& lo = stops[i];
    const auto& hi = stops[i + 1];
    if (t >= lo.position && t <= hi.position) {
      float range = hi.position - lo.position;
      if (range < 1e-10f) return lo.color;
      float local_t = (t - lo.position) / range;
      return lerp_oklch(lo.color, hi.color, local_t);
    }
  }

  return stops.back().color;
}

/** @brief @p count evenly spaced samples from @p a to @p b via @ref lerp. */
template <typename Color>
std::vector<Color> lerp_n(const Color& a, const Color& b, std::size_t count) {
  std::vector<Color> out;
  if (count == 0) return out;
  out.reserve(count);
  if (count == 1) {
    out.push_back(a);
    return out;
  }
  for (std::size_t i = 0; i < count; ++i)
    out.push_back(lerp(a, b, static_cast<float>(i) / static_cast<float>(count - 1)));
  return out;
}

/** @brief @p count samples via @ref lerp_hsl. */
template <typename Color>
std::vector<Color> lerp_hsl_n(const Color& a, const Color& b, std::size_t count) {
  std::vector<Color> out;
  if (count == 0) return out;
  out.reserve(count);
  if (count == 1) {
    out.push_back(a);
    return out;
  }
  for (std::size_t i = 0; i < count; ++i)
    out.push_back(lerp_hsl(a, b, static_cast<float>(i) / static_cast<float>(count - 1)));
  return out;
}

/** @brief @p count samples via @ref lerp_oklab. */
template <typename Color>
std::vector<Color> lerp_oklab_n(const Color& a, const Color& b, std::size_t count) {
  std::vector<Color> out;
  if (count == 0) return out;
  out.reserve(count);
  if (count == 1) {
    out.push_back(a);
    return out;
  }
  for (std::size_t i = 0; i < count; ++i)
    out.push_back(lerp_oklab(a, b, static_cast<float>(i) / static_cast<float>(count - 1)));
  return out;
}

/** @brief @p count samples via @ref lerp_oklch. */
template <typename Color>
std::vector<Color> lerp_oklch_n(const Color& a, const Color& b, std::size_t count) {
  std::vector<Color> out;
  if (count == 0) return out;
  out.reserve(count);
  if (count == 1) {
    out.push_back(a);
    return out;
  }
  for (std::size_t i = 0; i < count; ++i)
    out.push_back(lerp_oklch(a, b, static_cast<float>(i) / static_cast<float>(count - 1)));
  return out;
}

}  // namespace colorcpp::operations::interpolate
