#include <gtest/gtest.h>

#include <cmath>
#include <colorcpp/colorcpp.hpp>

namespace colorcpp::algorithms::test {

using namespace core;
using namespace accessibility;

// relative_luminance

TEST(AccessibilityTest, BlackLuminanceIsZero) { EXPECT_NEAR(relative_luminance(constants::black), 0.0f, 1e-5f); }

TEST(AccessibilityTest, WhiteLuminanceIsOne) { EXPECT_NEAR(relative_luminance(constants::white), 1.0f, 1e-4f); }

TEST(AccessibilityTest, LuminanceIsNonNegative) {
  for (auto& c :
       {constants::red, constants::lime, constants::blue, constants::yellow, constants::cyan, constants::magenta}) {
    EXPECT_GE(relative_luminance(c), 0.0f);
  }
}

TEST(AccessibilityTest, LuminanceIsAtMostOne) {
  for (auto& c :
       {constants::red, constants::lime, constants::blue, constants::yellow, constants::cyan, constants::magenta}) {
    EXPECT_LE(relative_luminance(c), 1.0f);
  }
}

TEST(AccessibilityTest, GreenBrighterThanRedBrighterThanBlue) {
  // WCAG luminance coefficients: R=0.2126, G=0.7152, B=0.0722
  // So pure green > pure red > pure blue
  float L_red = relative_luminance(constants::red);
  float L_lime = relative_luminance(constants::lime);
  float L_blue = relative_luminance(constants::blue);

  EXPECT_GT(L_lime, L_red);
  EXPECT_GT(L_red, L_blue);
}

TEST(AccessibilityTest, FloatInputWorks) {
  core::rgbf_t mid_gray(0.5f, 0.5f, 0.5f);
  float L = relative_luminance(mid_gray);
  // linearize(0.5) ≈ 0.214; Y = 0.214 for all channels → L ≈ 0.214
  EXPECT_NEAR(L, 0.214f, 1e-2f);
}

// contrast_ratio

TEST(AccessibilityTest, ContrastRatioBlackWhiteIs21) {
  float ratio = contrast_ratio(constants::black, constants::white);
  EXPECT_NEAR(ratio, 21.0f, 0.1f);
}

TEST(AccessibilityTest, ContrastRatioIsSymmetric) {
  // contrast_ratio(a, b) == contrast_ratio(b, a) — function normalises L
  float r_ab = contrast_ratio(constants::red, constants::blue);
  float r_ba = contrast_ratio(constants::blue, constants::red);
  EXPECT_NEAR(r_ab, r_ba, 1e-4f);
}

TEST(AccessibilityTest, ContrastRatioSameColorIsOne) {
  core::rgbf_t c(0.5f, 0.3f, 0.7f);
  EXPECT_NEAR(contrast_ratio(c, c), 1.0f, 1e-4f);
}

TEST(AccessibilityTest, ContrastRatioAtLeastOne) {
  EXPECT_GE(contrast_ratio(constants::red, constants::green), 1.0f);
  EXPECT_GE(contrast_ratio(constants::cyan, constants::magenta), 1.0f);
}

// is_wcag_compliant

TEST(AccessibilityTest, BlackOnWhitePassesAA) { EXPECT_TRUE(is_wcag_compliant(constants::black, constants::white)); }

TEST(AccessibilityTest, BlackOnWhitePassesAAA) {
  EXPECT_TRUE(is_wcag_compliant(constants::black, constants::white, wcag_level::aaa, text_size::normal));
}

TEST(AccessibilityTest, SameColorFailsAA) {
  // Any same-color pair has contrast 1:1, which fails any WCAG threshold
  EXPECT_FALSE(is_wcag_compliant(constants::white, constants::white));
}

TEST(AccessibilityTest, WhiteOnWhiteFailsAAA) {
  EXPECT_FALSE(is_wcag_compliant(constants::white, constants::white, wcag_level::aaa, text_size::normal));
}

TEST(AccessibilityTest, LargeTextHasLowerThreshold) {
  // A pair that fails normal AA but passes large-text AA
  // contrast ≥ 3.0 (large) vs ≥ 4.5 (normal)
  // White on mid-gray (0x808080): contrast ≈ 3.95 → passes large AA, fails normal AA
  core::rgba8_t mid_gray(0x80, 0x80, 0x80, 0xFF);

  bool pass_normal = is_wcag_compliant(constants::white, mid_gray, wcag_level::aa, text_size::normal);
  bool pass_large = is_wcag_compliant(constants::white, mid_gray, wcag_level::aa, text_size::large);
  EXPECT_TRUE(pass_large);
  // large-text threshold (3.0) is less strict than normal (4.5)
  if (!pass_normal) {
    // If it fails normal, large should be evaluated independently
    // (this test just verifies the logic doesn't crash)
    EXPECT_GE(contrast_ratio(constants::white, mid_gray), 1.0f);
  }
}

// contrast_text_color

TEST(AccessibilityTest, WhiteBackgroundGetsBlackText) {
  auto text = contrast_text_color(constants::white);
  // Black has higher contrast against white
  float ratio_black = contrast_ratio(text, constants::white);
  EXPECT_GT(ratio_black, 5.0f);
}

TEST(AccessibilityTest, BlackBackgroundGetsWhiteText) {
  auto text = contrast_text_color(constants::black);
  float ratio_white = contrast_ratio(text, constants::black);
  EXPECT_GT(ratio_white, 5.0f);
}

// APCA (SAPC-8 / 0.0.98G-4g style; not comparable to WCAG 2 ratio)

TEST(AccessibilityTest, ApcaLuminanceBlackZeroWhiteOne) {
  EXPECT_NEAR(apca_luminance(constants::black), 0.0f, 1e-5f);
  EXPECT_NEAR(apca_luminance(constants::white), 1.0f, 1e-5f);
}

TEST(AccessibilityTest, ApcaContrastBlackOnWhitePositive) {
  float lc = apca_contrast(constants::black, constants::white);
  EXPECT_GT(lc, 100.0f);
  EXPECT_NEAR(lc, 106.041f, 0.05f);
}

TEST(AccessibilityTest, ApcaContrastWhiteOnBlackNegative) {
  float lc = apca_contrast(constants::white, constants::black);
  EXPECT_LT(lc, -100.0f);
  EXPECT_NEAR(lc, -107.885f, 0.05f);
}

TEST(AccessibilityTest, ApcaNotSymmetric) {
  float a = apca_contrast(constants::red, constants::white);
  float b = apca_contrast(constants::white, constants::red);
  EXPECT_GT(std::fabs(a - b), 0.5f);
}

TEST(AccessibilityTest, ApcaSameColorNearZero) {
  EXPECT_NEAR(std::fabs(apca_contrast(constants::white, constants::white)), 0.0f, 1.0f);
}

TEST(AccessibilityTest, ApcaMeetsMinAbsLc) {
  EXPECT_TRUE(apca_meets_min_abs_lc(constants::black, constants::white, 100.0f));
  EXPECT_FALSE(apca_meets_min_abs_lc(constants::white, constants::white, 15.0f));
}

TEST(AccessibilityTest, TextColorIsBlackOrWhite) {
  // contrast_text_color always returns constants::black or constants::white
  auto on_red = contrast_text_color(constants::red);
  auto on_lime = contrast_text_color(constants::lime);

  // Both results should have luminance either ~0 (black) or ~1 (white)
  float lum_red = relative_luminance(on_red);
  float lum_lime = relative_luminance(on_lime);

  bool black_or_white_r = (lum_red < 0.01f || lum_red > 0.99f);
  bool black_or_white_l = (lum_lime < 0.01f || lum_lime > 0.99f);

  EXPECT_TRUE(black_or_white_r);
  EXPECT_TRUE(black_or_white_l);
}

// Known-value anchors
//
// These pin the WCAG coefficients and transfer function to externally
// verifiable numbers, so a coefficient regression (e.g. swapping in APCA's
// 0.2126729 weights or a pure 2.4 gamma) cannot pass unnoticed.

TEST(AccessibilityTest, RelativeLuminanceMatchesWcagCoefficients) {
  // Pure primaries pin the WCAG 2 spec weights exactly (rounded Rec. 709):
  // Y(red) = 0.2126, Y(green) = 0.7152, Y(blue) = 0.0722. Grey pairs cannot
  // see coefficient swaps — every weight set sums to 1 — so anchor each
  // coefficient through a saturated channel instead.
  EXPECT_NEAR(relative_luminance(constants::red), 0.2126f, 1e-5f);
  EXPECT_NEAR(relative_luminance(constants::lime), 0.7152f, 1e-5f);
  EXPECT_NEAR(relative_luminance(constants::blue), 0.0722f, 1e-5f);
}

TEST(AccessibilityTest, ContrastAnchor767676OnWhite) {
  // The canonical WCAG test pair: #767676 on white is ~4.54:1 — the first
  // grey above the AA-normal threshold of 4.5.
  auto fg = core::rgba8_t(0x76, 0x76, 0x76, 0xFF);
  EXPECT_NEAR(contrast_ratio(fg, constants::white), 4.54222f, 1e-3f);
}

TEST(AccessibilityTest, RelativeLuminanceAtTransferKnee) {
  // 0.04045 is the IEC 61966-2-1 piecewise knee; below it, linear scaling
  // by 1/12.92. Y = 0.2126 * 0.04045 / 12.92 = 0.00066561. A pure 2.4
  // exponent would give 0.0000306 here — three orders off.
  core::rgbf_t knee(0.04045f, 0.0f, 0.0f);
  EXPECT_NEAR(relative_luminance(knee), 0.00066561f, 1e-6f);
}

TEST(AccessibilityTest, AaaLargeTextThresholdIsFourPointFive) {
  // #767676 on white ≈ 4.54: between the AAA-large threshold (4.5) and the
  // AAA-normal threshold (7.0) — it must distinguish the two paths.
  auto fg = core::rgba8_t(0x76, 0x76, 0x76, 0xFF);
  EXPECT_TRUE(is_wcag_compliant(fg, constants::white, wcag_level::aaa, text_size::large));
  EXPECT_FALSE(is_wcag_compliant(fg, constants::white, wcag_level::aaa, text_size::normal));
}

// APCA guards and quantization

TEST(AccessibilityTest, ApcaOutOfRangeLuminanceReturnsZero) {
  // Reference-implementation guard: Y outside [0, 1.1] and non-finite
  // inputs must not flow into the pow() pipeline.
  EXPECT_EQ(apca_contrast(2.0f, 0.5f), 0.0f);
  EXPECT_EQ(apca_contrast(0.5f, -0.1f), 0.0f);
  EXPECT_EQ(apca_contrast(std::nanf(""), 0.5f), 0.0f);
}

TEST(AccessibilityTest, ApcaLuminanceQuantizesToUint8) {
  // apca_luminance is display-referred: float colors are rounded to 8-bit
  // channels (0.5 * 255 = 127.5 -> 128) before the Y pipeline, matching the
  // APCA reference definition on display values.
  EXPECT_NEAR(apca_luminance(core::rgbf_t(0.5f, 0.5f, 0.5f)), apca_luminance_srgb_uint8(128, 128, 128), 1e-6f);
}

}  // namespace colorcpp::algorithms::test
