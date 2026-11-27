/**
 * @file integrate.hpp
 * @brief Trapezoidal spectrum integration against the CIE 1931 2° CMFs.
 *
 * The integrator is the generic core of the spectral module: the spectral
 * value is supplied by a callable f(λ_nm), so analytic sources (Planck's
 * law), sampled SPDs and (later) reflectance products all share one
 * accumulation path. Double accumulation is deliberate — a float32
 * integration carries ~4e-8 chromaticity noise that matters downstream
 * (issue #109: the Ohno parabolic fit resolves sub-step curvature).
 *
 * Wavelengths are nanometres on both sides of the callable, so the
 * trapezoid step dl is in nm; the resulting XYZ is on the "per nm" scale
 * (its magnitude follows f's units). Chromaticity-derived quantities
 * (xy, uv) are invariant to that scale.
 */

#pragma once

#include <colorcpp/algorithms/spectral/cmf.hpp>
#include <colorcpp/algorithms/spectral/spd.hpp>
#include <colorcpp/core/xyz.hpp>
#include <cstddef>
#include <utility>

namespace colorcpp::algorithms::spectral {

/** @brief Double-precision tristimulus result of @ref integrate_spectrum. */
struct spectrum_xyz {
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
};

/**
 * @brief Trapezoidal-rule ∫ f(λ)·x̄(λ) dλ over the CIE 1931 2° CMF grid.
 *
 * @param f Spectral value at a wavelength in nm (e.g. radiance, relative
 *          power); invoked once per CMF node in ascending order.
 * @return Raw XYZ in double precision (per-nm scale, see file note).
 */
template <typename F>
inline spectrum_xyz integrate_spectrum(F&& f) {
  constexpr std::size_t n = sizeof(details::CMF_1931_2DEG) / sizeof(details::CMF_1931_2DEG[0]);

  spectrum_xyz acc;
  double s_prev = 0.0;
  double nm_prev = 0.0;
  for (std::size_t i = 0; i < n; ++i) {
    const double nm = static_cast<double>(details::CMF_1931_2DEG[i].nm);
    const double s = static_cast<double>(f(nm));
    if (i > 0) {
      const double dl = nm - nm_prev;
      acc.x += 0.5 * (s * details::CMF_1931_2DEG[i].x_bar + s_prev * details::CMF_1931_2DEG[i - 1].x_bar) * dl;
      acc.y += 0.5 * (s * details::CMF_1931_2DEG[i].y_bar + s_prev * details::CMF_1931_2DEG[i - 1].y_bar) * dl;
      acc.z += 0.5 * (s * details::CMF_1931_2DEG[i].z_bar + s_prev * details::CMF_1931_2DEG[i - 1].z_bar) * dl;
    }
    s_prev = s;
    nm_prev = nm;
  }
  return acc;
}

/**
 * @brief Emissive SPD to raw XYZ (trapezoidal rule, 2° observer).
 *
 * The spd is linearly interpolated at the CMF nodes (edge-clamped outside
 * its grid). Output follows the spd's magnitude — normalize externally
 * (e.g. divide by Y) when a relative result is wanted.
 */
inline core::xyz_t spd_to_xyz(const spd& s) {
  const auto v = integrate_spectrum([&s](double nm) { return static_cast<double>(s.sample(static_cast<float>(nm))); });
  return core::xyz_t(static_cast<float>(v.x), static_cast<float>(v.y), static_cast<float>(v.z));
}

}  // namespace colorcpp::algorithms::spectral
