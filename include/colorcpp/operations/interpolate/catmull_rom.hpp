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
#include <colorcpp/operations/interpolate/lerp.hpp>

namespace colorcpp::operations::interpolate {

// Catmull-Rom parameterization modes
enum class cr_mode {
  uniform,      ///< Uniform parameterization (standard Catmull-Rom)
  centripetal,  ///< Centripetal parameterization (prevents cusps, sqrt of chord lengths)
  chordal       ///< Chordal parameterization (more even spacing, uses chord lengths)
};

namespace details {

// OkLab-space distance between two control points. Joint (per-curve, not
// per-channel) distances are the knot-spacing source for the non-uniform modes.
inline float cr_distance(const core::oklab_t& a, const core::oklab_t& b) {
  const float dl = a.template get_index<0>() - b.template get_index<0>();
  const float da = a.template get_index<1>() - b.template get_index<1>();
  const float db = a.template get_index<2>() - b.template get_index<2>();
  return std::sqrt(dl * dl + da * da + db * db);
}

// Ascending knot vector for the four control points.
// uniform: 0,1,2,3 · chordal: cumulative distances · centripetal: cumulative sqrt distances.
inline void cr_knots(float d01, float d12, float d23, cr_mode mode, float* k) {
  if (mode == cr_mode::uniform) {
    k[0] = 0.0f;
    k[1] = 1.0f;
    k[2] = 2.0f;
    k[3] = 3.0f;
    return;
  }
  if (mode == cr_mode::centripetal) {
    d01 = std::sqrt(d01);
    d12 = std::sqrt(d12);
    d23 = std::sqrt(d23);
  }
  // Degenerate spacings are lifted so the knots stay strictly increasing.
  const float eps = 1e-6f;
  d01 = std::max(d01, eps);
  d12 = std::max(d12, eps);
  d23 = std::max(d23, eps);
  k[0] = 0.0f;
  k[1] = d01;
  k[2] = d01 + d12;
  k[3] = d01 + d12 + d23;
}

// Non-uniform Catmull-Rom (Hermite form, tangents weighted by adjacent knot
// spacings). Reduces to the classic uniform Catmull-Rom for equally spaced
// knots; passes through p1 at t=0 and p2 at t=1 for every mode. A pure t-remap
// of the uniform basis cannot express this — the knots must enter the basis.
inline float catmull_rom(const float k[4], float p0, float p1, float p2, float p3, float t) {
  const float h0 = k[1] - k[0];
  const float h1 = k[2] - k[1];
  const float h2 = k[3] - k[2];
  const float d1 = (p1 - p0) / h0;
  const float d2 = (p2 - p1) / h1;
  const float d3 = (p3 - p2) / h2;
  // Tangents scaled by the segment length so the unit-parameter Hermite basis applies.
  const float m1 = ((h1 * d1 + h0 * d2) / (h0 + h1)) * h1;
  const float m2 = ((h2 * d2 + h1 * d3) / (h1 + h2)) * h1;
  const float t2 = t * t;
  const float t3 = t2 * t;
  return (2.0f * t3 - 3.0f * t2 + 1.0f) * p1 + (t3 - 2.0f * t2 + t) * m1 + (-2.0f * t3 + 3.0f * t2) * p2 +
         (t3 - t2) * m2;
}

// Uniform Catmull-Rom convenience. Alpha is interpolated uniformly; it does not
// take part in the OkLab knot distances.
inline float catmull_rom_uniform(float p0, float p1, float p2, float p3, float t) {
  constexpr float k[4] = {0.0f, 1.0f, 2.0f, 3.0f};
  return catmull_rom(k, p0, p1, p2, p3, t);
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
 * @note Produces smoother curves than linear interpolation while passing through p1 and p2
 *       in every mode. Non-uniform modes derive joint knots from the OkLab-space distances
 *       between control points: centripetal prevents cusps and self-intersections; chordal
 *       gives more even spacing.
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

  // Joint knots from the OkLab-space curve; the same parameterization applies
  // to every channel (per-channel knots would break the curve's geometry).
  float k[4];
  details::cr_knots(details::cr_distance(c0, c1), details::cr_distance(c1, c2), details::cr_distance(c2, c3), mode, k);

  core::oklab_t mid{details::catmull_rom(k, c0.template get_index<0>(), c1.template get_index<0>(),
                                         c2.template get_index<0>(), c3.template get_index<0>(), t),
                    details::catmull_rom(k, c0.template get_index<1>(), c1.template get_index<1>(),
                                         c2.template get_index<1>(), c3.template get_index<1>(), t),
                    details::catmull_rom(k, c0.template get_index<2>(), c1.template get_index<2>(),
                                         c2.template get_index<2>(), c3.template get_index<2>(), t)};

  // Alpha interpolation using uniform Catmull-Rom (alpha stays out of the knots)
  return details::lerp_result<Color>(
      mid, details::catmull_rom_uniform(details::src_alpha(p0), details::src_alpha(p1), details::src_alpha(p2),
                                        details::src_alpha(p3), t));
}

}  // namespace colorcpp::operations::interpolate
