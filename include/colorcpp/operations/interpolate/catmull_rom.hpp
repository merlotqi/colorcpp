/**
 * @file catmull_rom.hpp
 * @brief Catmull-Rom cubic interpolation in OkLab space.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::operations::interpolate {

// Catmull-Rom parameterization modes
enum class cr_mode {
  uniform,      ///< Uniform parameterization (standard Catmull-Rom)
  centripetal,  ///< Centripetal parameterization (prevents cusps, sqrt of chord lengths)
  chordal       ///< Chordal parameterization (more even spacing, uses chord lengths)
};

namespace details {

// Compute parameterized t value for Catmull-Rom based on mode
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
inline float catmull_rom(float p0, float p1, float p2, float p3, float t) {
  float t2 = t * t;
  float t3 = t2 * t;
  return 0.5f * ((2.0f * p1) + (-p0 + p2) * t + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
                 (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
}

}  // namespace details

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

}  // namespace colorcpp::operations::interpolate
