/**
 * @file de2000.hpp
 * @brief CIEDE2000 color difference metric (ISO 11664-6 / CIE 142-2001).
 */

#pragma once

#include <cmath>
#include <colorcpp/algorithms/delta_e/cie2000_intermediates.hpp>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::delta_e {

/**
 * @brief CIEDE2000 (ISO 11664-6 / CIE 142-2001).
 * @param a First color.
 * @param b Second color.
 * @param kL Lightness weight (default: 1.0). Non-positive values fall back to 1.0.
 * @param kC Chroma weight (default: 1.0). Non-positive values fall back to 1.0.
 * @param kH Hue weight (default: 1.0). Non-positive values fall back to 1.0.
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
  const auto m = details::cie2000_intermediates(la.template get_index<0>(), la.template get_index<1>(),
                                                la.template get_index<2>(), lb.template get_index<0>(),
                                                lb.template get_index<1>(), lb.template get_index<2>());

  // Step 7 — rotation term RT (blue-hue correction)
  const double RT = -std::sin(m.dTheta * (3.14159265358979323846 / 180.0) * 2.0) * m.RC;

  // Final — non-positive parametric weights fall back to 1.0 (din99 policy)
  const double safe_kL = kL > 0.0f ? static_cast<double>(kL) : 1.0;
  const double safe_kC = kC > 0.0f ? static_cast<double>(kC) : 1.0;
  const double safe_kH = kH > 0.0f ? static_cast<double>(kH) : 1.0;

  const double term_L = m.dLp / (safe_kL * m.SL);
  const double term_C = m.dCp / (safe_kC * m.SC);
  const double term_H = m.dHp / (safe_kH * m.SH);
  return static_cast<float>(std::sqrt(term_L * term_L + term_C * term_C + term_H * term_H + RT * term_C * term_H));
}

}  // namespace colorcpp::algorithms::delta_e
