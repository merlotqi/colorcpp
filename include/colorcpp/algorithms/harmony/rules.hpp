/**
 * @file rules.hpp
 * @brief Public harmony rule definitions and scheme detection.
 */

#pragma once

#include <cmath>
#include <colorcpp/algorithms/harmony/scheme.hpp>
#include <cstddef>
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
    return {scheme, count, {137.5f}, {0.0f, 137.5f}};
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
    return {scheme, count, {137.5f, 137.5f}, {0.0f, 137.5f, 275.0f}};
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
 * @brief Detect harmony scheme based on hue differences.
 * @param count Number of colors.
 * @param diffs Hue differences between adjacent colors.
 * @return Pair of detected scheme and its ideal steps.
 */
inline std::pair<harmony_scheme, std::vector<float>> detect_scheme(size_t count, const std::vector<float>& diffs) {
  if (count < 2) {
    return {harmony_scheme::single, {}};
  }

  if (count == 2) {
    if (std::abs(diffs[0] - 180.0f) < 15.0f) {
      return {harmony_scheme::complementary, rule_for(harmony_scheme::complementary, 2).ideal_steps};
    }
    if (std::abs(diffs[0] - 137.5f) < 15.0f) {
      return {harmony_scheme::golden, rule_for(harmony_scheme::golden, 2).ideal_steps};
    }
    return {harmony_scheme::pair, {}};
  }

  if (count == 3) {
    if (std::abs(diffs[0] - 120.0f) < 15.0f && std::abs(diffs[1] - 120.0f) < 15.0f) {
      return {harmony_scheme::triadic, rule_for(harmony_scheme::triadic, 3).ideal_steps};
    }
    if (std::abs(diffs[0] - 137.5f) < 15.0f && std::abs(diffs[1] - 137.5f) < 15.0f) {
      return {harmony_scheme::golden, rule_for(harmony_scheme::golden, 3).ideal_steps};
    }
    if (diffs[0] < 45.0f && diffs[1] < 45.0f) {
      return {harmony_scheme::analogous, rule_for(harmony_scheme::analogous, 3).ideal_steps};
    }
    if ((std::abs(diffs[0] - 150.0f) < 15.0f && std::abs(diffs[1] - 60.0f) < 15.0f) ||
        (std::abs(diffs[0] - 60.0f) < 15.0f && std::abs(diffs[1] - 150.0f) < 15.0f)) {
      return {harmony_scheme::split_complementary, rule_for(harmony_scheme::split_complementary, 3).ideal_steps};
    }
    if ((std::abs(diffs[0] - 60.0f) < 15.0f && diffs[1] < 45.0f) ||
        (diffs[0] < 45.0f && std::abs(diffs[1] - 60.0f) < 15.0f)) {
      return {harmony_scheme::split_analogous, rule_for(harmony_scheme::split_analogous, 3).ideal_steps};
    }
    return {harmony_scheme::triad, {}};
  }

  if (count == 4) {
    if (std::abs(diffs[0] - 90.0f) < 15.0f && std::abs(diffs[1] - 90.0f) < 15.0f &&
        std::abs(diffs[2] - 90.0f) < 15.0f) {
      return {harmony_scheme::square, rule_for(harmony_scheme::square, 4).ideal_steps};
    }
    if (diffs[0] < 35.0f && diffs[1] < 35.0f && diffs[2] < 35.0f) {
      return {harmony_scheme::analogous_wide, rule_for(harmony_scheme::analogous_wide, 4).ideal_steps};
    }
    if (diffs[0] < 45.0f && std::abs(diffs[1] - 150.0f) < 15.0f && diffs[2] < 45.0f) {
      return {harmony_scheme::compound, rule_for(harmony_scheme::compound, 4).ideal_steps};
    }
    if (std::abs(diffs[0] - 60.0f) < 15.0f && std::abs(diffs[1] - 120.0f) < 15.0f &&
        std::abs(diffs[2] - 60.0f) < 15.0f) {
      return {harmony_scheme::double_complementary, rule_for(harmony_scheme::double_complementary, 4).ideal_steps};
    }
    return {harmony_scheme::tetrad, {}};
  }

  if (count == 5) {
    if (diffs[0] < 35.0f && diffs[1] < 35.0f && diffs[2] < 35.0f && diffs[3] < 35.0f) {
      return {harmony_scheme::analogous, rule_for(harmony_scheme::analogous, 5).ideal_steps};
    }
    if (diffs[0] < 25.0f && diffs[1] < 25.0f && diffs[2] < 25.0f && diffs[3] < 25.0f) {
      return {harmony_scheme::analogous_wide, rule_for(harmony_scheme::analogous_wide, 5).ideal_steps};
    }
    return {harmony_scheme::custom, {}};
  }

  return {harmony_scheme::custom, {}};
}

}  // namespace colorcpp::algorithms::harmony