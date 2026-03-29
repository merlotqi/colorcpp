/**
 * @file scales.hpp
 * @brief Gradient scale generators for palettes.
 */

#pragma once

#include <colorcpp/operations/interpolate.hpp>
#include <colorcpp/operations/palette/palette_set.hpp>

namespace colorcpp::operations::palette {

/**
 * @brief Generate @p count colors from @p start to @p end via RGB linear interpolation.
 * @param start Start color.
 * @param end End color.
 * @param count Number of colors to generate.
 * @return Palette with interpolated colors.
 * @note When count == 0, returns empty palette.
 *       When count == 1, returns palette with only the start color.
 *       When count >= 2, returns palette from start to end inclusive.
 */
template <typename Color>
auto linear_scale(const Color& start, const Color& end, size_t count) {
  palette_set<Color> p;
  if (count == 0) return p;
  if (count == 1) {
    p.add(start);
    return p;
  }

  for (size_t i = 0; i < count; ++i) {
    float t = static_cast<float>(i) / (count - 1);
    p.add(interpolate::lerp(start, end, t));
  }
  return p;
}

/**
 * @brief Generate @p count colors via HSL interpolation (more vivid than linear_scale).
 * @param start Start color.
 * @param end End color.
 * @param count Number of colors to generate.
 * @return Palette with HSL-interpolated colors.
 * @note When count == 0, returns empty palette.
 *       When count == 1, returns palette with only the start color.
 *       When count >= 2, returns palette from start to end inclusive.
 */
template <typename Color>
auto visual_scale(const Color& start, const Color& end, size_t count) {
  palette_set<Color> p;
  if (count == 0) return p;
  if (count == 1) {
    p.add(start);
    return p;
  }

  for (size_t i = 0; i < count; ++i) {
    float t = static_cast<float>(i) / (count - 1);
    p.add(interpolate::lerp_hsl(start, end, t));
  }
  return p;
}

/**
 * @brief Generate @p count colors via Oklab interpolation.
 * @param start Start color.
 * @param end End color.
 * @param count Number of colors to generate.
 * @return Palette with perceptually-interpolated colors.
 * @note When count == 0, returns empty palette.
 *       When count == 1, returns palette with only the start color.
 *       When count >= 2, returns palette from start to end inclusive.
 */
template <typename Color>
auto perceptual_scale(const Color& start, const Color& end, size_t count) {
  palette_set<Color> p;
  if (count == 0) return p;
  if (count == 1) {
    p.add(start);
    return p;
  }

  for (size_t i = 0; i < count; ++i) {
    float t = static_cast<float>(i) / (count - 1);
    p.add(interpolate::lerp_oklab(start, end, t));
  }
  return p;
}

}  // namespace colorcpp::operations::palette
