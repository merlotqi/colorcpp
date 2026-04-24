
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

TEST(DeltaE94Test, ReferenceOrderAffectsResult) {
  const core::cielab_t reference(50.0f, 60.0f, 30.0f);
  const core::cielab_t comparison(50.0f, 20.0f, 10.0f);

  const float d_ref_cmp = delta_e_94(reference, comparison);
  const float d_cmp_ref = delta_e_94(comparison, reference);

  EXPECT_GT(d_ref_cmp, 0.0f);
  EXPECT_GT(d_cmp_ref, 0.0f);
  EXPECT_GT(d_cmp_ref - d_ref_cmp, 1.0f);
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

TEST(DeltaE2000Test, MatchesPublishedSharmaSamples) {
  struct Sample {
    core::cielab_t reference;
    core::cielab_t comparison;
    float expected;
  };

  // First six published Sharma/Wu/Dalal CIEDE2000 supplementary cases used in the audit/plan.
  const Sample samples[] = {
      {core::cielab_t(50.0000f, 2.6772f, -79.7751f), core::cielab_t(50.0000f, 0.0000f, -82.7485f), 2.0425f},
      {core::cielab_t(50.0000f, 3.1571f, -77.2803f), core::cielab_t(50.0000f, 0.0000f, -82.7485f), 2.8615f},
      {core::cielab_t(50.0000f, 2.8361f, -74.0200f), core::cielab_t(50.0000f, 0.0000f, -82.7485f), 3.4412f},
      {core::cielab_t(50.0000f, -1.3802f, -84.2814f), core::cielab_t(50.0000f, 0.0000f, -82.7485f), 1.0000f},
      {core::cielab_t(50.0000f, -1.1848f, -84.8006f), core::cielab_t(50.0000f, 0.0000f, -82.7485f), 1.0000f},
      {core::cielab_t(50.0000f, -0.9009f, -85.5211f), core::cielab_t(50.0000f, 0.0000f, -82.7485f), 1.0000f},
  };

  int sample_index = 0;
  for (const auto& sample : samples) {
    SCOPED_TRACE(testing::Message() << "Sharma sample #" << sample_index);
    EXPECT_NEAR(delta_e_2000(sample.reference, sample.comparison), sample.expected, 1e-3f);
    EXPECT_NEAR(delta_e_2000(sample.comparison, sample.reference), sample.expected, 1e-3f);
    ++sample_index;
  }
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

// Oklab ΔE_OK

TEST(DeltaEOkTest, SameColorIsZero) {
  core::rgbf_t c(0.4f, 0.5f, 0.6f);
  EXPECT_NEAR(delta_e_ok(c, c), 0.0f, 1e-5f);
}

TEST(DeltaEOkTest, SymmetricNonNegative) {
  core::rgb8_t a(200, 100, 50);
  core::rgb8_t b(50, 100, 200);
  float d1 = delta_e_ok(a, b);
  float d2 = delta_e_ok(b, a);
  EXPECT_NEAR(d1, d2, 1e-4f);
  EXPECT_GE(d1, 0.0f);
}

TEST(DeltaEOkTest, VisuallySameOkUsesJnd) {
  core::rgb8_t x(128, 128, 128);
  EXPECT_TRUE(is_visually_same_ok(x, x));
}

// CMC l:c

TEST(DeltaECMCTest, SameColorIsZero) {
  core::rgbf_t c(0.5f, 0.3f, 0.7f);
  EXPECT_NEAR(delta_e_cmc(c, c), 0.0f, 1e-4f);
}

TEST(DeltaECMCTest, BlackToWhiteIsLarge) {
  core::rgb8_t black(0, 0, 0);
  core::rgb8_t white(255, 255, 255);
  EXPECT_GT(delta_e_cmc(black, white), 50.0f);
}

TEST(DeltaECMCTest, ReferenceOrderAffectsResult) {
  const core::cielab_t reference(50.0f, 60.0f, 30.0f);
  const core::cielab_t comparison(50.0f, 20.0f, 10.0f);

  const float d_ref_cmp = delta_e_cmc(reference, comparison);
  const float d_cmp_ref = delta_e_cmc(comparison, reference);

  EXPECT_GT(d_ref_cmp, 0.0f);
  EXPECT_GT(d_cmp_ref, 0.0f);
  EXPECT_GT(d_cmp_ref - d_ref_cmp, 1.0f);
}

TEST(DeltaECMCTest, IsNonNegative) {
  core::rgbf_t a(0.3f, 0.6f, 0.1f);
  core::rgbf_t b(0.9f, 0.2f, 0.7f);
  EXPECT_GE(delta_e_cmc(a, b), 0.0f);
}

TEST(DeltaECMCTest, PerceptibilityVsAcceptability) {
  core::rgb8_t a(128, 128, 128);
  core::rgb8_t b(130, 128, 128);
  // l=1, c=1 (perceptibility) should be stricter than l=2, c=1 (acceptability)
  float perceptibility = delta_e_cmc(a, b, 1.0f, 1.0f);
  float acceptability = delta_e_cmc(a, b, 2.0f, 1.0f);
  EXPECT_GT(perceptibility, acceptability);
}

TEST(DeltaECMCTest, IsVisuallySameCmc) {
  core::rgb8_t a(100, 150, 200);
  core::rgb8_t b(100, 150, 200);
  EXPECT_TRUE(is_visually_same_cmc(a, b));
}

TEST(DeltaECMCTest, DifferentColorsAreFalse) {
  core::rgb8_t black(0, 0, 0);
  core::rgb8_t white(255, 255, 255);
  EXPECT_FALSE(is_visually_same_cmc(black, white));
}

// DIN99

TEST(DeltaEDIN99Test, SameColorIsZero) {
  core::rgbf_t c(0.5f, 0.3f, 0.7f);
  EXPECT_NEAR(delta_e_din99(c, c), 0.0f, 1e-4f);
}

TEST(DeltaEDIN99Test, BlackToWhiteIsLarge) {
  core::rgb8_t black(0, 0, 0);
  core::rgb8_t white(255, 255, 255);
  EXPECT_GT(delta_e_din99(black, white), 50.0f);
}

TEST(DeltaEDIN99Test, IsSymmetric) {
  core::rgb8_t red(255, 0, 0);
  core::rgb8_t blue(0, 0, 255);
  float d_ab = delta_e_din99(red, blue);
  float d_ba = delta_e_din99(blue, red);
  EXPECT_NEAR(d_ab, d_ba, 1e-3f);
}

TEST(DeltaEDIN99Test, IsNonNegative) {
  core::rgbf_t a(0.3f, 0.6f, 0.1f);
  core::rgbf_t b(0.9f, 0.2f, 0.7f);
  EXPECT_GE(delta_e_din99(a, b), 0.0f);
}

TEST(DeltaEDIN99Test, MatchesEquationDerivedReferencePairs) {
  struct Sample {
    const char* label;
    core::cielab_t reference;
    core::cielab_t comparison;
    float expected;
  };

  const Sample samples[] = {
      {"neutral_to_positive_b", core::cielab_t(50.0f, 0.0f, 0.0f), core::cielab_t(50.0f, 0.0f, 10.0f), 6.2908f},
      {"orthogonal_hue_swap", core::cielab_t(50.0f, 30.0f, 0.0f), core::cielab_t(50.0f, 0.0f, 30.0f), 21.7733f},
      {"mixed_lightness_chroma", core::cielab_t(20.0f, 40.0f, 10.0f), core::cielab_t(80.0f, -20.0f, -30.0f), 70.6783f},
      {"small_hue_rotation", core::cielab_t(50.0f, 5.0f, 0.0f), core::cielab_t(50.0f, 0.0f, 5.0f), 5.0306f},
  };

  for (const auto& sample : samples) {
    SCOPED_TRACE(sample.label);
    EXPECT_NEAR(delta_e_din99(sample.reference, sample.comparison), sample.expected, 1e-3f);
    EXPECT_NEAR(delta_e_din99(sample.comparison, sample.reference), sample.expected, 1e-3f);
  }
}

TEST(DeltaEDIN99Test, ScalingParametersMatchPublishedEquation) {
  const core::cielab_t mixed_reference(20.0f, 40.0f, 10.0f);
  const core::cielab_t mixed_comparison(80.0f, -20.0f, -30.0f);
  const core::cielab_t light_reference(20.0f, 0.0f, 0.0f);
  const core::cielab_t light_comparison(80.0f, 0.0f, 0.0f);

  EXPECT_NEAR(delta_e_din99(mixed_reference, mixed_comparison, 2.0f, 1.0f), 50.3771f, 1e-3f);
  EXPECT_NEAR(delta_e_din99(light_reference, light_comparison, 1.0f, 2.0f), 28.6215f, 1e-3f);
}

TEST(DeltaEDIN99Test, IsVisuallySameDin99) {
  core::rgb8_t a(100, 150, 200);
  core::rgb8_t b(100, 150, 200);
  EXPECT_TRUE(is_visually_same_din99(a, b));
}

TEST(DeltaEDIN99Test, DifferentColorsAreFalse) {
  core::rgb8_t black(0, 0, 0);
  core::rgb8_t white(255, 255, 255);
  EXPECT_FALSE(is_visually_same_din99(black, white));
}

}  // namespace colorcpp::algorithms::test
