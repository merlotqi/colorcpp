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

// Include color operations header
#include <colorcpp/operations/operations.hpp>

// Include color algorithms header
#include <colorcpp/algorithms/algorithms.hpp>

// Include color io header
#include <colorcpp/io/io.hpp>
