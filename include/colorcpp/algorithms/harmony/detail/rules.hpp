/**
 * @file rules.hpp
 * @brief Internal harmony rules and angle definitions.
 */

#pragma once

#include <cmath>
#include <colorcpp/algorithms/harmony/scheme.hpp>
#include <utility>
#include <vector>

namespace colorcpp::algorithms::harmony::detail {

/**
 * @brief Ideal angles for different harmony schemes.
 */
inline std::vector<float> get_ideal_angles(harmony_scheme scheme, size_t count) {
  // 2-color schemes
  if (scheme == harmony_scheme::complementary && count == 2) {
    return {180.0f};
  } else if (scheme == harmony_scheme::golden && count == 2) {
    return {137.5f};  // Golden angle

    // 3-color schemes
  } else if (scheme == harmony_scheme::analogous && count == 3) {
    return {30.0f, 30.0f};
  } else if (scheme == harmony_scheme::triadic && count == 3) {
    return {120.0f, 120.0f};
  } else if (scheme == harmony_scheme::split_complementary && count == 3) {
    return {150.0f, 60.0f};
  } else if (scheme == harmony_scheme::split_analogous && count == 3) {
    return {60.0f, 30.0f};
  } else if (scheme == harmony_scheme::golden && count == 3) {
    return {137.5f, 137.5f};  // Golden angle repeated

    // 4-color schemes
  } else if (scheme == harmony_scheme::tetradic && count == 4) {
    return {60.0f, 120.0f, 60.0f};
  } else if (scheme == harmony_scheme::square && count == 4) {
    return {90.0f, 90.0f, 90.0f};
  } else if (scheme == harmony_scheme::analogous_wide && count == 4) {
    return {20.0f, 20.0f, 20.0f};
  } else if (scheme == harmony_scheme::compound && count == 4) {
    // Sequential steps in palette order (e.g. 0°, 30°, 180°, 210°): 30, 150, 30
    return {30.0f, 150.0f, 30.0f};
  } else if (scheme == harmony_scheme::double_complementary && count == 4) {
    // Sequential steps (e.g. 0°, 60°, 180°, 240°): 60, 120, 60 — same rectangle as tetradic
    return {60.0f, 120.0f, 60.0f};

    // 5-color schemes
  } else if (scheme == harmony_scheme::analogous && count == 5) {
    return {22.5f, 22.5f, 22.5f, 22.5f};
  } else if (scheme == harmony_scheme::analogous_wide && count == 5) {
    return {15.0f, 15.0f, 15.0f, 15.0f};
  }
  return {};
}

/**
 * @brief Detect harmony scheme based on hue differences.
 * @param count Number of colors.
 * @param diffs Hue differences between adjacent colors.
 * @return Pair of scheme and ideal angles.
 */
inline std::pair<harmony_scheme, std::vector<float>> detect_scheme(size_t count, const std::vector<float>& diffs) {
  if (count < 2) {
    return {harmony_scheme::single, {}};
  }

  if (count == 2) {
    // Check for complementary (180°)
    if (std::abs(diffs[0] - 180.0f) < 15.0f) {
      return {harmony_scheme::complementary, get_ideal_angles(harmony_scheme::complementary, 2)};
    }
    // Check for golden (137.5°)
    if (std::abs(diffs[0] - 137.5f) < 15.0f) {
      return {harmony_scheme::golden, get_ideal_angles(harmony_scheme::golden, 2)};
    }
    return {harmony_scheme::pair, {}};
  }

  if (count == 3) {
    // Check for triadic (120°, 120°)
    if (std::abs(diffs[0] - 120.0f) < 15.0f && std::abs(diffs[1] - 120.0f) < 15.0f) {
      return {harmony_scheme::triadic, get_ideal_angles(harmony_scheme::triadic, 3)};
    }
    // Check for golden (137.5°, 137.5°)
    if (std::abs(diffs[0] - 137.5f) < 15.0f && std::abs(diffs[1] - 137.5f) < 15.0f) {
      return {harmony_scheme::golden, get_ideal_angles(harmony_scheme::golden, 3)};
    }
    // Check for analogous (30°, 30°)
    if (diffs[0] < 45.0f && diffs[1] < 45.0f) {
      return {harmony_scheme::analogous, get_ideal_angles(harmony_scheme::analogous, 3)};
    }
    // Check for split complementary (150°, 60°)
    if ((std::abs(diffs[0] - 150.0f) < 15.0f && std::abs(diffs[1] - 60.0f) < 15.0f) ||
        (std::abs(diffs[0] - 60.0f) < 15.0f && std::abs(diffs[1] - 150.0f) < 15.0f)) {
      return {harmony_scheme::split_complementary, get_ideal_angles(harmony_scheme::split_complementary, 3)};
    }
    // Check for split analogous (60°, 30°)
    if ((std::abs(diffs[0] - 60.0f) < 15.0f && diffs[1] < 45.0f) ||
        (diffs[0] < 45.0f && std::abs(diffs[1] - 60.0f) < 15.0f)) {
      return {harmony_scheme::split_analogous, get_ideal_angles(harmony_scheme::split_analogous, 3)};
    }
    return {harmony_scheme::triad, {}};
  }

  if (count == 4) {
    // Check for square (90°, 90°, 90°)
    if (std::abs(diffs[0] - 90.0f) < 15.0f && std::abs(diffs[1] - 90.0f) < 15.0f &&
        std::abs(diffs[2] - 90.0f) < 15.0f) {
      return {harmony_scheme::square, get_ideal_angles(harmony_scheme::square, 4)};
    }
    // Check for analogous_wide (20°, 20°, 20°)
    if (diffs[0] < 35.0f && diffs[1] < 35.0f && diffs[2] < 35.0f) {
      return {harmony_scheme::analogous_wide, get_ideal_angles(harmony_scheme::analogous_wide, 4)};
    }
    // Compound in palette order (0°, 30°, 180°, 210°): steps 30°, 150°, 30°
    if (diffs[0] < 45.0f && std::abs(diffs[1] - 150.0f) < 15.0f && diffs[2] < 45.0f) {
      return {harmony_scheme::compound, get_ideal_angles(harmony_scheme::compound, 4)};
    }
    // Rectangle: two complementary pairs (e.g. 0°, 60°, 180°, 240°) → steps 60°, 120°, 60°.
    // Classified as double_complementary (same sequential pattern as tetradic rectangle; see get_ideal_angles).
    if (std::abs(diffs[0] - 60.0f) < 15.0f && std::abs(diffs[1] - 120.0f) < 15.0f &&
        std::abs(diffs[2] - 60.0f) < 15.0f) {
      return {harmony_scheme::double_complementary, get_ideal_angles(harmony_scheme::double_complementary, 4)};
    }
    return {harmony_scheme::tetrad, {}};
  }

  if (count == 5) {
    // Check for analogous (22.5°, 22.5°, 22.5°, 22.5°)
    if (diffs[0] < 35.0f && diffs[1] < 35.0f && diffs[2] < 35.0f && diffs[3] < 35.0f) {
      return {harmony_scheme::analogous, get_ideal_angles(harmony_scheme::analogous, 5)};
    }
    // Check for analogous_wide (15°, 15°, 15°, 15°)
    if (diffs[0] < 25.0f && diffs[1] < 25.0f && diffs[2] < 25.0f && diffs[3] < 25.0f) {
      return {harmony_scheme::analogous_wide, get_ideal_angles(harmony_scheme::analogous_wide, 5)};
    }
    return {harmony_scheme::custom, {}};
  }

  return {harmony_scheme::custom, {}};
}

}  // namespace colorcpp::algorithms::harmony::detail
