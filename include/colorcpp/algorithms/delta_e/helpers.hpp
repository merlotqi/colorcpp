/**
 * @file helpers.hpp
 * @brief Helpers for **ΔE2000** (CIELAB). For Oklab distance helpers see @ref oklab.hpp (@c is_visually_same_ok).
 */

#pragma once

#include <colorcpp/algorithms/delta_e/de2000.hpp>

namespace colorcpp::algorithms::delta_e {

/**
 * @brief True if @ref delta_e_2000 is below @p threshold (default 1.0 ≈ imperceptible).
 * @param a First color.
 * @param b Second color.
 * @param threshold ΔE2000 threshold (default: 1.0).
 * @return true if colors are visually the same.
 */
template <typename ColorA, typename ColorB>
bool is_visually_same(const ColorA& a, const ColorB& b, float threshold = 1.0f) {
  return delta_e_2000(a, b) < threshold;
}

}  // namespace colorcpp::algorithms::delta_e
