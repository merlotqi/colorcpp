/**
 * @file accessibility.hpp
 * @brief WCAG 2.x relative luminance / contrast ratio, plus optional APCA Lc (WCAG 3 / Silver draft style).
 */

#pragma once

#include <cmath>
#include <colorcpp/core/core.hpp>
#include <colorcpp/operations/conversion.hpp>

/**
 * @brief WCAG 2.x metrics and APCA (Advanced Perceptual Contrast Algorithm) as parallel APIs.
 * @note Do not mix WCAG 2 @ref contrast_ratio with APCA @ref apca_contrast thresholds; they use different scales.
 */
namespace colorcpp::operations::accessibility {

/** @brief WCAG relative luminance (Rec. 709 coefficients on linear sRGB). */
template <typename Color>
float relative_luminance(const Color& c) {
  using namespace conversion;
  // color_cast<linear_rgbf_t> handles gamma removal for all registered models
  // (sRGB → IEC 61966-2-1 linearization, OkLab/CIELAB → XYZ → linear sRGB, etc.)
  auto lin = color_cast<core::linear_rgbf_t>(c);
  return 0.2126f * lin.template get_index<0>() + 0.7152f * lin.template get_index<1>() +
         0.0722f * lin.template get_index<2>();
}

/** @brief Contrast ratio @f$(L_{\max}+0.05)/(L_{\min}+0.05)@f$ between two colors. */
template <typename Color>
float contrast_ratio(const Color& a, const Color& b) {
  float la = relative_luminance(a);
  float lb = relative_luminance(b);
  if (la < lb) {
    float tmp = la;
    la = lb;
    lb = tmp;
  }
  return (la + 0.05f) / (lb + 0.05f);
}

/** @brief WCAG 2 AA vs AAA thresholds. */
enum class wcag_level {
  aa,
  aaa
};

/** @brief Normal vs large text minimum ratios (WCAG). */
enum class text_size {
  normal,
  large
};

/** @brief True if @ref contrast_ratio meets WCAG for @p level and @p size. */
template <typename Color>
bool is_wcag_compliant(const Color& fg, const Color& bg, wcag_level level = wcag_level::aa,
                       text_size size = text_size::normal) {
  float ratio = contrast_ratio(fg, bg);
  bool is_large = (size == text_size::large);
  float threshold = 4.5f;

  if (level == wcag_level::aa && is_large) threshold = 3.0f;
  if (level == wcag_level::aaa && !is_large) threshold = 7.0f;
  // wcag_level::aaa + large → 4.5 (same as aa normal, already default)

  return ratio >= threshold;
}

/** @brief @ref constants::black or @ref constants::white whichever contrasts more with @p background. */
template <typename Color>
auto contrast_text_color(const Color& background) {
  float ratio_black = contrast_ratio(background, constants::black);
  float ratio_white = contrast_ratio(background, constants::white);
  return (ratio_black >= ratio_white) ? constants::black : constants::white;
}

// --- APCA (W3 licensed reference: SAPC-8 / 0.0.98G-4g, apca-w3 style) ---

/**
 * @brief Display-referred luminance Y for APCA from sRGB 8-bit channels.
 *
 * Uses a single 2.4 exponent on normalized channels (@c pow(c/255, 2.4)), **not** the IEC 61966-2-1
 * piecewise transfer used by @ref relative_luminance. Coefficients match the W3 G-4g constants.
 *
 * @see https://www.w3.org/WAI/GL/wiki/Contrast_Ratio and Silver community resources on APCA.
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
 * @brief APCA luminance Y for any color @p c convertible to @c rgba8_t (alpha ignored for Y).
 */
template <typename Color>
float apca_luminance(const Color& c) {
  using namespace conversion;
  auto rgb = color_cast<core::rgba8_t>(c);
  return apca_luminance_srgb_uint8(static_cast<uint8_t>(rgb.r()), static_cast<uint8_t>(rgb.g()),
                                   static_cast<uint8_t>(rgb.b()));
}

/**
 * @brief APCA lightness contrast Lc from text and background luminance Y (0–1 scale).
 *
 * @param txtY Text luminance from @ref apca_luminance (or @ref apca_luminance_srgb_uint8).
 * @param bgY Background luminance, same convention.
 * @return Signed Lc (roughly ±110 max). **Positive** ≈ dark text on light background; **negative** ≈ light text on
 *         dark background. Magnitude is not comparable to WCAG 2 contrast ratio (e.g. |Lc| ≈ 60 is often discussed
 *         as a ballpark near old 4.5:1 for body text; use published APCA lookup tables for production thresholds).
 *
 * Returns @c 0 on non-finite inputs or Y outside [0, 1.1] (reference implementation guard).
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

  auto soft_clamp_y = [](float y) { return (y > blk_thrs) ? y : y + std::pow(blk_thrs - y, blk_clmp); };
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
 * @brief APCA Lc between @p text and @p background (order matters; polarity is preserved).
 */
template <typename Color>
float apca_contrast(const Color& text, const Color& background) {
  return apca_contrast(apca_luminance(text), apca_luminance(background));
}

/**
 * @brief True if |@ref apca_contrast(text, background)| is at least @p min_abs_lc.
 *
 * Use thresholds from the APCA / WCAG 3 draft or tooling you standardize on; this library does not fix a single
 * legal minimum because specifications are still evolving.
 */
template <typename Color>
bool apca_meets_min_abs_lc(const Color& text, const Color& background, float min_abs_lc) {
  float lc = apca_contrast(text, background);
  return std::fabs(lc) >= min_abs_lc;
}

}  // namespace colorcpp::operations::accessibility
