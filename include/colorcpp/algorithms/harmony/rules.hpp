/**
 * @file rules.hpp
 * @brief Public harmony rule definitions and scheme detection.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/algorithms/harmony/detail/angle_utils.hpp>
#include <colorcpp/algorithms/harmony/scheme.hpp>
#include <cstddef>
#include <numeric>
#include <utility>
#include <vector>

namespace colorcpp::algorithms::harmony {

/**
 * @brief Canonical metadata for a harmony scheme at a specific palette size.
 */
struct harmony_rule {
  harmony_scheme scheme = harmony_scheme::custom;
  size_t color_count = 0;
  std::vector<float> ideal_steps;
  std::vector<float> generation_offsets;

  bool empty() const { return ideal_steps.empty() && generation_offsets.empty(); }
};

/**
 * @brief Return the preferred palette size for a harmony scheme.
 */
inline size_t canonical_color_count(harmony_scheme scheme) {
  switch (scheme) {
    case harmony_scheme::complementary:
    case harmony_scheme::golden:
      return 2;
    case harmony_scheme::analogous:
    case harmony_scheme::triadic:
    case harmony_scheme::split_complementary:
    case harmony_scheme::split_analogous:
      return 3;
    case harmony_scheme::tetradic:
    case harmony_scheme::square:
    case harmony_scheme::analogous_wide:
    case harmony_scheme::compound:
    case harmony_scheme::double_complementary:
      return 4;
    default:
      return 0;
  }
}

/**
 * @brief Return the canonical rule data for a scheme and palette size.
 */
inline harmony_rule rule_for(harmony_scheme scheme, size_t count = 0) {
  if (count == 0) {
    count = canonical_color_count(scheme);
  }

  if (scheme == harmony_scheme::complementary && count == 2) {
    return {scheme, count, {180.0f}, {0.0f, 180.0f}};
  }
  if (scheme == harmony_scheme::golden && count == 2) {
    return {scheme, count, {detail::k_golden_angle}, {0.0f, detail::k_golden_angle}};
  }
  if (scheme == harmony_scheme::analogous && count == 3) {
    return {scheme, count, {30.0f, 30.0f}, {-30.0f, 0.0f, 30.0f}};
  }
  if (scheme == harmony_scheme::triadic && count == 3) {
    return {scheme, count, {120.0f, 120.0f}, {0.0f, 120.0f, 240.0f}};
  }
  if (scheme == harmony_scheme::split_complementary && count == 3) {
    return {scheme, count, {150.0f, 60.0f}, {0.0f, 150.0f, 210.0f}};
  }
  if (scheme == harmony_scheme::split_analogous && count == 3) {
    return {scheme, count, {60.0f, 30.0f}, {0.0f, 60.0f, 90.0f}};
  }
  if (scheme == harmony_scheme::golden && count == 3) {
    return {scheme,
            count,
            {detail::k_golden_angle, detail::k_golden_angle},
            {0.0f, detail::k_golden_angle, (2.0f * detail::k_golden_angle)}};
  }
  if (scheme == harmony_scheme::tetradic && count == 4) {
    return {scheme, count, {60.0f, 120.0f, 60.0f}, {0.0f, 60.0f, 180.0f, 240.0f}};
  }
  if (scheme == harmony_scheme::square && count == 4) {
    return {scheme, count, {90.0f, 90.0f, 90.0f}, {0.0f, 90.0f, 180.0f, 270.0f}};
  }
  if (scheme == harmony_scheme::analogous_wide && count == 4) {
    return {scheme, count, {20.0f, 20.0f, 20.0f}, {0.0f, 20.0f, 40.0f, 60.0f}};
  }
  if (scheme == harmony_scheme::compound && count == 4) {
    return {scheme, count, {30.0f, 150.0f, 30.0f}, {0.0f, 30.0f, 180.0f, 210.0f}};
  }
  if (scheme == harmony_scheme::double_complementary && count == 4) {
    return {scheme, count, {60.0f, 120.0f, 60.0f}, {0.0f, 60.0f, 180.0f, 240.0f}};
  }
  if (scheme == harmony_scheme::analogous && count == 5) {
    return {scheme, count, {22.5f, 22.5f, 22.5f, 22.5f}, {-45.0f, -22.5f, 0.0f, 22.5f, 45.0f}};
  }
  if (scheme == harmony_scheme::analogous_wide && count == 5) {
    return {scheme, count, {15.0f, 15.0f, 15.0f, 15.0f}, {-30.0f, -15.0f, 0.0f, 15.0f, 30.0f}};
  }

  return {};
}

/**
 * @brief Compute circular hue differences between adjacent colors.
 * @param hues Hue values in degrees (order does not matter).
 * @return Shortest-path differences between hue-sorted neighbors, including
 *         the wrap-around gap (size == hues.size()).
 */
