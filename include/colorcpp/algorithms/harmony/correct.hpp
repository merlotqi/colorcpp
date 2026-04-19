/**
 * @file correct.hpp
 * @brief Color harmony correction - adjust colors to improve harmony.
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
 * @brief Outcome of a harmony correction attempt.
 */
enum class correction_status {
  corrected,
  unchanged,
  insufficient_colors,
  unsupported_scheme,
  tolerance_exceeded,
};

/**
 * @brief Structured result for harmony correction.
 */
template <typename Color>
struct correction_result {
  core::palette_set<Color> palette;
  harmony_scheme detected_scheme = harmony_scheme::custom;
  harmony_scheme target_scheme = harmony_scheme::custom;
  correction_status status = correction_status::unchanged;

  bool applied() const { return status == correction_status::corrected; }
};

namespace detail {

inline std::pair<harmony_scheme, std::vector<float>> detect_palette_scheme(
    const std::vector<core::hsla_float_t>& hsl_colors) {
  if (hsl_colors.size() < 2) {
    return {harmony_scheme::single, {}};
  }

  std::vector<float> hues;
  hues.reserve(hsl_colors.size());
  for (const auto& hsl : hsl_colors) {
    hues.push_back(hsl.template get_index<0>());
  }

  std::vector<float> original_diffs;
  original_diffs.reserve(hues.size() - 1);

  // Detect scheme expects (N-1) sequential differences for N colors
  // Linear sequence only, no wrap-around closing difference
  for (size_t i = 1; i < hues.size(); ++i) {
    float diff = hues[i] - hues[i - 1];
    if (diff < 0) diff += 360.0f;
    original_diffs.push_back(diff);
  }

  return detect_scheme(hsl_colors.size(), original_diffs);
}

}  // namespace detail

/**
 * @brief Correct a palette to improve harmony.
 * @tparam Color Color type.
 * @param palette The palette to correct.
 * @param target_scheme Target harmony scheme (default: auto-detect).
 * @return Structured correction result with palette and status metadata.
 */
