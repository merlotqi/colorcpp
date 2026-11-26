/**
 * @file test_scrgb.cpp
 * @brief Tests for scRGB (IEC 61966-2-1): linear sRGB primaries with an
 *        extended range for rendering headroom.
 *
 * scRGB shares the sRGB primaries and D65 white, so its XYZ matrix is the
 * linear-sRGB matrix; the distinguishing features are the type (signal
 * semantics, negatives allowed) and the extended-range pass-through.
 * Following colour-science, the reference white is the diffuse white
 * (Y = 1) — the spec's absolute 80 cd/m^2 figure is not modeled here
 * (absolute anchoring lives in the PQ step, see kRelativeWhiteNits).
 */

#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

namespace colorcpp::core::test {

using colorcpp::operations::conversion::color_cast;

TEST(Scrgb, ConstructionAndAccessors) {
  // Extended values (negatives, >1) construct without clamping.
  scrgb_rgbf_t c(-0.1f, 0.5f, 1.2f);
  EXPECT_FLOAT_EQ(c.r(), -0.1f);
  EXPECT_FLOAT_EQ(c.g(), 0.5f);
  EXPECT_FLOAT_EQ(c.b(), 1.2f);
}

// Same primaries and white as linear sRGB, so both types must agree
// channel-for-channel on every XYZ input.
TEST(Scrgb, MatchesLinearSrgbMatrix) {
  const xyz_t samples[] = {
      xyz_t(0.95047f, 1.0f, 1.08883f),
      xyz_t(0.4124f, 0.2126f, 0.0193f),
      xyz_t(0.2f, 0.4f, 0.6f),
  };
  for (const auto& xyz : samples) {
    const auto sc = color_cast<scrgb_rgbf_t>(xyz);
    const auto lin = color_cast<linear_rgbf_t>(xyz);
    EXPECT_NEAR(sc.r(), lin.r(), 1e-6f) << "r";
    EXPECT_NEAR(sc.g(), lin.g(), 1e-6f) << "g";
    EXPECT_NEAR(sc.b(), lin.b(), 1e-6f) << "b";
  }
}

TEST(Scrgb, WhiteMapsToD65) {
  const auto sc = color_cast<scrgb_rgbf_t>(xyz_t(0.95047f, 1.0f, 1.08883f));
  EXPECT_NEAR(sc.r(), 1.0f, 1e-3f);
  EXPECT_NEAR(sc.g(), 1.0f, 1e-3f);
  EXPECT_NEAR(sc.b(), 1.0f, 1e-3f);
}

// The whole point of the type: negatives and >1 values survive a
// round trip through XYZ instead of being clamped away.
TEST(Scrgb, ExtendedValuesRoundTrip) {
  const scrgb_rgbf_t c(-0.1f, 0.5f, 1.2f);
  const auto back = color_cast<scrgb_rgbf_t>(color_cast<xyz_t>(c));
  EXPECT_NEAR(back.r(), -0.1f, 1e-4f);
  EXPECT_NEAR(back.g(), 0.5f, 1e-4f);
  EXPECT_NEAR(back.b(), 1.2f, 1e-4f);
}

TEST(Scrgb, GraphRoutesFromRgbf) {
  static_assert(operations::conversion::graph::minimal_conversion_cost<rgbf_t, scrgb_rgbf_t>() == 3);
  SUCCEED();
}

}  // namespace colorcpp::core::test
