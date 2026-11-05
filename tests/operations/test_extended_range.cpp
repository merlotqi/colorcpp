/**
 * @file test_extended_range.cpp
 * @brief Extended-range (scRGB/HDR) conversion intermediates (issue #76).
 *
 * Policy under test:
 * - Linear RGB float models and XYZ opt into extended range: conversion hops
 *   preserve negative and >1 float values instead of clamping them, so
 *   rgb → linear → xyz → linear keeps over/under-range intermediates intact
 *   and Rec.2020's sign-preserving transfer curve actually sees its input.
 * - Display-encoded models (sRGB gamma etc.) keep clamping at the encode
 *   boundary — a device cannot show those values.
 * - Integral channels always clamp (storage has no room for out-of-range).
 * - The opt-in is a per-model trait; everything else defaults to clamping.
 */

#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;
using namespace colorcpp::core;
using namespace colorcpp::operations::conversion;

// ============================================================
// Trait opt-in (issue #76): which models allow out-of-range floats
// ============================================================

// Extended-range models: algebraic middle of a pipeline must not clamp.
static_assert(traits::model_extended_range_v<linear_rgb::model::linear_rgbf>);
static_assert(traits::model_extended_range_v<linear_rgb::model::linear_rgbaf>);
static_assert(traits::model_extended_range_v<adobe_rgb::model::linear_adobe_rgbf>);
static_assert(traits::model_extended_range_v<display_p3::model::linear_display_p3f>);
static_assert(traits::model_extended_range_v<rec2020::model::linear_rec2020f>);
static_assert(traits::model_extended_range_v<prophoto_rgb::model::linear_prophoto_rgbf>);
static_assert(traits::model_extended_range_v<xyz::model::xyze>);

// Display-encoded, integral and bounded perceptual models keep clamping.
static_assert(!traits::model_extended_range_v<rgb::model::rgb_float>);
static_assert(!traits::model_extended_range_v<rgb::model::rgb8>);
static_assert(!traits::model_extended_range_v<lab::model::cielab>);
static_assert(!traits::model_extended_range_v<xyy::model::xyy>);

// ============================================================
// The issue's example: first hop must not destroy the value
// ============================================================

TEST(ExtendedRangeTest, OverRangeSurvivesRgbToLinear) {
  // Pre-#76: from_unit clamped to [0,1] at the first hop → (0, 0.214, 1).
  const auto lin = color_cast<linear_rgbf_t>(rgbf_t{-0.1f, 0.5f, 1.2f});
  EXPECT_LT(lin.r(), 0.0f) << "pre-fix: r clamped to 0";
  EXPECT_NEAR(lin.g(), 0.2140f, 1e-3f);
  EXPECT_GT(lin.b(), 1.0f) << "pre-fix: b clamped to 1";
}

TEST(ExtendedRangeTest, ExtendedLinearRoundTripsThroughXyz) {
  const linear_rgbf_t lin(-0.1f, 0.5f, 1.2f);
  const auto back = color_cast<linear_rgbf_t>(color_cast<xyz_t>(lin));
  EXPECT_NEAR(back.r(), -0.1f, 1e-3f) << "pre-fix: r clamped to 0 at the xyz hop";
  EXPECT_NEAR(back.g(), 0.5f, 1e-3f);
  EXPECT_NEAR(back.b(), 1.2f, 1e-3f) << "pre-fix: b clamped to 1 at the xyz hop";
}

TEST(ExtendedRangeTest, XyzBeyondNominalRoundTripsThroughLinear) {
  // XYZ nominal range is [-1, 2] and is documented as non-clipping; values
  // beyond it must survive linear round trips (from_value also skips the
  // clamp for extended-range models).
  const xyz_t hot(2.5f, -1.5f, 3.0f);
  const auto back = color_cast<xyz_t>(color_cast<linear_rgbf_t>(hot));
  EXPECT_NEAR(back.x(), 2.5f, 1e-3f) << "pre-fix: X clipped at 2 (or collapsed via clamped linear)";
  EXPECT_NEAR(back.y(), -1.5f, 1e-3f);
  EXPECT_NEAR(back.z(), 3.0f, 1e-3f);
}

