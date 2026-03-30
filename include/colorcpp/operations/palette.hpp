/**
 * @file palette.hpp
 * @brief Palettes: ramps between two colors and classical harmony generators (complementary, triadic, etc.).
 *
 * This header includes all palette sub-modules:
 * - palette_set.hpp: Ordered list of colors with wrap indexing
 * - details.hpp: Internal helper functions
 * - scales.hpp: Gradient scale generators (linear, visual, perceptual)
 * - advanced_scales.hpp: Advanced gradient generators with easing functions, spline curves, and multi-color transitions
 * - generators.hpp: Color harmony generators (complementary, analogous, triadic, etc.)
 * - rainbow.hpp: Rainbow palette generator
 * - temperature.hpp: Warm and cool color palettes
 * - material.hpp: Material Design 3 color system
 */

#pragma once

#include <colorcpp/operations/palette/advanced_scales.hpp>
#include <colorcpp/operations/palette/details.hpp>
#include <colorcpp/operations/palette/generators.hpp>
#include <colorcpp/operations/palette/material.hpp>
#include <colorcpp/operations/palette/palette_set.hpp>
#include <colorcpp/operations/palette/rainbow.hpp>
#include <colorcpp/operations/palette/scales.hpp>
#include <colorcpp/operations/palette/temperature.hpp>

/** @brief Named palettes and gradient scales built on @ref interpolate. */
namespace colorcpp::operations::palette {
// All functionality is provided by the included sub-modules
}
