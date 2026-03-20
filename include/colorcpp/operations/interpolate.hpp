#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/hsv.hpp>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <initializer_list>
#include <stdexcept>
#include <vector>

namespace colorcpp::operations::interpolate {

namespace details {

inline float lerp_f(float a, float b, float t) { return a + (b - a) * t; }

// Shortest-path hue interpolation in degree space [0, 360).
// Wraps delta into [-180, 180] first so the shorter arc is always taken.
// Example: lerp_angle_deg(10, 350, 0.5) = 0  (via 0°, not via 180°)
inline float lerp_angle_deg(float a, float b, float t) {
  float delta = std::fmod(b - a + 540.0f, 360.0f) - 180.0f;
  return std::fmod(a + delta * t + 360.0f, 360.0f);
}

}  // namespace details

// Easing functions
// All functions map t ∈ [0, 1] → [0, 1] (approximately; back overshoots slightly).
// Pass any of these to ease() / ease_hsl() as the easing functor.

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

// Smoothstep — zero derivative at both endpoints (same curve as in_out_quad Hermite)
inline float smoothstep(float t) noexcept { return t * t * (3.0f - 2.0f * t); }

// Smootherstep — zero first AND second derivative at endpoints (Ken Perlin)
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

// Back — slight overshoot past start (in) or end (out)
// Colour channels will be clamped at the model boundaries if the overshoot exceeds range.
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

// Core interpolation

// RGB-space linear interpolation (channel-wise through rgbaf_t).
// Fast, preserves alpha. Hue transitions may appear "muddy" for vivid colours.
template <typename Color>
Color lerp(const Color& a, const Color& b, float t) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);

  auto ca = color_cast<core::rgbaf_t>(a);
  auto cb = color_cast<core::rgbaf_t>(b);

  core::rgbaf_t out{details::lerp_f(ca.template get_index<0>(), cb.template get_index<0>(), t),
                    details::lerp_f(ca.template get_index<1>(), cb.template get_index<1>(), t),
                    details::lerp_f(ca.template get_index<2>(), cb.template get_index<2>(), t),
                    details::lerp_f(ca.template get_index<3>(), cb.template get_index<3>(), t)};

  return color_cast<Color>(out);
}

// HSL-space interpolation: hue takes the shortest arc, S/L/A are linear.
// More perceptually uniform than RGB lerp; keeps colours vivid across the transition.
template <typename Color>
Color lerp_hsl(const Color& a, const Color& b, float t) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);

  auto ca = color_cast<core::hsla_float_t>(a);
  auto cb = color_cast<core::hsla_float_t>(b);

  // H channel is in [0, 360] — use degree-space shortest-arc interpolation.
  core::hsla_float_t out{details::lerp_angle_deg(ca.template get_index<0>(), cb.template get_index<0>(), t),
                         details::lerp_f(ca.template get_index<1>(), cb.template get_index<1>(), t),
                         details::lerp_f(ca.template get_index<2>(), cb.template get_index<2>(), t),
                         details::lerp_f(ca.template get_index<3>(), cb.template get_index<3>(), t)};

  return color_cast<Color>(out);
}

// HSV-space interpolation: shortest-arc hue, linear S/V/A.
template <typename Color>
Color lerp_hsv(const Color& a, const Color& b, float t) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);

  auto ca = color_cast<core::hsva_float_t>(a);
  auto cb = color_cast<core::hsva_float_t>(b);

  core::hsva_float_t out{details::lerp_angle_deg(ca.template get_index<0>(), cb.template get_index<0>(), t),
                         details::lerp_f(ca.template get_index<1>(), cb.template get_index<1>(), t),
                         details::lerp_f(ca.template get_index<2>(), cb.template get_index<2>(), t),
                         details::lerp_f(ca.template get_index<3>(), cb.template get_index<3>(), t)};

  return color_cast<Color>(out);
}

// OkLab-space interpolation (perceptually uniform, recommended for UI gradients).
// L/a/b channels lerp linearly in OkLab; alpha is lerped via the rgba representation.
template <typename Color>
Color lerp_oklab(const Color& a, const Color& b, float t) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);

  auto ca = color_cast<core::oklab_t>(a);
  auto cb = color_cast<core::oklab_t>(b);

  core::oklab_t mid{details::lerp_f(ca.template get_index<0>(), cb.template get_index<0>(), t),
                    details::lerp_f(ca.template get_index<1>(), cb.template get_index<1>(), t),
                    details::lerp_f(ca.template get_index<2>(), cb.template get_index<2>(), t)};

  auto ra = color_cast<core::rgbaf_t>(a);
  auto rb = color_cast<core::rgbaf_t>(b);
  auto out = color_cast<core::rgbaf_t>(mid);
  out.template get_index<3>() = details::lerp_f(ra.template get_index<3>(), rb.template get_index<3>(), t);
  return color_cast<Color>(out);
}

