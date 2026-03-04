/**
 * @file core.hpp
 * @brief Core color space types and constants
 *
 * This header provides the main entry point for the core color functionality,
 * including all basic color space types and predefined color constants.
 * It serves as a convenience header that includes all core components.
 *
 * @author Merlot.Qi
 */

#pragma once

#include <color/core/cmyk.hpp>
#include <color/core/color_base.hpp>
#include <color/core/constants.hpp>
#include <color/core/hsl.hpp>
#include <color/core/hsv.hpp>
#include <color/core/io.hpp>
#include <color/core/rgb.hpp>

namespace color {

using namespace core;
using core::rgba8_t;
using core::rgba_float_t;

}  // namespace color
