/**
 * @file operations.hpp
 * @brief Aggregates color algorithms: conversion, blending, interpolation, palettes, ΔE, gamut, WCAG, vision, random.
 */

#pragma once

#include <colorcpp/operations/accessibility.hpp>
#include <colorcpp/operations/blend.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/delta_e.hpp>
#include <colorcpp/operations/gamut.hpp>
#include <colorcpp/operations/interpolate.hpp>
#include <colorcpp/operations/palette.hpp>
#include <colorcpp/operations/random.hpp>
#include <colorcpp/operations/vision.hpp>

/**
 * @namespace colorcpp::operations
 * @brief Library algorithms: @c blend, @c conversion, @c interpolate, @c palette, @c delta_e, @c gamut,
 *        @c accessibility, @c vision, @c random (each is a nested namespace).
 */

/** @brief Re-exports nested @c colorcpp::operations::* into @c colorcpp when using the umbrella header. */
namespace colorcpp {

using namespace operations;

}  // namespace colorcpp
