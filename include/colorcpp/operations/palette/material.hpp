/**
 * @file material.hpp
 * @brief Material Design 3 color palette generator.
 */

#pragma once

#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/palette/palette_set.hpp>
#include <colorcpp/operations/palette/scales.hpp>

namespace colorcpp::operations::palette {

/**
 * @brief Material Design color roles.
 */
struct material_palette {
  palette_set<core::rgba8_t> primary;
  palette_set<core::rgba8_t> secondary;
  palette_set<core::rgba8_t> tertiary;
  palette_set<core::rgba8_t> neutral;
  palette_set<core::rgba8_t> error;
};

/**
 * @brief Generate Material Design 3 primary color palette.
 * @param base Base color.
 * @param count Number of tonal variants (default: 10).
 * @return Palette with primary colors (10-99 tonal values).
 */
template <typename Color>
auto material_primary(const Color& base, size_t count = 10) {
  using namespace conversion;
  auto hsl = color_cast<core::hsla_float_t>(base);

  float hue = hsl.template get_index<0>();
  float sat = hsl.template get_index<1>();

  // Material primary: high saturation, varying lightness
  core::hsla_float_t primary_start{hue, sat, 0.1f, 1.0f};   // 10
  core::hsla_float_t primary_end{hue, sat, 0.99f, 1.0f};    // 99

  return visual_scale(color_cast<Color>(primary_start), color_cast<Color>(primary_end), count);
}

/**
 * @brief Generate Material Design 3 secondary color palette.
 * @param base Base color.
 * @param count Number of tonal variants (default: 10).
 * @return Palette with secondary colors.
 */
template <typename Color>
auto material_secondary(const Color& base, size_t count = 10) {
  using namespace conversion;
  auto hsl = color_cast<core::hsla_float_t>(base);

  float hue = hsl.template get_index<0>();
  // Secondary: shifted hue by 60°, lower saturation
  float secondary_hue = std::fmod(hue + 60.0f, 360.0f);

  core::hsla_float_t secondary_start{secondary_hue, 0.4f, 0.1f, 1.0f};
  core::hsla_float_t secondary_end{secondary_hue, 0.4f, 0.99f, 1.0f};

  return visual_scale(color_cast<Color>(secondary_start), color_cast<Color>(secondary_end), count);
}

/**
 * @brief Generate Material Design 3 tertiary color palette.
 * @param base Base color.
 * @param count Number of tonal variants (default: 10).
 * @return Palette with tertiary colors.
 */
template <typename Color>
auto material_tertiary(const Color& base, size_t count = 10) {
  using namespace conversion;
  auto hsl = color_cast<core::hsla_float_t>(base);

  float hue = hsl.template get_index<0>();
  // Tertiary: shifted hue by 120°, moderate saturation
  float tertiary_hue = std::fmod(hue + 120.0f, 360.0f);

  core::hsla_float_t tertiary_start{tertiary_hue, 0.5f, 0.1f, 1.0f};
  core::hsla_float_t tertiary_end{tertiary_hue, 0.5f, 0.99f, 1.0f};

  return visual_scale(color_cast<Color>(tertiary_start), color_cast<Color>(tertiary_end), count);
}

/**
 * @brief Generate Material Design 3 neutral color palette.
 * @param base Base color.
 * @param count Number of tonal variants (default: 10).
 * @return Palette with neutral colors.
 */
template <typename Color>
auto material_neutral(const Color& base, size_t count = 10) {
  using namespace conversion;
  auto hsl = color_cast<core::hsla_float_t>(base);

  float hue = hsl.template get_index<0>();
  // Neutral: very low saturation
  core::hsla_float_t neutral_start{hue, 0.05f, 0.1f, 1.0f};
  core::hsla_float_t neutral_end{hue, 0.05f, 0.99f, 1.0f};

  return visual_scale(color_cast<Color>(neutral_start), color_cast<Color>(neutral_end), count);
}

/**
 * @brief Generate Material Design 3 error color palette.
 * @param count Number of tonal variants (default: 10).
 * @return Palette with error colors (red-based).
 */
template <typename Color>
auto material_error(size_t count = 10) {
  using namespace conversion;

  // Error: pure red with high saturation
  core::hsla_float_t error_start{0.0f, 0.9f, 0.1f, 1.0f};
  core::hsla_float_t error_end{0.0f, 0.9f, 0.99f, 1.0f};

  return visual_scale(color_cast<Color>(error_start), color_cast<Color>(error_end), count);
}

/**
 * @brief Generate complete Material Design 3 color system.
 * @param base Base color for the design system.
 * @param count Number of tonal variants per role (default: 10).
 * @return Complete Material palette with all color roles.
 */
template <typename Color>
auto material_design(const Color& base, size_t count = 10) {
  material_palette palette;
  palette.primary = material_primary(base, count);
  palette.secondary = material_secondary(base, count);
  palette.tertiary = material_tertiary(base, count);
  palette.neutral = material_neutral(base, count);
  palette.error = material_error<core::rgba8_t>(count);
  return palette;
}

}  // namespace colorcpp::operations::palette
