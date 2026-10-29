/**
 * @file compare.hpp
 * @brief Color comparison module with policy-based strategies (aggregate header).
 *
 * Umbrella over every comparison policy plus the equal() dispatcher. This
 * header is intentionally heavy (perceptual comparison pulls the conversion
 * and delta-E graph); translation units that only need exact/epsilon/relative
 * comparison should include the individual policy headers instead, which do
 * not drag in that graph.
 *
 * Policies:
 * - exact: Exact channel-by-channel equality (hue ring, achromatic gate)
 * - epsilon: Absolute per-channel tolerance (hue ring, achromatic gate)
 * - relative: Relative tolerance with absolute floor (isclose-style)
 * - perceptual: delta-E with selectable metric (CIEDE2000 default, OkLab optional)
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
 * // Perceptual comparison (metric selectable; alpha participation explicit)
 * bool similar = compare::equal<compare::perceptual_policy>(color1, color2, 1.0f);
 * @endcode
 */

#pragma once

#include <colorcpp/operations/compare/epsilon.hpp>
#include <colorcpp/operations/compare/equal.hpp>
#include <colorcpp/operations/compare/perceptual.hpp>
#include <colorcpp/operations/compare/relative.hpp>

namespace colorcpp::operations {

/** @brief Color comparison functions and policies. */
namespace compare {
// All functionality is provided by the included sub-modules
}

}  // namespace colorcpp::operations