// OkLCH-space interpolation: shortest-arc hue, linear L/C; alpha via rgba.
template <typename Color>
Color lerp_oklch(const Color& a, const Color& b, float t) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);

  auto ca = color_cast<core::oklch_t>(a);
  auto cb = color_cast<core::oklch_t>(b);

  core::oklch_t mid{details::lerp_f(ca.template get_index<0>(), cb.template get_index<0>(), t),
                    details::lerp_f(ca.template get_index<1>(), cb.template get_index<1>(), t),
                    details::lerp_angle_deg(ca.template get_index<2>(), cb.template get_index<2>(), t)};

  auto ra = color_cast<core::rgbaf_t>(a);
  auto rb = color_cast<core::rgbaf_t>(b);
  auto out = color_cast<core::rgbaf_t>(mid);
  out.template get_index<3>() = details::lerp_f(ra.template get_index<3>(), rb.template get_index<3>(), t);
  return color_cast<Color>(out);
}

// CIELAB-space interpolation (ISO 11664-4 standard perceptual model).
// L*/a*/b* channels lerp linearly; alpha is lerped via the rgba representation.
template <typename Color>
Color lerp_lab(const Color& a, const Color& b, float t) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);

  auto ca = color_cast<core::cielab_t>(a);
  auto cb = color_cast<core::cielab_t>(b);

  core::cielab_t mid{details::lerp_f(ca.template get_index<0>(), cb.template get_index<0>(), t),
                     details::lerp_f(ca.template get_index<1>(), cb.template get_index<1>(), t),
                     details::lerp_f(ca.template get_index<2>(), cb.template get_index<2>(), t)};

  auto ra = color_cast<core::rgbaf_t>(a);
  auto rb = color_cast<core::rgbaf_t>(b);
  auto out = color_cast<core::rgbaf_t>(mid);
  out.template get_index<3>() = details::lerp_f(ra.template get_index<3>(), rb.template get_index<3>(), t);
  return color_cast<Color>(out);
}

// CIELCH-space interpolation: shortest-arc hue, linear L*/C*; alpha via rgba.
template <typename Color>
Color lerp_lch(const Color& a, const Color& b, float t) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);

  auto ca = color_cast<core::cielch_t>(a);
  auto cb = color_cast<core::cielch_t>(b);

  core::cielch_t mid{details::lerp_f(ca.template get_index<0>(), cb.template get_index<0>(), t),
                     details::lerp_f(ca.template get_index<1>(), cb.template get_index<1>(), t),
                     details::lerp_angle_deg(ca.template get_index<2>(), cb.template get_index<2>(), t)};

  auto ra = color_cast<core::rgbaf_t>(a);
  auto rb = color_cast<core::rgbaf_t>(b);
  auto out = color_cast<core::rgbaf_t>(mid);
  out.template get_index<3>() = details::lerp_f(ra.template get_index<3>(), rb.template get_index<3>(), t);
  return color_cast<Color>(out);
}

// Eased interpolation

// Apply any easing curve (from the easing namespace) to an RGB lerp.
// Example: ease(red, blue, t, easing::in_cubic)
template <typename Color, typename EasingFunc>
Color ease(const Color& a, const Color& b, float t, EasingFunc&& ef) {
  return lerp(a, b, ef(std::clamp(t, 0.0f, 1.0f)));
}

// Apply any easing curve to an HSL lerp.
template <typename Color, typename EasingFunc>
Color ease_hsl(const Color& a, const Color& b, float t, EasingFunc&& ef) {
  return lerp_hsl(a, b, ef(std::clamp(t, 0.0f, 1.0f)));
}

// Apply any easing curve to an OkLab lerp.
template <typename Color, typename EasingFunc>
Color ease_oklab(const Color& a, const Color& b, float t, EasingFunc&& ef) {
  return lerp_oklab(a, b, ef(std::clamp(t, 0.0f, 1.0f)));
}

// Apply any easing curve to an OkLCH lerp.
template <typename Color, typename EasingFunc>
Color ease_oklch(const Color& a, const Color& b, float t, EasingFunc&& ef) {
  return lerp_oklch(a, b, ef(std::clamp(t, 0.0f, 1.0f)));
}

// Convenience: smoothstep ease-in-out in RGB space (backward-compatible shortcut).
template <typename Color>
Color ease_in_out(const Color& a, const Color& b, float t) {
  return ease(a, b, t, easing::smoothstep);
}

// Multi-stop interpolation

// Interpolate across N colour stops evenly distributed over [0, 1].
// t=0 → stops.front(), t=1 → stops.back().
// Throws std::invalid_argument if fewer than 2 stops are given.
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

// Same as multi_lerp but interpolates in HSL space between adjacent stops.
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

// Interpolate across N stops in OkLab space (perceptually uniform gradients).
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

// Interpolate across N stops in OkLCH space (hue-preserving gradients).
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

// Batch generation

// Generate `count` evenly-spaced colours from a to b inclusive via RGB lerp.
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

// Generate `count` evenly-spaced colours from a to b inclusive via HSL lerp.
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

// Generate `count` evenly-spaced colours from a to b inclusive via OkLab lerp.
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

// Generate `count` evenly-spaced colours from a to b inclusive via OkLCH lerp.
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
