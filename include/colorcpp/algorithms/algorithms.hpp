/**
 * @file algorithms.hpp
 * @brief Aggregates color algorithms: accessibility, color temperature, delta E, gamut mapping, gradient, harmony,
 * vision simulation.
 *
 * @par Color difference
 * - @ref delta_e::delta_e_76 / @ref delta_e::delta_e_94 / @ref delta_e::delta_e_2000: **CIELAB D65** metrics.
 * - @ref delta_e::delta_e_ok: **Oklab** Euclidean distance; threshold @ref delta_e::oklab_jnd_typical matches the
 *   ~0.02 scale used in @ref gamut::gamut_clip_perceptual (not comparable numerically to ΔE2000).
 */

#pragma once

#include <colorcpp/algorithms/accessibility.hpp>
#include <colorcpp/algorithms/color_temperature.hpp>
#include <colorcpp/algorithms/delta_e.hpp>
#include <colorcpp/algorithms/gamut.hpp>
#include <colorcpp/algorithms/gradient.hpp>
#include <colorcpp/algorithms/harmony.hpp>
#include <colorcpp/algorithms/vision.hpp>

/**
 * @namespace colorcpp::algorithms
 * @brief Color algorithms: @c accessibility, @c color_temperature, @c delta_e, @c gamut, @c gradient, @c harmony, @c
 * vision (each is a nested namespace).
 */

/** @brief Convenience re-export into @c colorcpp; canonical docs use @c colorcpp::algorithms::* explicitly. */
namespace colorcpp {

using namespace algorithms;

}  // namespace colorcpp
