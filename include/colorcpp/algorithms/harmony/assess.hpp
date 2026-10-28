/**
 * @file assess.hpp
 * @brief Color harmony assessment - evaluate how harmonious a palette is.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/algorithms/harmony/detail/angle_utils.hpp>
#include <colorcpp/algorithms/harmony/rules.hpp>
#include <colorcpp/algorithms/harmony/scheme.hpp>
#include <colorcpp/core/palette_set.hpp>
#include <colorcpp/operations/conversion.hpp>
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
assessment_result assess(const core::palette_set<Color>& palette) {
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

  // Shortest-path diffs between hue-sorted neighbors (circular: size == count).
  // Detection and deviation share this convention so the result does not
  // depend on the palette's insertion order.
  std::vector<float> diffs = circular_hue_diffs(hues);

  auto [scheme, ideal_angles] = detect_scheme_from_hues(hues);

  // Calculate deviation from ideal angles.
  // ideal_angles describes the in-circle gaps (count-1 steps); the wrap-around
  // diff has no matching ideal step, so it is excluded from both the sum and
  // the divisor (a 10°-off complementary pair scores 90, not 95).
  const std::size_t in_circle = diffs.size() - 1;
  float total_deviation = 0.0f;
  std::size_t deviation_count = 0;
  if (ideal_angles.empty()) {
    // No ideal angles means no recognized harmony scheme
    // Give a moderate score based on color distribution
    total_deviation = detail::k_base_deviation_penalty;  // Base penalty for unrecognized scheme
    deviation_count = std::max<std::size_t>(1, in_circle);
  } else {
    for (size_t i = 0; i < in_circle && i < ideal_angles.size(); ++i) {
      total_deviation += std::abs(diffs[i] - ideal_angles[i]);
      ++deviation_count;
    }
  }
  float avg_deviation = total_deviation / static_cast<float>(std::max<std::size_t>(1, deviation_count));

  // Convert deviation to score (0 deviation = 100 score)
  float score = std::max(0.0f, 100.0f - avg_deviation);

  return {score, scheme, avg_deviation};
}

}  // namespace colorcpp::algorithms::harmony
