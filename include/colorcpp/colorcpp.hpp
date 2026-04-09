/**
 * @file colorcpp.hpp
 * @brief Single-include entry for the colorcpp library.
 *
 * Pulls in core color types, operations, algorithms, and I/O helpers.
 *
 * Canonical public namespaces remain @c colorcpp::core, @c colorcpp::operations::*,
 * @c colorcpp::algorithms::*, and @c colorcpp::io::*. Including this umbrella also
 * enables the convenience re-exports provided by the aggregate @c core / @c operations /
 * @c algorithms headers, so existing @c colorcpp::conversion-style code continues to work.
 * Prefer the canonical nested namespaces in new docs and examples; individual headers remain
 * available for finer-grained includes.
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