TEST(ExtendedRangeTest, FullChainPreservesIntermediates) {
  // Issue #76's chain: intermediates survive; the final display hop clamps
  // because a device cannot show over/under-range sRGB.
  const rgbf_t in(-0.1f, 0.5f, 1.2f);
  const auto lin1 = color_cast<linear_rgbf_t>(in);
  const auto xyz = color_cast<xyz_t>(lin1);
  const auto lin2 = color_cast<linear_rgbf_t>(xyz);
  const auto out = color_cast<rgbf_t>(lin2);

  EXPECT_LT(lin1.r(), 0.0f);
  EXPECT_GT(lin1.b(), 1.0f);
  EXPECT_NEAR(lin2.r(), lin1.r(), 1e-3f) << "linear must round-trip through xyz";
  EXPECT_NEAR(lin2.g(), lin1.g(), 1e-3f);
  EXPECT_NEAR(lin2.b(), lin1.b(), 1e-3f);

  // Display boundary keeps clamping (issue: "keep display-encoded spaces
  // clamping as today"). In-range channel 0.5 is an sRGB-encoded value and
  // round-trips exactly; only the over/under-range channels are clipped.
  EXPECT_FLOAT_EQ(out.r(), 0.0f);
  EXPECT_NEAR(out.g(), 0.5f, 1e-3f);
  EXPECT_FLOAT_EQ(out.b(), 1.0f);
}

// ============================================================
// Rec.2020 sign-preserving transfer: dead code until hops stop clamping
// ============================================================

TEST(ExtendedRangeTest, Rec2020SignedCurveCarriesOverRange) {
  const auto lin = color_cast<linear_rec2020_rgbf_t>(rec2020_rgbf_t{-0.1f, 0.5f, 1.2f});
  EXPECT_LT(lin.r(), 0.0f) << "pre-fix: sign-preserving curve output clamped to 0";
  EXPECT_GT(lin.b(), 1.0f) << "pre-fix: >1 output clamped to 1";
  EXPECT_NEAR(lin.g(), std::pow(0.5f, 2.4f), 1e-3f);
}

// ============================================================
// Display boundaries keep clamping (regression guards — green by design)
// ============================================================

TEST(ExtendedRangeTest, DisplayEncodeStillClamps) {
  const auto disp = color_cast<rgbf_t>(linear_rgbf_t{-0.1f, 0.5f, 1.2f});
  EXPECT_FLOAT_EQ(disp.r(), 0.0f) << "display encode must clamp negatives";
  EXPECT_NEAR(disp.g(), 0.7354f, 1e-3f);
  EXPECT_FLOAT_EQ(disp.b(), 1.0f) << "display encode must clamp >1";
}

TEST(ExtendedRangeTest, SameModelCastIsIdentityForOutOfRangeStorage) {
  // Storage never validates (color_base policy); a same-type color_cast is a
  // no-op, so out-of-range floats pass verbatim. Clamping happens only when a
  // conversion writes into a non-extended (display/integral) model.
  const rgbaf_t src(-0.2f, 0.5f, 1.4f, 1.5f);
  const auto out = color_cast<rgbaf_t>(src);
  EXPECT_FLOAT_EQ(out.r(), -0.2f);
  EXPECT_FLOAT_EQ(out.b(), 1.4f);
  EXPECT_FLOAT_EQ(out.a(), 1.5f);
}

TEST(ExtendedRangeTest, IntegralTargetStillClamps) {
  // Integral storage has no room for out-of-range regardless of trait.
  const auto px = color_cast<rgb8_t>(linear_rgbf_t{-0.1f, 0.5f, 1.2f});
  EXPECT_EQ(px.r(), 0);
  EXPECT_EQ(px.b(), 255);
}

// ============================================================
// Alpha rides the same policy (model-level trait)
// ============================================================

TEST(ExtendedRangeTest, ExtendedAlphaPreservedThroughLinearHop) {
  const auto lin = color_cast<linear_rgbaf_t>(rgbaf_t{-0.1f, 0.5f, 1.2f, 1.5f});
  EXPECT_NEAR(lin.a(), 1.5f, 1e-6f) << "pre-fix: alpha clamped to 1 by from_unit";
  EXPECT_LT(lin.r(), 0.0f);
  EXPECT_GT(lin.b(), 1.0f);
}
