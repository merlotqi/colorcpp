/**
 * @file helpers.hpp
 * @brief Helper functions for color difference metrics.
 *
 * Provides convenience functions for comparing colors using various ΔE metrics.
 * For Oklab distance helpers see @ref oklab.hpp (@c is_visually_same_ok).
 */

#pragma once

#include <colorcpp/algorithms/delta_e/cmc.hpp>
#include <colorcpp/algorithms/delta_e/de2000.hpp>
#include <colorcpp/algorithms/delta_e/din99.hpp>

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

/**
 * @brief True if @ref delta_e_cmc is below @p threshold.
 *
 * Typical CMC l:c thresholds (l=2, c=1):
 * - < 1.0: Acceptable match
 * - < 0.5: Very close match
 *
 * @param a First color.
 * @param b Second color.
 * @param threshold CMC threshold (default: 1.0).
 * @param l Lightness weight (default: 2.0).
 * @param c Chroma weight (default: 1.0).
 * @return true if colors are acceptable match by CMC metric.
 */
template <typename ColorA, typename ColorB>
bool is_visually_same_cmc(const ColorA& a, const ColorB& b, float threshold = 1.0f, float l = 2.0f, float c = 1.0f) {
  return delta_e_cmc(a, b, l, c) < threshold;
}

/**
 * @brief True if @ref delta_e_din99 is below @p threshold.
 *
 * Typical DIN99 thresholds:
 * - < 1.0: Very close match
 * - < 3.0: Acceptable for most applications
 *
 * @param a First color.
 * @param b Second color.
 * @param threshold DIN99 threshold (default: 1.0).
 * @param k_L Lightness scaling factor (default: 1.0).
 * @param k_E Overall scaling factor (default: 1.0).
 * @return true if colors are visually the same by DIN99 metric.
 */
template <typename ColorA, typename ColorB>
bool is_visually_same_din99(const ColorA& a, const ColorB& b, float threshold = 1.0f, float k_L = 1.0f, float k_E = 1.0f) {
  return delta_e_din99(a, b, k_L, k_E) < threshold;
}

}  // namespace colorcpp::algorithms::delta_e
