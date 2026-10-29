/**
 * @file test_xyy.cpp
 * @brief Tests for CIE xyY color model and conversion.
 */

#include <gtest/gtest.h>

#include <cmath>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/core/xyy.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::core::test {

using namespace colorcpp::operations::conversion;

TEST(XYYTest, DefaultConstruction) {
  xyy_t c;
  EXPECT_FLOAT_EQ(c.x(), 0.0f);
  EXPECT_FLOAT_EQ(c.y(), 0.0f);
  EXPECT_FLOAT_EQ(c.Y(), 0.0f);
}

TEST(XYYTest, ParameterizedConstruction) {
  xyy_t c(0.54369557f, 0.32107944f, 0.12197225f);
  EXPECT_FLOAT_EQ(c.x(), 0.54369557f);
  EXPECT_FLOAT_EQ(c.y(), 0.32107944f);
  EXPECT_FLOAT_EQ(c.Y(), 0.12197225f);
}

// Reference: colour-science XYZ_to_xyY doctest
TEST(XYYTest, XYZToXYYMatchesColorScienceReference) {
  const xyz_t xyz(0.20654008f, 0.12197225f, 0.05136952f);
  const auto xyy = color_cast<xyy_t>(xyz);
  EXPECT_NEAR(xyy.x(), 0.5436955f, 1e-5f);
  EXPECT_NEAR(xyy.y(), 0.3210794f, 1e-5f);
  EXPECT_NEAR(xyy.Y(), 0.1219722f, 1e-5f);
}

// Reference: colour-science xyY_to_XYZ doctest
TEST(XYYTest, XYYToXYZMatchesColorScienceReference) {
  const xyy_t xyy(0.54369557f, 0.32107944f, 0.12197225f);
  const auto xyz = color_cast<xyz_t>(xyy);
  EXPECT_NEAR(xyz.x(), 0.2065400f, 1e-5f);
  EXPECT_NEAR(xyz.y(), 0.1219722f, 1e-5f);
  EXPECT_NEAR(xyz.z(), 0.0513695f, 1e-5f);
}

TEST(XYYTest, RoundTripXYZXYYXYZ) {
  const xyz_t xyz(0.3f, 0.5f, 0.7f);
  const auto back = color_cast<xyz_t>(color_cast<xyy_t>(xyz));
  EXPECT_NEAR(back.x(), xyz.x(), 1e-5f);
  EXPECT_NEAR(back.y(), xyz.y(), 1e-5f);
  EXPECT_NEAR(back.z(), xyz.z(), 1e-5f);
}

TEST(XYYTest, D65WhiteChromaticity) {
  const xyz_t d65(0.95047f, 1.0f, 1.08883f);
  const auto xyy = color_cast<xyy_t>(d65);
  EXPECT_NEAR(xyy.x(), 0.312727f, 1e-4f);
  EXPECT_NEAR(xyy.y(), 0.329023f, 1e-4f);
  EXPECT_NEAR(xyy.Y(), 1.0f, 1e-6f);
}

TEST(XYYTest, BlackYieldsZeroChromaticityWithoutNaN) {
  const xyz_t black(0.0f, 0.0f, 0.0f);
  const auto xyy = color_cast<xyy_t>(black);
  EXPECT_FLOAT_EQ(xyy.x(), 0.0f);
  EXPECT_FLOAT_EQ(xyy.y(), 0.0f);
  EXPECT_FALSE(std::isnan(xyy.x()));
  EXPECT_FALSE(std::isnan(xyy.y()));
}

TEST(XYYTest, ZeroYChromaticityDoesNotCrash) {
  // Degenerate chromaticity (y = 0) must not divide by zero.
  const xyy_t xyy(0.4f, 0.0f, 1.0f);
  const auto xyz = color_cast<xyz_t>(xyy);
  EXPECT_FALSE(std::isnan(xyz.x()));
  EXPECT_FALSE(std::isnan(xyz.z()));
}

// Graph integration: xyY hangs off XYZ.
TEST(XYYTest, GraphRoutesThroughXYZ) {
  static_assert(operations::conversion::graph::minimal_conversion_cost<rgbf_t, xyy_t>() == 3);
  static_assert(std::is_same_v<operations::conversion::graph::next_hop_t<rgbf_t, xyy_t>, linear_rgbf_t>);
  static_assert(operations::conversion::graph::minimal_conversion_cost<xyy_t, oklab_t>() == 2);
  SUCCEED();
}

TEST(XYYTest, CastFromSRGBThroughGraph) {
  const auto xyy = color_cast<xyy_t>(rgbf_t(1.0f, 1.0f, 1.0f));
  const auto expected = color_cast<xyy_t>(color_cast<xyz_t>(rgbf_t(1.0f, 1.0f, 1.0f)));
  EXPECT_NEAR(xyy.x(), expected.x(), 1e-6f);
  EXPECT_NEAR(xyy.y(), expected.y(), 1e-6f);
  EXPECT_NEAR(xyy.Y(), expected.Y(), 1e-6f);
}

}  // namespace colorcpp::core::test
