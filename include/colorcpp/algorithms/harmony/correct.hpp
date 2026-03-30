/**
 * @file correct.hpp
 * @brief Color harmony correction - adjust colors to improve harmony.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/algorithms/harmony/detail/rules.hpp>
#include <colorcpp/algorithms/harmony/scheme.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/palette/palette_set.hpp>
#include <vector>

namespace colorcpp::algorithms::harmony {

/**
 * @brief Correct a palette to improve harmony.
 * @tparam Color Color type.
 * @param palette The palette to correct.
 * @param target_scheme Target harmony scheme (default: auto-detect).
 * @return Corrected palette with improved harmony.
 */
template <typename Color>
operations::palette::palette_set<Color> correct(const operations::palette::palette_set<Color>& palette,
                                                harmony_scheme target_scheme = harmony_scheme::custom) {
  using namespace operations::conversion;

  if (palette.size() < 2) {
    return palette;
  }

  // Convert all colors to HSL
  std::vector<core::hsla_float_t> hsl_colors;
  hsl_colors.reserve(palette.size());

  for (const auto& color : palette) {
    hsl_colors.push_back(color_cast<core::hsla_float_t>(color));
  }

  // Determine target scheme if not specified
  harmony_scheme scheme = target_scheme;
  if (scheme == harmony_scheme::custom) {
    // Auto-detect the closest scheme using original order (for golden angle detection)
    std::vector<float> hues;
    for (const auto& hsl : hsl_colors) {
      hues.push_back(hsl.template get_index<0>());
    }

    // Calculate differences in original order
    std::vector<float> original_diffs;
    for (size_t i = 1; i < hues.size(); ++i) {
      float diff = hues[i] - hues[i - 1];
      if (diff < 0) diff += 360.0f;
      original_diffs.push_back(diff);
    }
    // Add wrap-around difference for original order
    if (hues.size() > 1) {
      float wrap_diff = hues.front() - hues.back();
      if (wrap_diff < 0) wrap_diff += 360.0f;
      original_diffs.push_back(wrap_diff);
    }

    auto [detected_scheme, _] = detail::detect_scheme(palette.size(), original_diffs);
    scheme = detected_scheme;
  }

  // Get ideal angles for the target scheme
  auto ideal_angles = detail::get_ideal_angles(scheme, palette.size());

  if (ideal_angles.empty()) {
    return palette;  // No correction available for this scheme
  }

  // Sort colors by hue
  std::vector<size_t> indices(hsl_colors.size());
  for (size_t i = 0; i < indices.size(); ++i) {
    indices[i] = i;
  }
  std::sort(indices.begin(), indices.end(), [&hsl_colors](size_t a, size_t b) {
    return hsl_colors[a].template get_index<0>() < hsl_colors[b].template get_index<0>();
  });

  // Adjust hues to match ideal angles
  // Note: ideal_angles represent differences between consecutive colors, not absolute positions
  float base_hue = hsl_colors[indices[0]].template get_index<0>();
  float cumulative_angle = 0.0f;
  for (size_t i = 1; i < indices.size() && i - 1 < ideal_angles.size(); ++i) {
    cumulative_angle += ideal_angles[i - 1];
    float target_hue = std::fmod(base_hue + cumulative_angle, 360.0f);
    if (target_hue < 0) target_hue += 360.0f;

    hsl_colors[indices[i]] = core::hsla_float_t{target_hue, hsl_colors[indices[i]].template get_index<1>(),
                                                hsl_colors[indices[i]].template get_index<2>(),
                                                hsl_colors[indices[i]].template get_index<3>()};
  }

  // Convert back to original color type
  operations::palette::palette_set<Color> result;
  for (const auto& hsl : hsl_colors) {
    result.add(color_cast<Color>(hsl));
  }

  return result;
}

}  // namespace colorcpp::algorithms::harmony
