/**
 * @file material.hpp
 * @brief Heuristic Material-style palette generator.
 * Lightweight heuristic helpers, not a standards-faithful token pipeline.
 */

#pragma once

#include <colorcpp/algorithms/appearance.hpp>
#include <colorcpp/core/palette_set.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/palette/scales.hpp>

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
  // Material 3: the primary tonal palette is the key colour's own HCT
  // hue and chroma across the full tone ramp (HCT rewiring).
  const auto key = algorithms::appearance::xyz_to_hct(color_cast<core::xyz_t>(base));
  return algorithms::appearance::hct_tonal_palette<Color>(key.hue, key.chroma, count);
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
  // Material 3: secondary keeps the key hue at reduced chroma.
  const auto key = algorithms::appearance::xyz_to_hct(color_cast<core::xyz_t>(base));
  return algorithms::appearance::hct_tonal_palette<Color>(key.hue, key.chroma * 0.5f, count);
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
  // Material 3: tertiary sits 60 degrees from the key hue at key chroma
  // (the old HSL heuristic used 120 degrees).
  const auto key = algorithms::appearance::xyz_to_hct(color_cast<core::xyz_t>(base));
  const float tertiary_hue = std::fmod(key.hue + 60.0f, 360.0f);
  return algorithms::appearance::hct_tonal_palette<Color>(tertiary_hue, key.chroma, count);
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
  // Neutral: the key hue at a tenth of its chroma (near-gray tones).
  const auto key = algorithms::appearance::xyz_to_hct(color_cast<core::xyz_t>(base));
  return algorithms::appearance::hct_tonal_palette<Color>(key.hue, key.chroma * 0.1f, count);
}

/**
 * @brief Generate a heuristic Material-style error tonal palette approximation.
 * @param count Number of tonal variants (default: 10).
 * @return Palette with approximate error role tonal values.
 */
template <typename Color>
core::palette_set<Color> material_error(size_t count = 10) {
  using namespace conversion;
  // Error: Material's baseline #B3261E as the HCT seed.
  const core::rgba8_t seed{179, 38, 30, 255};
  const auto key = algorithms::appearance::xyz_to_hct(color_cast<core::xyz_t>(seed));
  return algorithms::appearance::hct_tonal_palette<Color>(key.hue, key.chroma, count);
}

/**
 * @brief Generate a heuristic Material-style aggregate palette approximation.
 * @param base Base color for the design helper.
 * @param count Number of tonal variants per role (default: 10).
 * @return Fixed material_palette aggregate with palette_set<rgba8_t> roles, normalized to 8-bit RGBA output.
 */
template <typename Color>
material_palette material_design(const Color& base, size_t count = 10) {
  using namespace conversion;
  // Roles are fixed to rgba8_t regardless of the caller's Color type, so the
  // generated tonal palettes are cast color-by-color (palette_set has no
  // converting assignment).
  const auto to_rgba8 = [](const core::palette_set<Color>& pset) {
    core::palette_set<core::rgba8_t> out;
    for (const auto& c : pset) {
      out.add(color_cast<core::rgba8_t>(c));
    }
    return out;
  };

  material_palette palette;
  palette.primary = to_rgba8(material_primary(base, count));
  palette.secondary = to_rgba8(material_secondary(base, count));
  palette.tertiary = to_rgba8(material_tertiary(base, count));
  palette.neutral = to_rgba8(material_neutral(base, count));
  palette.error = material_error<core::rgba8_t>(count);
  return palette;
}

}  // namespace colorcpp::operations::palette
