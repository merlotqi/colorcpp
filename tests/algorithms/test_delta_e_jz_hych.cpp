/**
 * @file test_delta_e_jz_hych.cpp
 * @brief Tests for ΔE_Jz (Jzazbz Euclidean) and ΔE_HyCH.
 */

#include <gtest/gtest.h>

#include <colorcpp/algorithms/delta_e/hych.hpp>
#include <colorcpp/algorithms/delta_e/jzazbz.hpp>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/core/jzazbz.hpp>
#include <colorcpp/core/xyz.hpp>

namespace colorcpp::algorithms::test {

using namespace delta_e;

// Euclidean distance in Jzazbz. Anchors computed from the Safdar 2017
// transform in double precision (peak-scaled modified PQ), matching the
// pinned XYZ->Jzazbz reference values in test_jzazbz.cpp.
TEST(DeltaEJzTest, XYPairMatchesEquationDerivedReference) {
  const core::xyz_t sample(0.2065401f, 0.1219723f, 0.0513695f);
  const core::xyz_t white(0.95047f, 1.0f, 1.08883f);
  EXPECT_NEAR(delta_e_jz(sample, white), 0.016229080f, 1e-4f);
}

TEST(DeltaEJzTest, DirectJzazbzInputs) {
  const core::jzazbz_t a(0.0053505f, 0.0092430f, 0.0052601f);
  const core::jzazbz_t b(0.0175800f, 0.0f, 0.0f);
  EXPECT_NEAR(delta_e_jz(a, b), 0.016206862f, 1e-6f);
}

TEST(DeltaEJzTest, SaturatedPrimaries) {
  const core::xyz_t red(0.4124564f, 0.3575761f, 0.1804375f);
  const core::xyz_t green(0.3575761f, 0.7151522f, 0.1191920f);
  const core::xyz_t blue(0.1804375f, 0.0721750f, 0.9503041f);
  EXPECT_NEAR(delta_e_jz(red, blue), 0.033613329f, 1e-4f);
  EXPECT_NEAR(delta_e_jz(red, green), 0.023770039f, 1e-4f);
}

TEST(DeltaEJzTest, SameColorIsZero) {
  const core::xyz_t xyz(0.2065401f, 0.1219723f, 0.0513695f);
  EXPECT_FLOAT_EQ(delta_e_jz(xyz, xyz), 0.0f);
}

TEST(DeltaEJzTest, IsSymmetric) {
  const core::xyz_t a(0.2065401f, 0.1219723f, 0.0513695f);
  const core::xyz_t b(0.95047f, 1.0f, 1.08883f);
  EXPECT_FLOAT_EQ(delta_e_jz(a, b), delta_e_jz(b, a));
}

// HyCH: |ΔL'/SL| + √((ΔC'/SC)² + (ΔH'/SH)²) built on CIE 2000 intermediates
// (Abasi et al. 2020). Anchors: colour-science delta_E_HyCH unit tests.
TEST(DeltaEHyCHTest, ReferencePairMatchesColorScience) {
  const core::cielab_t lab1(39.91531343f, 51.16658481f, 146.12933781f);
  const core::cielab_t lab2(53.12207516f, -39.92365056f, 249.54831278f);
  EXPECT_NEAR(delta_e_hych(lab1, lab2), 48.664279419760369f, 1e-4f);
}

TEST(DeltaEHyCHTest, SecondReferencePair) {
  const core::cielab_t lab1(39.91531343f, 51.16658481f, 146.12933781f);
  const core::cielab_t lab2(28.52234779f, 19.46628874f, 472.06042624f);
  EXPECT_NEAR(delta_e_hych(lab1, lab2), 39.260928157999118f, 1e-4f);
}

TEST(DeltaEHyCHTest, SmallDifferencePair) {
  const core::cielab_t lab1(48.99183622f, -0.10561667f, 400.65619925f);
  const core::cielab_t lab2(50.65907324f, -0.11671910f, 402.82235718f);
  EXPECT_NEAR(delta_e_hych(lab1, lab2), 1.7806293290163562f, 1e-4f);
}

// Low chroma: at C < 25 the G (a') adjustment has real leverage; the three
// high-chroma colour-science pairs alone would not catch its removal.
TEST(DeltaEHyCHTest, LowChromaPairExercisesGFactor) {
  const core::cielab_t lab1(50.0f, 18.0f, 22.0f);
  const core::cielab_t lab2(54.0f, -6.0f, 14.0f);
  EXPECT_NEAR(delta_e_hych(lab1, lab2), 27.2191353716f, 1e-4f);
}

// Textiles parametrization (kL = 2) halves the normalized lightness term.
TEST(DeltaEHyCHTest, TextilesKLScalesLightness) {
  const core::cielab_t lab1(39.91531343f, 51.16658481f, 146.12933781f);
  const core::cielab_t lab2(53.12207516f, -39.92365056f, 249.54831278f);
  EXPECT_NEAR(delta_e_hych(lab1, lab2, 2.0f), 42.2661307971f, 1e-4f);
}

// Pure lightness difference at L̄ = 55: chroma/hue terms vanish, SL scales.
TEST(DeltaEHyCHTest, PureLightnessDifference) {
  const core::cielab_t lab1(50.0f, 0.0f, 0.0f);
  const core::cielab_t lab2(60.0f, 0.0f, 0.0f);
  EXPECT_NEAR(delta_e_hych(lab1, lab2), 9.4705785636f, 1e-4f);
}

TEST(DeltaEHyCHTest, SameColorIsZero) {
  const core::cielab_t lab(40.0f, 20.0f, 30.0f);
  EXPECT_FLOAT_EQ(delta_e_hych(lab, lab), 0.0f);
}

TEST(DeltaEHyCHTest, IsSymmetric) {
  const core::cielab_t lab1(39.91531343f, 51.16658481f, 146.12933781f);
  const core::cielab_t lab2(53.12207516f, -39.92365056f, 249.54831278f);
  EXPECT_NEAR(delta_e_hych(lab1, lab2), delta_e_hych(lab2, lab1), 1e-6f);
}

}  // namespace colorcpp::algorithms::test
