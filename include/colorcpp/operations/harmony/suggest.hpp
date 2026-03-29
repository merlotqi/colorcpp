/**
 * @file suggest.hpp
 * @brief Color harmony suggestions - provide suggestions for improving harmony.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/palette/palette_set.hpp>
#include <colorcpp/operations/harmony/assess.hpp>
#include <colorcpp/operations/harmony/detail/rules.hpp>

namespace colorcpp::operations::harmony {

/**
 * @brief Suggestion for improving harmony.
 */
struct suggestion {
  size_t color_index;    ///< Index of the color to adjust
  std::string action;    ///< Action description (e.g., "shift hue by +30°")
  float hue_adjustment;  ///< Recommended hue adjustment in degrees
};

/**
 * @brief Get suggestions for improving palette harmony.
 * @tparam Color Color type.
 * @param palette The palette to analyze.
 * @return Vector of suggestions for improving harmony.
 */
template <typename Color>
std::vector<suggestion> suggest(const palette::palette_set<Color>& palette) {
  using namespace conversion;
  
  std::vector<suggestion> suggestions;
  
  if (palette.size() < 2) {
    return suggestions;
  }
  
  // Assess current harmony
  auto result = assess(palette);
  
  // If already harmonious, no suggestions needed
  if (result.score >= 90.0f) {
    return suggestions;
  }
  
  // Convert all colors to HSL
  std::vector<core::hsla_float_t> hsl_colors;
  hsl_colors.reserve(palette.size());
  
  for (const auto& color : palette) {
    hsl_colors.push_back(color_cast<core::hsla_float_t>(color));
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
  
  // Get ideal angles for detected scheme
  auto ideal_angles = detail::get_ideal_angles(result.scheme, palette.size());
  
  if (ideal_angles.empty()) {
    // No specific scheme detected, suggest complementary or triadic
    if (palette.size() == 2) {
      suggestions.push_back({indices[1], "Shift hue by +180° to create complementary pair", 180.0f});
    } else if (palette.size() == 3) {
      suggestions.push_back({indices[1], "Shift hue by +120° for triadic harmony", 120.0f});
      suggestions.push_back({indices[2], "Shift hue by +240° for triadic harmony", 240.0f});
    }
    return suggestions;
  }
  
  // Calculate adjustments needed
  float base_hue = hsl_colors[indices[0]].template get_index<0>();
  for (size_t i = 1; i < indices.size() && i - 1 < ideal_angles.size(); ++i) {
    float current_hue = hsl_colors[indices[i]].template get_index<0>();
    float target_hue = std::fmod(base_hue + ideal_angles[i - 1], 360.0f);
    if (target_hue < 0) target_hue += 360.0f;
    
    float adjustment = target_hue - current_hue;
    // Normalize to [-180, 180]
    if (adjustment > 180.0f) adjustment -= 360.0f;
    if (adjustment < -180.0f) adjustment += 360.0f;
    
    if (std::abs(adjustment) > 5.0f) {  // Only suggest if adjustment is significant
      using namespace std::string_literals;
      std::string action = "Shift hue by "s + 
                          (adjustment > 0 ? "+" : "") + 
                          std::to_string(static_cast<int>(adjustment)) + "°";
      suggestions.push_back({indices[i], action, adjustment});
    }
  }
  
  return suggestions;
}

}  // namespace colorcpp::operations::harmony
