/**
 * @file hue.hpp
 * @brief Hue-locked interpolation in OkLCH space.
 */

#pragma once

#include <algorithm>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/interpolate/lerp.hpp>

namespace colorcpp::operations::interpolate {

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
  float hue_a = ca.template get_index<2>();
  float hue_b = cb.template get_index<2>();
  // C ≈ 0 → hue missing (issue #51): locking to a powerless start hue falls
  // through to the end hue instead of freezing atan2 noise.
  details::substitute_powerless_hue(ca, cb, hue_a, hue_b);
  float H = hue_locked ? hue_a : details::lerp_angle_deg(hue_a, hue_b, t);

  core::oklch_t mid{L, C, H};

  return details::lerp_result<Color>(mid, details::lerp_f(details::src_alpha(a), details::src_alpha(b), t));
}

}  // namespace colorcpp::operations::interpolate
