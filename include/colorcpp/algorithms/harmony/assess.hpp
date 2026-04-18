/**
 * @file assess.hpp
 * @brief Color harmony assessment - evaluate how harmonious a palette is.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/algorithms/harmony/rules.hpp>
#include <colorcpp/algorithms/harmony/scheme.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/palette/palette_set.hpp>
#include <vector>

namespace colorcpp::algorithms::harmony {

/**
 * @brief Harmony assessment result.
 */
struct assessment_result {
  float score;            ///< Harmony score (0-100, higher is better)
  harmony_scheme scheme;  ///< Detected harmony scheme
  float deviation;        ///< Average deviation from ideal angles
};

/**
 * @brief Assess the harmony of a color palette.
 * @tparam Color Color type.
 * @param palette The palette to assess.
 * @return Assessment result with score and detected scheme.
 */
template <typename Color>
assessment_result assess(const operations::palette::palette_set<Color>& palette) {
  using namespace operations::conversion;

  if (palette.size() < 2) {
    return {100.0f, harmony_scheme::single, 0.0f};
  }

  // Convert all colors to HSL for hue analysis
  std::vector<float> hues;
  hues.reserve(palette.size());

  for (const auto& color : palette) {
    auto hsl = color_cast<core::hsla_float_t>(color);
    hues.push_back(hsl.template get_index<0>());
  }

  // Sort hues for easier analysis (copy to avoid modifying original order for scheme detection)
  std::vector<float> sorted_hues = hues;
  std::sort(sorted_hues.begin(), sorted_hues.end());

  // Calculate hue differences in sorted order
  std::vector<float> diffs;
  for (size_t i = 1; i < sorted_hues.size(); ++i) {
    diffs.push_back(sorted_hues[i] - sorted_hues[i - 1]);
  }
  // Add wrap-around difference
  if (sorted_hues.size() > 1) {
    diffs.push_back(360.0f - sorted_hues.back() + sorted_hues.front());
  }

  // Also calculate differences in original order for scheme detection
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

  // Detect harmony scheme and calculate score (use original order for golden angle detection)
  auto [scheme, ideal_angles] = detect_scheme(palette.size(), original_diffs);

  // Calculate deviation from ideal angles
  float total_deviation = 0.0f;
  if (ideal_angles.empty()) {
    // No ideal angles means no recognized harmony scheme
    // Give a moderate score based on color distribution
    total_deviation = 30.0f;  // Base penalty for unrecognized scheme
  } else {
    for (size_t i = 0; i < original_diffs.size() && i < ideal_angles.size(); ++i) {
      total_deviation += std::abs(original_diffs[i] - ideal_angles[i]);
    }
  }
  float avg_deviation = total_deviation / static_cast<float>(original_diffs.size());

  // Convert deviation to score (0 deviation = 100 score)
  float score = std::max(0.0f, 100.0f - avg_deviation);

  return {score, scheme, avg_deviation};
}

}  // namespace colorcpp::algorithms::harmony
