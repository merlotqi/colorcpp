/**
 * @file test_dci_p3.cpp
 * @brief Tests for DCI-P3 (theatrical): P3 primaries, D62 white, gamma 2.6.
 *
 * Distinct from Display P3 (same primaries, D65 white, sRGB transfer):
 * DCI-P3 carries the D62 white point, which is Bradford-adapted into the
 * library's D65 XYZ hub. Reference anchors come from an independent
 * double-precision evaluation: primaries -> XYZ(D62) scaled to the D62
 * white, Bradford D62->D65 adaptation (library matrices), gamma 2.6.
 */

#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

namespace colorcpp::core::test {

using colorcpp::operations::conversion::color_cast;

TEST(DciP3, ConstructionAndAccessors) {
  dci_p3_rgbf_t c(0.5f, 0.25f, 0.75f);
  EXPECT_FLOAT_EQ(c.r(), 0.5f);
  EXPECT_FLOAT_EQ(c.g(), 0.25f);
  EXPECT_FLOAT_EQ(c.b(), 0.75f);
}

// The D62 white maps exactly onto the library's D65 reference white after
// adaptation: signal (1, 1, 1) <-> XYZ white.
TEST(DciP3, WhiteMapsExactlyToD65) {
  const auto signal = color_cast<dci_p3_rgbf_t>(xyz_t(0.95047f, 1.0f, 1.08883f));
  EXPECT_NEAR(signal.r(), 1.0f, 1e-4f);
  EXPECT_NEAR(signal.g(), 1.0f, 1e-4f);
  EXPECT_NEAR(signal.b(), 1.0f, 1e-4f);

  const auto xyz = color_cast<xyz_t>(dci_p3_rgbf_t(1.0f, 1.0f, 1.0f));
  EXPECT_NEAR(xyz.x(), 0.95047f, 1e-4f);
  EXPECT_NEAR(xyz.y(), 1.0f, 1e-4f);
  EXPECT_NEAR(xyz.z(), 1.08883f, 1e-4f);
}

// sRGB primaries XYZ through the adapted D62 matrix + gamma 2.6 — pins
// matrix, adaptation and transfer function together.
TEST(DciP3, PrimariesAnchor) {
  const auto red = color_cast<dci_p3_rgbf_t>(xyz_t(0.4124564f, 0.3575761f, 0.1804375f));
  EXPECT_NEAR(red.r(), 0.8431116f, 1e-4f);
  EXPECT_NEAR(red.g(), 0.6232425f, 1e-4f);
  EXPECT_NEAR(red.b(), 0.4940463f, 1e-4f);

  const auto blue = color_cast<dci_p3_rgbf_t>(xyz_t(0.1804375f, 0.0721750f, 0.9503041f));
  EXPECT_NEAR(blue.r(), 0.0996551f, 1e-4f);
  EXPECT_NEAR(blue.g(), 0.1153827f, 1e-4f);
  EXPECT_NEAR(blue.b(), 0.9652587f, 1e-4f);
}

// Signal -> XYZ through gamma 2.6 (encoded 0.5 = linear 0.1649385).
TEST(DciP3, GammaAnchors) {
  const auto gray = color_cast<xyz_t>(dci_p3_rgbf_t(0.5f, 0.5f, 0.5f));
  EXPECT_NEAR(gray.x(), 0.1567691f, 1e-4f);
  EXPECT_NEAR(gray.y(), 0.1649385f, 1e-4f);
  EXPECT_NEAR(gray.z(), 0.1795900f, 1e-4f);

  const auto mixed = color_cast<xyz_t>(dci_p3_rgbf_t(0.5f, 0.25f, 0.75f));
  EXPECT_NEAR(mixed.x(), 0.1762767f, 1e-4f);
  EXPECT_NEAR(mixed.y(), 0.0906131f, 1e-4f);
  EXPECT_NEAR(mixed.z(), 0.4943475f, 1e-4f);
}

// Round trips stay inside the sRGB gamut; out-of-gamut XYZ clamps at the
// non-extended signal boundary by design (issue #76 policy).
TEST(DciP3, RoundTrip) {
  const xyz_t colors[] = {
      xyz_t(0.95047f, 1.0f, 1.08883f),
      xyz_t(0.4124f, 0.2126f, 0.0193f),
      xyz_t(0.3f, 0.4f, 0.5f),
  };
  for (const auto& xyz : colors) {
    const auto back = color_cast<xyz_t>(color_cast<dci_p3_rgbf_t>(xyz));
    EXPECT_NEAR(back.x(), xyz.x(), 1e-4f) << "x";
    EXPECT_NEAR(back.y(), xyz.y(), 1e-4f) << "y";
    EXPECT_NEAR(back.z(), xyz.z(), 1e-4f) << "z";
  }
}

TEST(DciP3, OutOfGamutClampsAtBoundary) {
  // XYZ (0.2, 0.4, 0.6) maps to a negative linear DCI-P3 red.
  const auto c = color_cast<dci_p3_rgbf_t>(xyz_t(0.2f, 0.4f, 0.6f));
  EXPECT_LE(c.r(), 0.0f);
}

}  // namespace colorcpp::core::test
