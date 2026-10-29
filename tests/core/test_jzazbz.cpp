/**
 * @file test_jzazbz.cpp
 * @brief Tests for Jzazbz (Safdar 2017) color model and conversion.
 */

#include <gtest/gtest.h>

#include <colorcpp/core/jzazbz.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::core::test {

using namespace colorcpp::operations::conversion;

TEST(JzazbzTest, DefaultConstruction) {
  jzazbz_t c;
  EXPECT_FLOAT_EQ(c.jz(), 0.0f);
  EXPECT_FLOAT_EQ(c.az(), 0.0f);
  EXPECT_FLOAT_EQ(c.bz(), 0.0f);
}

TEST(JzazbzTest, ParameterizedConstruction) {
  jzazbz_t c(0.0053505f, 0.0092430f, 0.0052601f);
  EXPECT_FLOAT_EQ(c.jz(), 0.0053505f);
  EXPECT_FLOAT_EQ(c.az(), 0.0092430f);
  EXPECT_FLOAT_EQ(c.bz(), 0.0052601f);
}

// Reference: colour-science XYZ_to_Jzazbz doctest (Safdar 2017 method).
TEST(JzazbzTest, XYZToJzazbzMatchesColorScienceReference) {
  const xyz_t xyz(0.20654008f, 0.12197225f, 0.05136952f);
  const auto jzazbz = color_cast<jzazbz_t>(xyz);
  EXPECT_NEAR(jzazbz.jz(), 0.0053505f, 1e-5f);
  EXPECT_NEAR(jzazbz.az(), 0.0092430f, 1e-5f);
  EXPECT_NEAR(jzazbz.bz(), 0.0052601f, 1e-5f);
}

// Reference: colour-science Jzazbz_to_XYZ doctest.
TEST(JzazbzTest, JzazbzToXYZMatchesColorScienceReference) {
  const jzazbz_t jzazbz(0.0053505f, 0.0092430f, 0.0052601f);
  const auto xyz = color_cast<xyz_t>(jzazbz);
  EXPECT_NEAR(xyz.x(), 0.2065401f, 1e-5f);
  EXPECT_NEAR(xyz.y(), 0.1219723f, 1e-5f);
  EXPECT_NEAR(xyz.z(), 0.0513695f, 1e-5f);
}

TEST(JzazbzTest, RoundTripXYZJzazbzXYZ) {
  // Tolerance 1e-4: the PQ encode/decode chain (four pow calls) accumulates
  // ~1e-4 relative error in float32. Reference-value tests above pin accuracy.
  const xyz_t xyz(0.3f, 0.5f, 0.7f);
  const auto back = color_cast<xyz_t>(color_cast<jzazbz_t>(xyz));
  EXPECT_NEAR(back.x(), xyz.x(), 1e-4f);
  EXPECT_NEAR(back.y(), xyz.y(), 1e-4f);
  EXPECT_NEAR(back.z(), xyz.z(), 1e-4f);
}

TEST(JzazbzTest, D65WhiteLightness) {
  // Jz of the D65 reference white is ~0.01758; a*, b* are achromatic.
  const xyz_t d65(0.95047f, 1.0f, 1.08883f);
  const auto jzazbz = color_cast<jzazbz_t>(d65);
  EXPECT_NEAR(jzazbz.jz(), 0.0175800f, 1e-4f);
  EXPECT_NEAR(jzazbz.az(), 0.0f, 1e-3f);
  EXPECT_NEAR(jzazbz.bz(), 0.0f, 1e-3f);
}

TEST(JzazbzTest, BlackYieldsZero) {
  const xyz_t black(0.0f, 0.0f, 0.0f);
  const auto jzazbz = color_cast<jzazbz_t>(black);
  EXPECT_NEAR(jzazbz.jz(), 0.0f, 1e-9f);
  EXPECT_NEAR(jzazbz.az(), 0.0f, 1e-9f);
  EXPECT_NEAR(jzazbz.bz(), 0.0f, 1e-9f);
}

// Graph integration: Jzazbz hangs off XYZ.
TEST(JzazbzTest, GraphRoutesThroughXYZ) {
  static_assert(operations::conversion::graph::minimal_conversion_cost<rgbf_t, jzazbz_t>() == 3);
  static_assert(std::is_same_v<operations::conversion::graph::next_hop_t<rgbf_t, jzazbz_t>, linear_rgbf_t>);
  static_assert(operations::conversion::graph::minimal_conversion_cost<jzazbz_t, oklab_t>() == 2);
  SUCCEED();
}

TEST(JzazbzTest, CastFromSRGBThroughGraph) {
  const auto jzazbz = color_cast<jzazbz_t>(rgbf_t(1.0f, 1.0f, 1.0f));
  const auto expected = color_cast<jzazbz_t>(color_cast<xyz_t>(rgbf_t(1.0f, 1.0f, 1.0f)));
  EXPECT_NEAR(jzazbz.jz(), expected.jz(), 1e-6f);
  EXPECT_NEAR(jzazbz.az(), expected.az(), 1e-6f);
  EXPECT_NEAR(jzazbz.bz(), expected.bz(), 1e-6f);
}

}  // namespace colorcpp::core::test
