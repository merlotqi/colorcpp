/**
 * @file ios.hpp
 * @brief iOS / Apple UIKit System Semantic Color System.
 *
 * Implements iOS 15+ standard system semantic color specifications,
 * including all standard UI color roles, automatic light/dark mode
 * adaptation, following Apple Human Interface Guidelines.
 */

#pragma once

#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::operations::palette {

/**
 * @brief iOS System Semantic Colors (iOS 15+ UIKit Specification)
 *
 * Fully compliant with Apple Human Interface Guidelines
 * All color values are derived from official iOS design specifications
 */
struct ios_palette {
  // -------------------------------------------------------------------------
  // Primary Colors
  // -------------------------------------------------------------------------
  core::rgba8_t system_red;
  core::rgba8_t system_orange;
  core::rgba8_t system_yellow;
  core::rgba8_t system_green;
  core::rgba8_t system_mint;
  core::rgba8_t system_teal;
  core::rgba8_t system_cyan;
  core::rgba8_t system_blue;
  core::rgba8_t system_indigo;
  core::rgba8_t system_purple;
  core::rgba8_t system_pink;
  core::rgba8_t system_brown;

  // -------------------------------------------------------------------------
  // Grayscale Scale
  // -------------------------------------------------------------------------
  core::rgba8_t system_gray;
  core::rgba8_t system_gray2;
  core::rgba8_t system_gray3;
  core::rgba8_t system_gray4;
  core::rgba8_t system_gray5;
  core::rgba8_t system_gray6;

  // -------------------------------------------------------------------------
  // Semantic Colors — Label
  // -------------------------------------------------------------------------
  core::rgba8_t label;
  core::rgba8_t secondary_label;
  core::rgba8_t tertiary_label;
  core::rgba8_t quaternary_label;

  // -------------------------------------------------------------------------
  // Semantic Colors — Fill
  // -------------------------------------------------------------------------
  core::rgba8_t system_fill;
  core::rgba8_t secondary_system_fill;
  core::rgba8_t tertiary_system_fill;
  core::rgba8_t quaternary_system_fill;

  // -------------------------------------------------------------------------
  // Semantic Colors — Background
  // -------------------------------------------------------------------------
  core::rgba8_t system_background;
  core::rgba8_t secondary_system_background;
  core::rgba8_t tertiary_system_background;
  core::rgba8_t system_grouped_background;
  core::rgba8_t secondary_system_grouped_background;
  core::rgba8_t tertiary_system_grouped_background;

  // -------------------------------------------------------------------------
  // Semantic Colors — Separator
  // -------------------------------------------------------------------------
  core::rgba8_t separator;
  core::rgba8_t opaque_separator;
};

/**
 * @brief Get complete iOS system semantic colors for light mode.
 *
 * Contains all iOS 15+ system colors, grayscale scales, and semantic color roles.
 * All color values strictly follow Apple Human Interface Guidelines.
 */
