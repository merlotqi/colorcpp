#pragma once

#include <cmath>
#include <colorcpp/core/core.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::operations::accessibility {

template <typename Color>
float relative_luminance(const Color& c) {
  using namespace conversion;
  // color_cast<linear_rgbf_t> handles gamma removal for all registered models
  // (sRGB → IEC 61966-2-1 linearization, OkLab/CIELAB → XYZ → linear sRGB, etc.)
  auto lin = color_cast<core::linear_rgbf_t>(c);
  return 0.2126f * lin.template get_index<0>() + 0.7152f * lin.template get_index<1>() +
         0.0722f * lin.template get_index<2>();
}

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

enum class wcag_level {
  aa,
  aaa
};
enum class text_size {
  normal,
  large
};

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

template <typename Color>
auto contrast_text_color(const Color& background) {
  float ratio_black = contrast_ratio(background, constants::black);
  float ratio_white = contrast_ratio(background, constants::white);
  return (ratio_black >= ratio_white) ? constants::black : constants::white;
}

}  // namespace colorcpp::operations::accessibility
