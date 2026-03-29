/**
 * @file helpers.hpp
 * @brief Helper functions for color vision deficiency accessibility.
 */

#pragma once

#include <colorcpp/algorithms/delta_e.hpp>
#include <colorcpp/algorithms/vision/achromatopsia.hpp>
#include <colorcpp/algorithms/vision/deuteranopia.hpp>
#include <colorcpp/algorithms/vision/protanopia.hpp>
#include <colorcpp/algorithms/vision/tritanopia.hpp>

namespace colorcpp::algorithms::vision {

/**
 * @brief Check if two colors are distinguishable for protanopia (red-blind) users.
 * @param a First color.
 * @param b Second color.
 * @param threshold ΔE2000 threshold (default: 2.3, roughly JND).
 * @return true if colors are distinguishable for protanopia users.
 */
template <typename ColorA, typename ColorB>
bool is_accessible_for_protanopia(const ColorA& a, const ColorB& b, float threshold = 2.3f) {
  auto sim_a = simulate_protanopia(a);
  auto sim_b = simulate_protanopia(b);
  return delta_e::delta_e_2000(sim_a, sim_b) >= threshold;
}

/**
 * @brief Check if two colors are distinguishable for deuteranopia (green-blind) users.
 * @param a First color.
 * @param b Second color.
 * @param threshold ΔE2000 threshold (default: 2.3, roughly JND).
 * @return true if colors are distinguishable for deuteranopia users.
 */
template <typename ColorA, typename ColorB>
bool is_accessible_for_deuteranopia(const ColorA& a, const ColorB& b, float threshold = 2.3f) {
  auto sim_a = simulate_deuteranopia(a);
  auto sim_b = simulate_deuteranopia(b);
  return delta_e::delta_e_2000(sim_a, sim_b) >= threshold;
}

/**
 * @brief Check if two colors are distinguishable for tritanopia (blue-blind) users.
 * @param a First color.
 * @param b Second color.
 * @param threshold ΔE2000 threshold (default: 2.3, roughly JND).
 * @return true if colors are distinguishable for tritanopia users.
 */
template <typename ColorA, typename ColorB>
bool is_accessible_for_tritanopia(const ColorA& a, const ColorB& b, float threshold = 2.3f) {
  auto sim_a = simulate_tritanopia(a);
  auto sim_b = simulate_tritanopia(b);
  return delta_e::delta_e_2000(sim_a, sim_b) >= threshold;
}

/**
 * @brief Check if two colors are distinguishable for achromatopsia (total color blindness) users.
 * @param a First color.
 * @param b Second color.
 * @param threshold ΔE2000 threshold (default: 2.3, roughly JND).
 * @return true if colors are distinguishable for achromatopsia users.
 */
template <typename ColorA, typename ColorB>
bool is_accessible_for_achromatopsia(const ColorA& a, const ColorB& b, float threshold = 2.3f) {
  auto sim_a = simulate_achromatopsia(a);
  auto sim_b = simulate_achromatopsia(b);
  return delta_e::delta_e_2000(sim_a, sim_b) >= threshold;
}

/**
 * @brief Check if two colors are accessible for all common CVD types.
 * @param a First color.
 * @param b Second color.
 * @param threshold ΔE2000 threshold (default: 2.3, roughly JND).
 * @return true if colors are distinguishable for all CVD types.
 */
template <typename ColorA, typename ColorB>
bool is_accessible_for_all_cvd(const ColorA& a, const ColorB& b, float threshold = 2.3f) {
  return is_accessible_for_protanopia(a, b, threshold) && is_accessible_for_deuteranopia(a, b, threshold) &&
         is_accessible_for_tritanopia(a, b, threshold) && is_accessible_for_achromatopsia(a, b, threshold);
}

}  // namespace colorcpp::algorithms::vision
