/**
 * @file palette.hpp
 * @brief Palettes: ramps, palette families, formal harmony scheme generation, and theme helpers.
 *
 * This header includes all palette sub-modules:
 * - core/palette_set.hpp: Ordered list of colors with wrap indexing
 * - algorithms/palette/scales.hpp: Gradient scale generators (linear, visual, perceptual)
 * - algorithms/palette/advanced_scales.hpp: Advanced gradient generators with easing functions, spline curves, and multi-color transitions
 * - operations/palette/schemes.hpp: Formal harmony scheme generators built on algorithms::harmony rules
 * - algorithms/palette/rainbow.hpp: Rainbow palette generator
 * - algorithms/palette/families.hpp: Warm, cool, and neutral palette families
 * - operations/palette/material.hpp: Material Design 3 color system
 * - operations/palette/theme.hpp: Semantic theme derivation from a seed color
 */

#pragma once

#include <colorcpp/core/palette_set.hpp>
#include <colorcpp/algorithms/palette/scales.hpp>
#include <colorcpp/algorithms/palette/advanced_scales.hpp>
#include <colorcpp/algorithms/palette/rainbow.hpp>
#include <colorcpp/algorithms/palette/families.hpp>
#include <colorcpp/operations/palette/schemes.hpp>
#include <colorcpp/operations/palette/material.hpp>
#include <colorcpp/operations/palette/theme.hpp>

/** @brief Named palettes and gradient scales built on @ref interpolate and harmony rules. */
namespace colorcpp::operations::palette {
// All functionality is provided by the included sub-modules.
// palette_set is available at colorcpp::core::palette_set<Color>.
}
