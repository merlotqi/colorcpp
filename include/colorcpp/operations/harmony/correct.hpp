/**
 * @file correct.hpp
 * @brief Color harmony correction - adjust colors to improve harmony.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <vector>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/palette/palette_set.hpp>
#include <colorcpp/operations/harmony/detail/rules.hpp>

namespace colorcpp::operations::harmony {

/**
 * @brief Correct a palette to improve harmony.
 * @tparam Color Color type.
 * @param palette The palette to correct.
 * @param target_scheme Target harmony scheme (e.g., "complementary", "triadic").
 * @return Corrected palette with improved harmony.
 */
template <typename Color>
palette::palette_set<Color> correct(const palette::palette_set<Color>& palette,
                                    const std::string& target_scheme = "") {
  using namespace conversion;
  
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
  std::string scheme = target_scheme;
  if (scheme.empty()) {
    // Auto-detect the closest scheme
    std::vector<float> hues;
    for (const auto& hsl : hsl_colors) {
      hues.push_back(hsl.template get_index<0>());
    }
    std::sort(hues.begin(), hues.end());
    
    std::vector<float> diffs;
    for (size_t i = 1; i < hues.size(); ++i) {
      diffs.push_back(hues[i] - hues[i - 1]);
    }
    if (hues.size() > 1) {
      diffs.push_back(360.0f - hues.back() + hues.front());
    }
    
    auto [detected_scheme, _] = detail::detect_scheme(palette.size(), diffs);
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
  std::sort(indices.begin(), indices.end(),
            [&hsl_colors](size_t a, size_t b) {
              return hsl_colors[a].template get_index<0>() < hsl_colors[b].template get_index<0>();
            });
  
  // Adjust hues to match ideal angles
  float base_hue = hsl_colors[indices[0]].template get_index<0>();
  for (size_t i = 1; i < indices.size() && i - 1 < ideal_angles.size(); ++i) {
    float target_hue = std::fmod(base_hue + ideal_angles[i - 1], 360.0f);
    if (target_hue < 0) target_hue += 360.0f;
    
    hsl_colors[indices[i]] = core::hsla_float_t{
      target_hue,
      hsl_colors[indices[i]].template get_index<1>(),
      hsl_colors[indices[i]].template get_index<2>(),
      hsl_colors[indices[i]].template get_index<3>()
    };
  }
  
  // Convert back to original color type
  palette::palette_set<Color> result;
  for (const auto& hsl : hsl_colors) {
    result.push_back(color_cast<Color>(hsl));
  }
  
  return result;
}

}  // namespace colorcpp::operations::harmony
