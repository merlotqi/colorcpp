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

  // Sort colors by hue
  std::sort(hsl_colors.begin(), hsl_colors.end(), [](const core::hsla_float_t& a, const core::hsla_float_t& b) {
    return a.template get_index<0>() < b.template get_index<0>();
  });

  // Compute hue differences
  std::vector<float> diffs;
  diffs.reserve(hsl_colors.size());

  for (size_t i = 1; i < hsl_colors.size(); ++i) {
    diffs.push_back(
        detail::angle_diff(hsl_colors[i - 1].template get_index<0>(), hsl_colors[i].template get_index<0>()));
  }

  // Calculate angle harmonic deviation
  auto minmax = std::minmax_element(diffs.begin(), diffs.end());
  float harmonic_deviation = *minmax.second - *minmax.first;

  // Detect scheme
  auto [scheme, ideal_steps] = detect_scheme(hsl_colors.size(), diffs);
  result.scheme = scheme;

  // Calculate angle score
  if (!ideal_steps.empty()) {
    float deviation = 0.0f;
    for (size_t i = 0; i < diffs.size() && i < ideal_steps.size(); ++i) {
      float diff = std::abs(diffs[i] - ideal_steps[i]);
      if (diff > 180.0f) diff = 360.0f - diff;
      deviation += diff;
    }
    deviation /= static_cast<float>(std::min(diffs.size(), ideal_steps.size()));

    if (scheme != harmony_scheme::custom) {
      result.angle_score = std::max(0.0f, 100.0f - deviation);
    } else {
      result.angle_score = std::max(0.0f, 100.0f - detail::k_base_deviation_penalty - deviation);
    }
  } else {
    result.angle_score = std::max(0.0f, 100.0f - detail::k_base_deviation_penalty - harmonic_deviation);
  }

  // Calculate saturation and lightness scores
  result.saturation_score = detail::saturation_uniformity(hsl_colors);
  result.lightness_score = detail::lightness_uniformity(hsl_colors);

  // Compute weighted total
  float w_sum = weights.total();
  if (w_sum > 0.0f) {
    result.score = (weights.angle_weight * result.angle_score + weights.saturation_weight * result.saturation_score +
                    weights.lightness_weight * result.lightness_score) /
                   w_sum;
  } else {
    result.score = result.angle_score;
  }

  result.score = std::max(0.0f, std::min(100.0f, result.score));
  return result;
}

}  // namespace colorcpp::algorithms::harmony
