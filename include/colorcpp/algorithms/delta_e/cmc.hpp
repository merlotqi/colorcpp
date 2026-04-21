/**
 * @file cmc.hpp
 * @brief CMC l:c color difference metric (1984).
 *
 * The CMC (Colour Measurement Committee) formula was developed for the textile industry.
 * It is based on CIELAB but applies non-linear tolerance adjustments to L*, C*, and ΔH.
 *
 * @see https://en.wikipedia.org/wiki/Color_difference#CMC_l:c_(1984)
 */

#pragma once

#include <cmath>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::delta_e {

/**
 * @brief CMC l:c color difference.
 *
 * Common parameter sets:
 * - l=2, c=1: Acceptability (typical for production)
 * - l=1, c=1: Perceptibility (stricter, for matching)
 *
 * @param a First color (reference).
 * @param b Second color.
 * @param l Lightness weight (default: 2.0 for acceptability).
 * @param c Chroma weight (default: 1.0).
 * @return ΔE_CMC value.
 */
template <typename ColorA, typename ColorB>
float delta_e_cmc(const ColorA& a, const ColorB& b, float l = 2.0f, float c = 1.0f) {
  using namespace operations::conversion;

  auto la = color_cast<core::cielab_t>(a);
  auto lb = color_cast<core::cielab_t>(b);

  // Use double internally for precision
  const double L1 = la.template get_index<0>();
  const double a1 = la.template get_index<1>();
  const double b1 = la.template get_index<2>();
  const double L2 = lb.template get_index<0>();
  const double a2 = lb.template get_index<1>();
  const double b2 = lb.template get_index<2>();

  constexpr double kPi = 3.14159265358979323846;

  // Step 1: Compute differences
  const double dL = L2 - L1;
  const double da = a2 - a1;
  const double db = b2 - b1;

  // Step 2: Compute chroma
  const double C1 = std::sqrt(a1 * a1 + b1 * b1);
  const double C2 = std::sqrt(a2 * a2 + b2 * b2);
  const double dC = C2 - C1;

  // Step 3: Compute ΔH²
  const double dH_sq = da * da + db * db - dC * dC;
  const double dH = (dH_sq > 0.0) ? std::sqrt(dH_sq) : 0.0;

  // Step 4: Compute hue angle (in degrees, 0-360)
  double h1 = 0.0;
  if (b1 != 0.0 || a1 != 0.0) {
    h1 = std::atan2(b1, a1) * 180.0 / kPi;
    if (h1 < 0.0) h1 += 360.0;
  }

  // Step 5: Compute SL (lightness weighting function)
  double SL;
  if (L1 < 16.0) {
    SL = 0.511;
  } else {
    SL = 0.040975 * L1 / (1.0 + 0.01765 * L1);
  }

  // Step 6: Compute SC (chroma weighting function)
  const double SC = 0.0638 * C1 / (1.0 + 0.0131 * C1) + 0.638;

  // Step 7: Compute F factor
  const double C1_pow4 = C1 * C1 * C1 * C1;
  const double F = std::sqrt(C1_pow4 / (C1_pow4 + 1900.0));

  // Step 8: Compute T (hue-dependent factor)
  double T;
  if (h1 >= 164.0 && h1 <= 345.0) {
    T = 0.56 + std::abs(0.2 * std::cos((h1 + 168.0) * kPi / 180.0));
  } else {
    T = 0.36 + std::abs(0.4 * std::cos((h1 + 35.0) * kPi / 180.0));
  }

  // Step 9: Compute SH (hue weighting function)
  const double SH = SC * (F * T + 1.0 - F);

  // Step 10: Final CMC formula
  const double term_L = dL / (static_cast<double>(l) * SL);
  const double term_C = dC / (static_cast<double>(c) * SC);
  const double term_H = dH / SH;

  return static_cast<float>(std::sqrt(term_L * term_L + term_C * term_C + term_H * term_H));
}

}  // namespace colorcpp::algorithms::delta_e