inline std::vector<float> circular_hue_diffs(std::vector<float> hues) {
  std::sort(hues.begin(), hues.end());
  std::vector<float> diffs;
  diffs.reserve(hues.size());
  for (size_t i = 1; i < hues.size(); ++i) {
    diffs.push_back(detail::angle_diff(hues[i - 1], hues[i]));
  }
  if (hues.size() > 1) {
    diffs.push_back(detail::angle_diff(hues.back(), hues.front()));
  }
  return diffs;
}

/**
 * @brief Detect harmony scheme based on hue differences.
 * @param count Number of colors.
 * @param diffs Shortest-path hue differences between adjacent colors. May be
 *        linear (size == count - 1, in-circle gaps only) or circular
 *        (size == count, wrap-around gap appended); the wrap entry is only
 *        consulted by the multi-group (count > 5) path.
 * @return Pair of detected scheme and its ideal steps.
 */
inline std::pair<harmony_scheme, std::vector<float>> detect_scheme(size_t count, const std::vector<float>& diffs) {
  if (count < 2) {
    return {harmony_scheme::single, {}};
  }

  if (count == 2) {
    if (std::abs(diffs[0] - 180.0f) < detail::k_default_tolerance) {
      return {harmony_scheme::complementary, rule_for(harmony_scheme::complementary, 2).ideal_steps};
    }
    if (std::abs(diffs[0] - detail::k_golden_angle) < detail::k_default_tolerance) {
      return {harmony_scheme::golden, rule_for(harmony_scheme::golden, 2).ideal_steps};
    }
    return {harmony_scheme::pair, {}};
  }

  if (count == 3) {
    if (std::abs(diffs[0] - 120.0f) < detail::k_default_tolerance &&
        std::abs(diffs[1] - 120.0f) < detail::k_default_tolerance) {
      return {harmony_scheme::triadic, rule_for(harmony_scheme::triadic, 3).ideal_steps};
    }
    if (std::abs(diffs[0] - detail::k_golden_angle) < detail::k_default_tolerance &&
        std::abs(diffs[1] - detail::k_golden_angle) < detail::k_default_tolerance) {
      return {harmony_scheme::golden, rule_for(harmony_scheme::golden, 3).ideal_steps};
    }
    if (diffs[0] < 45.0f && diffs[1] < 45.0f) {
      return {harmony_scheme::analogous, rule_for(harmony_scheme::analogous, 3).ideal_steps};
    }
    if ((std::abs(diffs[0] - 150.0f) < detail::k_default_tolerance &&
         std::abs(diffs[1] - 60.0f) < detail::k_default_tolerance) ||
        (std::abs(diffs[0] - 60.0f) < detail::k_default_tolerance &&
         std::abs(diffs[1] - 150.0f) < detail::k_default_tolerance)) {
      return {harmony_scheme::split_complementary, rule_for(harmony_scheme::split_complementary, 3).ideal_steps};
    }
    if ((std::abs(diffs[0] - 60.0f) < detail::k_default_tolerance && diffs[1] < 45.0f) ||
        (diffs[0] < 45.0f && std::abs(diffs[1] - 60.0f) < detail::k_default_tolerance)) {
      return {harmony_scheme::split_analogous, rule_for(harmony_scheme::split_analogous, 3).ideal_steps};
    }
    return {harmony_scheme::triad, {}};
  }

  if (count == 4) {
    if (std::abs(diffs[0] - 90.0f) < detail::k_default_tolerance &&
        std::abs(diffs[1] - 90.0f) < detail::k_default_tolerance &&
        std::abs(diffs[2] - 90.0f) < detail::k_default_tolerance) {
      return {harmony_scheme::square, rule_for(harmony_scheme::square, 4).ideal_steps};
    }
    if (diffs[0] < 35.0f && diffs[1] < 35.0f && diffs[2] < 35.0f) {
      return {harmony_scheme::analogous_wide, rule_for(harmony_scheme::analogous_wide, 4).ideal_steps};
    }
    if (diffs[0] < 45.0f && std::abs(diffs[1] - 150.0f) < detail::k_default_tolerance && diffs[2] < 45.0f) {
      return {harmony_scheme::compound, rule_for(harmony_scheme::compound, 4).ideal_steps};
    }
    if (std::abs(diffs[0] - 60.0f) < detail::k_default_tolerance &&
        std::abs(diffs[1] - 120.0f) < detail::k_default_tolerance &&
        std::abs(diffs[2] - 60.0f) < detail::k_default_tolerance) {
      return {harmony_scheme::double_complementary, rule_for(harmony_scheme::double_complementary, 4).ideal_steps};
    }
    return {harmony_scheme::tetrad, {}};
  }

  if (count == 5) {
    // Check the tighter analogous_wide first: canonical wide steps are 15°
    // and canonical analogous steps 22.5°, so a shared threshold ordering
    // must test wide against the midpoint (18.75) before the looser
    // analogous bound, or wide is unreachable.
    if (diffs[0] < 18.75f && diffs[1] < 18.75f && diffs[2] < 18.75f && diffs[3] < 18.75f) {
      return {harmony_scheme::analogous_wide, rule_for(harmony_scheme::analogous_wide, 5).ideal_steps};
    }
    if (diffs[0] < 35.0f && diffs[1] < 35.0f && diffs[2] < 35.0f && diffs[3] < 35.0f) {
      return {harmony_scheme::analogous, rule_for(harmony_scheme::analogous, 5).ideal_steps};
    }
    return {harmony_scheme::custom, {}};
  }

  // >5 colors: multi-group detection
  if (count > 5) {
    // Strategy: find the largest gap to split into groups, then check if groups follow known patterns.
    // 1. Find split point (largest hue gap)
    // 2. Evaluate each group independently
    // 3. If groups are valid sub-schemes, return a combined description

    // With circular input the last entry closes the loop (last -> first hue);
    // it is a real gap but not a group boundary, so exclude it from the split
    // search and from the sub-group diffs. Linear input has no wrap entry.
    const size_t wrap_idx = (diffs.size() == count) ? count - 1 : diffs.size();

    // Find the largest gap as natural split boundary
    size_t split_idx = 0;
    float max_gap = 0.0f;
    for (size_t i = 0; i < wrap_idx; ++i) {
      if (diffs[i] > max_gap) {
        max_gap = diffs[i];
        split_idx = i;
      }
    }

    // Only attempt grouping if the gap is significant (>60°)
    if (max_gap > 60.0f) {
      // Split into two groups at the gap
      // Group A: colors [0, split_idx] (size = split_idx + 1)
      // Group B: colors [split_idx+1, count-1] (size = count - split_idx - 1)
      size_t group_a_size = split_idx + 1;
      size_t group_b_size = count - split_idx - 1;

      // Build diffs for each group (both exclude the boundary gap and the wrap)
      std::vector<float> diffs_a(diffs.begin(), diffs.begin() + split_idx);
      std::vector<float> diffs_b(diffs.begin() + split_idx + 1, diffs.begin() + wrap_idx);

      // Detect scheme for each group
      auto [scheme_a, steps_a] = detect_scheme(group_a_size, diffs_a);
      auto [scheme_b, steps_b] = detect_scheme(group_b_size, diffs_b);

      // If both groups are valid (not custom/pair/triad/tetrad), this is a recognizable multi-group pattern
      bool a_valid = scheme_a != harmony_scheme::custom && scheme_a != harmony_scheme::pair &&
                     scheme_a != harmony_scheme::triad && scheme_a != harmony_scheme::tetrad &&
                     scheme_a != harmony_scheme::single;
      bool b_valid = scheme_b != harmony_scheme::custom && scheme_b != harmony_scheme::pair &&
                     scheme_b != harmony_scheme::triad && scheme_b != harmony_scheme::tetrad &&
                     scheme_b != harmony_scheme::single;

      if (a_valid && b_valid) {
        // Two complementary groups (e.g., 2 analogous groups on opposite sides)
        // Report as compound-like pattern; ideal_steps = combined steps with the large gap included
        std::vector<float> combined_steps;
        combined_steps.insert(combined_steps.end(), steps_a.begin(), steps_a.end());
        combined_steps.push_back(max_gap - std::accumulate(steps_a.begin(), steps_a.end(), 0.0f));
        combined_steps.insert(combined_steps.end(), steps_b.begin(), steps_b.end());
        return {harmony_scheme::compound, combined_steps};
      }

      // Check for 3 groups (e.g., 2+2+2 complementary pairs)
      if (group_a_size == group_b_size && group_a_size >= 2 && group_a_size <= 3) {
        auto canonical = canonical_color_count(scheme_a);
        if (canonical == group_a_size) {
          // Symmetric multi-group: treat as extended compound
          std::vector<float> combined_steps;
          combined_steps.insert(combined_steps.end(), steps_a.begin(), steps_a.end());
          combined_steps.push_back(max_gap);
          combined_steps.insert(combined_steps.end(), steps_b.begin(), steps_b.end());
          return {harmony_scheme::compound, combined_steps};
        }
      }
    }

    // Check if all diffs are roughly equal (even distribution)
    float avg_diff = std::accumulate(diffs.begin(), diffs.end(), 0.0f) / static_cast<float>(diffs.size());
    bool even_distribution = true;
    for (float d : diffs) {
      if (detail::angle_diff(d, avg_diff) > detail::k_default_tolerance) {
        even_distribution = false;
        break;
      }
    }
    if (even_distribution) {
      // All colors evenly spaced; ideal_steps = all the same
      std::vector<float> even_steps(count - 1, avg_diff);
      return {harmony_scheme::compound, even_steps};
    }

    return {harmony_scheme::custom, {}};
  }

  return {harmony_scheme::custom, {}};
}

/**
 * @brief Detect the harmony scheme of a set of hues.
 * @param hues Hue values in degrees; insertion order does not affect the result.
 * @return Pair of detected scheme and its ideal steps.
 */
inline std::pair<harmony_scheme, std::vector<float>> detect_scheme_from_hues(const std::vector<float>& hues) {
  if (hues.size() < 2) {
    return {harmony_scheme::single, {}};
  }
  return detect_scheme(hues.size(), circular_hue_diffs(hues));
}

}  // namespace colorcpp::algorithms::harmony
