/**
 * @file test_vision.cpp
 * @brief Unit tests for color vision deficiency simulation.
 */

#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>
#include <limits>

namespace colorcpp::algorithms::vision::test {

using namespace core;
using namespace vision;

// ============================================================================
// Protanopia tests
// ============================================================================

TEST(ProtanopiaTest, PureRedShifts) {
  rgb8_t red{255, 0, 0};
  auto result = simulate_protanopia(red);
  // Pure red should shift toward yellow/green for protanopia (L-cone absence)
  // Red will appear darker and shift toward greenish-yellow
  EXPECT_LT(result.r(), 255);
  EXPECT_GT(result.g(), 0);
}

TEST(ProtanopiaTest, PreservesAlpha) {
  rgba8_t c{255, 0, 0, 128};
  auto result = simulate_protanopia(c);
  EXPECT_EQ(result.a(), 128);
}

TEST(ProtanopiaTest, BlackStaysBlack) {
  rgb8_t black{0, 0, 0};
  auto result = simulate_protanopia(black);
  EXPECT_EQ(result.r(), 0);
  EXPECT_EQ(result.g(), 0);
  EXPECT_EQ(result.b(), 0);
}

TEST(ProtanopiaTest, WhiteStaysWhite) {
  rgb8_t white{255, 255, 255};
  auto result = simulate_protanopia(white);
  // Dichromacy simulation preserves the neutral axis: white stays white.
  EXPECT_NEAR(result.r(), 255, 2);
  EXPECT_NEAR(result.g(), 255, 2);
  EXPECT_NEAR(result.b(), 255, 2);
}

TEST(ProtanopiaTest, GrayStaysGray) {
  const rgbaf_t gray(0.5f, 0.5f, 0.5f, 1.0f);
  auto result = simulate_protanopia(gray);
  EXPECT_NEAR(result.r(), 0.5f, 0.005f);
  EXPECT_NEAR(result.g(), 0.5f, 0.005f);
  EXPECT_NEAR(result.b(), 0.5f, 0.005f);
}

// Reference value: Viénot/Brettel LMS simulation with Smith-Pokorny basis.
// Pure red protanopia appears as a dark ochre, matching published simulations.
TEST(ProtanopiaTest, RedBecomesDarkOchre) {
  rgb8_t red{255, 0, 0};
  auto result = simulate_protanopia(red);
  EXPECT_NEAR(result.r(), 94, 3);
  EXPECT_NEAR(result.g(), 94, 3);
  EXPECT_NEAR(result.b(), 13, 3);
}

TEST(ProtanopiaTest, GreenShifts) {
  rgb8_t green{0, 255, 0};
  auto result = simulate_protanopia(green);
  // Reference: green becomes a bright yellow under protanopia.
  EXPECT_NEAR(result.g(), 242, 3);
  EXPECT_NEAR(result.r(), 242, 3);
}

// ============================================================================
// Deuteranopia tests
// ============================================================================

TEST(DeuteranopiaTest, GreenShifts) {
  rgb8_t green{0, 255, 0};
  auto result = simulate_deuteranopia(green);
  // Pure green should shift for deuteranopia (M-cone absence)
  // Green will appear more yellowish
  EXPECT_GT(result.r(), 0);
  EXPECT_LT(result.g(), 255);
}

TEST(DeuteranopiaTest, PreservesAlpha) {
  rgba8_t c{0, 255, 0, 200};
  auto result = simulate_deuteranopia(c);
  EXPECT_EQ(result.a(), 200);
}

TEST(DeuteranopiaTest, BlackStaysBlack) {
  rgb8_t black{0, 0, 0};
  auto result = simulate_deuteranopia(black);
  EXPECT_EQ(result.r(), 0);
  EXPECT_EQ(result.g(), 0);
  EXPECT_EQ(result.b(), 0);
}

TEST(DeuteranopiaTest, GrayStaysGray) {
  const rgbaf_t gray(0.5f, 0.5f, 0.5f, 1.0f);
  auto result = simulate_deuteranopia(gray);
  EXPECT_NEAR(result.r(), 0.5f, 0.005f);
  EXPECT_NEAR(result.g(), 0.5f, 0.005f);
  EXPECT_NEAR(result.b(), 0.5f, 0.005f);
}

// Reference value: Viénot/Brettel LMS simulation with Smith-Pokorny basis.
// Pure red becomes a dark brownish yellow for deuteranopia.
TEST(DeuteranopiaTest, RedBecomesDarkYellow) {
  rgb8_t red{255, 0, 0};
  auto result = simulate_deuteranopia(red);
  EXPECT_NEAR(result.r(), 147, 3);
  EXPECT_NEAR(result.g(), 147, 3);
  EXPECT_NEAR(result.b(), 0, 3);
}

// ============================================================================
// Tritanopia tests
// ============================================================================

TEST(TritanopiaTest, BlueBecomesTeal) {
  rgb8_t blue{0, 0, 255};
  auto result = simulate_tritanopia(blue);
  // Reference value: pure blue becomes a teal/cyan under tritanopia.
  EXPECT_NEAR(result.r(), 0, 3);
  EXPECT_NEAR(result.g(), 105, 3);
  EXPECT_NEAR(result.b(), 105, 3);
}

TEST(TritanopiaTest, GrayStaysGray) {
  const rgbaf_t gray(0.5f, 0.5f, 0.5f, 1.0f);
  auto result = simulate_tritanopia(gray);
  EXPECT_NEAR(result.r(), 0.5f, 0.005f);
  EXPECT_NEAR(result.g(), 0.5f, 0.005f);
  EXPECT_NEAR(result.b(), 0.5f, 0.005f);
}

TEST(TritanopiaTest, PreservesAlpha) {
  rgba8_t c{0, 0, 255, 100};
  auto result = simulate_tritanopia(c);
  EXPECT_EQ(result.a(), 100);
}

TEST(TritanopiaTest, BlackStaysBlack) {
  rgb8_t black{0, 0, 0};
  auto result = simulate_tritanopia(black);
  EXPECT_EQ(result.r(), 0);
  EXPECT_EQ(result.g(), 0);
  EXPECT_EQ(result.b(), 0);
}

TEST(TritanopiaTest, YellowPreserved) {
  rgb8_t yellow{255, 255, 0};
  auto result = simulate_tritanopia(yellow);
  // Reference: yellow stays bright (slightly pinkish) for tritanopia.
  EXPECT_GT(result.r(), 200);
  EXPECT_GT(result.g(), 200);
}

// ============================================================================
// Accessibility helpers
// ============================================================================

TEST(HelpersTest, ProtanConfusedPairIsNotAccessible) {
  rgb8_t red{255, 0, 0};
  // Bronze shares red's (M, S) cone response in the Smith-Pokorny basis while
  // differing in L, so a protanope cannot distinguish the two.
  rgb8_t bronze{145, 85, 11};
  EXPECT_FALSE(is_accessible_for_protanopia(red, bronze));
}

TEST(HelpersTest, IdenticalColorsAreNotAccessible) {
  rgb8_t c{120, 60, 200};
  EXPECT_FALSE(is_accessible_for_all_cvd(c, c));
}

TEST(HelpersTest, BlackAndWhiteAreAccessible) {
  EXPECT_TRUE(is_accessible_for_all_cvd(rgb8_t{0, 0, 0}, rgb8_t{255, 255, 255}));
}

TEST(HelpersTest, AchromatopsiaEqualLuminanceIsNotAccessible) {
  rgb8_t red{255, 0, 0};           // Rec.709 luminance gamma-encodes to ~127
  rgb8_t mid_gray{127, 127, 127};  // same achromatic level
  EXPECT_FALSE(is_accessible_for_achromatopsia(red, mid_gray));
}

// ============================================================================
// Achromatopsia tests
// ============================================================================

TEST(AchromatopsiaTest, GrayscaleOutput) {
  rgb8_t red{255, 0, 0};
  auto result = simulate_achromatopsia(red);
  // Output should be grayscale (r=g=b)
  EXPECT_EQ(result.r(), result.g());
  EXPECT_EQ(result.g(), result.b());
}

TEST(AchromatopsiaTest, WhiteStaysWhite) {
  rgb8_t white{255, 255, 255};
  auto result = simulate_achromatopsia(white);
  // White should remain white (or very close)
  EXPECT_NEAR(result.r(), 255, 2);
  EXPECT_EQ(result.r(), result.g());
  EXPECT_EQ(result.g(), result.b());
}

TEST(AchromatopsiaTest, BlackStaysBlack) {
  rgb8_t black{0, 0, 0};
  auto result = simulate_achromatopsia(black);
  EXPECT_EQ(result.r(), 0);
  EXPECT_EQ(result.g(), 0);
  EXPECT_EQ(result.b(), 0);
}

TEST(AchromatopsiaTest, PreservesAlpha) {
  rgba8_t c{100, 150, 200, 180};
  auto result = simulate_achromatopsia(c);
  EXPECT_EQ(result.a(), 180);
  EXPECT_EQ(result.r(), result.g());
  EXPECT_EQ(result.g(), result.b());
}

TEST(AchromatopsiaTest, LuminanceWeighting) {
  // Using Rec. 709 luminance weights: Y = 0.2126*R + 0.7152*G + 0.0722*B
  // Then apply sRGB gamma encoding
  rgb8_t red{255, 0, 0};
  rgb8_t green{0, 255, 0};
  rgb8_t blue{0, 0, 255};

  auto gray_r = simulate_achromatopsia(red);
  auto gray_g = simulate_achromatopsia(green);
  auto gray_b = simulate_achromatopsia(blue);

  // Green has highest luminance weight (0.7152), should be brightest
  // Blue has lowest weight (0.0722), should be darkest
  EXPECT_GT(gray_g.r(), gray_r.r());
  EXPECT_GT(gray_r.r(), gray_b.r());

  // After sRGB gamma encoding, the values are approximately:
  // Red: 0.2126 -> 127, Green: 0.7152 -> 220, Blue: 0.0722 -> 76
  EXPECT_NEAR(gray_r.r(), 127, 2);
  EXPECT_NEAR(gray_g.r(), 220, 2);
  EXPECT_NEAR(gray_b.r(), 76, 2);
}

// ============================================================================
// Mixed type tests
// ============================================================================

TEST(VisionTest, MixedColorTypes) {
  rgb8_t rgb{255, 128, 64};
  rgbf_t rgbf{1.0f, 0.5f, 0.25f};

  // Should compile and run with different color types
  auto p1 = simulate_protanopia(rgb);
  auto p2 = simulate_protanopia(rgbf);

  EXPECT_GE(p1.r(), 0);
  EXPECT_GE(p2.r(), 0.0f);
  EXPECT_LE(p2.r(), 1.0f);
}

TEST(VisionTest, HSVTypes) {
  hsv_float_t hsv{0.0f, 1.0f, 1.0f};  // Pure red in HSV
  auto result = simulate_protanopia(hsv);
  // Should work with HSV type
  EXPECT_GE(result.h(), 0.0f);
  EXPECT_LE(result.h(), 360.0f);
}

// ============================================================================
// Machado et al. (severity)
// ============================================================================

TEST(MachadoTest, SeverityZeroIsIdentity) {
  rgb8_t green{0, 255, 0};
  auto d = simulate_deutan_machado(green, 0.0f);
  EXPECT_EQ(d.r(), green.r());
  EXPECT_EQ(d.g(), green.g());
  EXPECT_EQ(d.b(), green.b());
}

TEST(MachadoTest, FullSeverityChangesRed) {
  rgb8_t red{255, 0, 0};
  auto p = simulate_protan_machado(red, 1.0f);
  EXPECT_LT(p.r(), 255);
  EXPECT_GT(p.g(), 0);
}

TEST(MachadoTest, PreservesAlpha) {
  rgba8_t c{255, 0, 0, 100};
  auto out = simulate_tritan_machado(c, 0.5f);
  EXPECT_EQ(out.a(), 100);
}

// Reference values from the official Machado et al. (2009) supplementary
// severity-100 matrices:
//   protanomaly  row0 = { 0.152286,  1.052583, -0.204868}
//   deuteranomaly row0 = { 0.367322,  0.860646, -0.227968}
// Pure red linear (1,0,0) selects the first matrix column; results are
// gamma-encoded via sRGB. Verified against colour-science and colorspacious.
TEST(MachadoTest, ProtanRedMatchesPublishedMatrix) {
  const rgbaf_t red(1.0f, 0.0f, 0.0f, 1.0f);
  auto p = simulate_protan_machado(red, 1.0f);
  EXPECT_NEAR(p.r(), 0.4266f, 0.005f);
  EXPECT_NEAR(p.g(), 0.3727f, 0.005f);
  EXPECT_NEAR(p.b(), 0.0f, 0.005f);
}

TEST(MachadoTest, DeutanRedMatchesPublishedMatrix) {
  const rgbaf_t red(1.0f, 0.0f, 0.0f, 1.0f);
  auto d = simulate_deutan_machado(red, 1.0f);
  EXPECT_NEAR(d.r(), 0.6401f, 0.005f);
  EXPECT_NEAR(d.g(), 0.5658f, 0.005f);
  EXPECT_NEAR(d.b(), 0.0f, 0.005f);
}

// Every published Machado matrix preserves the neutral axis (each row sums
// to 1), so grays must map to gray at any severity.
TEST(MachadoTest, GrayPreservedAtFullSeverity) {
  const rgbaf_t gray(0.5f, 0.5f, 0.5f, 1.0f);
  for (float s : {0.0f, 0.37f, 1.0f}) {
    for (const auto& out :
         {simulate_protan_machado(gray, s), simulate_deutan_machado(gray, s), simulate_tritan_machado(gray, s)}) {
      EXPECT_NEAR(out.r(), 0.5f, 0.005f);
      EXPECT_NEAR(out.g(), 0.5f, 0.005f);
      EXPECT_NEAR(out.b(), 0.5f, 0.005f);
    }
  }
}

// The published Machado model provides precomputed matrices at 10% severity
// steps; intermediate severities interpolate between adjacent tables. At
// severity 0.5 the official protanomaly matrix row0 = {0.458064, 0.679578,
// -0.137642}, which differs from the naive (1-s)·I + s·M blend.
TEST(MachadoTest, SeverityHalfUsesPublishedMatrices) {
  const rgbaf_t red(1.0f, 0.0f, 0.0f, 1.0f);
  auto p = simulate_protan_machado(red, 0.5f);
  EXPECT_NEAR(p.r(), 0.7070f, 0.01f);
  EXPECT_NEAR(p.g(), 0.3368f, 0.01f);
  EXPECT_NEAR(p.b(), 0.0f, 0.01f);
}

// Severity 0.55 sits between the published 50% and 60% protanomaly tables;
// expected value is the midpoint interpolation of the two red columns,
// gamma-encoded. Guards against snapping to the nearest grid point.
TEST(MachadoTest, SeverityMidStepInterpolatesTables) {
  const rgbaf_t red(1.0f, 0.0f, 0.0f, 1.0f);
  auto p = simulate_protan_machado(red, 0.55f);
  EXPECT_NEAR(p.r(), 0.6813f, 0.01f);
  EXPECT_NEAR(p.g(), 0.3435f, 0.01f);
  EXPECT_NEAR(p.b(), 0.0f, 0.01f);
}

TEST(MachadoTest, NaNSeverityReturnsUnchanged) {
  const rgb8_t c{12, 34, 56};
  const float nan = std::numeric_limits<float>::quiet_NaN();
  EXPECT_EQ(simulate_protan_machado(c, nan), c);
  EXPECT_EQ(simulate_deutan_machado(c, nan), c);
  EXPECT_EQ(simulate_tritan_machado(c, nan), c);
}

}  // namespace colorcpp::algorithms::vision::test
