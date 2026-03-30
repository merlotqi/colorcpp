/**
 * @file colorcpp.hpp
 * @brief Single-include entry for the colorcpp library.
 *
 * Pulls in core color types, user-defined literals, and operations (conversion,
 * blending, interpolation, palettes, accessibility, etc.). Prefer this header
 * for quick integration; individual headers remain available for finer-grained
 * includes.
 */

#pragma once

// Include all color core headers
#include <colorcpp/core/core.hpp>

// Include color literal headers
#include <colorcpp/literals/literals.hpp>

// Include color operations header
#include <colorcpp/operations/operations.hpp>

// Include color algorithms header
#include <colorcpp/algorithms/algorithms.hpp>

// CSS Color string parsing (depends on conversion; include after operations)
#include <colorcpp/parsers/css_color.hpp>

/**
 * @brief Root namespace; re-exports @ref colorcpp::core and @ref colorcpp::operations.
 */
namespace colorcpp {

using namespace core;
using namespace operations;

}  // namespace colorcpp
