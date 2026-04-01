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
 * Compares colors using perceptual color difference (ΔE).
 * This is not constexpr due to the complexity of ΔE calculations.
 */
struct perceptual_policy {
  /**
   * @brief Compare two colors perceptually.
   * @tparam Color Color type.
   * @param a First color.
   * @param b Second color.
   * @param threshold ΔE threshold (default: 1.0).
   * @return True if ΔE is below threshold.
   */
  template <typename Color>
  static bool compare(const Color& a, const Color& b, float threshold = 1.0f) {
    return algorithms::delta_e::delta_e_2000(a, b) < threshold;
  }
};

/**
 * @brief Compare two colors perceptually.
 * @tparam Color Color type.
 * @param a First color.
 * @param b Second color.
 * @param threshold ΔE threshold.
 * @return True if ΔE is below threshold.
 */
template <typename Color>
bool perceptual_equal(const Color& a, const Color& b, float threshold = 1.0f) {
  return perceptual_policy::compare(a, b, threshold);
}

}  // namespace colorcpp::operations::compare
