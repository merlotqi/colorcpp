/**
 * @file accessibility.hpp
 * @brief WCAG 2.x relative luminance and contrast helpers (sRGB linearization via @ref conversion::color_cast).
 */

#pragma once

#include <cmath>
#include <colorcpp/core/core.hpp>
#include <colorcpp/operations/conversion.hpp>

/** @brief Luminance and contrast ratio checks per WCAG. */
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

}  // namespace colorcpp::operations::accessibility
