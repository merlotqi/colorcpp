/**
 * @file spectral.hpp
 * @brief Spectral colourimetry: sampled SPDs and CMF integration.
 *
 * Owns the CIE standard-observer colour matching function tables (the
 * 1931 2° table moved here from color_temperature/planckian.hpp) and the
 * generic trapezoidal integrator that Planck's law, standard illuminants
 * and reflectance products all share. Upcoming: the 1964 10° observer,
 * standard illuminant datasets (A / D series with the daylight locus /
 * F series / LED) and reflectance integration.
 *
 * @see CIE 15:2004
 */

#pragma once

#include <colorcpp/algorithms/spectral/cmf.hpp>
#include <colorcpp/algorithms/spectral/integrate.hpp>
#include <colorcpp/algorithms/spectral/spd.hpp>
