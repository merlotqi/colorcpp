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
#include <limits>
#include <numeric>
#include <optional>
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
  // tetradic is a generation-only alias of double_complementary: the
  // rectangle layout (two complementary pairs) has one geometry, so the
  // rules are intentionally byte-identical and detect_scheme never claims
  // tetradic (issue #104).
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

namespace detail {

// Rotation-invariant scheme matching for palettes of 2..5 colors (issue #103).
//
// circular_hue_diffs anchors the diff vector at the smallest hue, so adding
// a constant to every hue cyclically shifts the vector; matching fixed
// indices therefore classified the same relative layout differently depending
// on where it sat on the wheel. Each pattern below is evaluated against every
// cyclic rotation of the diff vector (each rotation excluding a different
// gap as the wrap entry), and the qualifying match with the smallest total
// deviation from the pattern's ideal steps wins; pattern order breaks ties,
// preserving today's priority for exact ties.

// Tolerance gate + deviation for one measured/ideal pair (degrees).
inline std::optional<float> tol_match(float measured, float ideal) {
  const float d = std::abs(measured - ideal);
  return d < k_default_tolerance ? std::optional<float>(d) : std::nullopt;
}

// Both elements must pass the tolerance gate; deviation is the sum.
inline std::optional<float> tol_match2(float m0, float i0, float m1, float i1) {
  const auto d0 = tol_match(m0, i0);
  const auto d1 = tol_match(m1, i1);
  if (!d0 || !d1) return std::nullopt;
  return *d0 + *d1;
}

inline std::optional<float> match_complementary(const std::vector<float>& w) { return tol_match(w[0], 180.0f); }

inline std::optional<float> match_golden_pair(const std::vector<float>& w) { return tol_match(w[0], k_golden_angle); }

inline std::optional<float> match_triadic(const std::vector<float>& w) {
  return tol_match2(w[0], 120.0f, w[1], 120.0f);
}

inline std::optional<float> match_golden_triple(const std::vector<float>& w) {
  return tol_match2(w[0], k_golden_angle, w[1], k_golden_angle);
}

// Threshold gate (both gaps under 45); deviation measured from the
// canonical analogous steps {30, 30}.
inline std::optional<float> match_analogous(const std::vector<float>& w) {
  if (!(w[0] < 45.0f && w[1] < 45.0f)) return std::nullopt;
  return std::abs(w[0] - 30.0f) + std::abs(w[1] - 30.0f);
}

// Both accepted orderings of the {150, 60} ideal; keep the better fit.
inline std::optional<float> match_split_complementary(const std::vector<float>& w) {
  std::optional<float> best;
  if (const auto d = tol_match2(w[0], 150.0f, w[1], 60.0f)) best = d;
  if (const auto d = tol_match2(w[0], 60.0f, w[1], 150.0f)) best = best ? std::min(*best, *d) : d;
  return best;
}

// One gap within tolerance of 60 and the other under 45 (either order).
inline std::optional<float> match_split_analogous(const std::vector<float>& w) {
  std::optional<float> best;
  if (const auto d0 = tol_match(w[0], 60.0f); d0 && w[1] < 45.0f) best = *d0 + std::abs(w[1] - 30.0f);
  if (const auto d1 = tol_match(w[1], 60.0f); d1 && w[0] < 45.0f) {
    const float d = *d1 + std::abs(w[0] - 30.0f);
    best = best ? std::min(*best, d) : d;
  }
  return best;
}

inline std::optional<float> match_square(const std::vector<float>& w) {
  const auto d01 = tol_match2(w[0], 90.0f, w[1], 90.0f);
  const auto d2 = tol_match(w[2], 90.0f);
  if (!d01 || !d2) return std::nullopt;
  return *d01 + *d2;
}

// Threshold gate (all gaps under 35); ideal wide steps {20, 20, 20}.
inline std::optional<float> match_analogous_wide4(const std::vector<float>& w) {
  if (!(w[0] < 35.0f && w[1] < 35.0f && w[2] < 35.0f)) return std::nullopt;
  return std::abs(w[0] - 20.0f) + std::abs(w[1] - 20.0f) + std::abs(w[2] - 20.0f);
}

// Outer gaps under 45, middle within tolerance of 150; ideal {30, 150, 30}.
inline std::optional<float> match_compound(const std::vector<float>& w) {
  const auto d1 = tol_match(w[1], 150.0f);
  if (!(w[0] < 45.0f && d1 && w[2] < 45.0f)) return std::nullopt;
  return std::abs(w[0] - 30.0f) + *d1 + std::abs(w[2] - 30.0f);
}

inline std::optional<float> match_double_complementary(const std::vector<float>& w) {
  const auto d0 = tol_match(w[0], 60.0f);
  const auto d1 = tol_match(w[1], 120.0f);
  const auto d2 = tol_match(w[2], 60.0f);
  if (!d0 || !d1 || !d2) return std::nullopt;
  return *d0 + *d1 + *d2;
}

// Five-color thresholds: wide must beat the looser analogous bound at the
// 18.75 midpoint, or wide is unreachable (both gates kept as before).
inline std::optional<float> match_analogous_wide5(const std::vector<float>& w) {
  if (!(w[0] < 18.75f && w[1] < 18.75f && w[2] < 18.75f && w[3] < 18.75f)) return std::nullopt;
  return std::abs(w[0] - 15.0f) + std::abs(w[1] - 15.0f) + std::abs(w[2] - 15.0f) + std::abs(w[3] - 15.0f);
}

inline std::optional<float> match_analogous5(const std::vector<float>& w) {
  if (!(w[0] < 35.0f && w[1] < 35.0f && w[2] < 35.0f && w[3] < 35.0f)) return std::nullopt;
  return std::abs(w[0] - 22.5f) + std::abs(w[1] - 22.5f) + std::abs(w[2] - 22.5f) + std::abs(w[3] - 22.5f);
}

using pattern_matcher = std::optional<float> (*)(const std::vector<float>&);

struct scheme_pattern {
  harmony_scheme scheme;
  pattern_matcher match;
};

// Rotation-aligned deviation between circular diffs and ideal steps.
//
// Tries every cyclic rotation of @p diffs (each excluding a different gap as
// the wrap entry) and returns the smallest total |diff - ideal| over the
// window. The result is identical for every rotation of the palette, so
// assess()/assess_weighted() scores no longer depend on where the palette
// sits on the wheel (issue #103).
inline float aligned_deviation(const std::vector<float>& diffs, const std::vector<float>& ideal) {
  const size_t n = diffs.size();
  if (n < 2 || ideal.empty()) return 0.0f;
  const size_t window = std::min(ideal.size(), n - 1);
  if (window == 0) return 0.0f;
  float best = std::numeric_limits<float>::max();
  for (size_t r = 0; r < n; ++r) {
    float sum = 0.0f;
    for (size_t i = 0; i < window; ++i) {
      sum += std::abs(diffs[(r + i) % n] - ideal[i]);
    }
    best = std::min(best, sum);
  }
  return best;
}

}  // namespace detail

