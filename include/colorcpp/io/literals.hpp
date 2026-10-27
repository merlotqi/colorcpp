/**
 * @file literals.hpp
 * @brief Aggregate header for the supported color literal operators.
 *
 * Includes the RGB/RGBA/ARGB hex and the #RRGGBB string literal operators.
 * For non-RGB color spaces use the constructors, e.g.
 * `hsl_float_t{120.0f, 0.5f, 0.75f}` (see docs/reference/literals.rst).
 */

#pragma once

#include <colorcpp/io/literals/rgb.hpp>
