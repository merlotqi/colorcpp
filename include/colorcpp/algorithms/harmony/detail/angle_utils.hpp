/**
 * @file angle_utils.hpp
 * @brief Angle mathematics utilities for harmony calculations.
 */

#pragma once

#include <cmath>

namespace colorcpp::algorithms::harmony::detail {

/// Golden angle constant (≈ 137.50776405°)
constexpr float k_golden_angle = 137.50776405f;

/// Default angle tolerance for comparisons (degrees)
constexpr float k_default_tolerance = 15.0f;

/// Small angle tolerance for "near" comparisons (degrees)
constexpr float k_small_tolerance = 5.0f;

/// Base deviation penalty for unrecognized harmony schemes (used in assess scoring)
constexpr float k_base_deviation_penalty = 30.0f;

/**
 * @brief Normalize angle to [0, 360) range.
 * @param angle Input angle in degrees.
 * @return Normalized angle in [0, 360).
 */
constexpr float normalize_angle(float angle) {
  angle = std::fmod(angle, 360.0f);
  if (angle < 0.0f) angle += 360.0f;
  return angle;
}

/**
 * @brief Calculate shortest angular distance between two angles.
 * @param from Start angle in degrees.
 * @param to End angle in degrees.
 * @return Absolute shortest distance in [0, 180].
 */
constexpr float angle_diff(float from, float to) {
  float diff = std::abs(normalize_angle(to) - normalize_angle(from));
  return diff > 180.0f ? 360.0f - diff : diff;
}

/**
 * @brief Calculate signed angular distance (positive = counterclockwise).
 * @param from Start angle in degrees.
 * @param to End angle in degrees.
 * @return Signed distance in [-180, 180].
 */
constexpr float angle_diff_signed(float from, float to) {
  float diff = normalize_angle(to) - normalize_angle(from);
  if (diff > 180.0f) diff -= 360.0f;
  if (diff < -180.0f) diff += 360.0f;
  return diff;
}

/**
 * @brief Check if two angles are approximately equal within tolerance.
 * @param a First angle in degrees.
 * @param b Second angle in degrees.
 * @param tolerance Maximum allowed deviation in degrees.
 * @return True if angles are within tolerance.
 */
constexpr bool angle_near(float a, float b, float tolerance = k_small_tolerance) {
  return angle_diff(a, b) <= tolerance;
}

/**
 * @brief Check if angle difference matches a target within tolerance.
 * @param diff Measured angle difference.
 * @param target Target angle difference.
 * @param tolerance Maximum allowed deviation in degrees.
 * @return True if difference matches target within tolerance.
 */
constexpr bool diff_near(float diff, float target, float tolerance = k_default_tolerance) {
  return std::abs(diff - target) <= tolerance;
}

}  // namespace colorcpp::algorithms::harmony::detail
