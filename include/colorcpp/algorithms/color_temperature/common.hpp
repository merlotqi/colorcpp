/**
 * @file common.hpp
 * @brief Shared types and helpers for colour temperature measurement.
 *
 * Colour temperature work uses the CIE 1960 UCS (u, v) chromaticity diagram
 * (u = 4X/(X+15Y+3Z), v = 6Y/(X+15Y+3Z)) — NOT the CIE 1976 u'v' diagram of
 * CIELUV (whose v' is 1.5x larger). Duv values are signed distances in that
 * 1960 plane.
 */

#pragma once

#include <cmath>
#include <colorcpp/core/xyy.hpp>
#include <colorcpp/core/xyz.hpp>
#include <utility>

namespace colorcpp::algorithms::color_temperature {

/**
 * @brief Correlated colour temperature result.
 */
struct cct_duv {
  float cct;  ///< Correlated colour temperature in kelvin.
  float duv;  ///< Signed distance from the Planckian locus in CIE 1960 uv.
};

/** @brief Convert micro reciprocal degrees (mired) to kelvin. */
inline float mired_to_cct(float mired) { return 1000000.0f / mired; }

/** @brief Convert kelvin to micro reciprocal degrees (mired). */
inline float cct_to_mired(float cct) { return 1000000.0f / cct; }

/**
 * @brief Convert xy chromaticity to CIE 1960 UCS uv.
 * u = 4x / (12y - 2x + 3), v = 6y / (12y - 2x + 3).
 */
inline std::pair<float, float> xy_to_uv1960(float x, float y) {
  const float d = 12.0f * y - 2.0f * x + 3.0f;
  if (d == 0.0f) return {0.0f, 0.0f};
  return {4.0f * x / d, 6.0f * y / d};
}

/**
 * @brief Convert CIE 1960 UCS uv chromaticity to xy.
 * x = 3u / (2u - 8v + 4), y = 2v / (2u - 8v + 4).
 */
inline std::pair<float, float> uv1960_to_xy(float u, float v) {
  const float d = 2.0f * u - 8.0f * v + 4.0f;
  if (d == 0.0f) return {0.0f, 0.0f};
  return {3.0f * u / d, 2.0f * v / d};
}

/** @brief XYZ tristimulus values to CIE 1960 UCS uv chromaticity. */
inline std::pair<float, float> xyz_to_uv1960(const core::xyz_t& xyz) {
  const float s = xyz.x() + 15.0f * xyz.y() + 3.0f * xyz.z();
  if (s == 0.0f) return {0.0f, 0.0f};
  return {4.0f * xyz.x() / s, 6.0f * xyz.y() / s};
}

/** @brief CIE xyY chromaticity to CIE 1960 UCS uv (luminance-independent). */
inline std::pair<float, float> xyy_to_uv1960(const core::xyy_t& xyy) { return xy_to_uv1960(xyy.x(), xyy.y()); }

}  // namespace colorcpp::algorithms::color_temperature
