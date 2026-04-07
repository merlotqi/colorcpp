/**
 * @file de76.hpp
 * @brief CIE 1976 ΔE*ab color difference metric (CIELAB D65).
 * @see oklab.hpp for Oklab Euclidean @c delta_e_ok (not comparable to ΔE*ab numerically).
 */

#pragma once

#include <cmath>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::delta_e {

/**
 * @brief CIE 1976 ΔE*ab — Euclidean distance in LAB (fast; JND ≈ 2.3).
 * @param a First color.
 * @param b Second color.
 * @return ΔE value.
 */
template <typename ColorA, typename ColorB>
float delta_e_76(const ColorA& a, const ColorB& b) {
  using namespace operations::conversion;

  auto la = color_cast<core::cielab_t>(a);
  auto lb = color_cast<core::cielab_t>(b);

  float dL = la.template get_index<0>() - lb.template get_index<0>();
  float da = la.template get_index<1>() - lb.template get_index<1>();
  float db = la.template get_index<2>() - lb.template get_index<2>();

  return std::sqrt(dL * dL + da * da + db * db);
}

}  // namespace colorcpp::algorithms::delta_e
