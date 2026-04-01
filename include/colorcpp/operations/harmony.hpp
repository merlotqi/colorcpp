/**
 * @file harmony.hpp
 * @brief Color harmony analysis, correction and suggestion module.
 *
 * This header includes all harmony sub-modules:
 * - assess.hpp: Evaluate how harmonious a palette is
 * - correct.hpp: Adjust colors to improve harmony
 * - suggest.hpp: Provide suggestions for improving harmony
 */

#pragma once

#include <colorcpp/operations/harmony/assess.hpp>
#include <colorcpp/operations/harmony/correct.hpp>
#include <colorcpp/operations/harmony/suggest.hpp>

/** @brief Color harmony analysis and correction built on @ref palette_set. */
namespace colorcpp::operations::harmony {
// All functionality is provided by the included sub-modules
}
