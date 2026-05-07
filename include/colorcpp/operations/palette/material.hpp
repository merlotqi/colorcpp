/**
 * @file material.hpp
 * @brief Heuristic Material-style palette generator.
 * Lightweight heuristic helpers, not a standards-faithful token pipeline.
 */

#pragma once

#include <colorcpp/algorithms/palette/scales.hpp>
#include <colorcpp/core/palette_set.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::operations::palette {

/**
 * @brief Material Design color roles.
 */
struct material_palette {
  core::palette_set<core::rgba8_t> primary;
  core::palette_set<core::rgba8_t> secondary;
  core::palette_set<core::rgba8_t> tertiary;
  core::palette_set<core::rgba8_t> neutral;
  core::palette_set<core::rgba8_t> error;
};

/**
 * @brief Generate a heuristic Material-style primary tonal palette approximation.
 * @param base Base color.
 * @param count Number of tonal variants (default: 10).
 * @return Palette with approximate primary role tonal values.
 */
template <typename Color>
core::palette_set<Color> material_primary(const Color& base, size_t count = 10) {
  using namespace conversion;
  auto hsl = color_cast<core::hsla_float_t>(base);

  float hue = hsl.template get_index<0>();
  float sat = hsl.template get_index<1>();

  // Material primary: high saturation, varying lightness
  core::hsla_float_t primary_start{hue, sat, 0.1f, 1.0f};  // 10
  core::hsla_float_t primary_end{hue, sat, 0.99f, 1.0f};   // 99

  return algorithms::palette::visual_scale(color_cast<Color>(primary_start), color_cast<Color>(primary_end), count);
}

/**
 * @brief Generate a heuristic Material-style secondary tonal palette approximation.
 * @param base Base color.
 * @param count Number of tonal variants (default: 10).
 * @return Palette with approximate secondary role tonal values.
 */
template <typename Color>
core::palette_set<Color> material_secondary(const Color& base, size_t count = 10) {
  using namespace conversion;
  auto hsl = color_cast<core::hsla_float_t>(base);

  // Secondary: shifted hue by 60°, lower saturation
  float secondary_hue = std::fmod(hsl.template get_index<0>() + 60.0f, 360.0f);

  core::hsla_float_t secondary_start{secondary_hue, 0.4f, 0.1f, 1.0f};
  core::hsla_float_t secondary_end{secondary_hue, 0.4f, 0.99f, 1.0f};

  return algorithms::palette::visual_scale(color_cast<Color>(secondary_start), color_cast<Color>(secondary_end), count);
}

/**
 * @brief Generate a heuristic Material-style tertiary tonal palette approximation.
 * @param base Base color.
 * @param count Number of tonal variants (default: 10).
 * @return Palette with approximate tertiary role tonal values.
 */
template <typename Color>
core::palette_set<Color> material_tertiary(const Color& base, size_t count = 10) {
  using namespace conversion;
  auto hsl = color_cast<core::hsla_float_t>(base);

  // Tertiary: shifted hue by 120°, moderate saturation
  float tertiary_hue = std::fmod(hsl.template get_index<0>() + 120.0f, 360.0f);

  core::hsla_float_t tertiary_start{tertiary_hue, 0.5f, 0.1f, 1.0f};
  core::hsla_float_t tertiary_end{tertiary_hue, 0.5f, 0.99f, 1.0f};

  return algorithms::palette::visual_scale(color_cast<Color>(tertiary_start), color_cast<Color>(tertiary_end), count);
}

/**
 * @brief Generate a heuristic Material-style neutral tonal palette approximation.
 * @param base Base color.
 * @param count Number of tonal variants (default: 10).
 * @return Palette with approximate neutral role tonal values.
 */
template <typename Color>
core::palette_set<Color> material_neutral(const Color& base, size_t count = 10) {
  using namespace conversion;
  auto hsl = color_cast<core::hsla_float_t>(base);

  // Neutral: very low saturation
  core::hsla_float_t neutral_start{hsl.template get_index<0>(), 0.05f, 0.1f, 1.0f};
  core::hsla_float_t neutral_end{hsl.template get_index<0>(), 0.05f, 0.99f, 1.0f};

  return algorithms::palette::visual_scale(color_cast<Color>(neutral_start), color_cast<Color>(neutral_end), count);
}

/**
 * @brief Generate a heuristic Material-style error tonal palette approximation.
 * @param count Number of tonal variants (default: 10).
 * @return Palette with approximate error role tonal values.
 */
template <typename Color>
core::palette_set<Color> material_error(size_t count = 10) {
  using namespace conversion;

  // Error: pure red with high saturation
  core::hsla_float_t error_start{0.0f, 0.9f, 0.1f, 1.0f};
  core::hsla_float_t error_end{0.0f, 0.9f, 0.99f, 1.0f};

  return algorithms::palette::visual_scale(color_cast<Color>(error_start), color_cast<Color>(error_end), count);
}

/**
 * @brief Generate a heuristic Material-style aggregate palette approximation.
 * @param base Base color for the design helper.
 * @param count Number of tonal variants per role (default: 10).
 * @return Fixed material_palette aggregate with palette_set<rgba8_t> roles, normalized to 8-bit RGBA output.
 */
template <typename Color>
material_palette material_design(const Color& base, size_t count = 10) {
  material_palette palette;
  palette.primary = material_primary(base, count);
  palette.secondary = material_secondary(base, count);
  palette.tertiary = material_tertiary(base, count);
  palette.neutral = material_neutral(base, count);
  palette.error = material_error<core::rgba8_t>(count);
  return palette;
}

}  // namespace colorcpp::operations::palette
