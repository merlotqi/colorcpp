/**
 * @file test_aces.cpp
 * @brief Reference tests for ACES2065-1 (AP0), ACEScg (AP1) and ACEScct.
 *
 * Reference anchors come from independent double-precision evaluation:
 *   - AP0 -> XYZ as published in SMPTE ST 2065-1 (white verified against
 *     the ACES D60 chromaticity 0.32168, 0.33767);
 *   - AP1 -> XYZ via the standard normalised-primary construction with the
 *     ACES white (reproduces the published matrix
 *     [0.6624541811, 0.1340042065, 0.1561876870; ...] digit for digit);
 *   - both Bradford-adapted D60 -> D65 into the library's D65 hub, so
 *     signal (1, 1, 1) maps exactly onto the reference white;
 *   - ACEScct per Academy S-2016-003: toe A*x + B below
 *     X_BRK = 0.0078125 (A = 10.5402377416545, B = 0.0729055341958355),
 *     else (log2(x) + 9.72) / 17.52 — continuous at Y_BRK = 0.155251141552511.
 */

#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

namespace colorcpp::core::test {

using colorcpp::operations::conversion::color_cast;

TEST(Aces, ConstructionAndAccessors) {
  aces2065_rgbf_t c(0.5f, 0.25f, 0.75f);
  EXPECT_FLOAT_EQ(c.r(), 0.5f);
  EXPECT_FLOAT_EQ(c.g(), 0.25f);
  EXPECT_FLOAT_EQ(c.b(), 0.75f);
  acescct_rgbaf_t d(0.4f, 0.5f, 0.6f, 0.3f);
  EXPECT_FLOAT_EQ(d.a(), 0.3f);
}

// The D60 white maps exactly onto the library's D65 reference white after
// adaptation — the same invariant DCI-P3 pins.
TEST(Aces, WhiteMapsExactlyToD65) {
  const xyz_t white(0.95047f, 1.0f, 1.08883f);
  const auto base = color_cast<aces2065_rgbf_t>(white);
  EXPECT_NEAR(base.r(), 1.0f, 1e-4f);
  EXPECT_NEAR(base.g(), 1.0f, 1e-4f);
  EXPECT_NEAR(base.b(), 1.0f, 1e-4f);

  const auto cg = color_cast<acescg_rgbf_t>(white);
  EXPECT_NEAR(cg.r(), 1.0f, 1e-4f);
  EXPECT_NEAR(cg.g(), 1.0f, 1e-4f);
  EXPECT_NEAR(cg.b(), 1.0f, 1e-4f);

  const auto back = color_cast<xyz_t>(aces2065_rgbf_t(1.0f, 1.0f, 1.0f));
  EXPECT_NEAR(back.x(), 0.95047f, 1e-4f);
  EXPECT_NEAR(back.y(), 1.0f, 1e-4f);
  EXPECT_NEAR(back.z(), 1.08883f, 1e-4f);
}

// sRGB primaries XYZ through the adapted AP0/AP1 matrices (XYZ -> ACES
// direction). XYZ (1, 0, 0) maps to a negative AP0 green (-0.49209) —
// pinning the extended-range pass-through of the linear ACES types.
TEST(Aces, PrimariesAnchors) {
  const xyz_t red(0.4124564f, 0.3575761f, 0.1804375f);
  const auto b2065 = color_cast<aces2065_rgbf_t>(red);
  EXPECT_NEAR(b2065.r(), 0.4380661f, 1e-4f);
  EXPECT_NEAR(b2065.g(), 0.3027704f, 1e-4f);
  EXPECT_NEAR(b2065.b(), 0.1658895f, 1e-4f);

  const auto cg = color_cast<acescg_rgbf_t>(red);
  EXPECT_NEAR(cg.r(), 0.5285635f, 1e-4f);
  EXPECT_NEAR(cg.g(), 0.3060568f, 1e-4f);
  EXPECT_NEAR(cg.b(), 0.1673272f, 1e-4f);

  const xyz_t green(0.3575761f, 0.7151522f, 0.1191920f);
  const auto g2065 = color_cast<aces2065_rgbf_t>(green);
  EXPECT_NEAR(g2065.r(), 0.3829530f, 1e-4f);
  EXPECT_NEAR(g2065.g(), 0.8134331f, 1e-4f);
  EXPECT_NEAR(g2065.b(), 0.1115616f, 1e-4f);

  // Negative AP0 component survives (extended range, issue #76).
  const auto pure_y = color_cast<aces2065_rgbf_t>(xyz_t(1.0f, 0.0f, 0.0f));
  EXPECT_NEAR(pure_y.g(), -0.4920891f, 1e-4f);
}

TEST(Aces, RoundTrip) {
  const xyz_t colors[] = {
      xyz_t(0.95047f, 1.0f, 1.08883f),
      xyz_t(0.4124f, 0.2126f, 0.0193f),
      xyz_t(0.3f, 0.4f, 0.5f),
  };
  for (const auto& xyz : colors) {
    const auto back2065 = color_cast<xyz_t>(color_cast<aces2065_rgbf_t>(xyz));
    EXPECT_NEAR(back2065.x(), xyz.x(), 1e-4f) << "2065 x";
    EXPECT_NEAR(back2065.y(), xyz.y(), 1e-4f) << "2065 y";
    EXPECT_NEAR(back2065.z(), xyz.z(), 1e-4f) << "2065 z";

    const auto backcg = color_cast<xyz_t>(color_cast<acescg_rgbf_t>(xyz));
    EXPECT_NEAR(backcg.x(), xyz.x(), 1e-4f) << "cg x";
    EXPECT_NEAR(backcg.y(), xyz.y(), 1e-4f) << "cg y";
    EXPECT_NEAR(backcg.z(), xyz.z(), 1e-4f) << "cg z";
  }
}

// ACEScct (S-2016-003): toe and log branches against the spec constants.
TEST(AcesCct, TransferFunctionAnchors) {
  const auto enc = [](float linear) { return color_cast<acescct_rgbf_t>(acescg_rgbf_t(linear, linear, linear)).r(); };
  EXPECT_NEAR(enc(0.0f), 0.0729055342f, 1e-6f);        // toe: A*0 + B
  EXPECT_NEAR(enc(0.001f), 0.0834457719f, 1e-6f);      // toe
  EXPECT_NEAR(enc(0.0078125f), 0.1552511416f, 1e-6f);  // breakpoint, continuous
  EXPECT_NEAR(enc(0.18f), 0.4135884025f, 1e-6f);       // log branch (spec example)
  EXPECT_NEAR(enc(1.0f), 0.5547945205f, 1e-6f);

  const auto dec = [](float signal) { return color_cast<acescg_rgbf_t>(acescct_rgbf_t(signal, signal, signal)).r(); };
  EXPECT_NEAR(dec(0.08344577f), 0.001f, 1e-6f);
  EXPECT_NEAR(dec(0.155251141552511f), 0.0078125f, 1e-6f);
  EXPECT_NEAR(dec(0.413588402492442f), 0.18f, 1e-6f);  // spec example inverse
}

TEST(AcesCct, RoundTripThroughAcescg) {
  for (float v : {0.001f, 0.05f, 0.18f, 0.5f, 1.0f}) {
    const auto back = color_cast<acescg_rgbf_t>(color_cast<acescct_rgbf_t>(acescg_rgbf_t(v, v, v)));
    EXPECT_NEAR(back.r(), v, 1e-5f) << "v=" << v;
  }
}

TEST(Aces, GraphRoutesFromRgbf) {
  static_assert(operations::conversion::graph::minimal_conversion_cost<rgbf_t, aces2065_rgbf_t>() == 3);
  static_assert(operations::conversion::graph::minimal_conversion_cost<acescct_rgbf_t, xyz_t>() == 2);
  SUCCEED();
}

}  // namespace colorcpp::core::test
