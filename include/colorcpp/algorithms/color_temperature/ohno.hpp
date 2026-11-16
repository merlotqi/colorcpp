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
 * Samples outside the table's uv range clamp to the table's temperature
 * bounds instead of extrapolating (issue #101).
 *
 * @param u CIE 1960 UCS u chromaticity.
 * @param v CIE 1960 UCS v chromaticity (NOT CIE 1976 v').
 * @param table Planckian locus table (defaults to 1000–25000 K, spacing 1.005).
 * @return Correlated colour temperature (clamped to the table range) and signed Duv.
 */
inline cct_duv uv_to_cct_ohno(float u_in, float v_in, const planckian_table& table) {
  // Evaluated in double against the table's double rows: at |Duv| ~ 0.05 the
  // distances to neighbouring rows differ by only ~1e-8, at the level of
  // float32 rounding, so a float evaluation returned FMA/libm-dependent
  // results (parabola pinned at Duv 0.050512 on x86 vs 0.050146 on ARM64 for
  // a true 0.05; triangular CCT 24665 instead of ~24996 at the table end).
  const double u = u_in;
  const double v = v_in;
  const auto& T = table.t_;
  const auto& U = table.u_;
  const auto& V = table.v_;

  // Nearest table row by uv distance.
  std::size_t index = 0;
  double min_dist2 = 3.0;  // any real sample is far closer than this
  for (std::size_t i = 0; i < table.size(); ++i) {
    const double du = u - U[i];
    const double dv = v - V[i];
    const double dist2 = du * du + dv * dv;
    if (dist2 < min_dist2) {
      min_dist2 = dist2;
      index = i;
    }
  }
  // Interpolation needs the previous and next rows.
  index = std::clamp(index, std::size_t(1), table.size() - 2);

  const std::size_t i_prev = index - 1;
  const std::size_t i_next = index + 1;
  const double t_prev = T[i_prev];
  const double t = T[index];
  const double t_next = T[i_next];
  const double u_prev = U[i_prev];
  const double v_prev = V[i_prev];
  const double u_cur = U[index];
  const double v_cur = V[index];
  const double u_next = U[i_next];
  const double v_next = V[i_next];
  const double d_prev = std::hypot(u - u_prev, v - v_prev);
  const double d_cur = std::hypot(u - u_cur, v - v_cur);
  const double d_next = std::hypot(u - u_next, v - v_next);

  // Triangular interpolation (Ohno 2013, eq. 4–6).
  const double l = std::hypot(u_next - u_prev, v_next - v_prev);
  const double x = (d_prev * d_prev - d_next * d_next + l * l) / (2.0 * l);
  const double t_tri = t_prev + (t_next - t_prev) * (x / l);
  const double v_vertex = v_prev + (v_next - v_prev) * (x / l);
  const double sign = (v < v_vertex) ? -1.0 : 1.0;
  const double d_uv_tri = std::sqrt(std::max(d_prev * d_prev - x * x, 0.0)) * sign;

  // Parabolic interpolation through the three table points
  // (Ohno 2014, eqs. 26-28; signs per colour-science).
  const double X = (t_next - t) * (t_prev - t_next) * (t - t_prev);
  const double a = (t_prev * (d_next - d_cur) + t * (d_prev - d_next) + t_next * (d_cur - d_prev)) / X;
  const double b =
      ((t_prev * t_prev) * (d_cur - d_next) + (t * t) * (d_next - d_prev) + (t_next * t_next) * (d_prev - d_cur)) / X;
  const double c = -(d_prev * (t_next - t) * t * t_next + d_cur * (t_prev - t_next) * t_prev * t_next +
                     d_next * (t - t_prev) * t_prev * t) /
                   X;
  const double t_par = -b / (2.0 * a);
  const double d_uv_par = (a * t_par * t_par + b * t_par + c) * sign;

  // Sanity guards for the parabola (issue #109). The fit divides by X built
  // from the row spacing, so on the near-duplicate appended end rows
  // (24996/24999/25000: 98 K / 3 K / 1 K gaps) — and on the matching start
  // rows (1000/1001/1005) — it explodes: probes at Duv=-0.005 came back as
  // +0.0216/+0.0407 (sign flip), and an absurd vertex extrapolated Duv=-0.93
  // for a point above the locus. Exclude degenerate spacing (either
  // neighbouring gap more than twice the other) and require the vertex to be
  // the projection onto the locus, i.e. between the bracketing rows; fall
  // back to the triangular interpolant otherwise.
  const double gap_prev = t - t_prev;
  const double gap_next = t_next - t;
  const bool spacing_ok = !(gap_prev > 2.0 * gap_next) && !(gap_next > 2.0 * gap_prev);
  const bool vertex_ok = std::isfinite(t_par) && t_par >= t_prev && t_par <= t_next;
  const bool parabola_ok = spacing_ok && vertex_ok;

  // The interpolants extrapolate beyond the clamped index for samples
  // outside the table's uv range (issue #101: uv of 500 K returned 396 K,
  // uv of 1 MK returned 47513 K). Keep the result inside the table.
  const double t_min = T.front();
  const double t_max = T.back();
  if (std::abs(d_uv_tri) >= details::OHNO_PARABOLIC_DUV_MIN && parabola_ok) {
    return {static_cast<float>(std::clamp(t_par, t_min, t_max)), static_cast<float>(d_uv_par)};
  }
  return {static_cast<float>(std::clamp(t_tri, t_min, t_max)), static_cast<float>(d_uv_tri)};
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
 * @param cct Correlated colour temperature in kelvin. Non-positive or
 *            non-finite values clamp to the table minimum (1000 K) —
 *            planck_uv(0) integrates to zero XYZ and negatives yield
 *            garbage (issue #101). Physically valid T > 0 is not clamped.
 * @param duv Signed distance from the locus in CIE 1960 uv.
 * @return CIE 1960 UCS uv chromaticity.
 */
inline std::pair<float, float> cct_to_uv_ohno(float cct, float duv) {
  if (!std::isfinite(cct) || cct <= 0.0f) cct = 1000.0f;  // issue #101
  const double T = cct;
  const auto uv0 = details::planck_uv_precise(T);
  // Temperature-relative central difference (issue #109). The previous
  // fixed 1 K forward step shrank to a ~5.2e-7 uv delta at 24000 K — below
  // the ~4e-8 float noise of planck_uv — so the normal was noise-dominated:
  // offsets drifted tangentially (8.0e-4 coordinate error vs a
  // double-precision reference at 24000 K / Duv 0.02, and round-trips
  // returned cct=25000, duv=+0.412). Stepping +/-0.5% of T keeps the delta
  // ~2.6e-5 uv at 24000 K, and the central difference cancels the chord's
  // first-order bias. Matches the issue's relative-step fix; colour-science
  // equivalently uses a 0.01 K step in double precision.
  const double step = T * 0.005;
  const auto uv_lo = details::planck_uv_precise(T - step);
  const auto uv_hi = details::planck_uv_precise(T + step);
  const double du = uv_lo.first - uv_hi.first;
  const double dv = uv_lo.second - uv_hi.second;
  const double h = std::hypot(du, dv);
  if (h == 0.0) return {static_cast<float>(uv0.first), static_cast<float>(uv0.second)};
  // Normal pointing toward increasing v; Duv sign convention matches uv_to_cct.
  const double D = duv;
  return {static_cast<float>(uv0.first - D * dv / h), static_cast<float>(uv0.second + D * du / h)};
}

}  // namespace colorcpp::algorithms::color_temperature
