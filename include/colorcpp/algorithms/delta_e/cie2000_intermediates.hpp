/**
 * @file cie2000_intermediates.hpp
 * @brief Shared CIE 2000 intermediate attributes (internal).
 *
 * Steps 1–6 of ISO 11664-6: G-adjusted a', C', h', ΔL'/ΔC'/ΔH', the mean
 * L̄'/C̄'/h̄', the weighting functions SL/SC/SH, and the rotation ingredients
 * (RC, Δθ). Used by @ref de2000.hpp (adds RT and the Euclidean combination)
 * and @ref hych.hpp (city-block lightness + planar chroma/hue combination).
 */

#pragma once

#include <cmath>

namespace colorcpp::algorithms::delta_e::details {

/** @brief CIE 2000 intermediate attributes for one colour pair. */
struct Cie2000Intermediates {
  double dLp;     // L2 - L1
  double dCp;     // C'2 - C'1 (G-adjusted chroma)
  double dHp;     // 2*sqrt(C'1*C'2)*sin(Δh'/2)
  double LpAvg;   // (L1 + L2) / 2
  double CpAvg;   // (C'1 + C'2) / 2
  double HpAvg;   // mean hue angle (degrees, hue-continuity adjusted)
  double SL;      // lightness weighting
  double SC;      // chroma weighting
  double SH;      // hue weighting
  double RC;      // rotation magnitude (R_T = -sin(2*Δθ) * RC)
  double dTheta;  // rotation centre offset from 275° (degrees)
};

/**
 * @brief Compute the ISO 11664-6 intermediate attributes of a CIELAB pair.
 * @param L1,a1s,b1s First colour (CIELAB D65).
 * @param L2,a2s,b2s Second colour (CIELAB D65).
 */
inline Cie2000Intermediates cie2000_intermediates(double L1, double a1s, double b1s, double L2, double a2s,
                                                  double b2s) {
  constexpr double kPi = 3.14159265358979323846;
  auto deg2rad_d = [&](double d) { return d * kPi / 180.0; };
  auto rad2deg_d = [&](double r) { return r * 180.0 / kPi; };
  auto pow7_d = [](double x) {
    double x2 = x * x, x4 = x2 * x2;
    return x4 * x2 * x;
  };
  constexpr double k25_7d = 6103515625.0;  // 25^7

  // Step 1 — compute adjusted a' (chroma weighting G factor)
  const double C1ab = std::sqrt(a1s * a1s + b1s * b1s);
  const double C2ab = std::sqrt(a2s * a2s + b2s * b2s);
  const double Cab_avg = (C1ab + C2ab) * 0.5;
  const double Cab_avg7 = pow7_d(Cab_avg);
  const double G = 0.5 * (1.0 - std::sqrt(Cab_avg7 / (Cab_avg7 + k25_7d)));

  const double a1p = a1s * (1.0 + G);
  const double a2p = a2s * (1.0 + G);

  // Step 2 — C' and h' (hue angle in [0, 360))
  const double C1p = std::sqrt(a1p * a1p + b1s * b1s);
  const double C2p = std::sqrt(a2p * a2p + b2s * b2s);

  auto hprime = [&](double bp, double ap) -> double {
    if (bp == 0.0 && ap == 0.0) return 0.0;
    double h = rad2deg_d(std::atan2(bp, ap));
    return (h < 0.0) ? h + 360.0 : h;
  };

  const double h1p = hprime(b1s, a1p);
  const double h2p = hprime(b2s, a2p);

  // Step 3 — ΔL', ΔC', Δh', ΔH'
  const double dLp = L2 - L1;
  const double dCp = C2p - C1p;

  // |Δh'| == 180 is a genuine tie (Sharma: keep the unwrapped difference), but
  // atan2 rounding plus the ±360 normalization of h' leaves the difference a few
  // ulps past 180 for antipodal hues, which would silently pick the wrapped
  // branch. Tolerate that noise; real near-tie samples in the published dataset
  // sit ~1e-3 degrees away from the boundary.
  constexpr double kHueTieTol = 1e-9;

  double dhp = 0.0;
  if (C1p * C2p != 0.0) {
    double diff = h2p - h1p;
    if (std::abs(diff) <= 180.0 + kHueTieTol)
      dhp = diff;
    else if (diff > 180.0)
      dhp = diff - 360.0;
    else
      dhp = diff + 360.0;
  }

  const double dHp = 2.0 * std::sqrt(C1p * C2p) * std::sin(deg2rad_d(dhp * 0.5));

  // Step 4 — arithmetic mean L̄', C̄', h̄'
  const double Lp_avg = (L1 + L2) * 0.5;
  const double Cp_avg = (C1p + C2p) * 0.5;

  double hp_avg;
  if (C1p * C2p == 0.0) {
    hp_avg = h1p + h2p;
  } else if (std::abs(h1p - h2p) <= 180.0 + kHueTieTol) {
    hp_avg = (h1p + h2p) * 0.5;
  } else if (h1p + h2p < 360.0) {
    hp_avg = (h1p + h2p + 360.0) * 0.5;
  } else {
    hp_avg = (h1p + h2p - 360.0) * 0.5;
  }

  // Step 5 — T (hue weighting for SH)
  const double T = 1.0 - 0.17 * std::cos(deg2rad_d(hp_avg - 30.0)) + 0.24 * std::cos(deg2rad_d(2.0 * hp_avg)) +
                   0.32 * std::cos(deg2rad_d(3.0 * hp_avg + 6.0)) - 0.20 * std::cos(deg2rad_d(4.0 * hp_avg - 63.0));

  // Step 6 — weighting functions SL, SC, SH
  const double Lm50sq = (Lp_avg - 50.0) * (Lp_avg - 50.0);
  const double SL = 1.0 + 0.015 * Lm50sq / std::sqrt(20.0 + Lm50sq);
  const double SC = 1.0 + 0.045 * Cp_avg;
  const double SH = 1.0 + 0.015 * Cp_avg * T;

  // Rotation ingredients (Step 7 needs only these two)
  const double Cp_avg7 = pow7_d(Cp_avg);
  const double RC = 2.0 * std::sqrt(Cp_avg7 / (Cp_avg7 + k25_7d));
  const double d_theta = 30.0 * std::exp(-((hp_avg - 275.0) / 25.0) * ((hp_avg - 275.0) / 25.0));

  return Cie2000Intermediates{dLp, dCp, dHp, Lp_avg, Cp_avg, hp_avg, SL, SC, SH, RC, d_theta};
}

}  // namespace colorcpp::algorithms::delta_e::details
