/**
 * @file interpolate.hpp
 * @brief Interpolation and easing between colors in RGB, HSL, HSV, OkLab, OkLCH, CIELAB, and CIELCH spaces.
 *
 * This header includes all interpolation sub-modules:
 * - lerp.hpp: Basic linear interpolation
 * - catmull_rom.hpp: Catmull-Rom cubic interpolation
 * - cubic.hpp: Cubic Hermite interpolation
 * - gamut.hpp: Gamut boundary utilities
 * - hue.hpp: Hue-locked interpolation
 * - chroma.hpp: Chroma-preserving interpolation
 * - color_lerp.hpp: Multi-stop gradients, easing, and alpha control
 */

#pragma once

#include <colorcpp/operations/interpolate/catmull_rom.hpp>
#include <colorcpp/operations/interpolate/chroma.hpp>
#include <colorcpp/operations/interpolate/color_lerp.hpp>
#include <colorcpp/operations/interpolate/cubic.hpp>
#include <colorcpp/operations/interpolate/gamut.hpp>
#include <colorcpp/operations/interpolate/hue.hpp>
#include <colorcpp/operations/interpolate/lerp.hpp>

/** @brief Lerps, multi-stop gradients, and easing helpers built on @ref conversion::color_cast. */
namespace colorcpp::operations::interpolate {
// All functionality is provided by the included sub-modules
}
