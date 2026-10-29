/**
 * @file hych.hpp
 * @brief Hybrid chroma-hue colour difference ΔE_HyCH in CIELAB D65.
 *
 * ΔE_HyCH = |ΔL'/(kL·SL)| + √((ΔC'/(kC·SC))² + (ΔH'/(kH·SH))²) — the HyAB
 * partner metric (Abasi et al. 2020) that splits the planar term into the
 * CIE 2000 chroma/hue weightings instead of raw Euclidean a*b*. Built on the
 * same intermediates as CIEDE2000 (G adjustment, SL/SC/SH, ΔH' arc form);
 * intended for large differences (~10 CIELAB units and up). The textile
 * parametrization is the kL = 2 preset.
 *
 * @see Abasi, S., Akyüz, M. S., & Lenz, R. (2020). Accuracy of Color
 *      Difference Formulae in Complex Scenes. Journal of Imaging.
 */

#pragma once

#include <cmath>
#include <colorcpp/algorithms/delta_e/cie2000_intermediates.hpp>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::delta_e {

/**
 * @brief Hybrid chroma-hue colour difference |ΔL'/SL| + √((ΔC'/SC)² + (ΔH'/SH)²).
 * @tparam ColorA, ColorB Any types supported by @ref
 *         operations::conversion::color_cast to @c cielab_t.
 * @param kL Lightness weight (default: 1.0; textiles use 2.0). Non-positive
 *        values fall back to 1.0.
 * @param kC Chroma weight (default: 1.0). Non-positive values fall back to 1.0.
 * @param kH Hue weight (default: 1.0). Non-positive values fall back to 1.0.
 */
template <typename ColorA, typename ColorB>
float delta_e_hych(const ColorA& a, const ColorB& b, float kL = 1.0f, float kC = 1.0f, float kH = 1.0f) {
  using namespace operations::conversion;

  auto la = color_cast<core::cielab_t>(a);
  auto lb = color_cast<core::cielab_t>(b);

  // Double intermediates via the shared CIE 2000 machinery (same rationale as
  // delta_e_2000).
  const auto m = details::cie2000_intermediates(la.template get_index<0>(), la.template get_index<1>(),
                                                la.template get_index<2>(), lb.template get_index<0>(),
                                                lb.template get_index<1>(), lb.template get_index<2>());

  // Non-positive parametric weights fall back to 1.0 (din99 policy).
  const double safe_kL = kL > 0.0f ? static_cast<double>(kL) : 1.0;
  const double safe_kC = kC > 0.0f ? static_cast<double>(kC) : 1.0;
  const double safe_kH = kH > 0.0f ? static_cast<double>(kH) : 1.0;

  const double term_L = m.dLp / (safe_kL * m.SL);
  const double term_C = m.dCp / (safe_kC * m.SC);
  const double term_H = m.dHp / (safe_kH * m.SH);
  return static_cast<float>(std::abs(term_L) + std::hypot(term_C, term_H));
}

}  // namespace colorcpp::algorithms::delta_e