template <typename Color>
correction_result<Color> correct(const core::palette_set<Color>& palette,
                                 harmony_scheme target_scheme = harmony_scheme::custom) {
  using namespace operations::conversion;

  correction_result<Color> result;
  result.palette = palette;

  if (palette.size() < 2) {
    result.detected_scheme = harmony_scheme::single;
    result.target_scheme = target_scheme == harmony_scheme::custom ? harmony_scheme::single : target_scheme;
    result.status = correction_status::insufficient_colors;
    return result;
  }

  // Convert all colors to HSL
  std::vector<core::hsla_float_t> hsl_colors;
  hsl_colors.reserve(palette.size());

  for (const auto& color : palette) {
    hsl_colors.push_back(color_cast<core::hsla_float_t>(color));
  }

  auto [detected_scheme, _] = detail::detect_palette_scheme(hsl_colors);
  result.detected_scheme = detected_scheme;

  harmony_scheme scheme = target_scheme == harmony_scheme::custom ? detected_scheme : target_scheme;
  result.target_scheme = scheme;

  size_t rule_count = (target_scheme != harmony_scheme::custom) ? canonical_color_count(target_scheme) : palette.size();
  const auto rule = rule_for(scheme, rule_count);

  if (rule.ideal_steps.empty()) {
    result.status = correction_status::unsupported_scheme;
    return result;
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
  constexpr float kCorrectionEpsilon = 0.1f;
  float base_hue = hsl_colors[indices[0]].template get_index<0>();
  float cumulative_angle = 0.0f;
  bool changed = false;

  for (size_t i = 1; i < indices.size() && (i - 1) < rule.ideal_steps.size(); ++i) {
    cumulative_angle += rule.ideal_steps[i - 1];
    float target_hue = std::fmod(base_hue + cumulative_angle, 360.0f);
    if (target_hue < 0) target_hue += 360.0f;

    float current_hue = hsl_colors[indices[i]].template get_index<0>();
    float adjustment = target_hue - current_hue;

    // Normalize adjustment to [-180, 180] range
    if (adjustment > 180.0f) adjustment -= 360.0f;
    if (adjustment < -180.0f) adjustment += 360.0f;

    if (std::abs(adjustment) >= kCorrectionEpsilon) {
      changed = true;
    }

    hsl_colors[indices[i]] = core::hsla_float_t{target_hue, hsl_colors[indices[i]].template get_index<1>(),
                                                hsl_colors[indices[i]].template get_index<2>(),
                                                hsl_colors[indices[i]].template get_index<3>()};
  }

  // Convert back to original color type
  core::palette_set<Color> corrected_palette;
  for (const auto& hsl : hsl_colors) {
    corrected_palette.add(color_cast<Color>(hsl));
  }

  result.palette = corrected_palette;
  result.status = changed ? correction_status::corrected : correction_status::unchanged;
  return result;
}

/**
 * @brief Correct a palette with tolerance threshold.
 * @tparam Color Color type.
 * @param palette The palette to correct.
 * @param target_scheme Target harmony scheme (default: auto-detect).
 * @param tolerance Maximum allowed deviation in degrees before correction is applied.
 * @return Structured correction result with palette and status metadata.
 */
template <typename Color>
correction_result<Color> correct(const core::palette_set<Color>& palette, harmony_scheme target_scheme,
                                 float tolerance) {
  using namespace operations::conversion;

  correction_result<Color> result;
  result.palette = palette;

  if (palette.size() < 2) {
    result.detected_scheme = harmony_scheme::single;
    result.target_scheme = target_scheme == harmony_scheme::custom ? harmony_scheme::single : target_scheme;
    result.status = correction_status::insufficient_colors;
    return result;
  }

  // Convert all colors to HSL
  std::vector<core::hsla_float_t> hsl_colors;
  hsl_colors.reserve(palette.size());

  for (const auto& color : palette) {
    hsl_colors.push_back(color_cast<core::hsla_float_t>(color));
  }

  auto [detected_scheme, _] = detail::detect_palette_scheme(hsl_colors);
  result.detected_scheme = detected_scheme;

  harmony_scheme scheme = target_scheme == harmony_scheme::custom ? detected_scheme : target_scheme;
  result.target_scheme = scheme;

  size_t rule_count = (target_scheme != harmony_scheme::custom) ? canonical_color_count(target_scheme) : palette.size();
  const auto rule = rule_for(scheme, rule_count);

  if (rule.ideal_steps.empty()) {
    result.status = correction_status::unsupported_scheme;
    return result;
  }

  // Sort colors by hue
  std::vector<size_t> indices(hsl_colors.size());
  for (size_t i = 0; i < indices.size(); ++i) {
    indices[i] = i;
  }
  std::sort(indices.begin(), indices.end(), [&hsl_colors](size_t a, size_t b) {
    return hsl_colors[a].template get_index<0>() < hsl_colors[b].template get_index<0>();
  });

  // Check if any deviation exceeds tolerance
  float base_hue = hsl_colors[indices[0]].template get_index<0>();
  float cumulative_angle = 0.0f;
  bool changed = false;
  bool tolerance_exceeded = false;

  for (size_t i = 1; i < indices.size() && (i - 1) < rule.ideal_steps.size(); ++i) {
    cumulative_angle += rule.ideal_steps[i - 1];
    float target_hue = std::fmod(base_hue + cumulative_angle, 360.0f);
    if (target_hue < 0) target_hue += 360.0f;

    float current_hue = hsl_colors[indices[i]].template get_index<0>();
    float adjustment = target_hue - current_hue;

    if (adjustment > 180.0f) adjustment -= 360.0f;
    if (adjustment < -180.0f) adjustment += 360.0f;

    if (std::abs(adjustment) > tolerance) {
      tolerance_exceeded = true;
    }

    if (std::abs(adjustment) >= 0.1f) {
      changed = true;
    }

    hsl_colors[indices[i]] = core::hsla_float_t{target_hue, hsl_colors[indices[i]].template get_index<1>(),
                                                hsl_colors[indices[i]].template get_index<2>(),
                                                hsl_colors[indices[i]].template get_index<3>()};
  }

  // Convert back to original color type
  core::palette_set<Color> corrected_palette;
  for (const auto& hsl : hsl_colors) {
    corrected_palette.add(color_cast<Color>(hsl));
  }

  result.palette = corrected_palette;
  if (tolerance_exceeded) {
    result.status = correction_status::tolerance_exceeded;
  } else {
    result.status = changed ? correction_status::corrected : correction_status::unchanged;
  }
  return result;
}

/**
 * @brief Minimal correction - adjust only the color with largest deviation.
 * @tparam Color Color type.
 * @param palette The palette to correct.
 * @param target_scheme Target harmony scheme (default: auto-detect).
 * @return Structured correction result with palette and status metadata.
 */
template <typename Color>
correction_result<Color> correct_minimal(const core::palette_set<Color>& palette,
                                         harmony_scheme target_scheme = harmony_scheme::custom) {
  using namespace operations::conversion;

  correction_result<Color> result;
  result.palette = palette;

  if (palette.size() < 2) {
    result.detected_scheme = harmony_scheme::single;
    result.target_scheme = target_scheme == harmony_scheme::custom ? harmony_scheme::single : target_scheme;
    result.status = correction_status::insufficient_colors;
    return result;
  }

  // Convert all colors to HSL
  std::vector<core::hsla_float_t> hsl_colors;
  hsl_colors.reserve(palette.size());

  for (const auto& color : palette) {
    hsl_colors.push_back(color_cast<core::hsla_float_t>(color));
  }

  auto [detected_scheme, _] = detail::detect_palette_scheme(hsl_colors);
  result.detected_scheme = detected_scheme;

  harmony_scheme scheme = target_scheme == harmony_scheme::custom ? detected_scheme : target_scheme;
  result.target_scheme = scheme;

  size_t rule_count = (target_scheme != harmony_scheme::custom) ? canonical_color_count(target_scheme) : palette.size();
  const auto rule = rule_for(scheme, rule_count);

  if (rule.ideal_steps.empty()) {
    result.status = correction_status::unsupported_scheme;
    return result;
  }

  // Sort colors by hue
  std::vector<size_t> indices(hsl_colors.size());
  for (size_t i = 0; i < indices.size(); ++i) {
    indices[i] = i;
  }
  std::sort(indices.begin(), indices.end(), [&hsl_colors](size_t a, size_t b) {
    return hsl_colors[a].template get_index<0>() < hsl_colors[b].template get_index<0>();
  });

  // Find the color with the largest deviation
  float base_hue = hsl_colors[indices[0]].template get_index<0>();
  float cumulative_angle = 0.0f;
  size_t max_deviation_idx = 0;
  float max_deviation = 0.0f;

  for (size_t i = 1; i < indices.size() && (i - 1) < rule.ideal_steps.size(); ++i) {
    cumulative_angle += rule.ideal_steps[i - 1];
    float target_hue = std::fmod(base_hue + cumulative_angle, 360.0f);
    if (target_hue < 0) target_hue += 360.0f;

    float current_hue = hsl_colors[indices[i]].template get_index<0>();
    float adjustment = target_hue - current_hue;

    if (adjustment > 180.0f) adjustment -= 360.0f;
    if (adjustment < -180.0f) adjustment += 360.0f;

    if (std::abs(adjustment) > max_deviation) {
      max_deviation = std::abs(adjustment);
      max_deviation_idx = i;
    }
  }

  // Only adjust the color with largest deviation
  if (max_deviation < 0.1f) {
    result.status = correction_status::unchanged;
    return result;
  }

  // Recalculate target hue for the max deviation color
  cumulative_angle = 0.0f;
  for (size_t i = 1; i <= max_deviation_idx && (i - 1) < rule.ideal_steps.size(); ++i) {
    cumulative_angle += rule.ideal_steps[i - 1];
  }
  float target_hue = std::fmod(base_hue + cumulative_angle, 360.0f);
  if (target_hue < 0) target_hue += 360.0f;

  size_t color_idx = indices[max_deviation_idx];
  hsl_colors[color_idx] =
      core::hsla_float_t{target_hue, hsl_colors[color_idx].template get_index<1>(),
                         hsl_colors[color_idx].template get_index<2>(), hsl_colors[color_idx].template get_index<3>()};

  // Convert back to original color type
  core::palette_set<Color> corrected_palette;
  for (const auto& hsl : hsl_colors) {
    corrected_palette.add(color_cast<Color>(hsl));
  }

  result.palette = corrected_palette;
  result.status = correction_status::corrected;
  return result;
}

/**
 * @brief Hue-only correction - adjust only hue channel, preserve saturation and lightness.
 * @tparam Color Color type.
 * @param palette The palette to correct.
 * @param target_scheme Target harmony scheme (default: auto-detect).
 * @return Structured correction result with palette and status metadata.
 */
template <typename Color>
correction_result<Color> correct_hue_only(const core::palette_set<Color>& palette,
                                          harmony_scheme target_scheme = harmony_scheme::custom) {
  // The standard correct() already only adjusts hue channel (preserves saturation and lightness).
  // This function is an explicit alias emphasizing the hue-only behavior.
  return correct(palette, target_scheme);
}

}  // namespace colorcpp::algorithms::harmony
