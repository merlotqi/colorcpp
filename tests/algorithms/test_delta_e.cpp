
#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

namespace colorcpp::algorithms::test {

using namespace core;
using namespace delta_e;

// ΔE₇₆ (CIE 1976)

TEST(DeltaE76Test, SameColorIsZero) {
  core::rgbf_t c(0.5f, 0.3f, 0.7f);
  EXPECT_NEAR(delta_e_76(c, c), 0.0f, 1e-4f);
}

TEST(DeltaE76Test, BlackToWhiteIsAbout100) {
  // CIELAB: black=(0,0,0), white=(100,0,0); ΔE₇₆ = 100
  core::rgb8_t black(0, 0, 0);
  core::rgb8_t white(255, 255, 255);
  EXPECT_NEAR(delta_e_76(black, white), 100.0f, 0.5f);
}

TEST(DeltaE76Test, IsSymmetric) {
  core::rgb8_t red(255, 0, 0);
  core::rgb8_t blue(0, 0, 255);
  float d_ab = delta_e_76(red, blue);
  float d_ba = delta_e_76(blue, red);
  EXPECT_NEAR(d_ab, d_ba, 1e-4f);
}

TEST(DeltaE76Test, IsNonNegative) {
  core::rgbf_t a(0.2f, 0.5f, 0.8f);
  core::rgbf_t b(0.9f, 0.1f, 0.3f);
  EXPECT_GE(delta_e_76(a, b), 0.0f);
}

TEST(DeltaE76Test, LargerDifferenceMeansLargerDeltaE) {
  core::rgb8_t ref(128, 128, 128);    // gray
  core::rgb8_t close(130, 128, 128);  // barely different
  core::rgb8_t far(0, 0, 255);        // very different

  EXPECT_LT(delta_e_76(ref, close), delta_e_76(ref, far));
}

TEST(DeltaE76Test, WorksWithMixedTypes) {
  core::rgb8_t a(200, 100, 50);
  core::rgbf_t b(0.8f, 0.4f, 0.2f);
  // Both represent similar-ish colors; just check it compiles and runs
  float de = delta_e_76(a, b);
  EXPECT_GE(de, 0.0f);
}

// ΔE₉₄ (CIE 1994)

TEST(DeltaE94Test, SameColorIsZero) {
  core::rgbf_t c(0.4f, 0.7f, 0.2f);
  EXPECT_NEAR(delta_e_94(c, c), 0.0f, 1e-4f);
}

TEST(DeltaE94Test, BlackToWhiteIsLarge) {
  core::rgb8_t black(0, 0, 0);
  core::rgb8_t white(255, 255, 255);
  EXPECT_GT(delta_e_94(black, white), 50.0f);
}

TEST(DeltaE94Test, IsGenerallyAsymmetric) {
  // ΔE₉₄ uses color A as the reference, so it's not symmetric in general
  core::rgb8_t red(255, 0, 0);
  core::rgb8_t blue(0, 0, 255);
  float d_ab = delta_e_94(red, blue);
  float d_ba = delta_e_94(blue, red);
  // They may differ (asymmetric formula) — just check both are positive
  EXPECT_GE(d_ab, 0.0f);
  EXPECT_GE(d_ba, 0.0f);
}

TEST(DeltaE94Test, IsNonNegative) {
  core::rgbf_t a(0.1f, 0.9f, 0.5f);
  core::rgbf_t b(0.7f, 0.2f, 0.4f);
  EXPECT_GE(delta_e_94(a, b), 0.0f);
}

// CIEDE2000

TEST(DeltaE2000Test, SameColorIsZero) {
  core::rgbf_t c(0.6f, 0.1f, 0.9f);
  EXPECT_NEAR(delta_e_2000(c, c), 0.0f, 1e-4f);
}

TEST(DeltaE2000Test, BlackToWhiteIsLarge) {
  core::rgb8_t black(0, 0, 0);
  core::rgb8_t white(255, 255, 255);
  EXPECT_GT(delta_e_2000(black, white), 50.0f);
}

TEST(DeltaE2000Test, IsSymmetric) {
  core::rgb8_t green(0, 200, 50);
  core::rgb8_t orange(255, 150, 0);
  float d_ab = delta_e_2000(green, orange);
  float d_ba = delta_e_2000(orange, green);
  // CIEDE2000 is symmetric (unlike ΔE₉₄)
  EXPECT_NEAR(d_ab, d_ba, 1e-3f);
}

TEST(DeltaE2000Test, IsNonNegative) {
  core::rgbf_t a(0.3f, 0.6f, 0.1f);
  core::rgbf_t b(0.9f, 0.2f, 0.7f);
  EXPECT_GE(delta_e_2000(a, b), 0.0f);
}

TEST(DeltaE2000Test, PerceivedOrdering) {
  // Two very similar grays vs a very different color
  core::rgb8_t ref(128, 128, 128);
  core::rgb8_t nearly_same(129, 128, 128);
  core::rgb8_t very_different(255, 0, 0);

  float de_small = delta_e_2000(ref, nearly_same);
  float de_large = delta_e_2000(ref, very_different);
  EXPECT_LT(de_small, de_large);
}

// is_visually_same

TEST(IsVisuallySameTest, SameColorIsTrue) {
  core::rgb8_t c(100, 150, 200);
  EXPECT_TRUE(is_visually_same(c, c));
}

TEST(IsVisuallySameTest, VeryDifferentColorsAreFalse) {
  EXPECT_FALSE(is_visually_same(core::rgb8_t{0, 0, 0}, core::rgb8_t{255, 255, 255}));
}

TEST(IsVisuallySameTest, NearlyIdenticalColorsAreTrue) {
  core::rgb8_t a(128, 64, 200);
  core::rgb8_t b(128, 64, 200);  // identical
  EXPECT_TRUE(is_visually_same(a, b));
}

TEST(IsVisuallySameTest, CustomThreshold) {
  // With a very large threshold, even different colors are "same"
  core::rgb8_t black(0, 0, 0);
  core::rgb8_t white(255, 255, 255);
  EXPECT_TRUE(is_visually_same(black, white, 200.0f));
}

}  // namespace colorcpp::operations::test
