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
  // Clamp out-of-gamut linear values (negative or > 1) into the displayable
  // range; simulation results may exceed the sRGB gamut.
  v = std::clamp(v, 0.0f, 1.0f);
  return (v <= 0.0031308f) ? (12.92f * v) : (1.055f * std::pow(v, 1.0f / 2.4f) - 0.055f);
}

using vec3 = std::array<float, 3>;

// 3x3 matrix-vector multiplication
inline vec3 mat3_mul(const float M[3][3], const vec3& v) noexcept {
  return {M[0][0] * v[0] + M[0][1] * v[1] + M[0][2] * v[2], M[1][0] * v[0] + M[1][1] * v[1] + M[1][2] * v[2],
          M[2][0] * v[0] + M[2][1] * v[1] + M[2][2] * v[2]};
}

// Linearized sRGB → LMS (Smith & Pokorny 1975 cone fundamentals).
// The Brettel/Viénot simulation coefficients used by the protanopia,
// deuteranopia and tritanopia simulators are only valid in this basis,
// where the neutral axis maps to ~(0.655, 0.345, 0.0168) — NOT to
// (1, 1, 1). Pairing them with an HPE-style matrix breaks gray/white
// preservation. Same matrix as the coloraide/colorspacious references.
constexpr float kRGBtoLMS[3][3] = {
    {0.178824041258f, 0.4351609057f, 0.04119349692f},
    {0.034556423182f, 0.27155382458f, 0.03867130836f},
    {0.000299565576f, 0.0018430896f, 0.01467086136f},
};

// LMS → linearized sRGB (exact inverse of kRGBtoLMS)
constexpr float kLMStoRGB[3][3] = {
    {8.09443559803f, -13.05043146050f, 11.672058453917f},
    {-1.02485055866f, 5.401931309675f, -11.361471490599f},
    {-0.03652974716f, -0.412162807001f, 69.351324238209f},
};

}  // namespace colorcpp::algorithms::vision::details
