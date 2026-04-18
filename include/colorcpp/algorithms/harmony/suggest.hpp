/**
 * @file suggest.hpp
 * @brief Color harmony suggestions - provide suggestions for improving harmony.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/algorithms/harmony/assess.hpp>
#include <colorcpp/algorithms/harmony/rules.hpp>
#include <colorcpp/algorithms/harmony/scheme.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/palette/palette_set.hpp>
#include <string>
#include <vector>

namespace colorcpp::algorithms::harmony {

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
std::vector<suggestion> suggest(const operations::palette::palette_set<Color>& palette) {
  using namespace operations::conversion;

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
  std::sort(indices.begin(), indices.end(), [&hsl_colors](size_t a, size_t b) {
    return hsl_colors[a].template get_index<0>() < hsl_colors[b].template get_index<0>();
  });

  // Get ideal angles for detected scheme
  const auto rule = rule_for(result.scheme, palette.size());

  // If scheme is not recognized (pair, triad, tetrad, custom) or ideal_angles is empty,
  // suggest improvements based on palette size
  if (rule.ideal_steps.empty() || result.scheme == harmony_scheme::pair || result.scheme == harmony_scheme::triad ||
      result.scheme == harmony_scheme::tetrad || result.scheme == harmony_scheme::custom) {
    if (palette.size() == 2) {
      const auto fallback = rule_for(harmony_scheme::complementary);
      suggestions.push_back({indices[1], "Shift hue by +180° to create complementary pair", fallback.generation_offsets[1]});
      return suggestions;
    } else if (palette.size() == 3) {
      const auto fallback = rule_for(harmony_scheme::triadic);
      suggestions.push_back({indices[1], "Shift hue by +120° for triadic harmony", fallback.generation_offsets[1]});
      suggestions.push_back({indices[2], "Shift hue by +240° for triadic harmony", fallback.generation_offsets[2]});
      return suggestions;
    } else if (palette.size() == 4) {
      const auto fallback = rule_for(harmony_scheme::square);
      suggestions.push_back({indices[1], "Shift hue by +90° for square harmony", fallback.generation_offsets[1]});
      suggestions.push_back({indices[2], "Shift hue by +180° for square harmony", fallback.generation_offsets[2]});
      suggestions.push_back({indices[3], "Shift hue by +270° for square harmony", fallback.generation_offsets[3]});
      return suggestions;
    }
  }

  // Calculate adjustments needed
  float base_hue = hsl_colors[indices[0]].template get_index<0>();
  for (size_t i = 1; i < indices.size() && i - 1 < rule.ideal_steps.size(); ++i) {
    float current_hue = hsl_colors[indices[i]].template get_index<0>();
    float target_hue = std::fmod(base_hue + rule.ideal_steps[i - 1], 360.0f);
    if (target_hue < 0) target_hue += 360.0f;

    float adjustment = target_hue - current_hue;
    // Normalize to [-180, 180]
    if (adjustment > 180.0f) adjustment -= 360.0f;
    if (adjustment < -180.0f) adjustment += 360.0f;

    if (std::abs(adjustment) > 5.0f) {  // Only suggest if adjustment is significant
      using namespace std::string_literals;
      std::string action =
          "Shift hue by "s + (adjustment > 0 ? "+" : "") + std::to_string(static_cast<int>(adjustment)) + "°";
      suggestions.push_back({indices[i], action, adjustment});
    }
  }

  return suggestions;
}

}  // namespace colorcpp::algorithms::harmony
