/**
 * @file weights.hpp
 * @brief Weighted color harmony assessment with configurable channel weights.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/algorithms/harmony/assess.hpp>
#include <colorcpp/algorithms/harmony/detail/angle_utils.hpp>
#include <colorcpp/algorithms/harmony/scheme.hpp>
#include <colorcpp/core/palette_set.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <vector>

namespace colorcpp::algorithms::harmony {

/**
 * @brief Configuration weights for harmony assessment channels.
 */
struct harmony_weights {
  float angle_weight = 1.0f;       ///< Weight for angle/hue harmony (0.0-1.0)
  float saturation_weight = 0.5f;  ///< Weight for saturation uniformity (0.0-1.0)
  float lightness_weight = 0.3f;   ///< Weight for lightness uniformity (0.0-1.0)

  /**
   * @brief Validate and clamp weights to [0, 1] range.
   */
  void validate() {
    angle_weight = std::max(0.0f, std::min(1.0f, angle_weight));
    saturation_weight = std::max(0.0f, std::min(1.0f, saturation_weight));
    lightness_weight = std::max(0.0f, std::min(1.0f, lightness_weight));
  }

  /**
   * @brief Get total weight sum.
   */
  [[nodiscard]] float total() const { return angle_weight + saturation_weight + lightness_weight; }

  /**
   * @brief Create angle-only weights (ignore saturation/lightness).
   */
  static harmony_weights angle_only() { return {1.0f, 0.0f, 0.0f}; }

  /**
   * @brief Create balanced weights (equal importance).
   */
  static harmony_weights balanced() { return {1.0f, 1.0f, 1.0f}; }

  /**
   * @brief Create default weights (angle-dominant).
   */
  static harmony_weights default_weights() { return {1.0f, 0.5f, 0.3f}; }
};

/**
 * @brief Weighted harmony assessment result.
 */
struct weighted_assessment_result {
  float score = 0.0f;             ///< Weighted harmony score (0-100)
  float angle_score = 0.0f;       ///< Angle/hue harmony component score
  float saturation_score = 0.0f;  ///< Saturation uniformity component score
  float lightness_score = 0.0f;   ///< Lightness uniformity component score
  harmony_scheme scheme = harmony_scheme::custom;
};

namespace detail {

/**
 * @brief Calculate saturation uniformity score (0-100).
 */
inline float saturation_uniformity(const std::vector<core::hsla_float_t>& colors) {
  if (colors.size() < 2) return 100.0f;

  float mean_sat = 0.0f;
  for (const auto& c : colors) {
    mean_sat += c.template get_index<1>();
  }
  mean_sat /= static_cast<float>(colors.size());

  float variance = 0.0f;
  for (const auto& c : colors) {
    float diff = c.template get_index<1>() - mean_sat;
    variance += diff * diff;
  }
  variance /= static_cast<float>(colors.size());
  float stddev = std::sqrt(variance);

  float cv = (mean_sat > 0.01f) ? (stddev / mean_sat) : 0.0f;
  float score = 100.0f * (1.0f - std::min(1.0f, cv * 2.0f));
  return std::max(0.0f, score);
}

/**
 * @brief Calculate lightness uniformity score (0-100).
 */
inline float lightness_uniformity(const std::vector<core::hsla_float_t>& colors) {
  if (colors.size() < 2) return 100.0f;

  float mean_lit = 0.0f;
  for (const auto& c : colors) {
    mean_lit += c.template get_index<2>();
  }
  mean_lit /= static_cast<float>(colors.size());

  float variance = 0.0f;
  for (const auto& c : colors) {
    float diff = c.template get_index<2>() - mean_lit;
    variance += diff * diff;
  }
  variance /= static_cast<float>(colors.size());
  float stddev = std::sqrt(variance);

  float cv = (mean_lit > 0.01f) ? (stddev / mean_lit) : 0.0f;
  float score = 100.0f * (1.0f - std::min(1.0f, cv * 2.0f));
  return std::max(0.0f, score);
}

}  // namespace detail

/**
 * @brief Assess palette harmony with configurable channel weights.
 * @tparam Color Color type.
 * @param palette The palette to analyze.
 * @param weights Channel weights for scoring.
 * @return Weighted assessment result with per-channel scores.
 */
template <typename Color>
weighted_assessment_result assess_weighted(const core::palette_set<Color>& palette,
                                           const harmony_weights& weights = harmony_weights::default_weights()) {
  using namespace operations::conversion;

  weighted_assessment_result result;

  if (palette.size() < 2) {
    result.score = 100.0f;
    result.angle_score = 100.0f;
    result.saturation_score = 100.0f;
    result.lightness_score = 100.0f;
    result.scheme = harmony_scheme::single;
    return result;
  }

  // Convert all colors to HSL
  std::vector<core::hsla_float_t> hsl_colors;
  hsl_colors.reserve(palette.size());

  for (const auto& color : palette) {
    hsl_colors.push_back(color_cast<core::hsla_float_t>(color));
  }

  // Extract hues and use the shared sorted-circular convention so detection
  // does not depend on insertion order (same as assess()).
  std::vector<float> hues;
  hues.reserve(hsl_colors.size());
  for (const auto& c : hsl_colors) {
    hues.push_back(c.template get_index<0>());
  }
  std::vector<float> diffs = circular_hue_diffs(hues);

  // Calculate angle harmonic deviation over the in-circle gaps
  const std::size_t in_circle = diffs.size() - 1;
  auto minmax = std::minmax_element(diffs.begin(), diffs.begin() + in_circle);
  float harmonic_deviation = *minmax.second - *minmax.first;

  // Detect scheme
  auto [scheme, ideal_steps] = detect_scheme_from_hues(hues);
  result.scheme = scheme;

  // Calculate angle score (ideal_steps covers the in-circle gaps only)
  if (!ideal_steps.empty()) {
    float deviation = 0.0f;
    std::size_t count = 0;
    for (size_t i = 0; i < in_circle && i < ideal_steps.size(); ++i) {
      deviation += std::abs(diffs[i] - ideal_steps[i]);
      ++count;
    }
    deviation /= static_cast<float>(std::max<std::size_t>(1, count));
    result.angle_score = std::max(0.0f, 100.0f - deviation);
  } else {
    result.angle_score = std::max(0.0f, 100.0f - detail::k_base_deviation_penalty - harmonic_deviation);
  }

  // Calculate saturation and lightness scores
  result.saturation_score = detail::saturation_uniformity(hsl_colors);
  result.lightness_score = detail::lightness_uniformity(hsl_colors);

  // Compute weighted total (clamp out-of-range weights first)
  harmony_weights clamped = weights;
  clamped.validate();
  float w_sum = clamped.total();
  if (w_sum > 0.0f) {
    result.score = (clamped.angle_weight * result.angle_score + clamped.saturation_weight * result.saturation_score +
                    clamped.lightness_weight * result.lightness_score) /
                   w_sum;
  } else {
    result.score = result.angle_score;
  }

  result.score = std::max(0.0f, std::min(100.0f, result.score));
  return result;
}

}  // namespace colorcpp::algorithms::harmony
