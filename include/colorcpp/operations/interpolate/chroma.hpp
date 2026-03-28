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

}  // namespace colorcpp::operations::interpolate
