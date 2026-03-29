/**
 * @file rules.hpp
 * @brief Internal harmony rules and angle definitions.
 */

#pragma once

#include <cmath>
#include <string>
#include <vector>
#include <utility>

namespace colorcpp::operations::harmony::detail {

/**
 * @brief Ideal angles for different harmony schemes.
 */
inline std::vector<float> get_ideal_angles(const std::string& scheme, size_t count) {
  if (scheme == "complementary" && count == 2) {
    return {180.0f};
  } else if (scheme == "analogous" && count == 3) {
    return {30.0f, 30.0f};
  } else if (scheme == "triadic" && count == 3) {
    return {120.0f, 120.0f};
  } else if (scheme == "split_complementary" && count == 3) {
    return {150.0f, 60.0f};
  } else if (scheme == "tetradic" && count == 4) {
    return {60.0f, 120.0f, 60.0f};
  } else if (scheme == "square" && count == 4) {
    return {90.0f, 90.0f, 90.0f};
  }
  return {};
}

/**
 * @brief Detect harmony scheme based on hue differences.
 * @param count Number of colors.
 * @param diffs Hue differences between adjacent colors.
 * @return Pair of scheme name and ideal angles.
 */
inline std::pair<std::string, std::vector<float>> detect_scheme(
    size_t count, const std::vector<float>& diffs) {
  
  if (count < 2) {
    return {"single", {}};
  }
  
  if (count == 2) {
    // Check for complementary (180°)
    if (std::abs(diffs[0] - 180.0f) < 15.0f) {
      return {"complementary", get_ideal_angles("complementary", 2)};
    }
    return {"pair", {}};
  }
  
  if (count == 3) {
    // Check for triadic (120°, 120°)
    if (std::abs(diffs[0] - 120.0f) < 15.0f && std::abs(diffs[1] - 120.0f) < 15.0f) {
      return {"triadic", get_ideal_angles("triadic", 3)};
    }
    // Check for analogous (30°, 30°)
    if (diffs[0] < 45.0f && diffs[1] < 45.0f) {
      return {"analogous", get_ideal_angles("analogous", 3)};
    }
    // Check for split complementary (150°, 60°)
    if ((std::abs(diffs[0] - 150.0f) < 15.0f && std::abs(diffs[1] - 60.0f) < 15.0f) ||
        (std::abs(diffs[0] - 60.0f) < 15.0f && std::abs(diffs[1] - 150.0f) < 15.0f)) {
      return {"split_complementary", get_ideal_angles("split_complementary", 3)};
    }
    return {"triad", {}};
  }
  
  if (count == 4) {
    // Check for square (90°, 90°, 90°)
    if (std::abs(diffs[0] - 90.0f) < 15.0f && 
        std::abs(diffs[1] - 90.0f) < 15.0f && 
        std::abs(diffs[2] - 90.0f) < 15.0f) {
      return {"square", get_ideal_angles("square", 4)};
    }
    // Check for tetradic (60°, 120°, 60°)
    if (std::abs(diffs[0] - 60.0f) < 15.0f && 
        std::abs(diffs[1] - 120.0f) < 15.0f && 
        std::abs(diffs[2] - 60.0f) < 15.0f) {
      return {"tetradic", get_ideal_angles("tetradic", 4)};
    }
    return {"tetrad", {}};
  }
  
  return {"custom", {}};
}

}  // namespace colorcpp::operations::harmony::detail
