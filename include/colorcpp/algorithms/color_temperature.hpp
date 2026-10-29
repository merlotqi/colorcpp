/**
 * @file color_temperature.hpp
 * @brief Colour temperature utilities (aggregate header).
 *
 * Two families are provided:
 * - Tanner Helland's kelvin <-> sRGB approximation (legacy, no chromaticity
 *   accuracy claims) for warm/cool UI sliders — @ref color_temperature/kelvin_rgb.hpp.
 * - Colour-temperature metrology: Planckian locus, Ohno (2013), McCamy
 *   (1992) and Robertson (1968) CCT / Duv estimation — the remaining
 *   color_temperature/ subdirectory headers, included below.
 *
 * Individual subdirectory headers can be included directly for faster
 * compilation.
 */

#pragma once

#include <colorcpp/algorithms/color_temperature/common.hpp>
#include <colorcpp/algorithms/color_temperature/kelvin_rgb.hpp>
#include <colorcpp/algorithms/color_temperature/mccamy.hpp>
#include <colorcpp/algorithms/color_temperature/ohno.hpp>
#include <colorcpp/algorithms/color_temperature/planckian.hpp>
#include <colorcpp/algorithms/color_temperature/robertson.hpp>
