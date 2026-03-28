/**
 * @file interpolate.hpp
 * @brief Interpolation and easing between colors in RGB, HSL, HSV, OkLab, OkLCH, CIELAB, and CIELCH spaces.
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
#include <initializer_list>
#include <stdexcept>
#include <vector>

/** @brief Lerps, multi-stop gradients, and easing helpers built on @ref conversion::color_cast. */
namespace colorcpp::operations::interpolate {

// Catmull-Rom parameterization modes
enum class cr_mode {
  uniform,      ///< Uniform parameterization (standard Catmull-Rom)
  centripetal,  ///< Centripetal parameterization (prevents cusps, sqrt of chord lengths)
  chordal       ///< Chordal parameterization (more even spacing, uses chord lengths)
};

namespace details {

inline float lerp_f(float a, float b, float t) { return a + (b - a) * t; }

// Shortest-path hue interpolation in degree space [0, 360).
// Wraps delta into [-180, 180] first so the shorter arc is always taken.
// Example: lerp_angle_deg(10, 350, 0.5) = 0  (via 0°, not via 180°)
inline float lerp_angle_deg(float a, float b, float t) {
  float delta = std::fmod(b - a + 540.0f, 360.0f) - 180.0f;
  return std::fmod(a + delta * t + 360.0f, 360.0f);
}

// Compute parameterized t value for Catmull-Rom based on mode
// This reparameterizes t based on distances between control points
inline float cr_parameterize(float p0, float p1, float p2, float p3, float t, cr_mode mode) {
  if (mode == cr_mode::uniform) return t;

  // Compute chord lengths between consecutive points
  float d01 = std::abs(p1 - p0);
  float d12 = std::abs(p2 - p1);
  float d23 = std::abs(p3 - p2);

  // Avoid division by zero
  const float eps = 1e-10f;
  d01 = std::max(d01, eps);
  d12 = std::max(d12, eps);
  d23 = std::max(d23, eps);

  if (mode == cr_mode::centripetal) {
    // Centripetal: use sqrt of chord lengths
    d01 = std::sqrt(d01);
    d12 = std::sqrt(d12);
    d23 = std::sqrt(d23);
  }
  // For chordal mode, use chord lengths directly

  // Compute cumulative distances
  float t01 = 0.0f;
  float t12 = d01;
  float t23 = d01 + d12;
  float t34 = d01 + d12 + d23;

  // Normalize to [0, 1]
  float t_total = t34;
  if (t_total < eps) return t;

  // Map input t to parameterized space
  float t_param = t01 + t * (t23 - t01);

  // Convert back to [0, 1] range for the segment [p1, p2]
  float segment_start = t12;
  float segment_end = t23;
  float segment_length = segment_end - segment_start;

  if (segment_length < eps) return t;

  return (t_param - segment_start) / segment_length;
}

// Catmull-Rom cubic interpolation
// Requires four control points: p0, p1, p2, p3
// Interpolates between p1 and p2 using t ∈ [0, 1]
inline float catmull_rom(float p0, float p1, float p2, float p3, float t) {
  float t2 = t * t;
  float t3 = t2 * t;
  return 0.5f * ((2.0f * p1) + (-p0 + p2) * t + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
                 (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
}

// Cubic Hermite interpolation
// Requires four control points: p0, p1 (start), p2 (end), p3
// Tension parameter controls curve tightness: 0 = smooth (Catmull-Rom), 1 = linear
inline float cubic_hermite(float p0, float p1, float p2, float p3, float t, float tension = 0.0f) {
  float t2 = t * t;
  float t3 = t2 * t;
  float m0 = (1.0f - tension) * (p2 - p0) * 0.5f;
  float m1 = (1.0f - tension) * (p3 - p1) * 0.5f;
  return (2.0f * t3 - 3.0f * t2 + 1.0f) * p1 + (t3 - 2.0f * t2 + t) * m0 + (-2.0f * t3 + 3.0f * t2) * p2 +
         (t3 - t2) * m1;
}

/**
 * @brief Compute maximum safe chroma for a given lightness and hue in OkLCH space.
 * @param L Lightness in [0, 1]
 * @param H Hue angle in degrees [0, 360)
 * @return Maximum chroma that can be represented in sRGB gamut for the given L and H.
 * @note Uses binary search to find the gamut boundary by checking if the resulting RGB is in [0,1].
 */
inline float max_chroma_for(float L, float H) {
  // Binary search for maximum chroma that stays in sRGB gamut
  float lo = 0.0f;
  float hi = 0.4f;  // OkLCH chroma is typically in [0, ~0.4]

  // Create OkLCH color and check if it's in gamut
  auto in_gamut = [&](float C) -> bool {
    core::oklch_t oklch{L, C, H};
    auto rgb = operations::conversion::color_cast<core::rgbaf_t>(oklch);
    return rgb.r() >= 0.0f && rgb.r() <= 1.0f && rgb.g() >= 0.0f && rgb.g() <= 1.0f && rgb.b() >= 0.0f &&
           rgb.b() <= 1.0f;
  };

  // Binary search with 10 iterations gives ~0.0004 precision
  for (int i = 0; i < 10; ++i) {
    float mid = (lo + hi) / 2.0f;
    if (in_gamut(mid)) {
      lo = mid;
    } else {
      hi = mid;
    }
  }

  return lo;
}

}  // namespace details

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

  // H channel is in [0, 360] — use degree-space shortest-arc interpolation.
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

/** @brief OkLab interpolation (perceptually uniform); alpha lerped in RGBA. */
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

/** @brief OkLCH interpolation: shortest-arc hue; L, C linear; alpha via RGBA. */
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

/** @brief CIELAB interpolation; alpha via RGBA. */
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

/** @brief CIELCH interpolation: shortest-arc hue; L*, C* linear; alpha via RGBA. */
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

/**
 * @brief Catmull-Rom cubic interpolation in OkLab space.
 * @param p0 Previous color (anchor before start)
 * @param p1 Start color
 * @param p2 End color
 * @param p3 Next color (anchor after end)
 * @param t Interpolation factor in [0, 1] (clamped).
 * @param mode Parameterization mode: uniform (standard), centripetal (prevents cusps), or chordal (even spacing).
 * @note Produces smoother curves than linear interpolation while passing through p1 and p2.
 *       Centripetal mode prevents cusps and self-intersections; chordal gives more even spacing.
 */
template <typename Color>
Color lerp_catmull_rom(const Color& p0, const Color& p1, const Color& p2, const Color& p3, float t,
                       cr_mode mode = cr_mode::uniform) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);

