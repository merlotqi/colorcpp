/**
 * @file vision.hpp
 * @brief Color vision deficiency simulation (Brettel / Viénot / Mollon) in linear sRGB / LMS space.
 *
 * This header includes all vision sub-modules:
 * - details.hpp: Internal helper functions (linearize, gamma_encode, LMS matrices)
 * - protanopia.hpp: L-cone absence simulation
 * - deuteranopia.hpp: M-cone absence simulation
 * - tritanopia.hpp: S-cone absence simulation (Brettel two-plane model)
 * - achromatopsia.hpp: Total color blindness simulation (grayscale)
 * - helpers.hpp: Accessibility check functions for CVD
 */

#pragma once

#include <colorcpp/algorithms/vision/achromatopsia.hpp>
#include <colorcpp/algorithms/vision/details.hpp>
#include <colorcpp/algorithms/vision/deuteranopia.hpp>
#include <colorcpp/algorithms/vision/helpers.hpp>
#include <colorcpp/algorithms/vision/protanopia.hpp>
#include <colorcpp/algorithms/vision/tritanopia.hpp>

/**
 * @brief CVD simulation; input/output types match via @ref conversion::color_cast (linear sRGB ↔ LMS).
 * @note References: Brettel et al. 1997; Viénot et al. 1999 (HPE LMS, D65).
 */
namespace colorcpp::algorithms::vision {
// All functionality is provided by the included sub-modules
}
