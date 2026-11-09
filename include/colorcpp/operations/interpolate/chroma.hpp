/**
 * @file chroma.hpp
 * @brief Chroma-preserving interpolation in OkLCH space.
 */

#pragma once

#include <algorithm>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/interpolate/gamut.hpp>
#include <colorcpp/operations/interpolate/lerp.hpp>

namespace colorcpp::operations::interpolate {

enum class chroma_mode {
  lerp,          ///< Interpolate chroma linearly between the two colors
  preserve,      ///< Ramp chroma up to max(Ca, Cb) and back (C(0)=Ca, C(0.5)=max, C(1)=Cb)
  preserve_safe  ///< Preserve chroma at max(Ca, Cb) but clamp to sRGB gamut boundary
};

/**
 * @brief Chroma-preserving interpolation in OkLCH space.
 * @param a Start color
 * @param b End color
 * @param t Interpolation factor in [0, 1] (clamped).
 * @param mode Chroma interpolation mode (lerp, preserve, or preserve_safe).
 * @note In preserve mode, C ramps continuously: C(0)=Ca, C(1)=Cb and the
 *       interior bulges up to max(Ca, Cb) at t = 0.5 (issue #81).
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
  float hue_a = ca.template get_index<2>();
  float hue_b = cb.template get_index<2>();
  details::substitute_powerless_hue(ca, cb, hue_a, hue_b);  // C ≈ 0 → hue missing (issue #51)
  float H = details::lerp_angle_deg(hue_a, hue_b, t);

  // Determine chroma based on mode
  float C;
  switch (mode) {
    case chroma_mode::lerp:
      C = details::lerp_f(ca.template get_index<1>(), cb.template get_index<1>(), t);
      break;
    case chroma_mode::preserve: {
      // Continuous peak ramp (issue #81): the old constant max(Ca, Cb)
      // stepped at the exact endpoints, where the short-circuits return
      // Ca / Cb. C(t) = lerp(Ca, Cb, t) + (max - lerp) * 4t(1-t) keeps
      // C(0) = Ca, C(1) = Cb and C(0.5) = max(Ca, Cb).
      const float ca_c = ca.template get_index<1>();
      const float cb_c = cb.template get_index<1>();
      const float base = details::lerp_f(ca_c, cb_c, t);
      C = base + (std::max(ca_c, cb_c) - base) * 4.0f * t * (1.0f - t);
      break;
    }
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

  return details::lerp_result<Color>(mid, details::lerp_f(details::src_alpha(a), details::src_alpha(b), t));
}

}  // namespace colorcpp::operations::interpolate
