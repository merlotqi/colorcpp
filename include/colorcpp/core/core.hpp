/**
 * @file core.hpp
 * @brief Aggregates all core color space definitions and I/O helpers.
 *
 * Includes RGB/HSV/HSL/CMYK, linear RGB, CIELAB/CIELCH, OkLab/OkLCH, XYZ, shared
 * @ref colorcpp::core::basic_color machinery, constants, and string parsing/formatting.
 */

#pragma once

#include <colorcpp/core/cielab.hpp>
#include <colorcpp/core/cmyk.hpp>
#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/constants.hpp>
#include <colorcpp/core/css_color.hpp>
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/hsv.hpp>
#include <colorcpp/core/io.hpp>
#include <colorcpp/core/linear_rgb.hpp>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/core/xyz.hpp>

/** @brief Re-exports @c colorcpp::core into @c colorcpp when this header is included from the umbrella. */
namespace colorcpp {
using namespace core;
}
