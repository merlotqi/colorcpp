/**
 * @file algorithms.hpp
 * @brief Separable RGB blend algorithms (per-channel).
 */

#pragma once

#include <algorithm>
#include <cmath>

namespace colorcpp::operations::blend::details {

/**
 * @brief Separable blend functions (per-channel, inputs in [0,1]).
 */
struct algorithms {
  static constexpr float normal(float /*a*/, float b) { return b; }
  static constexpr float multiply(float a, float b) { return a * b; }
  static constexpr float screen(float a, float b) { return 1.0f - (1.0f - a) * (1.0f - b); }
  static constexpr float darken(float a, float b) { return std::min(a, b); }
  static constexpr float lighten(float a, float b) { return std::max(a, b); }
  static constexpr float addition(float a, float b) { return std::min(1.0f, a + b); }
  static constexpr float subtraction(float a, float b) { return std::max(0.0f, a - b); }
  static constexpr float difference(float a, float b) {
    const float d = a - b;
    return (d < 0.0f) ? -d : d;
  }
  static constexpr float exclusion(float a, float b) { return a + b - 2.0f * a * b; }

  static constexpr float overlay(float a, float b) {
    return (a < 0.5f) ? (2.0f * a * b) : (1.0f - 2.0f * (1.0f - a) * (1.0f - b));
  }

  // hard_light = overlay with roles swapped (src controls the threshold)
  static constexpr float hard_light(float a, float b) { return overlay(b, a); }

  static constexpr float color_dodge(float a, float b) {
    if (b >= 1.0f) return 1.0f;
    return std::min(1.0f, a / (1.0f - b));
  }

  static constexpr float color_burn(float a, float b) {
    if (b <= 0.0f) return 0.0f;
    return 1.0f - std::min(1.0f, (1.0f - a) / b);
  }

  static constexpr float divide(float a, float b) {
    if (b <= 0.0f) return 1.0f;
    return std::min(1.0f, a / b);
  }

  // W3C soft-light formula (matches Photoshop behaviour).
  // Not constexpr: std::sqrt is not constexpr before C++23.
  static float soft_light(float a, float b) {
    if (b <= 0.5f) {
      return a - (1.0f - 2.0f * b) * a * (1.0f - a);
    }
    float d = (a <= 0.25f) ? (((16.0f * a - 12.0f) * a + 4.0f) * a) : std::sqrt(a);
    return a + (2.0f * b - 1.0f) * (d - a);
  }
};

}  // namespace colorcpp::operations::blend::details
