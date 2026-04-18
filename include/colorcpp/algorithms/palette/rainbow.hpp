/**
 * @file rainbow.hpp
 * @brief Rainbow palette generator.
 */

#pragma once

#include <colorcpp/core/palette_set.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::palette {

/**
 * @brief Generate a rainbow palette with evenly distributed hues.
 * @param count Number of colors (default: 7).
 * @return Palette with rainbow colors.
 */
template <typename Color>
auto rainbow(size_t count = 7) {
  using namespace operations::conversion;

  core::palette_set<Color> p;
  if (count == 0) return p;

  for (size_t i = 0; i < count; ++i) {
    float hue = (360.0f * i) / count;
    core::hsla_float_t hsl{hue, 1.0f, 0.5f, 1.0f};
    p.add(color_cast<Color>(hsl));
  }
  return p;
}

/**
 * @brief Generate a rainbow palette with custom saturation and lightness.
 * @param count Number of colors.
 * @param saturation Saturation value [0, 1].
 * @param lightness Lightness value [0, 1].
 * @return Palette with rainbow colors.
 */
template <typename Color>
auto rainbow_custom(size_t count, float saturation, float lightness) {
  using namespace operations::conversion;

  core::palette_set<Color> p;
  if (count == 0) return p;

  saturation = std::max(0.0f, std::min(1.0f, saturation));
  lightness = std::max(0.0f, std::min(1.0f, lightness));

  for (size_t i = 0; i < count; ++i) {
    float hue = (360.0f * i) / count;
    core::hsla_float_t hsl{hue, saturation, lightness, 1.0f};
    p.add(color_cast<Color>(hsl));
  }
  return p;
}

}  // namespace colorcpp::algorithms::palette