  auto c0 = color_cast<core::oklab_t>(p0);
  auto c1 = color_cast<core::oklab_t>(p1);
  auto c2 = color_cast<core::oklab_t>(p2);
  auto c3 = color_cast<core::oklab_t>(p3);

  // Apply parameterization to t for each channel
  float t_L = details::cr_parameterize(c0.template get_index<0>(), c1.template get_index<0>(),
                                       c2.template get_index<0>(), c3.template get_index<0>(), t, mode);
  float t_A = details::cr_parameterize(c0.template get_index<1>(), c1.template get_index<1>(),
                                       c2.template get_index<1>(), c3.template get_index<1>(), t, mode);
  float t_B = details::cr_parameterize(c0.template get_index<2>(), c1.template get_index<2>(),
                                       c2.template get_index<2>(), c3.template get_index<2>(), t, mode);

  core::oklab_t mid{details::catmull_rom(c0.template get_index<0>(), c1.template get_index<0>(),
                                         c2.template get_index<0>(), c3.template get_index<0>(), t_L),
                    details::catmull_rom(c0.template get_index<1>(), c1.template get_index<1>(),
                                         c2.template get_index<1>(), c3.template get_index<1>(), t_A),
                    details::catmull_rom(c0.template get_index<2>(), c1.template get_index<2>(),
                                         c2.template get_index<2>(), c3.template get_index<2>(), t_B)};

  // Alpha interpolation using cubic hermite
  auto r0 = color_cast<core::rgbaf_t>(p0);
  auto r1 = color_cast<core::rgbaf_t>(p1);
  auto r2 = color_cast<core::rgbaf_t>(p2);
  auto r3 = color_cast<core::rgbaf_t>(p3);
  auto out = color_cast<core::rgbaf_t>(mid);
  out.template get_index<3>() = details::catmull_rom(r0.template get_index<3>(), r1.template get_index<3>(),
                                                     r2.template get_index<3>(), r3.template get_index<3>(), t);
  return color_cast<Color>(out);
}

/**
 * @brief Cubic Hermite interpolation in OkLab space.
 * @param p0 Previous color (anchor before start)
 * @param p1 Start color
 * @param p2 End color
 * @param p3 Next color (anchor after end)
 * @param t Interpolation factor in [0, 1] (clamped).
 * @param tension Tension parameter: 0 = Catmull-Rom, higher = tighter curve.
 */
template <typename Color>
Color lerp_cubic_hermite(const Color& p0, const Color& p1, const Color& p2, const Color& p3, float t,
                         float tension = 0.0f) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);

  auto c0 = color_cast<core::oklab_t>(p0);
  auto c1 = color_cast<core::oklab_t>(p1);
  auto c2 = color_cast<core::oklab_t>(p2);
  auto c3 = color_cast<core::oklab_t>(p3);

  core::oklab_t mid{details::cubic_hermite(c0.template get_index<0>(), c1.template get_index<0>(),
                                           c2.template get_index<0>(), c3.template get_index<0>(), t, tension),
                    details::cubic_hermite(c0.template get_index<1>(), c1.template get_index<1>(),
                                           c2.template get_index<1>(), c3.template get_index<1>(), t, tension),
                    details::cubic_hermite(c0.template get_index<2>(), c1.template get_index<2>(),
                                           c2.template get_index<2>(), c3.template get_index<2>(), t, tension)};

  // Alpha interpolation
  auto r0 = color_cast<core::rgbaf_t>(p0);
  auto r1 = color_cast<core::rgbaf_t>(p1);
  auto r2 = color_cast<core::rgbaf_t>(p2);
  auto r3 = color_cast<core::rgbaf_t>(p3);
  auto out = color_cast<core::rgbaf_t>(mid);
  out.template get_index<3>() =
      details::cubic_hermite(r0.template get_index<3>(), r1.template get_index<3>(), r2.template get_index<3>(),
                             r3.template get_index<3>(), t, tension);
  return color_cast<Color>(out);
}

