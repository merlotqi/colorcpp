/**
 * @file blend.hpp
 * @brief Porter-Duff-style compositing and W3C-style blend modes between two colors (converted via @ref
 * conversion::color_cast).
 *
 * This header includes all blend sub-modules:
 * - mode.hpp: Blend mode enumeration
 * - algorithms.hpp: Separable RGB blend algorithms (per-channel)
 * - non_separable.hpp: Non-separable blend modes (hue, saturation, color, luminosity)
 * - details.hpp: Porter-Duff compositing helper
 * - blend.hpp: Main blend function implementation
 */

#pragma once

#include <colorcpp/operations/blend/algorithms.hpp>
#include <colorcpp/operations/blend/blend.hpp>
#include <colorcpp/operations/blend/details.hpp>
#include <colorcpp/operations/blend/mode.hpp>
#include <colorcpp/operations/blend/non_separable.hpp>

/** @brief Alpha-aware blending with selectable blend mode in `blend(dst, src, mode)` order; current implementation uses
 * an encoded `rgbaf_t` working space rather than a verified linear-sRGB compositing path. */
namespace colorcpp::operations::blend {
// All functionality is provided by the included sub-modules
}
