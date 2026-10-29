/**
 * @file robertson.hpp
 * @brief Robertson (1968) CCT and Duv estimation via isothermal lines.
 *
 * Port of colour-science's uv_to_CCT_Robertson1968 / CCT_to_uv_Robertson1968.
 * Thirty isothermal lines (mired-spaced) with local slopes; the sample is
 * projected onto the bracketing lines and the crossing ratio interpolates
 * temperature, uv and Duv. CIE 1960 UCS uv throughout.
 *
 * The table carries Lindbloom's correction of the 325 mired row (u
 * 0.24702 -> 0.24792); with the original value the published colour-science
 * anchor (6500.0162 K) misses by ~0.2 K.
 *
 * @see Robertson, A. R. (1968). Calculation of correlated colour temperature
 *      from the chromaticity diagram. JOSA, 58(11), 1528–1532.
 */

#pragma once

#include <cmath>
#include <colorcpp/algorithms/color_temperature/common.hpp>
#include <cstddef>
#include <utility>

namespace colorcpp::algorithms::color_temperature {

namespace details {

// Robertson (1968) isothermal lines: {mired, u, v, dv/du of the line}.
struct robertson_line {
  float mired;
  float u;
  float v;
  float slope;
};

// Verbatim from colour-science DATA_ISOTEMPERATURE_LINES_ROBERTSON1968
// (includes the Lindbloom 325 mired correction).
inline constexpr robertson_line ROBERTSON1968_LINES[] = {
    {0.0f, 0.18006f, 0.26352f, -0.24341f},     {10.0f, 0.18066f, 0.26589f, -0.25479f},
    {20.0f, 0.18133f, 0.26846f, -0.26876f},    {30.0f, 0.18208f, 0.27119f, -0.28539f},
    {40.0f, 0.18293f, 0.27407f, -0.30470f},    {50.0f, 0.18388f, 0.27709f, -0.32675f},
    {60.0f, 0.18494f, 0.28021f, -0.35156f},    {70.0f, 0.18611f, 0.28342f, -0.37915f},
    {80.0f, 0.18740f, 0.28668f, -0.40955f},    {90.0f, 0.18880f, 0.28997f, -0.44278f},
    {100.0f, 0.19032f, 0.29326f, -0.47888f},   {125.0f, 0.19462f, 0.30141f, -0.58204f},
    {150.0f, 0.19962f, 0.30921f, -0.70471f},   {175.0f, 0.20525f, 0.31647f, -0.84901f},
    {200.0f, 0.21142f, 0.32312f, -1.01820f},   {225.0f, 0.21807f, 0.32909f, -1.21680f},
    {250.0f, 0.22511f, 0.33439f, -1.45120f},   {275.0f, 0.23247f, 0.33904f, -1.72980f},
    {300.0f, 0.24010f, 0.34308f, -2.06370f},   {325.0f, 0.24792f, 0.34655f, -2.46810f},
    {350.0f, 0.25591f, 0.34951f, -2.96410f},   {375.0f, 0.26400f, 0.35200f, -3.58140f},
    {400.0f, 0.27218f, 0.35407f, -4.36330f},   {425.0f, 0.28039f, 0.35577f, -5.37620f},
    {450.0f, 0.28863f, 0.35714f, -6.72620f},   {475.0f, 0.29685f, 0.35823f, -8.59550f},
    {500.0f, 0.30505f, 0.35907f, -11.32400f},  {525.0f, 0.31320f, 0.35968f, -15.62800f},
    {550.0f, 0.32129f, 0.36011f, -23.32500f},  {575.0f, 0.32931f, 0.36038f, -40.77000f},
    {600.0f, 0.33724f, 0.36051f, -116.45000f},
};

}  // namespace details

/**
 * @brief Estimate CCT and Duv from CIE 1960 uv (Robertson 1968).
 *
 * @param u CIE 1960 UCS u chromaticity.
 * @param v CIE 1960 UCS v chromaticity (NOT CIE 1976 v').
 * @return Correlated colour temperature and signed Duv.
 */
inline cct_duv uv_to_cct_robertson1968(float u, float v) {
  using namespace details;

  constexpr std::size_t n = sizeof(ROBERTSON1968_LINES) / sizeof(ROBERTSON1968_LINES[0]);

  // Normalized direction vectors of the isothermal lines.
  float du_unit[n], dv_unit[n];
  for (std::size_t i = 0; i < n; ++i) {
    const float t = ROBERTSON1968_LINES[i].slope;
    const float inv_h = 1.0f / std::hypot(1.0f, t);
    du_unit[i] = inv_h;
    dv_unit[i] = t * inv_h;
  }

  // Signed perpendicular distance of the sample to each line (lines 1..n-1);
  // the first sign crossing brackets the sample.
  float dt[n] = {};
  for (std::size_t i = 1; i < n; ++i) {
    const float uu = u - ROBERTSON1968_LINES[i].u;
    const float vv = v - ROBERTSON1968_LINES[i].v;
    dt[i] = -uu * dv_unit[i] + vv * du_unit[i];
  }

  std::size_t i_max = n - 1;  // beyond the table: clamp to the last line
  for (std::size_t i = 1; i < n; ++i) {
    if (dt[i] <= 0.0f) {
      i_max = i;
      break;
    }
  }

  // Crossing ratio: distance at i_max over the total across the two lines.
  float f = 0.0f;
  if (i_max != 1) {
    const float dt_cur = -std::min(dt[i_max], 0.0f);
    const float dt_prev = dt[i_max - 1];
    const float denom = dt_prev + dt_cur;
    if (denom > 0.0f) f = dt_cur / denom;
  }
  const std::size_t i_prev = i_max - 1;
  const std::size_t i_cur = i_max;

  // Interpolate mired, uv and direction, then renormalize the direction.
  const float mired =
      ROBERTSON1968_LINES[i_cur].mired - f * (ROBERTSON1968_LINES[i_cur].mired - ROBERTSON1968_LINES[i_prev].mired);
  const float u_line =
      ROBERTSON1968_LINES[i_cur].u - f * (ROBERTSON1968_LINES[i_cur].u - ROBERTSON1968_LINES[i_prev].u);
  const float v_line =
      ROBERTSON1968_LINES[i_cur].v - f * (ROBERTSON1968_LINES[i_cur].v - ROBERTSON1968_LINES[i_prev].v);
  float du = du_unit[i_cur] - f * (du_unit[i_cur] - du_unit[i_prev]);
  float dv = dv_unit[i_cur] - f * (dv_unit[i_cur] - dv_unit[i_prev]);
  const float inv_h = 1.0f / std::hypot(du, dv);
  du *= inv_h;
  dv *= inv_h;

  // Signed distance along the line direction.
  const float duv = -((u - u_line) * du + (v - v_line) * dv);

  return {mired_to_cct(mired), duv};
}

/**
 * @brief Planckian-locus uv for a CCT and Duv offset (Robertson 1968).
 *
 * @param cct Correlated colour temperature in kelvin.
 * @param duv Signed distance from the locus in CIE 1960 uv.
 * @return CIE 1960 UCS uv chromaticity.
 */
inline std::pair<float, float> cct_to_uv_robertson1968(float cct, float duv) {
  using namespace details;

  constexpr std::size_t n = sizeof(ROBERTSON1968_LINES) / sizeof(ROBERTSON1968_LINES[0]);

  const float mired = cct_to_mired(cct);

  // Bracketing lines: i_cur is the first row with mired >= mired(cct).
  std::size_t i_cur = n - 1;
  for (std::size_t i = 1; i < n; ++i) {
    if (ROBERTSON1968_LINES[i].mired >= mired) {
      i_cur = i;
      break;
    }
  }
  const std::size_t i_prev = i_cur - 1;

  const float m_prev = ROBERTSON1968_LINES[i_prev].mired;
  const float m_cur = ROBERTSON1968_LINES[i_cur].mired;
  const float f = (m_cur != m_prev) ? (mired - m_prev) / (m_cur - m_prev) : 0.0f;

  const float u_line =
      ROBERTSON1968_LINES[i_prev].u + f * (ROBERTSON1968_LINES[i_cur].u - ROBERTSON1968_LINES[i_prev].u);
  const float v_line =
      ROBERTSON1968_LINES[i_prev].v + f * (ROBERTSON1968_LINES[i_cur].v - ROBERTSON1968_LINES[i_prev].v);

  const float t_prev = ROBERTSON1968_LINES[i_prev].slope;
  const float t_cur = ROBERTSON1968_LINES[i_cur].slope;
  const float slope = t_prev + f * (t_cur - t_prev);
  const float inv_h = 1.0f / std::hypot(1.0f, slope);
  const float du = inv_h;
  const float dv = slope * inv_h;

  // Offset along the normal; sign convention matches uv_to_cct_robertson1968.
  return {u_line - duv * du, v_line - duv * dv};
}

}  // namespace colorcpp::algorithms::color_temperature
