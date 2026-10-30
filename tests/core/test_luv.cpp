/**
 * @file test_luv.cpp
 * @brief Tests for CIE L*u*v* color model and conversion.
 */

#include <gtest/gtest.h>

#include <colorcpp/core/display_p3.hpp>
#include <colorcpp/core/luv.hpp>
#include <colorcpp/core/prophoto_rgb.hpp>
#include <colorcpp/core/rec2020.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::core::test {

using namespace colorcpp::operations::conversion;

TEST(LuvTest, DefaultConstruction) {
  luv_t c;
  EXPECT_FLOAT_EQ(c.l(), 0.0f);
  EXPECT_FLOAT_EQ(c.u(), 0.0f);
  EXPECT_FLOAT_EQ(c.v(), 0.0f);
}

TEST(LuvTest, ParameterizedConstruction) {
  luv_t c(41.527875f, 96.830960f, 17.743297f);
  EXPECT_FLOAT_EQ(c.l(), 41.527875f);
  EXPECT_FLOAT_EQ(c.u(), 96.830960f);
  EXPECT_FLOAT_EQ(c.v(), 17.743297f);
}

// Reference: colour-science XYZ_to_Luv doctest, adapted to the library D65
// white (0.95047, 1, 1.08883). With colour-science's own CIE D65 the same
// formula yields exactly (41.527875, 96.836260, 17.752101).
TEST(LuvTest, XYZToLuvMatchesReference) {
  const xyz_t xyz(0.20654008f, 0.12197225f, 0.05136952f);
  const auto luv = color_cast<luv_t>(xyz);
  EXPECT_NEAR(luv.l(), 41.527875f, 1e-4f);
  EXPECT_NEAR(luv.u(), 96.830960f, 1e-4f);
  EXPECT_NEAR(luv.v(), 17.743297f, 1e-4f);
}

// Reference: colour-science Luv_to_XYZ inverse of the doctest above.
TEST(LuvTest, LuvToXYZMatchesReference) {
  const luv_t luv(41.527875f, 96.830960f, 17.743297f);
  const auto xyz = color_cast<xyz_t>(luv);
  EXPECT_NEAR(xyz.x(), 0.2065400f, 1e-4f);
  EXPECT_NEAR(xyz.y(), 0.1219722f, 1e-4f);
  EXPECT_NEAR(xyz.z(), 0.0513695f, 1e-4f);
}

TEST(LuvTest, RoundTripXZZuvXYZ) {
  const xyz_t xyz(0.3f, 0.5f, 0.7f);
  const auto back = color_cast<xyz_t>(color_cast<luv_t>(xyz));
  EXPECT_NEAR(back.x(), xyz.x(), 1e-4f);
  EXPECT_NEAR(back.y(), xyz.y(), 1e-4f);
  EXPECT_NEAR(back.z(), xyz.z(), 1e-4f);
}

TEST(LuvTest, D65WhiteIsAchromatic) {
  const xyz_t d65(0.95047f, 1.0f, 1.08883f);
  const auto luv = color_cast<luv_t>(d65);
  EXPECT_NEAR(luv.l(), 100.0f, 1e-4f);
  EXPECT_NEAR(luv.u(), 0.0f, 1e-4f);
  EXPECT_NEAR(luv.v(), 0.0f, 1e-4f);
}

TEST(LuvTest, BlackYieldsZeroWithoutNaN) {
  const xyz_t black(0.0f, 0.0f, 0.0f);
  const auto luv = color_cast<luv_t>(black);
  EXPECT_FLOAT_EQ(luv.l(), 0.0f);
  EXPECT_FLOAT_EQ(luv.u(), 0.0f);
  EXPECT_FLOAT_EQ(luv.v(), 0.0f);
  EXPECT_FALSE(std::isnan(luv.u()));
  EXPECT_FALSE(std::isnan(luv.v()));
}

