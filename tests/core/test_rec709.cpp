/**
 * @file test_rec709.cpp
 * @brief Tests for Rec.709 signal encoding (BT.1886 pure gamma 2.4).
 *
 * Rec.709 shares its primaries and D65 white with sRGB — only the
 * transfer function differs (pure gamma 2.4 per BT.1886 / CSS Color 4
 * section 10.8, the same curve Rec.2020 uses). Anchors: independent
 * double-precision evaluation of v^(1/2.4) and v^2.4.
 */

#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

namespace colorcpp::core::test {

using colorcpp::operations::conversion::color_cast;

TEST(Rec709, ConstructionAndAccessors) {
  rec709_rgbf_t c(0.5f, 0.25f, 0.75f);
  EXPECT_FLOAT_EQ(c.r(), 0.5f);
  EXPECT_FLOAT_EQ(c.g(), 0.25f);
  EXPECT_FLOAT_EQ(c.b(), 0.75f);
}

// Gamma 2.4 anchors (BT.1886): linear 0.5 encodes to 0.7491535 and the
// signal 0.25 decodes to 0.0358968 — not the sRGB piecewise curve.
TEST(Rec709, GammaAnchors) {
  const auto enc = [](float linear) { return color_cast<rec709_rgbf_t>(linear_rgbf_t(linear, linear, linear)).r(); };
  EXPECT_NEAR(enc(0.0f), 0.0f, 1e-7f);
  EXPECT_NEAR(enc(0.25f), 0.5612310f, 1e-5f);
  EXPECT_NEAR(enc(0.5f), 0.7491535f, 1e-5f);
  EXPECT_NEAR(enc(0.75f), 0.8870379f, 1e-5f);
  EXPECT_NEAR(enc(1.0f), 1.0f, 1e-6f);

  const auto dec = [](float signal) { return color_cast<linear_rgbf_t>(rec709_rgbf_t(signal, signal, signal)).r(); };
  EXPECT_NEAR(dec(0.25f), 0.0358968f, 1e-5f);
  EXPECT_NEAR(dec(0.5f), 0.1894646f, 1e-5f);
  EXPECT_NEAR(dec(0.75f), 0.5013569f, 1e-5f);
}

TEST(Rec709, WhiteBlackIdentity) {
  const auto white = color_cast<rec709_rgbf_t>(linear_rgbf_t(1.0f, 1.0f, 1.0f));
  EXPECT_NEAR(white.r(), 1.0f, 1e-6f);
  EXPECT_NEAR(white.g(), 1.0f, 1e-6f);
  EXPECT_NEAR(white.b(), 1.0f, 1e-6f);
  const auto black = color_cast<rec709_rgbf_t>(linear_rgbf_t(0.0f, 0.0f, 0.0f));
  EXPECT_NEAR(black.r(), 0.0f, 1e-7f);
}

// Rec.709 primaries are the sRGB primaries: sRGB red must encode to the
// Rec.709 red primary (1, 0, 0).
TEST(Rec709, SrgbRedPrimary) {
  const auto c = color_cast<rec709_rgbf_t>(xyz_t(0.4124f, 0.2126f, 0.0193f));
  EXPECT_NEAR(c.r(), 1.0f, 2e-3f);
  EXPECT_NEAR(c.g(), 0.0f, 2e-3f);
  EXPECT_NEAR(c.b(), 0.0f, 2e-3f);
}

// Round trips stay inside the sRGB gamut: a display encoding is a
// non-extended type, so out-of-gamut XYZ clamps at the signal boundary
// by design (issue #76 policy — see OutOfGamutClampsAtBoundary).
TEST(Rec709, RoundTrip) {
  const xyz_t colors[] = {
      xyz_t(0.95047f, 1.0f, 1.08883f),
      xyz_t(0.4124f, 0.2126f, 0.0193f),
      xyz_t(0.3f, 0.4f, 0.5f),
  };
  for (const auto& xyz : colors) {
    const auto back = color_cast<xyz_t>(color_cast<rec709_rgbf_t>(xyz));
    EXPECT_NEAR(back.x(), xyz.x(), 1e-4f) << "x";
    EXPECT_NEAR(back.y(), xyz.y(), 1e-4f) << "y";
    EXPECT_NEAR(back.z(), xyz.z(), 1e-4f) << "z";
  }
}

// The clamp policy is part of the contract: XYZ (0.2, 0.4, 0.6) has a
// negative linear-sRGB red, and the non-extended signal type must clamp
// it to 0 rather than carry it (issue #76).
TEST(Rec709, OutOfGamutClampsAtBoundary) {
  const auto c = color_cast<rec709_rgbf_t>(xyz_t(0.2f, 0.4f, 0.6f));
  EXPECT_LE(c.r(), 0.0f);
}

}  // namespace colorcpp::core::test
