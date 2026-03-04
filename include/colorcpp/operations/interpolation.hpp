#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/operations/color_cast.hpp>
#include <colorcpp/traits/concepts.hpp>

namespace color::operations::interpolate {

namespace details {

template <typename Color>
auto to_rgb_float(const Color& c) {
  return conversion::color_cast<core::basic_rgba<float, std::ratio<1>>, Color>(c);
}

template <typename Color>
Color from_rgb_float(const core::basic_rgba<float, std::ratio<1>>& c) {
  return conversion::color_cast<Color>(c);
}

inline float lerp(float a, float b, float t) { return a + (b - a) * t; }

}  // namespace details

template <typename Color>
Color lerp(const Color& a, const Color& b, float t) {
  t = std::clamp(t, 0.0f, 1.0f);

  auto ca = details::to_rgb_float(a);
  auto cb = details::to_rgb_float(b);

  core::basic_rgba<float, std::ratio<1>> out{details::lerp(ca.r, cb.r, t), details::lerp(ca.g, cb.g, t),
                                             details::lerp(ca.b, cb.b, t), details::lerp(ca.a, cb.a, t)};

  return details::from_rgb_float<Color>(out);
}

template <typename Color>
Color ease_in_out(const Color& a, const Color& b, float t) {
  t = std::clamp(t, 0.0f, 1.0f);

  // smoothstep
  float tt = t * t * (3.0f - 2.0f * t);

  return lerp(a, b, tt);
}

template <typename Color, typename Func>
Color custom(const Color& a, const Color& b, float t, Func&& fn) {
  t = std::clamp(t, 0.0f, 1.0f);

  auto ca = details::to_rgb_float(a);
  auto cb = details::to_rgb_float(b);

  auto out_rgb = fn(ca, cb, t);

  return details::from_rgb_float<Color>(out_rgb);
}

template <typename Color>
Color alpha_lerp(const Color& a, const Color& b, float t) {
  t = std::clamp(t, 0.0f, 1.0f);

  auto ca = details::to_rgb_float(a);
  auto cb = details::to_rgb_float(b);

  core::basic_rgba<float, std::ratio<1>> out{ca.r, ca.g, ca.b, details::lerp(ca.a, cb.a, t)};

  return details::from_rgb_float<Color>(out);
}

}  // namespace color::operations::interpolate