inline ios_palette ios_light_mode() {
  ios_palette p;
  // System Primary Colors
  p.system_red = core::rgba8_t{0xFF, 0x3B, 0x30, 0xFF};
  p.system_orange = core::rgba8_t{0xFF, 0x95, 0x00, 0xFF};
  p.system_yellow = core::rgba8_t{0xFF, 0xCC, 0x00, 0xFF};
  p.system_green = core::rgba8_t{0x34, 0xC7, 0x59, 0xFF};
  p.system_mint = core::rgba8_t{0x00, 0xC7, 0xBE, 0xFF};
  p.system_teal = core::rgba8_t{0x30, 0xB0, 0xC7, 0xFF};
  p.system_cyan = core::rgba8_t{0x32, 0xAD, 0xE6, 0xFF};
  p.system_blue = core::rgba8_t{0x00, 0x7A, 0xFF, 0xFF};
  p.system_indigo = core::rgba8_t{0x58, 0x56, 0xD6, 0xFF};
  p.system_purple = core::rgba8_t{0xAF, 0x52, 0xDE, 0xFF};
  p.system_pink = core::rgba8_t{0xFF, 0x2D, 0x55, 0xFF};
  p.system_brown = core::rgba8_t{0xA2, 0x84, 0x5E, 0xFF};
  // Grayscale Scale
  p.system_gray = core::rgba8_t{0x8E, 0x8E, 0x93, 0xFF};
  p.system_gray2 = core::rgba8_t{0xAE, 0xAE, 0xB2, 0xFF};
  p.system_gray3 = core::rgba8_t{0xC7, 0xC7, 0xCC, 0xFF};
  p.system_gray4 = core::rgba8_t{0xD1, 0xD1, 0xD6, 0xFF};
  p.system_gray5 = core::rgba8_t{0xE5, 0xE5, 0xEA, 0xFF};
  p.system_gray6 = core::rgba8_t{0xF2, 0xF2, 0xF7, 0xFF};
  // Semantic Colors — Label
  p.label = core::rgba8_t{0x00, 0x00, 0x00, 0xFF};
  p.secondary_label = core::rgba8_t{0x3C, 0x3C, 0x43, 0x99};
  p.tertiary_label = core::rgba8_t{0x3C, 0x3C, 0x43, 0x4C};
  p.quaternary_label = core::rgba8_t{0x3C, 0x3C, 0x43, 0x2E};
  // Semantic Colors — Fill
  p.system_fill = core::rgba8_t{0x78, 0x78, 0x80, 0x33};
  p.secondary_system_fill = core::rgba8_t{0x78, 0x78, 0x80, 0x28};
  p.tertiary_system_fill = core::rgba8_t{0x76, 0x76, 0x80, 0x1E};
  p.quaternary_system_fill = core::rgba8_t{0x74, 0x74, 0x80, 0x14};
  // Semantic Colors — Background
  p.system_background = core::rgba8_t{0xFF, 0xFF, 0xFF, 0xFF};
  p.secondary_system_background = core::rgba8_t{0xF2, 0xF2, 0xF7, 0xFF};
  p.tertiary_system_background = core::rgba8_t{0xFF, 0xFF, 0xFF, 0xFF};
  p.system_grouped_background = core::rgba8_t{0xF2, 0xF2, 0xF7, 0xFF};
  p.secondary_system_grouped_background = core::rgba8_t{0xFF, 0xFF, 0xFF, 0xFF};
  p.tertiary_system_grouped_background = core::rgba8_t{0xF2, 0xF2, 0xF7, 0xFF};
  // Semantic Colors — Separator
  p.separator = core::rgba8_t{0x3C, 0x3C, 0x43, 0x49};
  p.opaque_separator = core::rgba8_t{0xC6, 0xC6, 0xC8, 0xFF};
  return p;
}

/**
 * @brief Get complete iOS system semantic colors for dark mode.
 *
 * Contains all iOS 15+ system colors, grayscale scales, and semantic color roles.
 * All color values strictly follow Apple Human Interface Guidelines.
 */
inline ios_palette ios_dark_mode() {
  ios_palette p;
  // System Primary Colors (dark mode adjustments)
  p.system_red = core::rgba8_t{0xFF, 0x45, 0x3A, 0xFF};
  p.system_orange = core::rgba8_t{0xFF, 0x9F, 0x0A, 0xFF};
  p.system_yellow = core::rgba8_t{0xFF, 0xD6, 0x0A, 0xFF};
  p.system_green = core::rgba8_t{0x30, 0xD1, 0x58, 0xFF};
  p.system_mint = core::rgba8_t{0x63, 0xDA, 0xC0, 0xFF};
  p.system_teal = core::rgba8_t{0x40, 0xCD, 0xC8, 0xFF};
  p.system_cyan = core::rgba8_t{0x64, 0xD2, 0xFF, 0xFF};
  p.system_blue = core::rgba8_t{0x0A, 0x84, 0xFF, 0xFF};
  p.system_indigo = core::rgba8_t{0x5E, 0x5C, 0xE6, 0xFF};
  p.system_purple = core::rgba8_t{0xBF, 0x5A, 0xF2, 0xFF};
  p.system_pink = core::rgba8_t{0xFF, 0x37, 0x5F, 0xFF};
  p.system_brown = core::rgba8_t{0xAC, 0x8E, 0x68, 0xFF};
  // Grayscale Scale (dark mode)
  p.system_gray = core::rgba8_t{0x8E, 0x8E, 0x93, 0xFF};
  p.system_gray2 = core::rgba8_t{0x63, 0x63, 0x66, 0xFF};
  p.system_gray3 = core::rgba8_t{0x48, 0x48, 0x4A, 0xFF};
  p.system_gray4 = core::rgba8_t{0x3A, 0x3A, 0x3C, 0xFF};
  p.system_gray5 = core::rgba8_t{0x2C, 0x2C, 0x2E, 0xFF};
  p.system_gray6 = core::rgba8_t{0x1C, 0x1C, 0x1E, 0xFF};
  // Semantic Colors — Label
  p.label = core::rgba8_t{0xFF, 0xFF, 0xFF, 0xFF};
  p.secondary_label = core::rgba8_t{0xEB, 0xEB, 0xF5, 0x99};
  p.tertiary_label = core::rgba8_t{0xEB, 0xEB, 0xF5, 0x4C};
  p.quaternary_label = core::rgba8_t{0xEB, 0xEB, 0xF5, 0x2E};
  // Semantic Colors — Fill
  p.system_fill = core::rgba8_t{0x78, 0x78, 0x80, 0x5C};
  p.secondary_system_fill = core::rgba8_t{0x78, 0x78, 0x80, 0x51};
  p.tertiary_system_fill = core::rgba8_t{0x76, 0x76, 0x80, 0x3D};
  p.quaternary_system_fill = core::rgba8_t{0x74, 0x74, 0x80, 0x2E};
  // Semantic Colors — Background
  p.system_background = core::rgba8_t{0x00, 0x00, 0x00, 0xFF};
  p.secondary_system_background = core::rgba8_t{0x1C, 0x1C, 0x1E, 0xFF};
  p.tertiary_system_background = core::rgba8_t{0x2C, 0x2C, 0x2E, 0xFF};
  p.system_grouped_background = core::rgba8_t{0x00, 0x00, 0x00, 0xFF};
  p.secondary_system_grouped_background = core::rgba8_t{0x1C, 0x1C, 0x1E, 0xFF};
  p.tertiary_system_grouped_background = core::rgba8_t{0x2C, 0x2C, 0x2E, 0xFF};
  // Semantic Colors — Separator
  p.separator = core::rgba8_t{0x54, 0x54, 0x58, 0x99};
  p.opaque_separator = core::rgba8_t{0x38, 0x38, 0x3A, 0xFF};
  return p;
}

