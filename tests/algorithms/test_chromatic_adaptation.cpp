/**
 * @file test_chromatic_adaptation.cpp
 * @brief Tests for chromatic adaptation (Bradford and Von Kries).
 */

#include <gtest/gtest.h>

#include <colorcpp/algorithms/chromatic_adaptation.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::test {

using namespace core;
using namespace chromatic_adaptation;
using namespace operations::conversion;

// --- White point constants ---

TEST(WhitePointTest, D65Values) {
  EXPECT_NEAR(WHITEPOINT_D65.x(), 0.95047f, 1e-5f);
  EXPECT_NEAR(WHITEPOINT_D65.y(), 1.00000f, 1e-5f);
  EXPECT_NEAR(WHITEPOINT_D65.z(), 1.08883f, 1e-5f);
}

TEST(WhitePointTest, D50Values) {
  EXPECT_NEAR(WHITEPOINT_D50.x(), 0.96422f, 1e-5f);
  EXPECT_NEAR(WHITEPOINT_D50.y(), 1.00000f, 1e-5f);
  EXPECT_NEAR(WHITEPOINT_D50.z(), 0.82521f, 1e-5f);
}

// --- Bradford adaptation ---

TEST(BradfordTest, SameWhitePointIsIdentity) {
  xyz_t color(0.5f, 0.4f, 0.3f);
  auto result = bradford_adapt(color, WHITEPOINT_D65, WHITEPOINT_D65);
  EXPECT_NEAR(result.x(), 0.5f, 1e-5f);
  EXPECT_NEAR(result.y(), 0.4f, 1e-5f);
  EXPECT_NEAR(result.z(), 0.3f, 1e-5f);
}

TEST(BradfordTest, RoundTripD65D50D65) {
  xyz_t color(0.5f, 0.4f, 0.3f);
  auto d50 = bradford_adapt(color, WHITEPOINT_D65, WHITEPOINT_D50);
  auto back = bradford_adapt(d50, WHITEPOINT_D50, WHITEPOINT_D65);
  EXPECT_NEAR(back.x(), color.x(), 1e-4f);
  EXPECT_NEAR(back.y(), color.y(), 1e-4f);
  EXPECT_NEAR(back.z(), color.z(), 1e-4f);
}

TEST(BradfordTest, D65ToD50KnownReference) {
  // Bruce Lindbloom reference: D65 white (0.95047, 1.0, 1.08883) → D50
  auto result = bradford_adapt(WHITEPOINT_D65, WHITEPOINT_D65, WHITEPOINT_D50);
  EXPECT_NEAR(result.x(), 0.96422f, 1e-4f);
  EXPECT_NEAR(result.y(), 1.00000f, 1e-4f);
  EXPECT_NEAR(result.z(), 0.82521f, 1e-4f);
}

TEST(BradfordTest, NonNegativeOutput) {
  xyz_t color(0.1f, 0.8f, 0.5f);
  auto result = bradford_adapt(color, WHITEPOINT_D65, WHITEPOINT_A);
  EXPECT_GE(result.x(), -1e-5f);
  EXPECT_GE(result.y(), -1e-5f);
  EXPECT_GE(result.z(), -1e-5f);
}

TEST(BradfordTest, WorksWithRgbTypes) {
  rgbf_t color(0.5f, 0.3f, 0.7f);
  auto result = bradford_adapt(color, WHITEPOINT_D65, WHITEPOINT_D50);
  // Result should be a valid rgbf_t (within [0,1])
  EXPECT_GE(result.r(), 0.0f);
  EXPECT_LE(result.r(), 1.0f);
  EXPECT_GE(result.g(), 0.0f);
  EXPECT_LE(result.g(), 1.0f);
  EXPECT_GE(result.b(), 0.0f);
  EXPECT_LE(result.b(), 1.0f);
}

// --- Von Kries adaptation ---

TEST(VonKriesTest, SameWhitePointIsIdentity) {
  xyz_t color(0.5f, 0.4f, 0.3f);
  auto result = von_kries_adapt(color, WHITEPOINT_D65, WHITEPOINT_D65);
  EXPECT_NEAR(result.x(), 0.5f, 1e-5f);
  EXPECT_NEAR(result.y(), 0.4f, 1e-5f);
  EXPECT_NEAR(result.z(), 0.3f, 1e-5f);
}

TEST(VonKriesTest, RoundTripD65D50D65) {
  xyz_t color(0.5f, 0.4f, 0.3f);
  auto d50 = von_kries_adapt(color, WHITEPOINT_D65, WHITEPOINT_D50);
  auto back = von_kries_adapt(d50, WHITEPOINT_D50, WHITEPOINT_D65);
  EXPECT_NEAR(back.x(), color.x(), 1e-3f);
  EXPECT_NEAR(back.y(), color.y(), 1e-3f);
  EXPECT_NEAR(back.z(), color.z(), 1e-3f);
}

