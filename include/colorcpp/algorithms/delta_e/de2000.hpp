/**
 * @file de2000.hpp
 * @brief CIEDE2000 color difference metric (ISO 11664-6 / CIE 142-2001).
 */

#pragma once

#include <cmath>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::delta_e {

/**
 * @brief CIEDE2000 (ISO 11664-6 / CIE 142-2001).
 * @param a First color.
 * @param b Second color.
 * @param kL Lightness weight (default: 1.0).
 * @param kC Chroma weight (default: 1.0).
 * @param kH Hue weight (default: 1.0).
 * @return ΔE2000 value.
 * @note Rule-of-thumb: <1 imperceptible, 1–2 close inspection, 2–10 obvious, >50 very different.
 */
template <typename ColorA, typename ColorB>
float delta_e_2000(const ColorA& a, const ColorB& b, float kL = 1.0f, float kC = 1.0f, float kH = 1.0f) {
  using namespace operations::conversion;

  auto la = color_cast<core::cielab_t>(a);
  auto lb = color_cast<core::cielab_t>(b);

  // Use double internally: the formula involves pow7 of small chroma values and
  // atan2 near the achromatic axis, both of which accumulate float rounding error.
  const double L1 = la.template get_index<0>();
  const double a1s = la.template get_index<1>();
  const double b1s = la.template get_index<2>();
  const double L2 = lb.template get_index<0>();
  const double a2s = lb.template get_index<1>();
  const double b2s = lb.template get_index<2>();

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

  double dhp = 0.0;
  if (C1p * C2p != 0.0) {
    double diff = h2p - h1p;
    if (std::abs(diff) <= 180.0)
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
  } else if (std::abs(h1p - h2p) <= 180.0) {
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

  // Step 7 — rotation term RT (blue-hue correction)
  const double Cp_avg7 = pow7_d(Cp_avg);
  const double RC = 2.0 * std::sqrt(Cp_avg7 / (Cp_avg7 + k25_7d));
  const double d_theta = 30.0 * std::exp(-((hp_avg - 275.0) / 25.0) * ((hp_avg - 275.0) / 25.0));
  const double RT = -std::sin(deg2rad_d(2.0 * d_theta)) * RC;

  // Final
  const double term_L = dLp / (static_cast<double>(kL) * SL);
  const double term_C = dCp / (static_cast<double>(kC) * SC);
  const double term_H = dHp / (static_cast<double>(kH) * SH);
  return static_cast<float>(std::sqrt(term_L * term_L + term_C * term_C + term_H * term_H + RT * term_C * term_H));
}

}  // namespace colorcpp::algorithms::delta_e
