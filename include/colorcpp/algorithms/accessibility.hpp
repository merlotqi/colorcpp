/**
 * @file accessibility.hpp
 * @brief WCAG 2.x relative luminance / contrast ratio, plus optional APCA Lc (WCAG 3 / Silver draft style).
 *
 * This header includes all accessibility sub-modules:
 * - wcag.hpp: WCAG 2.x relative luminance and contrast ratio
 * - apca.hpp: APCA (Advanced Perceptual Contrast Algorithm)
 */

#pragma once

#include <colorcpp/algorithms/accessibility/apca.hpp>
#include <colorcpp/algorithms/accessibility/wcag.hpp>

/**
 * @brief WCAG 2.x metrics and APCA (Advanced Perceptual Contrast Algorithm) as parallel APIs.
 * @note Do not mix WCAG 2 contrast_ratio with APCA apca_contrast thresholds; they use different scales.
 */
namespace colorcpp::algorithms::accessibility {
// All functionality is provided by the included sub-modules
}
