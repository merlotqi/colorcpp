/**
 * @file palette.hpp
 * @brief Palettes: ramps between two colors and classical harmony generators (complementary, triadic, etc.).
 *
 * This header includes all palette sub-modules:
 * - palette_set.hpp: Ordered list of colors with wrap indexing
 * - details.hpp: Internal helper functions
 * - scales.hpp: Gradient scale generators (linear, visual, perceptual)
 * - harmony.hpp: Color harmony generators (complementary, analogous, triadic, etc.)
 * - rainbow.hpp: Rainbow palette generator
 * - temperature.hpp: Warm and cool color palettes
 * - material.hpp: Material Design 3 color system
 */

#pragma once

#include <colorcpp/operations/palette/details.hpp>
#include <colorcpp/operations/palette/harmony.hpp>
#include <colorcpp/operations/palette/material.hpp>
#include <colorcpp/operations/palette/palette_set.hpp>
#include <colorcpp/operations/palette/rainbow.hpp>
#include <colorcpp/operations/palette/scales.hpp>
#include <colorcpp/operations/palette/temperature.hpp>

/** @brief Named palettes and gradient scales built on @ref interpolate. */
namespace colorcpp::operations::palette {
// All functionality is provided by the included sub-modules
}
