
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

TEST(DeltaE94Test, MatchesPublishedReferenceSamples) {
  // Published reference pairs (colour-science test suite, verified against an
  // independent double-precision implementation of the CIE94 formula). These
  // pin the K1/K2 weighting constants and the graphic-arts vs textiles split —
  // property tests alone cannot see a coefficient swap. Graphic-arts samples
  // call with defaults so the default K1/K2 values themselves are anchored.
  struct Sample {
    core::cielab_t reference;
    core::cielab_t comparison;
    bool textiles;
    float expected;
  };

  const Sample samples[] = {
      // graphic arts (default kL=1, K1=0.045, K2=0.015)
      {core::cielab_t(48.99183622f, -0.10561667f, 400.65619925f),
       core::cielab_t(50.65907324f, -0.11671910f, 402.82235718f), false, 1.6711191305f},
      {core::cielab_t(100.0f, 21.57210357f, 272.22819350f), core::cielab_t(100.0f, 426.67945353f, 72.39590835f), false,
       83.7792255009f},
      {core::cielab_t(100.0f, 21.57210357f, 272.22819350f), core::cielab_t(100.0f, 74.05216981f, 276.45318193f), false,
       10.0539319546f},
      // textiles (kL=2, K1=0.048, K2=0.014)
      {core::cielab_t(100.0f, 21.57210357f, 272.22819350f), core::cielab_t(100.0f, 426.67945353f, 72.39590835f), true,
       88.3355530575f},
      {core::cielab_t(100.0f, 21.57210357f, 272.22819350f), core::cielab_t(100.0f, 74.05216981f, 276.45318193f), true,
       10.6126578900f},
      {core::cielab_t(100.0f, 21.57210357f, 272.22819350f), core::cielab_t(100.0f, 8.32281957f, -73.58297716f), true,
       60.3686872611f},
  };

  int sample_index = 0;
  for (const auto& sample : samples) {
    SCOPED_TRACE(testing::Message() << "CIE94 sample #" << sample_index);
    const float actual = sample.textiles
                             ? delta_e_94(sample.reference, sample.comparison, 2.0f, 1.0f, 1.0f, 0.048f, 0.014f)
                             : delta_e_94(sample.reference, sample.comparison);
    EXPECT_NEAR(actual, sample.expected, 1e-3f);
    ++sample_index;
  }
}

