/**
 * @file scales.hpp
 * @brief Gradient scale generators for palettes.
 */

#pragma once

#include <colorcpp/core/palette_set.hpp>
#include <colorcpp/operations/interpolate.hpp>

namespace colorcpp::algorithms::palette {

/**
 * @brief Generate @p count colors from @p start to @p end via RGB linear interpolation.
 */
template <typename Color>
auto linear_scale(const Color& start, const Color& end, size_t count) {
  core::palette_set<Color> p;
  if (count == 0) return p;
  if (count == 1) {
    p.add(start);
    return p;
  }

  for (size_t i = 0; i < count; ++i) {
    float t = static_cast<float>(i) / (count - 1);
    p.add(operations::interpolate::lerp(start, end, t));
  }
  return p;
}

/**
 * @brief Generate @p count colors via HSL interpolation (more vivid than linear_scale).
 */
template <typename Color>
auto visual_scale(const Color& start, const Color& end, size_t count) {
  core::palette_set<Color> p;
  if (count == 0) return p;
  if (count == 1) {
    p.add(start);
    return p;
  }

  for (size_t i = 0; i < count; ++i) {
    float t = static_cast<float>(i) / (count - 1);
    p.add(operations::interpolate::lerp_hsl(start, end, t));
  }
  return p;
}

/**
 * @brief Generate @p count colors via Oklab interpolation.
 */
template <typename Color>
auto perceptual_scale(const Color& start, const Color& end, size_t count) {
  core::palette_set<Color> p;
  if (count == 0) return p;
  if (count == 1) {
    p.add(start);
    return p;
  }

  for (size_t i = 0; i < count; ++i) {
    float t = static_cast<float>(i) / (count - 1);
    p.add(operations::interpolate::lerp_oklab(start, end, t));
  }
  return p;
}

}  // namespace colorcpp::algorithms::palette
