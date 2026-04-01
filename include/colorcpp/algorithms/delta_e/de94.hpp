/**
 * @file de94.hpp
 * @brief CIE 1994 ΔE color difference metric.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::delta_e {

/**
 * @brief CIE 1994 ΔE; asymmetric in @p a / @p b (reference = @p a).
 * @param a Reference color.
 * @param b Comparison color.
 * @param kL Lightness weight (default: 1.0).
 * @param kC Chroma weight (default: 1.0).
 * @param kH Hue weight (default: 1.0).
 * @param K1 Graphic arts: 0.045; textiles: 0.048.
 * @param K2 Graphic arts: 0.015; textiles: 0.014.
 * @return ΔE94 value.
 */
template <typename ColorA, typename ColorB>
float delta_e_94(const ColorA& a, const ColorB& b, float kL = 1.0f, float kC = 1.0f, float kH = 1.0f, float K1 = 0.045f,
                 float K2 = 0.015f) {
  using namespace operations::conversion;

  auto la = color_cast<core::cielab_t>(a);
  auto lb = color_cast<core::cielab_t>(b);

  const float L1 = la.template get_index<0>();
  const float a1 = la.template get_index<1>();
  const float b1 = la.template get_index<2>();
  const float L2 = lb.template get_index<0>();
  const float a2 = lb.template get_index<1>();
  const float b2 = lb.template get_index<2>();

  const float dL = L1 - L2;
  const float C1 = std::sqrt(a1 * a1 + b1 * b1);
  const float C2 = std::sqrt(a2 * a2 + b2 * b2);
  const float dC = C1 - C2;

  // ΔH² = Δa² + Δb² − ΔC²  (avoids atan2; clamp guards against float rounding)
  const float dH_sq = std::max(0.0f, (a1 - a2) * (a1 - a2) + (b1 - b2) * (b1 - b2) - dC * dC);

  const float SL = 1.0f;
  const float SC = 1.0f + K1 * C1;
  const float SH = 1.0f + K2 * C1;

  const float tL = dL / (kL * SL);
  const float tC = dC / (kC * SC);
  const float tH = std::sqrt(dH_sq) / (kH * SH);
  return std::sqrt(tL * tL + tC * tC + tH * tH);
}

}  // namespace colorcpp::algorithms::delta_e