TEST(DeltaE94Test, NonPositiveWeightsFallBackToUnity) {
  // Zero or negative weights must not divide by zero — they fall back to 1.0,
  // matching delta_e_din99's established policy.
  const core::cielab_t reference(50.0f, 20.0f, 10.0f);
  const core::cielab_t comparison(55.0f, 15.0f, 25.0f);
  const float expected = delta_e_94(reference, comparison);

  EXPECT_NEAR(delta_e_94(reference, comparison, 0.0f, 1.0f, 1.0f), expected, 1e-5f);
  EXPECT_NEAR(delta_e_94(reference, comparison, 1.0f, 0.0f, 1.0f), expected, 1e-5f);
  EXPECT_NEAR(delta_e_94(reference, comparison, 1.0f, 1.0f, 0.0f), expected, 1e-5f);
  EXPECT_NEAR(delta_e_94(reference, comparison, -1.0f, -2.0f, -3.0f), expected, 1e-5f);
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

  // Full published Sharma/Wu/Dalal CIEDE2000 supplementary dataset (34 pairs).
  // Includes the hue-discontinuity family (samples 9–16) where |Δh'| sits at or
  // near 180°, zero-chroma pairs, and the hue-wrap pairs — all of which pin
  // branch choices the first six samples cannot see.
  const Sample samples[] = {
      {core::cielab_t(50.0000f, 2.6772f, -79.7751f), core::cielab_t(50.0000f, 0.0000f, -82.7485f), 2.0425f},
      {core::cielab_t(50.0000f, 3.1571f, -77.2803f), core::cielab_t(50.0000f, 0.0000f, -82.7485f), 2.8615f},
      {core::cielab_t(50.0000f, 2.8361f, -74.0200f), core::cielab_t(50.0000f, 0.0000f, -82.7485f), 3.4412f},
      {core::cielab_t(50.0000f, -1.3802f, -84.2814f), core::cielab_t(50.0000f, 0.0000f, -82.7485f), 1.0000f},
      {core::cielab_t(50.0000f, -1.1848f, -84.8006f), core::cielab_t(50.0000f, 0.0000f, -82.7485f), 1.0000f},
      {core::cielab_t(50.0000f, -0.9009f, -85.5211f), core::cielab_t(50.0000f, 0.0000f, -82.7485f), 1.0000f},
      {core::cielab_t(50.0000f, 0.0000f, 0.0000f), core::cielab_t(50.0000f, -1.0000f, 2.0000f), 2.3669f},
      {core::cielab_t(50.0000f, -1.0000f, 2.0000f), core::cielab_t(50.0000f, 0.0000f, 0.0000f), 2.3669f},
      {core::cielab_t(50.0000f, 2.4900f, -0.0010f), core::cielab_t(50.0000f, -2.4900f, 0.0009f), 7.1792f},
      {core::cielab_t(50.0000f, 2.4900f, -0.0010f), core::cielab_t(50.0000f, -2.4900f, 0.0010f), 7.1792f},
      {core::cielab_t(50.0000f, 2.4900f, -0.0010f), core::cielab_t(50.0000f, -2.4900f, 0.0011f), 7.2195f},
      {core::cielab_t(50.0000f, 2.4900f, -0.0010f), core::cielab_t(50.0000f, -2.4900f, 0.0012f), 7.2195f},
      {core::cielab_t(50.0000f, -0.0010f, 2.4900f), core::cielab_t(50.0000f, 0.0009f, -2.4900f), 4.8045f},
      {core::cielab_t(50.0000f, -0.0010f, 2.4900f), core::cielab_t(50.0000f, 0.0010f, -2.4900f), 4.8045f},
      {core::cielab_t(50.0000f, -0.0010f, 2.4900f), core::cielab_t(50.0000f, 0.0011f, -2.4900f), 4.7461f},
      {core::cielab_t(50.0000f, -0.0010f, 2.4900f), core::cielab_t(50.0000f, 0.0012f, -2.4900f), 4.7461f},
      {core::cielab_t(50.0000f, 2.5000f, 0.0000f), core::cielab_t(50.0000f, 0.0000f, -2.5000f), 4.3065f},
      {core::cielab_t(50.0000f, 2.5000f, 0.0000f), core::cielab_t(73.0000f, 25.0000f, -18.0000f), 27.1492f},
      {core::cielab_t(50.0000f, 2.5000f, 0.0000f), core::cielab_t(61.0000f, -5.0000f, 29.0000f), 22.8977f},
      {core::cielab_t(50.0000f, 2.5000f, 0.0000f), core::cielab_t(56.0000f, -27.0000f, -3.0000f), 31.9030f},
      {core::cielab_t(50.0000f, 2.5000f, 0.0000f), core::cielab_t(58.0000f, 24.0000f, 15.0000f), 19.4535f},
      {core::cielab_t(50.0000f, 2.5000f, 0.0000f), core::cielab_t(50.0000f, 3.1736f, 0.5854f), 1.0000f},
      {core::cielab_t(50.0000f, 2.5000f, 0.0000f), core::cielab_t(50.0000f, 3.2972f, 0.0000f), 1.0000f},
      {core::cielab_t(50.0000f, 2.5000f, 0.0000f), core::cielab_t(50.0000f, 1.8634f, 0.5757f), 1.0000f},
      {core::cielab_t(50.0000f, 2.5000f, 0.0000f), core::cielab_t(50.0000f, 3.2592f, 0.3350f), 1.0000f},
      {core::cielab_t(60.2574f, -34.0099f, 36.2677f), core::cielab_t(60.4626f, -34.1751f, 39.4387f), 1.2644f},
      {core::cielab_t(63.0109f, -31.0961f, -5.8663f), core::cielab_t(62.8187f, -29.7946f, -4.0864f), 1.2630f},
      {core::cielab_t(61.2901f, 3.7196f, -5.3901f), core::cielab_t(61.4292f, 2.2480f, -4.9620f), 1.8731f},
      {core::cielab_t(35.0831f, -44.1164f, 3.7933f), core::cielab_t(35.0232f, -40.0716f, 1.5901f), 1.8645f},
      {core::cielab_t(22.7233f, 20.0904f, -46.6940f), core::cielab_t(23.0331f, 14.9730f, -42.5619f), 2.0373f},
      {core::cielab_t(36.4612f, 47.8580f, 18.3852f), core::cielab_t(36.2715f, 50.5065f, 21.2231f), 1.4146f},
      {core::cielab_t(90.8027f, -2.0831f, 1.4410f), core::cielab_t(91.1528f, -1.6435f, 0.0447f), 1.4441f},
      {core::cielab_t(90.9257f, -0.5406f, -0.9208f), core::cielab_t(88.6381f, -0.8985f, -0.7239f), 1.5381f},
      {core::cielab_t(6.7747f, -0.2908f, -2.4247f), core::cielab_t(5.8714f, -0.0985f, -2.2286f), 0.6377f},
      {core::cielab_t(2.0776f, 0.0795f, -1.1350f), core::cielab_t(0.9033f, -0.0636f, -0.5514f), 0.9082f},
  };

  int sample_index = 0;
  for (const auto& sample : samples) {
    SCOPED_TRACE(testing::Message() << "Sharma sample #" << sample_index);
    EXPECT_NEAR(delta_e_2000(sample.reference, sample.comparison), sample.expected, 1e-3f);
    EXPECT_NEAR(delta_e_2000(sample.comparison, sample.reference), sample.expected, 1e-3f);
    ++sample_index;
  }
}

