/**
 * @file planckian.hpp
 * @brief Planckian (blackbody) locus by spectral integration.
 *
 * Integrates Planck's law against the embedded CIE 1931 2° standard observer
 * colour matching functions (5 nm steps, 360–830 nm) with the trapezoidal
 * rule, then converts the resulting tristimulus values to CIE 1960 UCS uv
 * chromaticity. The 5 nm sampling agrees with 1 nm integration to ~3e-7 in
 * uv (validated against colour-science).
 *
 * @see CIE 15:2004, Appendix E (Planck's equation for standard air).
 */

#pragma once

#include <cmath>
#include <colorcpp/algorithms/color_temperature/common.hpp>
#include <colorcpp/algorithms/spectral/integrate.hpp>
#include <cstddef>
#include <utility>
#include <vector>

namespace colorcpp::algorithms::color_temperature {

namespace details {

// Planck's law spectral radiance (CIE 15:2004 constants, n = 1);
// absolute scale cancels in chromaticity so only the shape matters.
inline constexpr double PLANCK_C1 = 3.741771e-16;
inline constexpr double PLANCK_C2 = 1.4388e-2;

// Integrated in double: a float32 integration carries ~4e-8 uv noise that
// varies with FMA contraction and libm, which is the same order as the
// sub-step curvature signal Ohno's parabolic fit and the forward normal
// depend on at high CCT (issue #109 follow-up: ARM64/macOS/MSVC results
// diverged from x86 by >300 K at the table end).
inline std::pair<double, double> planck_uv_precise(double cct) {
  // The generic trapezoidal integrator (algorithms/spectral) supplies the
  // double-precision accumulation over the shared CMF table; Planck's law is
  // the spectral value. Working in nm (instead of the previous metres)
  // keeps the wavelength steps exact integers — chromaticity cancels the
  // unit scale, and the change is ~1e-16 in uv (guarded by the Ohno
  // reference tests).
  const auto xyz = ::colorcpp::algorithms::spectral::integrate_spectrum([cct](double nm) {
    const double lambda = nm * 1e-9;
    return PLANCK_C1 * std::pow(lambda, -5.0) / std::expm1(PLANCK_C2 / (lambda * cct));
  });
  const double d = xyz.x + 15.0 * xyz.y + 3.0 * xyz.z;
  if (!(d > 0.0)) return {0.0, 0.0};
  return {4.0 * xyz.x / d, 6.0 * xyz.y / d};
}

}  // namespace details

/**
 * @brief Planckian locus chromaticity at a thermodynamic temperature.
 *
 * Spectral radiance S(λ) ∝ λ^-5 / (exp(c2 / (λ T)) - 1) integrated against
 * the CIE 1931 2° CMFs (trapezoidal rule) yields XYZ; the result is returned
 * as CIE 1960 UCS (u, v).
 *
 * @param cct Temperature in kelvin (> 0).
 * @return CIE 1960 UCS uv chromaticity of the blackbody at @p cct.
 */
inline std::pair<float, float> planck_uv(float cct) {
  const auto uv = details::planck_uv_precise(cct);
  return {static_cast<float>(uv.first), static_cast<float>(uv.second)};
}

/**
 * @brief Planckian locus table for Ohno (2013) CCT lookup.
 *
 * Geometrically spaced temperatures from @p start to @p end (Ohno 2014,
 * section "Planckian table"): after each row the step multiplier decays
 * linearly from @p spacing toward 1 + (spacing - 1) / 10 at high CCT, so
 * density increases where the locus curves most. Rows @c end - 1 and
 * @c end are always appended.
 *
 * Rows are stored in double precision; the accessors round to float.
 */
class planckian_table {
 public:
  /**
   * @brief Build the table.
   * @param start Lowest temperature in kelvin (inclusive).
   * @param end Highest temperature in kelvin (inclusive).
   * @param spacing Geometric step multiplier (> 1; smaller = more precise).
   */
  planckian_table(float start = 1000.0f, float end = 25000.0f, float spacing = 1.005f) {
    t_.reserve(1024);
    u_.reserve(1024);
    v_.reserve(1024);

    auto push = [this](double t) {
      const auto uv = details::planck_uv_precise(t);
      t_.push_back(t);
      u_.push_back(uv.first);
      v_.push_back(uv.second);
    };

    const double t_start = start;
    const double t_end = end;
    const double k = spacing;

    push(t_start);
    push(t_start + 1.0);

    double next_t = t_start + 1.0;
    double next_spacing = k;
    while (true) {
      next_t *= next_spacing;
      if (next_t >= t_end) break;
      push(next_t);
      // Slightly decrease step-size for higher CCT (Ohno 2014).
      double D = (next_t - 1000.0) / 99000.0;
      D = D < 0.0 ? 0.0 : (D > 1.0 ? 1.0 : D);
      next_spacing = k * (1.0 - D) + (1.0 + (k - 1.0) / 10.0) * D;
    }
    push(t_end - 1.0);
    push(t_end);
  }

  std::size_t size() const { return t_.size(); }
  float temperature(std::size_t i) const { return static_cast<float>(t_[i]); }
  float u(std::size_t i) const { return static_cast<float>(u_[i]); }
  float v(std::size_t i) const { return static_cast<float>(v_[i]); }

 private:
  // Ohno's parabolic fit resolves ~1e-8 uv curvature differences between
  // neighbouring rows; float-rounded rows (~7e-9 ulp) would swamp it.
  friend cct_duv uv_to_cct_ohno(float u, float v, const planckian_table& table);

  std::vector<double> t_, u_, v_;
};

}  // namespace colorcpp::algorithms::color_temperature