// --- CAT02 / CAT16 / CMCCAT2000 / XYZ scaling ---
//
// Reference adaptation matrices: colour-science's matrix_chromatic_adaptation_VonKries
// for CAT02 and Bradford (its doctests), and matrices generated from the published
// CAT cones (colour/adaptation/datasets/cat.py) with the same von Kries construction
// for CAT16 / CMCCAT2000. Whites are the CIE 15:2004 D65/D50 values used by the
// colour-science doctests.

namespace {

constexpr xyz_t CS_WHITE_D65(0.95045593f, 1.0f, 1.08905775f);
constexpr xyz_t CS_WHITE_D50(0.96429568f, 1.0f, 0.82510460f);

// Adapting the XYZ basis vectors exposes the adaptation matrix: the image of
// e_i is column i of the effective matrix Minv * diag(ratio) * M.
// `expected` holds the 9 matrix entries in row-major order.
template <typename AdaptFn>
void ExpectAdaptationMatrix(AdaptFn&& adapt, const float (&expected)[9]) {
  const xyz_t c0 = adapt(xyz_t(1.0f, 0.0f, 0.0f));
  const xyz_t c1 = adapt(xyz_t(0.0f, 1.0f, 0.0f));
  const xyz_t c2 = adapt(xyz_t(0.0f, 0.0f, 1.0f));
  EXPECT_NEAR(c0.x(), expected[0], 1e-5f);
  EXPECT_NEAR(c1.x(), expected[1], 1e-5f);
  EXPECT_NEAR(c2.x(), expected[2], 1e-5f);
  EXPECT_NEAR(c0.y(), expected[3], 1e-5f);
  EXPECT_NEAR(c1.y(), expected[4], 1e-5f);
  EXPECT_NEAR(c2.y(), expected[5], 1e-5f);
  EXPECT_NEAR(c0.z(), expected[6], 1e-5f);
  EXPECT_NEAR(c1.z(), expected[7], 1e-5f);
  EXPECT_NEAR(c2.z(), expected[8], 1e-5f);
}

}  // namespace

TEST(CAT02Test, AdaptationMatrixMatchesColorScienceReference) {
  const float expected[9] = {1.042573893f,  0.030891076f,  -0.052812566f, 0.022193451f, 1.001856633f,
                             -0.021073749f, -0.001164880f, -0.003420528f, 0.761789074f};
  ExpectAdaptationMatrix([&](const xyz_t& c) { return cat02_adapt(c, CS_WHITE_D65, CS_WHITE_D50); }, expected);
}

TEST(CAT02Test, SameWhitePointIsIdentity) {
  const xyz_t color(0.5f, 0.4f, 0.3f);
  const auto result = cat02_adapt(color, WHITEPOINT_D65, WHITEPOINT_D65);
  EXPECT_NEAR(result.x(), 0.5f, 1e-5f);
  EXPECT_NEAR(result.y(), 0.4f, 1e-5f);
  EXPECT_NEAR(result.z(), 0.3f, 1e-5f);
}

TEST(CAT02Test, WhiteMapsToDestinationWhite) {
  const auto result = cat02_adapt(WHITEPOINT_D65, WHITEPOINT_D65, WHITEPOINT_D50);
  EXPECT_NEAR(result.x(), WHITEPOINT_D50.x(), 1e-5f);
  EXPECT_NEAR(result.y(), WHITEPOINT_D50.y(), 1e-5f);
  EXPECT_NEAR(result.z(), WHITEPOINT_D50.z(), 1e-5f);
}

TEST(CAT02Test, RoundTripD65D50D65) {
  const xyz_t color(0.5f, 0.4f, 0.3f);
  const auto d50 = cat02_adapt(color, WHITEPOINT_D65, WHITEPOINT_D50);
  const auto back = cat02_adapt(d50, WHITEPOINT_D50, WHITEPOINT_D65);
  EXPECT_NEAR(back.x(), color.x(), 1e-4f);
  EXPECT_NEAR(back.y(), color.y(), 1e-4f);
  EXPECT_NEAR(back.z(), color.z(), 1e-4f);
}

TEST(CAT16Test, AdaptationMatrixMatchesReference) {
  const float expected[9] = {1.010854329f, 0.040708611f, -0.034144583f, 0.005428142f, 0.993581926f,
                             0.001155920f, 0.000250722f, -0.011491876f, 0.767964945f};
  ExpectAdaptationMatrix([&](const xyz_t& c) { return cat16_adapt(c, CS_WHITE_D65, CS_WHITE_D50); }, expected);
}

