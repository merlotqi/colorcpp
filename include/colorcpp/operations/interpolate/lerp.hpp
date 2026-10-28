/**
 * @file lerp.hpp
 * @brief Basic linear interpolation between colors in various color spaces.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/hsv.hpp>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::operations::interpolate {

namespace details {

inline float lerp_f(float a, float b, float t) { return a + (b - a) * t; }

// Shortest-path hue interpolation in degree space [0, 360).
// Wraps delta into [-180, 180] first so the shorter arc is always taken.
inline float lerp_angle_deg(float a, float b, float t) {
  float delta = std::fmod(b - a + 540.0f, 360.0f) - 180.0f;
  return std::fmod(a + delta * t + 360.0f, 360.0f);
}

// Unit-space alpha of a source color (1.0 when the model carries no alpha).
template <typename Color>
inline float src_alpha(const Color& c) {
  return ::colorcpp::operations::conversion::details::get_src_alpha(c);
}

/**
 * @brief Assemble an interpolation result: working-space midpoint + unit-space alpha.
 *
 * Casts the mid straight to Color so an out-of-gamut wide-space midpoint
 * (e.g. lerp_oklab with Color = oklab_t) is never clamped through sRGB;
 * RGB destinations still clamp via the normal conversion path. Alpha is
 * written into Color's own alpha channel when the model has one.
 */
template <typename Color, typename Mid>
inline Color lerp_result(const Mid& mid, float alpha) {
  Color out = ::colorcpp::operations::conversion::color_cast<Color>(mid);
  if constexpr (::colorcpp::operations::conversion::details::has_alpha_v<Color>) {
    constexpr std::size_t ai = ::colorcpp::operations::conversion::details::alpha_index_v<Color>;
    out.template get_index<ai>() = ::colorcpp::operations::conversion::details::from_unit<Color, ai>(alpha);
  }
  return out;
}

}  // namespace details

/**
 * @brief RGB-space linear interpolation (via @c rgbaf_t); fast; vivid hues may look muddy.
 * @param t Interpolation factor in [0, 1] (clamped).
 */
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

/** @brief HSL interpolation with shortest-arc hue; S, L, A linear. */
template <typename Color>
Color lerp_hsl(const Color& a, const Color& b, float t) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);

  auto ca = color_cast<core::hsla_float_t>(a);
  auto cb = color_cast<core::hsla_float_t>(b);

  core::hsla_float_t out{details::lerp_angle_deg(ca.template get_index<0>(), cb.template get_index<0>(), t),
                         details::lerp_f(ca.template get_index<1>(), cb.template get_index<1>(), t),
                         details::lerp_f(ca.template get_index<2>(), cb.template get_index<2>(), t),
                         details::lerp_f(ca.template get_index<3>(), cb.template get_index<3>(), t)};

  return color_cast<Color>(out);
}

/** @brief HSV interpolation with shortest-arc hue; S, V, A linear. */
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

/** @brief OkLab interpolation (perceptually uniform); alpha lerped from endpoints. */
template <typename Color>
Color lerp_oklab(const Color& a, const Color& b, float t) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);

  auto ca = color_cast<core::oklab_t>(a);
  auto cb = color_cast<core::oklab_t>(b);

  core::oklab_t mid{details::lerp_f(ca.template get_index<0>(), cb.template get_index<0>(), t),
                    details::lerp_f(ca.template get_index<1>(), cb.template get_index<1>(), t),
                    details::lerp_f(ca.template get_index<2>(), cb.template get_index<2>(), t)};

  return details::lerp_result<Color>(mid, details::lerp_f(details::src_alpha(a), details::src_alpha(b), t));
}

/** @brief OkLCH interpolation: shortest-arc hue; L, C linear; alpha from endpoints. */
template <typename Color>
Color lerp_oklch(const Color& a, const Color& b, float t) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);

  auto ca = color_cast<core::oklch_t>(a);
  auto cb = color_cast<core::oklch_t>(b);

  core::oklch_t mid{details::lerp_f(ca.template get_index<0>(), cb.template get_index<0>(), t),
                    details::lerp_f(ca.template get_index<1>(), cb.template get_index<1>(), t),
                    details::lerp_angle_deg(ca.template get_index<2>(), cb.template get_index<2>(), t)};

  return details::lerp_result<Color>(mid, details::lerp_f(details::src_alpha(a), details::src_alpha(b), t));
}

/** @brief CIELAB interpolation; alpha from endpoints. */
template <typename Color>
Color lerp_lab(const Color& a, const Color& b, float t) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);

  auto ca = color_cast<core::cielab_t>(a);
  auto cb = color_cast<core::cielab_t>(b);

  core::cielab_t mid{details::lerp_f(ca.template get_index<0>(), cb.template get_index<0>(), t),
                     details::lerp_f(ca.template get_index<1>(), cb.template get_index<1>(), t),
                     details::lerp_f(ca.template get_index<2>(), cb.template get_index<2>(), t)};

  return details::lerp_result<Color>(mid, details::lerp_f(details::src_alpha(a), details::src_alpha(b), t));
}

/** @brief CIELCH interpolation: shortest-arc hue; L*, C* linear; alpha from endpoints. */
template <typename Color>
Color lerp_lch(const Color& a, const Color& b, float t) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);

  auto ca = color_cast<core::cielch_t>(a);
  auto cb = color_cast<core::cielch_t>(b);

  core::cielch_t mid{details::lerp_f(ca.template get_index<0>(), cb.template get_index<0>(), t),
                     details::lerp_f(ca.template get_index<1>(), cb.template get_index<1>(), t),
                     details::lerp_angle_deg(ca.template get_index<2>(), cb.template get_index<2>(), t)};

  return details::lerp_result<Color>(mid, details::lerp_f(details::src_alpha(a), details::src_alpha(b), t));
}

}  // namespace colorcpp::operations::interpolate
