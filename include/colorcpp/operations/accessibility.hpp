#pragma once

#include <colorcpp/core/core.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::operations::accessibility {

template <typename Color>
float relative_luminance(const Color& c) {
  using namespace conversion;
  auto rgb = color_cast<core::rgbaf_t>(c);

  auto calc = [](float v) { return (v <= 0.03928f) ? (v / 12.92f) : std::pow((v + 0.055f) / 1.055f, 2.4f); };

  float r = calc(rgb.template get_index<0>());
  float g = calc(rgb.template get_index<1>());
  float b = calc(rgb.template get_index<2>());

  return 0.2126f * r + 0.7152f * g + 0.0722f * b;
}

template <typename Color>
auto contrast_text_color(const Color& background) {
  return (relative_luminance(background) > 0.179f) ? constants::black : constants::white;
}

}  // namespace colorcpp::operations::accessibility
