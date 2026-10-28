/**
 * @file gamut.hpp
 * @brief Gamut boundary utilities for color interpolation.
 */

#pragma once

#include <cmath>
#include <colorcpp/algorithms/gamut/details.hpp>
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
 * @note Binary search over linear RGB without clamping (algorithms::gamut::details):
 *       the gamut predicate must see out-of-range values — a cast through an RGB
 *       color type would clamp them and report every chroma as in-gamut.
 */
inline float max_chroma_for(float L, float H) {
  // Binary search for maximum chroma that stays in sRGB gamut
  float lo = 0.0f;
  float hi = 0.4f;  // sRGB's global max OkLCH chroma (~0.316) is below this bound

  constexpr float kDegToRad = 3.14159265358979323846f / 180.0f;
  const float h_rad = H * kDegToRad;

  // Unclamped OkLCH -> linear sRGB in-gamut check. color_cast would clamp the
  // RGB channels to [0, 1] on the way, making this predicate always true.
  auto in_gamut = [&](float C) -> bool {
    float r, g, b;
    algorithms::gamut::details::oklab_to_linrgb(L, C * std::cos(h_rad), C * std::sin(h_rad), r, g, b);
    return algorithms::gamut::details::linrgb_in_gamut(r, g, b);
  };

  // 20 iterations: 0.4 / 2^20 ≈ 4e-7 precision in chroma
  for (int i = 0; i < 20; ++i) {
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
