/**
 * @file cubic.hpp
 * @brief Cubic Hermite interpolation in OkLab space.
 */

#pragma once

#include <algorithm>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/interpolate/lerp.hpp>

namespace colorcpp::operations::interpolate {

namespace details {

// Cubic Hermite interpolation
inline float cubic_hermite(float p0, float p1, float p2, float p3, float t, float tension = 0.0f) {
  float t2 = t * t;
  float t3 = t2 * t;
  float m0 = (1.0f - tension) * (p2 - p0) * 0.5f;
  float m1 = (1.0f - tension) * (p3 - p1) * 0.5f;
  return (2.0f * t3 - 3.0f * t2 + 1.0f) * p1 + (t3 - 2.0f * t2 + t) * m0 + (-2.0f * t3 + 3.0f * t2) * p2 +
         (t3 - t2) * m1;
}

}  // namespace details

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

  // Alpha interpolation with the same Hermite basis
  return details::lerp_result<Color>(
      mid, details::cubic_hermite(details::src_alpha(p0), details::src_alpha(p1), details::src_alpha(p2),
                                  details::src_alpha(p3), t, tension));
}

}  // namespace colorcpp::operations::interpolate
