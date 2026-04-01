/**
 * @file wcag.hpp
 * @brief WCAG 2.x relative luminance and contrast ratio.
 */

#pragma once

#include <cmath>
#include <colorcpp/core/core.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::accessibility {

/**
 * @brief WCAG relative luminance (Rec. 709 coefficients on linear sRGB).
 * @tparam Color Any color type supported by conversion::color_cast.
 * @param c Input color.
 * @return Relative luminance Y (0.0 to 1.0).
 */
template <typename Color>
float relative_luminance(const Color& c) {
  using namespace operations::conversion;
  // color_cast<linear_rgbf_t> handles gamma removal for all registered models
  auto lin = color_cast<core::linear_rgbf_t>(c);
  return 0.2126f * lin.template get_index<0>() + 0.7152f * lin.template get_index<1>() +
         0.0722f * lin.template get_index<2>();
}

/**
 * @brief Contrast ratio (L_max + 0.05) / (L_min + 0.05) between two colors.
 * @tparam ColorA First color type.
 * @tparam ColorB Second color type.
 * @param a First color.
 * @param b Second color.
 * @return Contrast ratio (1.0 to 21.0).
 */
template <typename ColorA, typename ColorB>
float contrast_ratio(const ColorA& a, const ColorB& b) {
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
  aa,  // Level AA compliance
  aaa  // Level AAA compliance (stricter)
};

/** @brief Normal vs large text minimum ratios (WCAG). */
enum class text_size {
  normal,  // Normal text (< 18pt or < 14pt bold)
  large    // Large text (>= 18pt or >= 14pt bold)
};

/**
 * @brief True if contrast_ratio meets WCAG for level and size.
 * @tparam ColorA Foreground color type.
 * @tparam ColorB Background color type.
 * @param fg Foreground (text) color.
 * @param bg Background color.
 * @param level WCAG compliance level (default: AA).
 * @param size Text size (default: normal).
 * @return true if compliant.
 */
template <typename ColorA, typename ColorB>
bool is_wcag_compliant(const ColorA& fg, const ColorB& bg, wcag_level level = wcag_level::aa,
                       text_size size = text_size::normal) {
  float ratio = contrast_ratio(fg, bg);
  bool is_large = (size == text_size::large);
  float threshold = 4.5f;

  if (level == wcag_level::aa && is_large) threshold = 3.0f;
  if (level == wcag_level::aaa && !is_large) threshold = 7.0f;
  // wcag_level::aaa + large → 4.5 (same as aa normal, already default)

  return ratio >= threshold;
}

/**
 * @brief Choose black or white whichever contrasts more with background.
 * @tparam Color Background color type.
 * @param background Background color.
 * @return constants::black or constants::white.
 */
template <typename Color>
auto contrast_text_color(const Color& background) {
  float ratio_black = contrast_ratio(background, constants::black);
  float ratio_white = contrast_ratio(background, constants::white);
  return (ratio_black >= ratio_white) ? constants::black : constants::white;
}

}  // namespace colorcpp::algorithms::accessibility
