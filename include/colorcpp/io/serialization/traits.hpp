/**
 * @file traits.hpp
 * @brief Serialization format traits and configuration.
 */

#pragma once

#include <string_view>

namespace colorcpp::io::serialization {

/**
 * @brief Serialization output format.
 */
enum class serialization_format {
  /** @brief Compact array: [r, g, b] */
  compact,
  /** @brief Named object: {"r": 0.5, "g": 0.3, "b": 0.1} */
  named,
};

/**
 * @brief Color space identifier for serialization metadata.
 */
enum class color_space_id {
  rgb8,
  rgba8,
  rgbf,
  rgbaf,
  linear_rgbf,
  linear_rgbaf,
  hsl,
  hsla,
  hsv,
  hsva,
  hwb,
  hwba,
  cmyk,
  cielab,
  cielch,
  oklab,
  oklch,
  display_p3,
  display_p3a,
  xyz,
};

/**
 * @brief Get string name for a color_space_id.
 */
constexpr std::string_view color_space_name(color_space_id id) {
  switch (id) {
    case color_space_id::rgb8:
      return "rgb8";
    case color_space_id::rgba8:
      return "rgba8";
    case color_space_id::rgbf:
      return "rgbf";
    case color_space_id::rgbaf:
      return "rgbaf";
    case color_space_id::linear_rgbf:
      return "linear_rgbf";
    case color_space_id::linear_rgbaf:
      return "linear_rgbaf";
    case color_space_id::hsl:
      return "hsl";
    case color_space_id::hsla:
      return "hsla";
    case color_space_id::hsv:
      return "hsv";
    case color_space_id::hsva:
      return "hsva";
    case color_space_id::hwb:
      return "hwb";
    case color_space_id::hwba:
      return "hwba";
    case color_space_id::cmyk:
      return "cmyk";
    case color_space_id::cielab:
      return "cielab";
    case color_space_id::cielch:
      return "cielch";
    case color_space_id::oklab:
      return "oklab";
    case color_space_id::oklch:
      return "oklch";
    case color_space_id::display_p3:
      return "display_p3";
    case color_space_id::display_p3a:
      return "display_p3a";
    case color_space_id::xyz:
      return "xyz";
    default:
      return "unknown";
  }
}

/**
 * @brief Serialization options.
 */
struct serialization_options {
  /** @brief Output format (compact array or named object). */
  serialization_format format = serialization_format::compact;

  /** @brief Whether to include color space metadata in named format. */
  bool include_color_space = false;
};

}  // namespace colorcpp::io::serialization
