/**
 * @file gamut.hpp
 * @brief Gamut boundary utilities for color interpolation.
 */

#pragma once

#include <cmath>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::operations::interpolate {

namespace details {

/**
 * @brief Compute maximum safe chroma for a given lightness and hue in OkLCH space.
 * @param L Lightness in [0, 1]
 * @param H Hue angle in degrees [0, 360)
 * @return Maximum chroma that can be represented in sRGB gamut for the given L and H.
 * @note Uses binary search to find the gamut boundary by checking if the resulting RGB is in [0,1].
 */
inline float max_chroma_for(float L, float H) {
  // Binary search for maximum chroma that stays in sRGB gamut
  float lo = 0.0f;
  float hi = 0.4f;  // OkLCH chroma is typically in [0, ~0.4]

  // Create OkLCH color and check if it's in gamut
  auto in_gamut = [&](float C) -> bool {
    core::oklch_t oklch{L, C, H};
    auto rgb = operations::conversion::color_cast<core::rgbaf_t>(oklch);
    return rgb.r() >= 0.0f && rgb.r() <= 1.0f && rgb.g() >= 0.0f && rgb.g() <= 1.0f && rgb.b() >= 0.0f &&
           rgb.b() <= 1.0f;
  };

  // Binary search with 10 iterations gives ~0.0004 precision
  for (int i = 0; i < 10; ++i) {
    float mid = (lo + hi) / 2.0f;
    if (in_gamut(mid)) {
      lo = mid;
    } else {
      hi = mid;
    }
  }

  return lo;
}

}  // namespace details

}  // namespace colorcpp::operations::interpolate
