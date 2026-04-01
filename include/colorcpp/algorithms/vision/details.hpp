/**
 * @file details.hpp
 * @brief Internal helper functions for color vision deficiency simulation.
 */

#pragma once

#include <algorithm>
#include <array>
#include <cmath>

namespace colorcpp::algorithms::vision::details {

// sRGB linearization (IEC 61966-2-1)
inline float linearize(float v) noexcept {
  // Handle NaN and Inf
  if (!std::isfinite(v)) return 0.0f;
  return (v <= 0.04045f) ? (v / 12.92f) : std::pow((v + 0.055f) / 1.055f, 2.4f);
}

// sRGB gamma encoding
inline float gamma_encode(float v) noexcept {
  // Handle NaN and Inf
  if (!std::isfinite(v)) return 0.0f;
  v = std::clamp(v, 0.0f, 1.0f);
  return (v <= 0.0031308f) ? (12.92f * v) : (1.055f * std::pow(v, 1.0f / 2.4f) - 0.055f);
}

using vec3 = std::array<float, 3>;

// 3x3 matrix-vector multiplication
inline vec3 mat3_mul(const float M[3][3], const vec3& v) noexcept {
  return {M[0][0] * v[0] + M[0][1] * v[1] + M[0][2] * v[2], M[1][0] * v[0] + M[1][1] * v[1] + M[1][2] * v[2],
          M[2][0] * v[0] + M[2][1] * v[1] + M[2][2] * v[2]};
}

// Linearized sRGB → LMS (Hunt-Pointer-Estevez, D65 normalization)
// Source: Viénot et al. 1999, Table 1
constexpr float kRGBtoLMS[3][3] = {
    {0.31399022f, 0.63951294f, 0.04649755f},
    {0.15537241f, 0.75789446f, 0.08670142f},
    {0.01775239f, 0.10944209f, 0.87256922f},
};

// LMS → linearized sRGB (exact inverse of kRGBtoLMS)
constexpr float kLMStoRGB[3][3] = {
    {5.47221206f, -4.64196010f, 0.16963708f},
    {-1.12524191f, 2.29317094f, -0.16789520f},
    {0.02980165f, -0.19318073f, 1.16364789f},
};

}  // namespace colorcpp::algorithms::vision::details
