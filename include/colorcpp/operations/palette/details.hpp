/**
 * @file details.hpp
 * @brief Internal helper functions for palette operations.
 */

#pragma once

#include <cmath>

namespace colorcpp::operations::palette::details {

/**
 * @brief Rotate hue by specified degrees.
 * @param h Hue value in [0, 360].
 * @param degrees Rotation amount in degrees.
 * @return Rotated hue in [0, 360], or 0 if input is NaN/Inf.
 */
inline float rotate_hue(float h, float degrees) {
  // Handle NaN and Inf
  if (!std::isfinite(h) || !std::isfinite(degrees)) {
    return 0.0f;
  }

  float res = std::fmod(h + degrees, 360.0f);
  if (res < 0.0f) res += 360.0f;
  return res;
}

/**
 * @brief Create a new HSL color with modified hue.
 * @param src Source HSL color.
 * @param new_h New hue value (should be in [0, 360]).
 * @return Color with modified hue.
 * @note If new_h is NaN or Inf, it will be clamped to 0.
 */
template <typename HSLColor>
constexpr HSLColor modify_hue(const HSLColor& src, float new_h) {
  // Clamp invalid values to 0
  if (!std::isfinite(new_h)) {
    new_h = 0.0f;
  }

  if constexpr (HSLColor::channels == 4) {
    return HSLColor{new_h, src.template get_index<1>(), src.template get_index<2>(), src.template get_index<3>()};
  } else {
    return HSLColor{new_h, src.template get_index<1>(), src.template get_index<2>()};
  }
}

}  // namespace colorcpp::operations::palette::details
