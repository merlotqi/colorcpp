/**
 * @file reflectance.hpp
 * @brief Reflectance integration: R(λ)·S(λ) against the CIE CMFs.
 *
 * A reflecting sample is characterised by its spectral reflectance R(λ)
 * (0..1, or percent) under an illuminant SPD S(λ). The tristimulus values
 * are the double integral
 *
 *   X = k ∫ R(λ) S(λ) x̄(λ) dλ,  Y = ...,  Z = ...,
 *
 * with the normalization k = 100 / ∫ S(λ) ȳ(λ) dλ. The k factor makes the
 * result independent of the illuminant's absolute magnitude: a perfect
 * reflecting diffuser (R ≡ 1) yields Y = 100 and XYZ equal to 100 times the
 * illuminant white point, the CIE 15:2004 convention for reflecting samples.
 */

#pragma once

#include <colorcpp/algorithms/spectral/integrate.hpp>
#include <colorcpp/algorithms/spectral/spd.hpp>

namespace colorcpp::algorithms::spectral {

/**
 * @brief Tristimulus values of a reflecting sample under an illuminant.
 *
 * Both spectra are sampled at the CMF nodes with spd::sample() semantics
 * (linear interpolation, edge-clamped outside the sampled range), then
 * integrated with the same trapezoidal rule as integrate_spectrum().
 * Reflectance values are used as given (no clamping): negative values are
 * meaningful for fluorescence corrections, values above 1 for non-physical
 * test data.
 *
 * @param reflectance Spectral reflectance R(λ).
 * @param illuminant  Illuminant SPD S(λ); must have positive Y.
 * @param obs         Standard observer whose CMFs are used.
 * @return XYZ with Y normalized so that R ≡ 1 gives Y = 100. Returns black
 *         when the illuminant integrates to zero or negative Y.
 */
inline core::xyz_t reflectance_to_xyz(const spd& reflectance, const spd& illuminant,
                                      observer obs = observer::cie_1931_2deg) {
  const spectrum_xyz white = integrate_spectrum(
      [&illuminant](double nm) { return static_cast<double>(illuminant.sample(static_cast<float>(nm))); }, obs);
  if (!(white.y > 0.0)) {
    return core::xyz_t(0.0f, 0.0f, 0.0f);
  }
  const double k = 100.0 / white.y;
  const spectrum_xyz product = integrate_spectrum(
      [&reflectance, &illuminant](double nm) {
        const float lambda = static_cast<float>(nm);
        return static_cast<double>(reflectance.sample(lambda)) * static_cast<double>(illuminant.sample(lambda));
      },
      obs);
  return core::xyz_t(static_cast<float>(k * product.x), static_cast<float>(k * product.y),
                     static_cast<float>(k * product.z));
}

}  // namespace colorcpp::algorithms::spectral
