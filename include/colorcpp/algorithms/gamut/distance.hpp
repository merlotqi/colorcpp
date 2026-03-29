/**
 * @file distance.hpp
 * @brief Gamut distance calculation functions.
 */

#pragma once

#include <cmath>
#include <colorcpp/algorithms/gamut/details.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::gamut {

/**
 * @brief Calculate the distance from a color to the gamut boundary.
 * @tparam Color Input color type.
 * @param c The color to check.
 * @return Distance to gamut boundary in OKLab space (ΔE_OK).
 *         Returns 0.0 if the color is already in gamut.
 */
template <typename Color>
float gamut_distance(const Color& c) {
  using namespace operations::conversion;

  auto lab = color_cast<core::oklab_t>(c);
  const float L = lab.template get_index<0>();
  const float a = lab.template get_index<1>();
  const float b = lab.template get_index<2>();

  // Check if already in gamut
  float r, g, bl;
  details::oklab_to_linrgb(L, a, b, r, g, bl);
  if (details::linrgb_in_gamut(r, g, bl)) {
    return 0.0f;
  }

  // Calculate chroma
  const float C = std::sqrt(a * a + b * b);
  if (C < 1e-6f) {
    // Achromatic color - distance is lightness-based
    if (L > 0.5f) {
      return L - 1.0f;  // Distance to white
    } else {
      return -L;  // Distance to black
    }
  }

  // Binary search for the maximum chroma that stays in gamut
  const float cos_h = a / C;
  const float sin_h = b / C;

  float min_C = 0.0f;
  float max_C = C;
  constexpr float kEps = 1e-4f;

  while (max_C - min_C > kEps) {
    const float mid_C = (min_C + max_C) * 0.5f;
    float r_test, g_test, b_test;
    details::oklab_to_linrgb(L, mid_C * cos_h, mid_C * sin_h, r_test, g_test, b_test);

    if (details::linrgb_in_gamut(r_test, g_test, b_test)) {
      min_C = mid_C;
    } else {
      max_C = mid_C;
    }
  }

  // Distance is the difference in chroma
  return C - min_C;
}

/**
 * @brief Check if a color is near the gamut boundary.
 * @tparam Color Input color type.
 * @param c The color to check.
 * @param threshold Distance threshold (default: 0.02 in OKLab space).
 * @return True if the color is within threshold distance of the gamut boundary.
 */
template <typename Color>
bool is_near_gamut_boundary(const Color& c, float threshold = 0.02f) {
  const float distance = gamut_distance(c);
  return distance > 0.0f && distance < threshold;
}

}  // namespace colorcpp::algorithms::gamut
