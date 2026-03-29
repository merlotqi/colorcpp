/**
 * @file compare.hpp
 * @brief Color comparison module with policy-based strategies.
 *
 * Provides various color comparison strategies:
 * - exact: Exact channel-by-channel equality
 * - epsilon: Comparison within fixed epsilon tolerance
 * - relative: Comparison within relative tolerance
 * - perceptual: Comparison using ΔE2000 perceptual distance
 *
 * Usage:
 * @code
 * #include <colorcpp/operations/compare.hpp>
 *
 * // Default exact comparison
 * bool same = compare::equal(color1, color2);
 *
 * // Epsilon comparison
 * bool close = compare::equal<compare::epsilon_policy>(color1, color2, 0.01f);
 *
 * // Perceptual comparison
 * bool similar = compare::equal<compare::perceptual_policy>(color1, color2, 1.0f);
 * @endcode
 */

#pragma once

#include <colorcpp/operations/compare/equal.hpp>

namespace colorcpp::operations {

/** @brief Color comparison functions and policies. */
namespace compare {
// All functionality is provided by the included sub-modules
}

}  // namespace colorcpp::operations
