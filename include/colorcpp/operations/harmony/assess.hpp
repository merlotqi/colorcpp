/**
 * @file assess.hpp
 * @brief Color harmony assessment - evaluate how harmonious a palette is.
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
 * @brief Harmony assessment result.
 */
struct assessment_result {
  float score;           ///< Harmony score (0-100, higher is better)
  std::string scheme;    ///< Detected harmony scheme name
  float deviation;       ///< Average deviation from ideal angles
};

/**
 * @brief Assess the harmony of a color palette.
 * @tparam Color Color type.
 * @param palette The palette to assess.
 * @return Assessment result with score and detected scheme.
 */
template <typename Color>
assessment_result assess(const palette::palette_set<Color>& palette) {
  using namespace conversion;
  
  if (palette.size() < 2) {
    return {100.0f, "single", 0.0f};
  }
  
  // Convert all colors to HSL for hue analysis
  std::vector<float> hues;
  hues.reserve(palette.size());
  
  for (const auto& color : palette) {
    auto hsl = color_cast<core::hsla_float_t>(color);
    hues.push_back(hsl.template get_index<0>());
  }
  
  // Sort hues for easier analysis
  std::sort(hues.begin(), hues.end());
  
  // Calculate hue differences
  std::vector<float> diffs;
  for (size_t i = 1; i < hues.size(); ++i) {
    diffs.push_back(hues[i] - hues[i - 1]);
  }
  // Add wrap-around difference
  if (hues.size() > 1) {
    diffs.push_back(360.0f - hues.back() + hues.front());
  }
  
  // Detect harmony scheme and calculate score
  auto [scheme, ideal_angles] = detail::detect_scheme(palette.size(), diffs);
  
  // Calculate deviation from ideal angles
  float total_deviation = 0.0f;
  for (size_t i = 0; i < diffs.size() && i < ideal_angles.size(); ++i) {
    total_deviation += std::abs(diffs[i] - ideal_angles[i]);
  }
  float avg_deviation = total_deviation / static_cast<float>(diffs.size());
  
  // Convert deviation to score (0 deviation = 100 score)
  float score = std::max(0.0f, 100.0f - avg_deviation);
  
  return {score, scheme, avg_deviation};
}

}  // namespace colorcpp::operations::harmony