TEST(LuvTest, LZeroYieldsZeroXYZ) {
  const luv_t luv(0.0f, 50.0f, -30.0f);
  const auto xyz = color_cast<xyz_t>(luv);
  EXPECT_NEAR(xyz.x(), 0.0f, 1e-6f);
  EXPECT_NEAR(xyz.y(), 0.0f, 1e-6f);
  EXPECT_NEAR(xyz.z(), 0.0f, 1e-6f);
}

// Graph integration: Luv hangs off XYZ.
TEST(LuvTest, GraphRoutesThroughXYZ) {
  static_assert(operations::conversion::graph::minimal_conversion_cost<rgbf_t, luv_t>() == 3);
  static_assert(std::is_same_v<operations::conversion::graph::next_hop_t<rgbf_t, luv_t>, linear_rgbf_t>);
  static_assert(operations::conversion::graph::minimal_conversion_cost<luv_t, oklab_t>() == 2);
  SUCCEED();
}

TEST(LuvTest, CastFromSRGBThroughGraph) {
  const auto luv = color_cast<luv_t>(rgbf_t(1.0f, 1.0f, 1.0f));
  const auto expected = color_cast<luv_t>(color_cast<xyz_t>(rgbf_t(1.0f, 1.0f, 1.0f)));
  EXPECT_NEAR(luv.l(), expected.l(), 1e-6f);
  EXPECT_NEAR(luv.u(), expected.u(), 1e-6f);
  EXPECT_NEAR(luv.v(), expected.v(), 1e-6f);
}

// --- Issue #56: u/v were limited to ±128, clipping real colors (sRGB red
//     u* = 175.01, blue v* ≈ -130.3) and breaking wide-gamut round-trips. ---

TEST(LuvTest, SRGBPrimariesMatchReference) {
  const auto red = color_cast<luv_t>(rgbf_t(1.0f, 0.0f, 0.0f));
  EXPECT_NEAR(red.l(), 53.24f, 0.05f);
  EXPECT_NEAR(red.u(), 175.01f, 0.05f);
  EXPECT_NEAR(red.v(), 37.76f, 0.05f);

  const auto blue = color_cast<luv_t>(rgbf_t(0.0f, 0.0f, 1.0f));
  EXPECT_NEAR(blue.v(), -130.34f, 0.05f);
}

TEST(LuvTest, WideGamutPrimariesRoundTrip) {
  // 5e-3, not 2e-3: at an encoded-0 baseline (rec2020 green's b), float32
  // round-trip noise of ~1e-6 in the linear channel is amplified ~450x by
  // the gamma encode — still under one 8-bit level, whereas the clipping
  // bugs this test guards against move channels by >= 0.08.
  const auto check = [](const auto& rgb, const char* name) {
    using RGB = std::decay_t<decltype(rgb)>;
    const auto back = color_cast<RGB>(color_cast<luv_t>(rgb));
    EXPECT_NEAR(back.r(), rgb.r(), 5e-3f) << name;
    EXPECT_NEAR(back.g(), rgb.g(), 5e-3f) << name;
    EXPECT_NEAR(back.b(), rgb.b(), 5e-3f) << name;
  };
  check(rgbf_t(1.0f, 0.0f, 0.0f), "srgb red");
  check(rgbf_t(0.0f, 0.0f, 1.0f), "srgb blue");
  check(display_p3f_t(1.0f, 0.0f, 0.0f), "p3 red");
  check(rec2020_rgbf_t(1.0f, 0.0f, 0.0f), "rec2020 red");  // u* ≈ 272
  check(rec2020_rgbf_t(0.0f, 1.0f, 0.0f), "rec2020 green");
  check(prophoto_rgbf_t(1.0f, 0.0f, 0.0f), "prophoto red");  // u* ≈ 331
  check(prophoto_rgbf_t(0.0f, 1.0f, 0.0f), "prophoto green");
}

}  // namespace colorcpp::core::test
