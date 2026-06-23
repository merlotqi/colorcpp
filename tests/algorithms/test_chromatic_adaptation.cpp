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

}  // namespace colorcpp::algorithms::test
