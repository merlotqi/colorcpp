/**
 * @file operations.hpp
 * @brief Aggregates the operational modules: conversion, blending, comparison, interpolation, palettes, and
 * random generation.
 */

#pragma once

#include <colorcpp/operations/blend.hpp>
#include <colorcpp/operations/compare.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/interpolate.hpp>
#include <colorcpp/operations/palette.hpp>
#include <colorcpp/operations/random.hpp>

/**
 * @namespace colorcpp::operations
 * @brief Operational building blocks: @c blend, @c compare, @c conversion, @c interpolate, @c palette,
 *        @c random (each is a nested namespace).
 */

/** @brief Convenience re-export into @c colorcpp; canonical docs use @c colorcpp::operations::* explicitly. */
namespace colorcpp {

using namespace operations;

}  // namespace colorcpp
