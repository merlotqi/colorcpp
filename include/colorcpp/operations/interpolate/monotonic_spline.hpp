/**
 * @file monotonic_spline.hpp
 * @brief Shape-preserving monotonic cubic interpolation in OkLab space.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::operations::interpolate {

namespace details {

// Steffen slope limiting for unit-spaced samples. The returned tangent preserves local monotonicity.
inline float monotonic_tangent(float prev_slope, float next_slope) {
  if (prev_slope == 0.0f || next_slope == 0.0f || prev_slope * next_slope <= 0.0f) return 0.0f;

  const float average_slope = 0.5f * (prev_slope + next_slope);
  const float limited_magnitude =
      std::min({std::abs(prev_slope), std::abs(next_slope), 0.5f * std::abs(average_slope)});
  return std::copysign(2.0f * limited_magnitude, average_slope);
}

inline float monotonic_spline(float y0, float y1, float y2, float y3, float t) {
  const float d0 = y1 - y0;
  const float d1 = y2 - y1;
  const float d2 = y3 - y2;

  const float m1 = monotonic_tangent(d0, d1);
  const float m2 = monotonic_tangent(d1, d2);

  const float t2 = t * t;
  const float t3 = t2 * t;

  const float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f;
  const float h10 = t3 - 2.0f * t2 + t;
  const float h01 = -2.0f * t3 + 3.0f * t2;
  const float h11 = t3 - t2;

  return h00 * y1 + h10 * m1 + h01 * y2 + h11 * m2;
}

}  // namespace details

/**
 * @brief Shape-preserving cubic interpolation between @p p1 and @p p2 in OkLab space.
 * @param p0 Previous color (anchor before start)
 * @param p1 Start color
 * @param p2 End color
 * @param p3 Next color (anchor after end)
 * @param t Interpolation factor in [0, 1] (clamped).
 * @note Uses Steffen slope limiting to avoid overshoot between adjacent control points.
 */
template <typename Color>
Color lerp_monotonic_spline(const Color& p0, const Color& p1, const Color& p2, const Color& p3, float t) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);

  auto c0 = color_cast<core::oklab_t>(p0);
  auto c1 = color_cast<core::oklab_t>(p1);
  auto c2 = color_cast<core::oklab_t>(p2);
  auto c3 = color_cast<core::oklab_t>(p3);

  core::oklab_t mid{details::monotonic_spline(c0.template get_index<0>(), c1.template get_index<0>(),
                                              c2.template get_index<0>(), c3.template get_index<0>(), t),
                    details::monotonic_spline(c0.template get_index<1>(), c1.template get_index<1>(),
                                              c2.template get_index<1>(), c3.template get_index<1>(), t),
                    details::monotonic_spline(c0.template get_index<2>(), c1.template get_index<2>(),
                                              c2.template get_index<2>(), c3.template get_index<2>(), t)};

  auto r0 = color_cast<core::rgbaf_t>(p0);
  auto r1 = color_cast<core::rgbaf_t>(p1);
  auto r2 = color_cast<core::rgbaf_t>(p2);
  auto r3 = color_cast<core::rgbaf_t>(p3);
  auto out = color_cast<core::rgbaf_t>(mid);
  out.template get_index<3>() = details::monotonic_spline(r0.template get_index<3>(), r1.template get_index<3>(),
                                                          r2.template get_index<3>(), r3.template get_index<3>(), t);
  return color_cast<Color>(out);
}

/** @brief Backward-compatible alias for @ref lerp_monotonic_spline. */
template <typename Color>
Color lerp_spline(const Color& p0, const Color& p1, const Color& p2, const Color& p3, float t) {
  return lerp_monotonic_spline(p0, p1, p2, p3, t);
}

}  // namespace colorcpp::operations::interpolate
