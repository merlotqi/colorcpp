/**
 * @file ohno.hpp
 * @brief Ohno (2013) correlated colour temperature and Duv estimation.
 *
 * Port of colour-science's uv_to_CCT_Ohno2013 / CCT_to_uv_Ohno2013: nearest
 * Planckian table row, then triangular interpolation with a parabolic
 * correction when the sample sits more than 0.002 from the locus. All uv
 * values are CIE 1960 UCS.
 *
 * @see Ohno, N. (2013). Practical Locus and CCT Estimation. Proc. CIE 2013.
 * @see Ohno, N. (2014). Practical Use of Planckian Locus Table. Proc. AIC.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/algorithms/color_temperature/common.hpp>
#include <colorcpp/algorithms/color_temperature/planckian.hpp>
#include <colorcpp/core/xyy.hpp>
#include <colorcpp/core/xyz.hpp>
#include <cstddef>
#include <utility>

namespace colorcpp::algorithms::color_temperature {

namespace details {

// Below this |Duv| the locus is essentially straight over one table step;
// Ohno (2013) then prefers the triangular interpolation.
inline constexpr float OHNO_PARABOLIC_DUV_MIN = 0.002f;

}  // namespace details

/**
 * @brief Estimate CCT and Duv from CIE 1960 uv chromaticity (Ohno 2013).
 *
 * @param u CIE 1960 UCS u chromaticity.
 * @param v CIE 1960 UCS v chromaticity (NOT CIE 1976 v').
 * @param table Planckian locus table (defaults to 1000–25000 K, spacing 1.005).
 * @return Correlated colour temperature and signed Duv.
 */
inline cct_duv uv_to_cct_ohno(float u, float v, const planckian_table& table) {
  // Nearest table row by uv distance.
  std::size_t index = 0;
  float min_dist2 = 3.0f;  // any real sample is far closer than this
  for (std::size_t i = 0; i < table.size(); ++i) {
    const float du = u - table.u(i);
    const float dv = v - table.v(i);
    const float dist2 = du * du + dv * dv;
    if (dist2 < min_dist2) {
      min_dist2 = dist2;
      index = i;
    }
  }
  // Interpolation needs the previous and next rows.
  index = std::clamp(index, std::size_t(1), table.size() - 2);

  const std::size_t i_prev = index - 1;
  const std::size_t i_next = index + 1;
  const float t_prev = table.temperature(i_prev);
  const float t = table.temperature(index);
  const float t_next = table.temperature(i_next);
  const float u_prev = table.u(i_prev);
  const float v_prev = table.v(i_prev);
  const float u_cur = table.u(index);
  const float v_cur = table.v(index);
  const float u_next = table.u(i_next);
  const float v_next = table.v(i_next);
  const float d_prev = std::hypot(u - u_prev, v - v_prev);
  const float d_cur = std::hypot(u - u_cur, v - v_cur);
  const float d_next = std::hypot(u - u_next, v - v_next);

  // Triangular interpolation (Ohno 2013, eq. 4–6).
  const float l = std::hypot(u_next - u_prev, v_next - v_prev);
  const float x = (d_prev * d_prev - d_next * d_next + l * l) / (2.0f * l);
  const float t_tri = t_prev + (t_next - t_prev) * (x / l);
  const float u_vertex = u_prev + (u_cur - u_prev) * (x / l);
  const float v_vertex = v_prev + (v_cur - v_prev) * (x / l);
  const float sign = (v < v_vertex) ? -1.0f : 1.0f;
  const float d_uv_tri = std::sqrt(std::max(d_prev * d_prev - x * x, 0.0f)) * sign;

  // Parabolic interpolation through the three table points
  // (Ohno 2014, eqs. 26-28; signs per colour-science).
  const float X = (t_next - t) * (t_prev - t_next) * (t - t_prev);
  const float a = (t_prev * (d_next - d_cur) + t * (d_prev - d_next) + t_next * (d_cur - d_prev)) / X;
  const float b =
      ((t_prev * t_prev) * (d_cur - d_next) + (t * t) * (d_next - d_prev) + (t_next * t_next) * (d_prev - d_cur)) / X;
  const float c = -(d_prev * (t_next - t) * t * t_next + d_cur * (t_prev - t_next) * t_prev * t_next +
                    d_next * (t - t_prev) * t_prev * t) /
                  X;
  const float t_par = -b / (2.0f * a);
  const float d_uv_par = (a * t_par * t_par + b * t_par + c) * sign;

  if (std::abs(d_uv_tri) >= details::OHNO_PARABOLIC_DUV_MIN) {
    return {t_par, d_uv_par};
  }
  return {t_tri, d_uv_tri};
}

/** @brief Estimate CCT and Duv using the default Planckian table. */
inline cct_duv uv_to_cct_ohno(float u, float v) {
  static const planckian_table default_table;
  return uv_to_cct_ohno(u, v, default_table);
}

/** @brief Estimate CCT and Duv from CIE xyY chromaticity (luminance ignored). */
inline cct_duv xyy_to_cct_ohno(const core::xyy_t& xyy) {
  const auto uv = xyy_to_uv1960(xyy);
  return uv_to_cct_ohno(uv.first, uv.second);
}

/** @brief Estimate CCT and Duv from XYZ tristimulus values. */
inline cct_duv xyz_to_cct_ohno(const core::xyz_t& xyz) {
  const auto uv = xyz_to_uv1960(xyz);
  return uv_to_cct_ohno(uv.first, uv.second);
}

/**
 * @brief Planckian-locus uv for a given CCT and Duv offset (Ohno 2013).
 *
 * Offsets the locus chromaticity by @p duv along the local normal,
 * estimated from the finite difference of two nearby Planckian points.
 *
 * @param cct Correlated colour temperature in kelvin.
 * @param duv Signed distance from the locus in CIE 1960 uv.
 * @return CIE 1960 UCS uv chromaticity.
 */
inline std::pair<float, float> cct_to_uv_ohno(float cct, float duv) {
  const auto uv0 = planck_uv(cct);
  // Colour-science uses a 0.01 K step in double precision; in float32 the
  // resulting uv delta (~5e-8) cancels below the mantissa, so use 1 K
  // (direction error ~1e-7 in uv — far inside test tolerance).
  const auto uv1 = planck_uv(cct + 1.0f);
  const float du = uv0.first - uv1.first;
  const float dv = uv0.second - uv1.second;
  const float h = std::hypot(du, dv);
  if (h == 0.0f) return uv0;
  // Normal pointing toward increasing v; Duv sign convention matches uv_to_cct.
  return {uv0.first - duv * dv / h, uv0.second + duv * du / h};
}

}  // namespace colorcpp::algorithms::color_temperature
