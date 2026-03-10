#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::operations::interpolate {

namespace details {

inline float lerp_f(float a, float b, float t) { return a + (b - a) * t; }

inline float lerp_angle(float a, float b, float t) {
  float delta = b - a;
  if (delta > 0.5f)
    delta -= 1.0f;
  else if (delta < -0.5f)
    delta += 1.0f;
  float res = a + delta * t;
  return (res < 0.0f) ? (res + 1.0f) : ((res > 1.0f) ? (res - 1.0f) : res);
}

}  // namespace details

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

template <typename Color>
Color lerp_hsl(const Color& a, const Color& b, float t) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);

  auto ca = color_cast<core::hsla_float_t>(a);
  auto cb = color_cast<core::hsla_float_t>(b);

  core::hsla_float_t out{details::lerp_angle(ca.template get_index<0>(), cb.template get_index<0>(), t),
                         details::lerp_f(ca.template get_index<1>(), cb.template get_index<1>(), t),
                         details::lerp_f(ca.template get_index<2>(), cb.template get_index<2>(), t),
                         details::lerp_f(ca.template get_index<3>(), cb.template get_index<3>(), t)};

  return color_cast<Color>(out);
}

template <typename Color, typename EasingFunc>
Color ease(const Color& a, const Color& b, float t, EasingFunc&& ef) {
  return lerp(a, b, ef(t));
}

template <typename Color>
Color ease_in_out(const Color& a, const Color& b, float t) {
  float tt = t * t * (3.0f - 2.0f * t);
  return lerp(a, b, tt);
}

}  // namespace colorcpp::operations::interpolate