/**
 * @brief Detect harmony scheme based on hue differences.
 *
 * For 2..5 colors the match is rotation-invariant (issue #103): circular
 * input (size == count) is tested against every cyclic rotation of the diff
 * vector and the best-fitting qualifying pattern wins, so adding a constant
 * to every hue cannot change the result. Linear input (size == count - 1)
 * has no wrap entry to rotate through and is matched as a single window.
 *
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

  if (count <= 5) {
    // One window per cyclic rotation of the circular vector; linear input
    // (or an unexpected size) keeps a single window of the first count-1
    // entries, matching the historical anchored behavior.
    std::vector<std::vector<float>> windows;
    if (diffs.size() == count) {
      windows.reserve(count);
      for (size_t r = 0; r < count; ++r) {
        std::vector<float> w;
        w.reserve(count - 1);
        for (size_t i = 0; i + 1 < count; ++i) {
          w.push_back(diffs[(r + i) % count]);
        }
        windows.push_back(std::move(w));
      }
    } else if (diffs.size() >= count - 1) {
      windows.emplace_back(diffs.begin(), diffs.begin() + static_cast<std::ptrdiff_t>(count - 1));
    }

    using namespace detail;
    std::vector<scheme_pattern> patterns;
    switch (count) {
      case 2:
        patterns = {{harmony_scheme::complementary, match_complementary}, {harmony_scheme::golden, match_golden_pair}};
        break;
      case 3:
        patterns = {{harmony_scheme::triadic, match_triadic},
                    {harmony_scheme::golden, match_golden_triple},
                    {harmony_scheme::analogous, match_analogous},
                    {harmony_scheme::split_complementary, match_split_complementary},
                    {harmony_scheme::split_analogous, match_split_analogous}};
        break;
      case 4:  // tetradic intentionally absent: generation-only alias of
               // double_complementary, which claims the rectangle (issue #104)
        patterns = {{harmony_scheme::square, match_square},
                    {harmony_scheme::analogous_wide, match_analogous_wide4},
                    {harmony_scheme::compound, match_compound},
                    {harmony_scheme::double_complementary, match_double_complementary}};
        break;
      default:  // count == 5: tighter wide gate first (see match_analogous_wide5)
        patterns = {{harmony_scheme::analogous_wide, match_analogous_wide5},
                    {harmony_scheme::analogous, match_analogous5}};
        break;
    }

    bool found = false;
    float best_dev = 0.0f;
    harmony_scheme best_scheme = harmony_scheme::custom;
    for (const auto& pattern : patterns) {  // priority order: ties keep the earlier pattern
      for (const auto& w : windows) {
        if (const auto dev = pattern.match(w)) {
          if (!found || *dev < best_dev) {
            found = true;
            best_dev = *dev;
            best_scheme = pattern.scheme;
          }
        }
      }
    }
    if (found) {
      return {best_scheme, rule_for(best_scheme, count).ideal_steps};
    }
    if (count == 2) return {harmony_scheme::pair, {}};
    if (count == 3) return {harmony_scheme::triad, {}};
    if (count == 4) return {harmony_scheme::tetrad, {}};
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