TEST(DeltaE2000Test, NonPositiveWeightsFallBackToUnity) {
  // Zero or negative weights must not divide by zero — they fall back to 1.0,
  // matching delta_e_din99's established policy.
  const core::cielab_t reference(50.0f, 20.0f, 10.0f);
  const core::cielab_t comparison(55.0f, 15.0f, 25.0f);
  const float expected = delta_e_2000(reference, comparison);

  EXPECT_NEAR(delta_e_2000(reference, comparison, 0.0f, 1.0f, 1.0f), expected, 1e-5f);
  EXPECT_NEAR(delta_e_2000(reference, comparison, 1.0f, 0.0f, 1.0f), expected, 1e-5f);
  EXPECT_NEAR(delta_e_2000(reference, comparison, 1.0f, 1.0f, 0.0f), expected, 1e-5f);
  EXPECT_NEAR(delta_e_2000(reference, comparison, -1.0f, -2.0f, -3.0f), expected, 1e-5f);
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

TEST(DeltaECMCTest, MatchesPublishedReferenceSamples) {
  // Published reference pairs (colour-science test suite, verified against an
  // independent double-precision implementation of the CMC formula). These pin
  // the SL/SC/SH weighting constants and the h1 T-branch split.
  struct Sample {
    core::cielab_t reference;
    core::cielab_t comparison;
    float l;
    float expected;
  };

  const Sample samples[] = {
      {core::cielab_t(48.99183622f, -0.10561667f, 400.65619925f),
       core::cielab_t(50.65907324f, -0.11671910f, 402.82235718f), 2.0f, 0.8996999757f},
      {core::cielab_t(100.0f, 21.57210357f, 272.22819350f), core::cielab_t(100.0f, 426.67945353f, 72.39590835f), 2.0f,
       172.70477129f},
      {core::cielab_t(100.0f, 21.57210357f, 272.22819350f), core::cielab_t(100.0f, 74.05216981f, 276.45318193f), 2.0f,
       20.59732717f},
      {core::cielab_t(100.0f, 21.57210357f, 272.22819350f), core::cielab_t(100.0f, 426.67945353f, 72.39590835f), 1.0f,
       172.70477129f},
      {core::cielab_t(100.0f, 21.57210357f, 272.22819350f), core::cielab_t(100.0f, 74.05216981f, 276.45318193f), 1.0f,
       20.59732717f},
      {core::cielab_t(100.0f, 21.57210357f, 272.22819350f), core::cielab_t(100.0f, 8.32281957f, -73.58297716f), 1.0f,
       121.71841479f},
  };

  int sample_index = 0;
  for (const auto& sample : samples) {
    SCOPED_TRACE(testing::Message() << "CMC sample #" << sample_index);
    EXPECT_NEAR(delta_e_cmc(sample.reference, sample.comparison, sample.l, 1.0f), sample.expected, 1e-3f);
    ++sample_index;
  }
}

TEST(DeltaECMCTest, MatchesEquationDerivedEdgeSamples) {
  // Equation-derived pairs (published CMC formula, independent double-precision
  // implementation) covering the branches the published sample set misses:
  // SL's L1<16 constant and SH's h1∈[164,345] T-branch. Called with defaults so
  // the default l=2 is anchored at the same time.
  struct Sample {
    const char* label;
    core::cielab_t reference;
    core::cielab_t comparison;
    float expected;
  };

  const Sample samples[] = {
      {"dark_reference_SL_constant", core::cielab_t(10.0f, 5.0f, 2.0f), core::cielab_t(12.0f, 4.0f, 3.0f),
       2.7231480874f},
      {"blue_hue_T_branch", core::cielab_t(50.0f, -30.0f, -10.0f), core::cielab_t(50.0f, -28.0f, -8.0f), 1.4754735625f},
  };

  for (const auto& sample : samples) {
    SCOPED_TRACE(sample.label);
    EXPECT_NEAR(delta_e_cmc(sample.reference, sample.comparison), sample.expected, 1e-3f);
  }
}

TEST(DeltaECMCTest, NonPositiveWeightsFallBackToUnity) {
  // Zero or negative weights must not divide by zero — they fall back to 1.0,
  // matching delta_e_din99's established policy.
  const core::cielab_t reference(50.0f, 20.0f, 10.0f);
  const core::cielab_t comparison(55.0f, 15.0f, 25.0f);
  const float expected = delta_e_cmc(reference, comparison, 1.0f, 1.0f);

  EXPECT_NEAR(delta_e_cmc(reference, comparison, 0.0f, 1.0f), expected, 1e-5f);
  EXPECT_NEAR(delta_e_cmc(reference, comparison, 1.0f, 0.0f), expected, 1e-5f);
  EXPECT_NEAR(delta_e_cmc(reference, comparison, -1.0f, -2.0f), expected, 1e-5f);
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
