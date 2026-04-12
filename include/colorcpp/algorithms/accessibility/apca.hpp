/**
 * @file apca.hpp
 * @brief APCA (Advanced Perceptual Contrast Algorithm) for WCAG 3 / Silver draft.
 */

#pragma once

#include <cmath>
#include <colorcpp/core/core.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::accessibility {

/**
 * @brief Display-referred luminance Y for APCA from sRGB 8-bit channels.
 *
 * Uses a single 2.4 exponent on normalized channels (pow(c/255, 2.4)), not the IEC 61966-2-1
 * piecewise transfer used by WCAG relative_luminance. Coefficients match the W3 G-4g constants.
 *
 * @see https://www.w3.org/WAI/GL/wiki/Contrast_Ratio
 */
inline float apca_luminance_srgb_uint8(uint8_t r, uint8_t g, uint8_t b) {
  constexpr float k_main_trc = 2.4f;
  constexpr float s_r = 0.2126729f;
  constexpr float s_g = 0.7151522f;
  constexpr float s_b = 0.0721750f;
  auto raised = [&](uint8_t ch) {
    float x = static_cast<float>(ch) / 255.0f;
    return std::pow(x, k_main_trc);
  };
  return s_r * raised(r) + s_g * raised(g) + s_b * raised(b);
}

/**
 * @brief APCA luminance Y for any color c convertible to rgba8_t (alpha ignored for Y).
 * @tparam Color Any color type supported by conversion::color_cast.
 * @param c Input color.
 * @return Luminance Y (0.0 to 1.0).
 */
template <typename Color>
float apca_luminance(const Color& c) {
  using namespace operations::conversion;
  auto rgb = color_cast<core::rgba8_t>(c);
  return apca_luminance_srgb_uint8(static_cast<uint8_t>(rgb.r()), static_cast<uint8_t>(rgb.g()),
                                   static_cast<uint8_t>(rgb.b()));
}

/**
 * @brief APCA lightness contrast Lc from text and background luminance Y (0-1 scale).
 *
 * @param txtY Text luminance from apca_luminance.
 * @param bgY Background luminance, same convention.
 * @return Signed Lc (roughly ±110 max). Positive ≈ dark text on light background;
 *         negative ≈ light text on dark background.
 *
 * Returns 0 on non-finite inputs or Y outside [0, 1.1] (reference implementation guard).
 */
inline float apca_contrast(float txtY, float bgY) {
  constexpr float k_icp_hi = 1.1f;
  if (!std::isfinite(txtY) || !std::isfinite(bgY)) return 0.0f;
  if (txtY < 0.0f || txtY > k_icp_hi || bgY < 0.0f || bgY > k_icp_hi) return 0.0f;

  constexpr float blk_thrs = 0.022f;
  constexpr float blk_clmp = 1.414f;
  constexpr float norm_bg = 0.56f;
  constexpr float norm_txt = 0.57f;
  constexpr float rev_txt = 0.62f;
  constexpr float rev_bg = 0.65f;
  constexpr float scale_bow = 1.14f;
  constexpr float scale_wob = 1.14f;
  constexpr float lo_bow_offset = 0.027f;
  constexpr float lo_wob_offset = 0.027f;
  constexpr float delta_y_min = 0.0005f;
  constexpr float lo_clip = 0.1f;

  auto soft_clamp_y = [blk_thrs, blk_clmp](float y) {
    return (y > blk_thrs) ? y : y + std::pow(blk_thrs - y, blk_clmp);
  };
  txtY = soft_clamp_y(txtY);
  bgY = soft_clamp_y(bgY);

  if (std::fabs(bgY - txtY) < delta_y_min) return 0.0f;

  float sapc = 0.0f;
  float out = 0.0f;
  if (bgY > txtY) {
    sapc = (std::pow(bgY, norm_bg) - std::pow(txtY, norm_txt)) * scale_bow;
    out = (sapc < lo_clip) ? 0.0f : sapc - lo_bow_offset;
  } else {
    sapc = (std::pow(bgY, rev_bg) - std::pow(txtY, rev_txt)) * scale_wob;
    out = (sapc > -lo_clip) ? 0.0f : sapc + lo_wob_offset;
  }
  return out * 100.0f;
}

/**
 * @brief APCA Lc between text and background (order matters; polarity is preserved).
 * @tparam ColorA Text color type.
 * @tparam ColorB Background color type.
 * @param text Text color.
 * @param background Background color.
 * @return Signed Lc value.
 */
template <typename ColorA, typename ColorB>
float apca_contrast(const ColorA& text, const ColorB& background) {
  return apca_contrast(apca_luminance(text), apca_luminance(background));
}

/**
 * @brief True if |apca_contrast(text, background)| is at least min_abs_lc.
 *
 * Use thresholds from the APCA / WCAG 3 draft or tooling you standardize on.
 * @tparam ColorA Text color type.
 * @tparam ColorB Background color type.
 * @param text Text color.
 * @param background Background color.
 * @param min_abs_lc Minimum absolute Lc value.
 * @return true if |Lc| >= min_abs_lc.
 */
template <typename ColorA, typename ColorB>
bool apca_meets_min_abs_lc(const ColorA& text, const ColorB& background, float min_abs_lc) {
  float lc = apca_contrast(text, background);
  return std::fabs(lc) >= min_abs_lc;
}

}  // namespace colorcpp::algorithms::accessibility
