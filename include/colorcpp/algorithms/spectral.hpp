/**
 * @file spectral.hpp
 * @brief Spectral colourimetry: sampled SPDs, CIE observers and integration.
 *
 * Owns the CIE standard-observer CMF tables (1931 2° and 1964 10°), the
 * standard illuminant datasets (A, the D series with its S0/S1/S2 daylight
 * synthesizer, F1-F12 and typical LEDs) and the shared trapezoidal
 * integrator used by Planck's law, emissive SPDs and reflectance products.
 *
 * @see CIE 15:2004
 */

#pragma once

#include <colorcpp/algorithms/spectral/cmf.hpp>
#include <colorcpp/algorithms/spectral/fluorescent.hpp>
#include <colorcpp/algorithms/spectral/illuminants.hpp>
#include <colorcpp/algorithms/spectral/integrate.hpp>
#include <colorcpp/algorithms/spectral/led.hpp>
#include <colorcpp/algorithms/spectral/reflectance.hpp>
#include <colorcpp/algorithms/spectral/spd.hpp>
