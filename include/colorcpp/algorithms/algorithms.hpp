/**
 * @file algorithms.hpp
 * @brief Aggregates color algorithms: accessibility, color temperature, delta E, gamut mapping, gradient, harmony,
 * vision simulation.
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

/** @brief Re-exports nested @c colorcpp::algorithms::* into @c colorcpp when using the umbrella header. */
namespace colorcpp {

using namespace algorithms;

}  // namespace colorcpp