TEST(CAT16Test, SameWhitePointIsIdentity) {
  const xyz_t color(0.5f, 0.4f, 0.3f);
  const auto result = cat16_adapt(color, WHITEPOINT_D65, WHITEPOINT_D65);
  EXPECT_NEAR(result.x(), 0.5f, 1e-5f);
  EXPECT_NEAR(result.y(), 0.4f, 1e-5f);
  EXPECT_NEAR(result.z(), 0.3f, 1e-5f);
}

TEST(CAT16Test, RoundTripD65D50D65) {
  const xyz_t color(0.5f, 0.4f, 0.3f);
  const auto d50 = cat16_adapt(color, WHITEPOINT_D65, WHITEPOINT_D50);
  const auto back = cat16_adapt(d50, WHITEPOINT_D50, WHITEPOINT_D65);
  EXPECT_NEAR(back.x(), color.x(), 1e-4f);
  EXPECT_NEAR(back.y(), color.y(), 1e-4f);
  EXPECT_NEAR(back.z(), color.z(), 1e-4f);
}

TEST(CMCCAT2000Test, AdaptationMatrixMatchesReference) {
  const float expected[9] = {1.040195450f,  0.024007597f,  -0.044416241f, 0.022338466f, 0.991001823f,
                             -0.011233151f, -0.000774667f, -0.005603480f, 0.763452963f};
  ExpectAdaptationMatrix([&](const xyz_t& c) { return cmccat2000_adapt(c, CS_WHITE_D65, CS_WHITE_D50); }, expected);
}

TEST(CMCCAT2000Test, SameWhitePointIsIdentity) {
  const xyz_t color(0.5f, 0.4f, 0.3f);
  const auto result = cmccat2000_adapt(color, WHITEPOINT_D65, WHITEPOINT_D65);
  EXPECT_NEAR(result.x(), 0.5f, 1e-5f);
  EXPECT_NEAR(result.y(), 0.4f, 1e-5f);
  EXPECT_NEAR(result.z(), 0.3f, 1e-5f);
}

TEST(CMCCAT2000Test, RoundTripD65D50D65) {
  const xyz_t color(0.5f, 0.4f, 0.3f);
  const auto d50 = cmccat2000_adapt(color, WHITEPOINT_D65, WHITEPOINT_D50);
  const auto back = cmccat2000_adapt(d50, WHITEPOINT_D50, WHITEPOINT_D65);
  EXPECT_NEAR(back.x(), color.x(), 1e-4f);
  EXPECT_NEAR(back.y(), color.y(), 1e-4f);
  EXPECT_NEAR(back.z(), color.z(), 1e-4f);
}

TEST(XYZScalingTest, AdaptationMatrixMatchesReference) {
  // Pure diagonal: (X_r/X_w, Y_r/Y_w, Z_r/Z_w) — the von Kries part of CIE 1994.
  const float expected[9] = {1.014561170f, 0.0f, 0.0f, 0.0f, 1.000000000f, 0.0f, 0.0f, 0.0f, 0.757631632f};
  ExpectAdaptationMatrix([&](const xyz_t& c) { return xyz_scaling_adapt(c, CS_WHITE_D65, CS_WHITE_D50); }, expected);
}

TEST(XYZScalingTest, SameWhitePointIsIdentity) {
  const xyz_t color(0.5f, 0.4f, 0.3f);
  const auto result = xyz_scaling_adapt(color, WHITEPOINT_D65, WHITEPOINT_D65);
  EXPECT_NEAR(result.x(), 0.5f, 1e-5f);
  EXPECT_NEAR(result.y(), 0.4f, 1e-5f);
  EXPECT_NEAR(result.z(), 0.3f, 1e-5f);
}

TEST(XYZScalingTest, WhiteMapsToDestinationWhite) {
  const auto result = xyz_scaling_adapt(WHITEPOINT_D65, WHITEPOINT_D65, WHITEPOINT_D50);
  EXPECT_NEAR(result.x(), WHITEPOINT_D50.x(), 1e-5f);
  EXPECT_NEAR(result.y(), WHITEPOINT_D50.y(), 1e-5f);
  EXPECT_NEAR(result.z(), WHITEPOINT_D50.z(), 1e-5f);
}

TEST(XYZScalingTest, RoundTripD65D50D65) {
  const xyz_t color(0.5f, 0.4f, 0.3f);
  const auto d50 = xyz_scaling_adapt(color, WHITEPOINT_D65, WHITEPOINT_D50);
  const auto back = xyz_scaling_adapt(d50, WHITEPOINT_D50, WHITEPOINT_D65);
  EXPECT_NEAR(back.x(), color.x(), 1e-4f);
  EXPECT_NEAR(back.y(), color.y(), 1e-4f);
  EXPECT_NEAR(back.z(), color.z(), 1e-4f);
}

}  // namespace colorcpp::algorithms::test
