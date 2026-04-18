/**
 * @file palette.hpp
 * @brief Palettes: ramps, palette families, formal harmony scheme generation, and theme helpers.
 *
 * This header includes all palette sub-modules:
 * - palette_set.hpp: Ordered list of colors with wrap indexing
 * - scales.hpp: Gradient scale generators (linear, visual, perceptual)
 * - advanced_scales.hpp: Advanced gradient generators with easing functions, spline curves, and multi-color transitions
 * - schemes.hpp: Formal harmony scheme generators built on algorithms::harmony rules
 * - rainbow.hpp: Rainbow palette generator
 * - families.hpp: Warm, cool, and neutral palette families
 * - material.hpp: Material Design 3 color system
 * - theme.hpp: Semantic theme derivation from a seed color
 */

#pragma once

#include <colorcpp/operations/palette/advanced_scales.hpp>
#include <colorcpp/operations/palette/families.hpp>
#include <colorcpp/operations/palette/material.hpp>
#include <colorcpp/operations/palette/palette_set.hpp>
#include <colorcpp/operations/palette/rainbow.hpp>
#include <colorcpp/operations/palette/scales.hpp>
#include <colorcpp/operations/palette/schemes.hpp>
#include <colorcpp/operations/palette/theme.hpp>

/** @brief Named palettes and gradient scales built on @ref interpolate and harmony rules. */
namespace colorcpp::operations::palette {
// All functionality is provided by the included sub-modules
}
