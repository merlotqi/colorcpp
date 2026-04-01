/**
 * @file temperature.hpp
 * @brief Warm and cool color palette generators.
 */

#pragma once

#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/palette/palette_set.hpp>
#include <colorcpp/operations/palette/scales.hpp>

namespace colorcpp::operations::palette {

/**
 * @brief Generate a warm color palette (reds, oranges, yellows).
 * @param base Base color (used for saturation and lightness).
 * @param count Number of colors (default: 5).
 * @return Palette with warm colors.
 * @note Warm hues range from 0° to 60° (red to yellow).
 */
template <typename Color>
auto warm_palette(const Color& base, size_t count = 5) {
  using namespace conversion;
  auto hsl = color_cast<core::hsla_float_t>(base);

  float sat = hsl.template get_index<1>();
  float lit = hsl.template get_index<2>();
  float alpha = hsl.template get_index<3>();

  core::hsla_float_t warm_start{0.0f, sat, lit, alpha};  // Red
  core::hsla_float_t warm_end{60.0f, sat, lit, alpha};   // Yellow

  return visual_scale(color_cast<Color>(warm_start), color_cast<Color>(warm_end), count);
}

/**
 * @brief Generate a cool color palette (blues, greens, purples).
 * @param base Base color (used for saturation and lightness).
 * @param count Number of colors (default: 5).
 * @return Palette with cool colors.
 * @note Cool hues range from 180° to 300° (cyan to purple).
 */
template <typename Color>
auto cool_palette(const Color& base, size_t count = 5) {
  using namespace conversion;
  auto hsl = color_cast<core::hsla_float_t>(base);

  float sat = hsl.template get_index<1>();
  float lit = hsl.template get_index<2>();
  float alpha = hsl.template get_index<3>();

  core::hsla_float_t cool_start{180.0f, sat, lit, alpha};  // Cyan
  core::hsla_float_t cool_end{300.0f, sat, lit, alpha};    // Purple

  return visual_scale(color_cast<Color>(cool_start), color_cast<Color>(cool_end), count);
}

/**
 * @brief Generate a neutral color palette (grays with subtle warm/cool tint).
 * @param base Base color (used for hue tint).
 * @param count Number of colors (default: 5).
 * @return Palette with neutral colors.
 */
template <typename Color>
auto neutral_palette(const Color& base, size_t count = 5) {
  using namespace conversion;
  auto hsl = color_cast<core::hsla_float_t>(base);

  float hue = hsl.template get_index<0>();

  // Create neutral colors with low saturation
  core::hsla_float_t neutral_start{hue, 0.1f, 0.2f, 1.0f};  // Dark neutral
  core::hsla_float_t neutral_end{hue, 0.1f, 0.9f, 1.0f};    // Light neutral

  return visual_scale(color_cast<Color>(neutral_start), color_cast<Color>(neutral_end), count);
}

}  // namespace colorcpp::operations::palette