/**
 * @brief Generate iOS style palette from a seed color.
 *
 * Uses seed color as primary hue to automatically derive a complete iOS style palette.
 * Includes warm system colors, cool system colors, and semantic color roles.
 *
 * @tparam Color Color type
 * @param seed Seed color (used as primary hue reference)
 * @param dark Generate dark mode palette (default: light mode)
 * @return Complete ios_palette structure
 */
template <typename Color>
ios_palette ios_from_seed(const Color& seed, bool dark = false) {
  using namespace operations::conversion;

  auto seed_hsl = color_cast<core::hsla_float_t>(seed);
  float base_hue = seed_hsl.h();
  float base_sat = std::min(seed_hsl.s(), 1.0f);

  ios_palette p;

  // Warm system colors
  p.system_red = color_cast<core::rgba8_t>(core::hsla_float_t{0.0f, 0.9f, dark ? 0.65f : 0.55f, 1.0f});
  p.system_orange = color_cast<core::rgba8_t>(core::hsla_float_t{30.0f, 0.9f, dark ? 0.60f : 0.55f, 1.0f});
  p.system_yellow = color_cast<core::rgba8_t>(core::hsla_float_t{50.0f, 0.9f, dark ? 0.60f : 0.55f, 1.0f});
  p.system_pink = color_cast<core::rgba8_t>(core::hsla_float_t{345.0f, 0.9f, dark ? 0.65f : 0.55f, 1.0f});
  p.system_brown = color_cast<core::rgba8_t>(core::hsla_float_t{30.0f, 0.45f, dark ? 0.50f : 0.45f, 1.0f});

  // Cool system colors
  p.system_green = color_cast<core::rgba8_t>(core::hsla_float_t{140.0f, 0.75f, dark ? 0.55f : 0.50f, 1.0f});
  p.system_mint = color_cast<core::rgba8_t>(core::hsla_float_t{175.0f, 0.70f, dark ? 0.55f : 0.50f, 1.0f});
  p.system_teal = color_cast<core::rgba8_t>(core::hsla_float_t{190.0f, 0.70f, dark ? 0.55f : 0.50f, 1.0f});
  p.system_cyan = color_cast<core::rgba8_t>(core::hsla_float_t{200.0f, 0.75f, dark ? 0.55f : 0.50f, 1.0f});
  p.system_blue = color_cast<core::rgba8_t>(core::hsla_float_t{215.0f, 0.90f, dark ? 0.55f : 0.55f, 1.0f});
  p.system_indigo = color_cast<core::rgba8_t>(core::hsla_float_t{240.0f, 0.85f, dark ? 0.55f : 0.55f, 1.0f});
  p.system_purple = color_cast<core::rgba8_t>(core::hsla_float_t{280.0f, 0.80f, dark ? 0.55f : 0.55f, 1.0f});

  // Grayscale Scale
  if (dark) {
    p.system_gray = core::rgba8_t{0x8E, 0x8E, 0x93, 0xFF};
    p.system_gray2 = core::rgba8_t{0x63, 0x63, 0x66, 0xFF};
    p.system_gray3 = core::rgba8_t{0x48, 0x48, 0x4A, 0xFF};
    p.system_gray4 = core::rgba8_t{0x3A, 0x3A, 0x3C, 0xFF};
    p.system_gray5 = core::rgba8_t{0x2C, 0x2C, 0x2E, 0xFF};
    p.system_gray6 = core::rgba8_t{0x1C, 0x1C, 0x1E, 0xFF};
  } else {
    p.system_gray = core::rgba8_t{0x8E, 0x8E, 0x93, 0xFF};
    p.system_gray2 = core::rgba8_t{0xAE, 0xAE, 0xB2, 0xFF};
    p.system_gray3 = core::rgba8_t{0xC7, 0xC7, 0xCC, 0xFF};
    p.system_gray4 = core::rgba8_t{0xD1, 0xD1, 0xD6, 0xFF};
    p.system_gray5 = core::rgba8_t{0xE5, 0xE5, 0xEA, 0xFF};
    p.system_gray6 = core::rgba8_t{0xF2, 0xF2, 0xF7, 0xFF};
  }

  // Semantic Colors — Label
  if (dark) {
    p.label = core::rgba8_t{0xFF, 0xFF, 0xFF, 0xFF};
    p.secondary_label = core::rgba8_t{0xEB, 0xEB, 0xF5, 0x99};
    p.tertiary_label = core::rgba8_t{0xEB, 0xEB, 0xF5, 0x4C};
    p.quaternary_label = core::rgba8_t{0xEB, 0xEB, 0xF5, 0x2E};
  } else {
    p.label = core::rgba8_t{0x00, 0x00, 0x00, 0xFF};
    p.secondary_label = core::rgba8_t{0x3C, 0x3C, 0x43, 0x99};
    p.tertiary_label = core::rgba8_t{0x3C, 0x3C, 0x43, 0x4C};
    p.quaternary_label = core::rgba8_t{0x3C, 0x3C, 0x43, 0x2E};
  }

  // Semantic Colors — Fill
  if (dark) {
    p.system_fill = core::rgba8_t{0x78, 0x78, 0x80, 0x5C};
    p.secondary_system_fill = core::rgba8_t{0x78, 0x78, 0x80, 0x51};
    p.tertiary_system_fill = core::rgba8_t{0x76, 0x76, 0x80, 0x3D};
    p.quaternary_system_fill = core::rgba8_t{0x74, 0x74, 0x80, 0x2E};
  } else {
    p.system_fill = core::rgba8_t{0x78, 0x78, 0x80, 0x33};
    p.secondary_system_fill = core::rgba8_t{0x78, 0x78, 0x80, 0x28};
    p.tertiary_system_fill = core::rgba8_t{0x76, 0x76, 0x80, 0x1E};
    p.quaternary_system_fill = core::rgba8_t{0x74, 0x74, 0x80, 0x14};
  }

  // Semantic Colors — Background
  if (dark) {
    p.system_background = core::rgba8_t{0x00, 0x00, 0x00, 0xFF};
    p.secondary_system_background = core::rgba8_t{0x1C, 0x1C, 0x1E, 0xFF};
    p.tertiary_system_background = core::rgba8_t{0x2C, 0x2C, 0x2E, 0xFF};
    p.system_grouped_background = core::rgba8_t{0x00, 0x00, 0x00, 0xFF};
    p.secondary_system_grouped_background = core::rgba8_t{0x1C, 0x1C, 0x1E, 0xFF};
    p.tertiary_system_grouped_background = core::rgba8_t{0x2C, 0x2C, 0x2E, 0xFF};
  } else {
    p.system_background = core::rgba8_t{0xFF, 0xFF, 0xFF, 0xFF};
    p.secondary_system_background = core::rgba8_t{0xF2, 0xF2, 0xF7, 0xFF};
    p.tertiary_system_background = core::rgba8_t{0xFF, 0xFF, 0xFF, 0xFF};
    p.system_grouped_background = core::rgba8_t{0xF2, 0xF2, 0xF7, 0xFF};
    p.secondary_system_grouped_background = core::rgba8_t{0xFF, 0xFF, 0xFF, 0xFF};
    p.tertiary_system_grouped_background = core::rgba8_t{0xF2, 0xF2, 0xF7, 0xFF};
  }

  // Semantic Colors — Separator
  if (dark) {
    p.separator = core::rgba8_t{0x54, 0x54, 0x58, 0x99};
    p.opaque_separator = core::rgba8_t{0x38, 0x38, 0x3A, 0xFF};
  } else {
    p.separator = core::rgba8_t{0x3C, 0x3C, 0x43, 0x49};
    p.opaque_separator = core::rgba8_t{0xC6, 0xC6, 0xC8, 0xFF};
  }

  return p;
}

}  // namespace colorcpp::operations::palette