/**
 * @brief Hue-locked interpolation in OkLCH space.
 * @param a Start color
 * @param b End color
 * @param t Interpolation factor in [0, 1] (clamped).
 * @param hue_locked If true, hue is locked to the start color's hue; only lightness and chroma interpolate.
 * @note When hue_locked=true, only L and C are interpolated; H stays constant.
 */
template <typename Color>
Color lerp_hue_locked(const Color& a, const Color& b, float t, bool hue_locked = true) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);

  auto ca = color_cast<core::oklch_t>(a);
  auto cb = color_cast<core::oklch_t>(b);

  float L = details::lerp_f(ca.template get_index<0>(), cb.template get_index<0>(), t);
  float C = details::lerp_f(ca.template get_index<1>(), cb.template get_index<1>(), t);
  float H = hue_locked ? ca.template get_index<2>()
                       : details::lerp_angle_deg(ca.template get_index<2>(), cb.template get_index<2>(), t);

  core::oklch_t mid{L, C, H};

  auto ra = color_cast<core::rgbaf_t>(a);
  auto rb = color_cast<core::rgbaf_t>(b);
  auto out = color_cast<core::rgbaf_t>(mid);
  out.template get_index<3>() = details::lerp_f(ra.template get_index<3>(), rb.template get_index<3>(), t);
  return color_cast<Color>(out);
}

enum class chroma_mode {
  lerp,          ///< Interpolate chroma linearly between the two colors
  preserve,      ///< Preserve chroma at max(Ca, Cb) throughout the interpolation
  preserve_safe  ///< Preserve chroma at max(Ca, Cb) but clamp to sRGB gamut boundary
};

/**
 * @brief Chroma-preserving interpolation in OkLCH space.
 * @param a Start color
 * @param b End color
 * @param t Interpolation factor in [0, 1] (clamped).
 * @param mode Chroma interpolation mode (lerp, preserve, or preserve_safe).
 * @note In preserve mode, C stays at max(Ca, Cb); only L and H are interpolated.
 *       In preserve_safe mode, C is clamped to the sRGB gamut boundary.
 *       At t=0, returns the first color; at t=1, returns the second color exactly.
 */
template <typename Color>
Color lerp_chroma_preserving(const Color& a, const Color& b, float t, chroma_mode mode = chroma_mode::preserve_safe) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);

  // Special case: return exact colors at endpoints
  if (t == 0.0f) return a;
  if (t == 1.0f) return b;

  auto ca = color_cast<core::oklch_t>(a);
  auto cb = color_cast<core::oklch_t>(b);

  float L = details::lerp_f(ca.template get_index<0>(), cb.template get_index<0>(), t);
  float H = details::lerp_angle_deg(ca.template get_index<2>(), cb.template get_index<2>(), t);

  // Determine chroma based on mode
  float C;
  switch (mode) {
    case chroma_mode::lerp:
      C = details::lerp_f(ca.template get_index<1>(), cb.template get_index<1>(), t);
      break;
    case chroma_mode::preserve:
      C = std::max(ca.template get_index<1>(), cb.template get_index<1>());
      break;
    case chroma_mode::preserve_safe: {
      float max_c = std::max(ca.template get_index<1>(), cb.template get_index<1>());
      float safe_c = details::max_chroma_for(L, H);
      C = std::min(max_c, safe_c);
      break;
    }
    default:
      C = details::lerp_f(ca.template get_index<1>(), cb.template get_index<1>(), t);
      break;
  }

  core::oklch_t mid{L, C, H};

  auto ra = color_cast<core::rgbaf_t>(a);
  auto rb = color_cast<core::rgbaf_t>(b);
  auto out = color_cast<core::rgbaf_t>(mid);
  out.template get_index<3>() = details::lerp_f(ra.template get_index<3>(), rb.template get_index<3>(), t);
  return color_cast<Color>(out);
}

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
 * @return Interpolated color with alpha determined by the specified mode.
 * @note Allows independent control of the alpha channel behavior during interpolation.
 *       RGB channels are always linearly interpolated regardless of the alpha mode.
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
    case alpha_mode::lerp:  // Interpolate alpha
      alpha = details::lerp_f(ca.template get_index<3>(), cb.template get_index<3>(), t);
      break;
    case alpha_mode::start:  // Use start alpha
      alpha = ca.template get_index<3>();
      break;
    case alpha_mode::end:  // Use end alpha
      alpha = cb.template get_index<3>();
      break;
    case alpha_mode::multiply:  // Multiply alphas
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
