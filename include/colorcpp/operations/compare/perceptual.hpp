/**
 * @file perceptual.hpp
 * @brief Perceptual color comparison policy using ΔE.
 */

#pragma once

#include <colorcpp/algorithms/delta_e.hpp>

namespace colorcpp::operations::compare {

/**
 * @brief Perceptual comparison policy.
 *
 * Compares colors using CIELAB-based ΔE2000 color difference.
 * This is not constexpr due to the complexity of ΔE calculations.
 */
struct perceptual_policy {
  /**
   * @brief Compare two colors perceptually using ΔE2000.
   * @tparam ColorA First color type.
   * @tparam ColorB Second color type.
   * @param a First color.
   * @param b Second color.
   * @param threshold ΔE2000 threshold (default: 1.0).
   * @return True if ΔE2000 is below threshold.
   */
  template <typename ColorA, typename ColorB>
  static bool compare(const ColorA& a, const ColorB& b, float threshold = 1.0f) {
    return algorithms::delta_e::delta_e_2000(a, b) < threshold;
  }
};

/**
 * @brief Compare two colors perceptually using ΔE2000.
 * @tparam ColorA First color type.
 * @tparam ColorB Second color type.
 * @param a First color.
 * @param b Second color.
 * @param threshold ΔE2000 threshold.
 * @return True if ΔE2000 is below threshold.
 */
template <typename ColorA, typename ColorB>
bool perceptual_equal_de2000(const ColorA& a, const ColorB& b, float threshold = 1.0f) {
  return perceptual_policy::compare(a, b, threshold);
}

/**
 * @brief Compare two colors perceptually.
 * @tparam ColorA First color type.
 * @tparam ColorB Second color type.
 * @param a First color.
 * @param b Second color.
 * @param threshold ΔE2000 threshold.
 * @return True if ΔE2000 is below threshold.
 */
template <typename ColorA, typename ColorB>
bool perceptual_equal(const ColorA& a, const ColorB& b, float threshold = 1.0f) {
  return perceptual_equal_de2000(a, b, threshold);
}

}  // namespace colorcpp::operations::compare
