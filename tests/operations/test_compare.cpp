/**
 * @file test_compare.cpp
 * @brief Unit tests for color comparison module.
 */

#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

namespace colorcpp::operations::test {

using namespace core;
using namespace compare;

// ============================================================================
// Operator comparison tests
// ============================================================================

TEST(OperatorCompareTest, SameColorsAreEqual) {
  rgb8_t a{128, 64, 32};
  rgb8_t b{128, 64, 32};
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);
}

TEST(OperatorCompareTest, DifferentColorsAreNotEqual) {
  rgb8_t a{128, 64, 32};
  rgb8_t b{128, 64, 33};
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
}

// ============================================================================
// Exact comparison tests
// ============================================================================

TEST(ExactCompareTest, SameColorsAreEqual) {
  rgb8_t a{128, 64, 32};
  rgb8_t b{128, 64, 32};
  EXPECT_TRUE(exact_equal(a, b));
  EXPECT_TRUE(equal(a, b));
  EXPECT_TRUE(a == b);
}

TEST(ExactCompareTest, DifferentColorsAreNotEqual) {
  rgb8_t a{128, 64, 32};
  rgb8_t b{128, 64, 33};
  EXPECT_FALSE(exact_equal(a, b));
  EXPECT_FALSE(equal(a, b));
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
}

TEST(ExactCompareTest, FloatColorsExact) {
  rgbf_t a{0.5f, 0.3f, 0.7f};
  rgbf_t b{0.5f, 0.3f, 0.7f};
  EXPECT_TRUE(exact_equal(a, b));
}

TEST(ExactCompareTest, FloatColorsSlightDifference) {
  rgbf_t a{0.5f, 0.3f, 0.7f};
  rgbf_t b{0.5000001f, 0.3f, 0.7f};
  EXPECT_FALSE(exact_equal(a, b));
}

// ============================================================================
// Epsilon comparison tests
// ============================================================================

TEST(EpsilonCompareTest, WithinEpsilon) {
  rgbf_t a{0.5f, 0.3f, 0.7f};
  rgbf_t b{0.5000001f, 0.3f, 0.7f};
  EXPECT_TRUE(epsilon_equal(a, b, 0.001f));
  EXPECT_TRUE((equal<epsilon_policy>(a, b, 0.001f)));
}

TEST(EpsilonCompareTest, OutsideEpsilon) {
  rgbf_t a{0.5f, 0.3f, 0.7f};
  rgbf_t b{0.51f, 0.3f, 0.7f};
  EXPECT_FALSE(epsilon_equal(a, b, 0.001f));
}

TEST(EpsilonCompareTest, IntegerColors) {
  rgb8_t a{100, 50, 200};
  rgb8_t b{101, 50, 200};
  EXPECT_TRUE(epsilon_equal(a, b, 1));
  EXPECT_FALSE(epsilon_equal(a, b, 0));
}

TEST(EpsilonCompareTest, LabScaleIsAbsolute) {
  // Before the fix, float_equal also scaled eps by max(|a|,|b|):
  // L* 50 vs 50.5 with eps=0.01 passed because 0.5 <= 0.01 * 50.5.
  cielab_t a{50.0f, 0.0f, 0.0f};
  cielab_t b{50.5f, 0.0f, 0.0f};
  EXPECT_FALSE(epsilon_equal(a, b, 0.01f));
}

TEST(EpsilonCompareTest, HueScaleIsAbsolute) {
  // Same inflation on hue magnitudes: 100 vs 100.5 with eps=0.01 passed
  // because 0.5 <= 0.01 * 100.5.
  hsl_float_t a{100.0f, 0.5f, 0.5f};
  hsl_float_t b{100.5f, 0.5f, 0.5f};
  EXPECT_FALSE(epsilon_equal(a, b, 0.01f));
}

TEST(EpsilonCompareTest, NegativeToleranceNeverMatches) {
  // Before the fix, the `a == b` early return made identical floats "equal"
  // even with a negative tolerance (the integer path rejects that).
  rgbf_t a{0.5f, 0.3f, 0.7f};
  EXPECT_FALSE(epsilon_equal(a, a, -0.1f));
}

// ============================================================================
// Relative comparison tests
// ============================================================================

TEST(RelativeCompareTest, WithinRelative) {
  rgbf_t a{0.5f, 0.3f, 0.8f};
  rgbf_t b{0.505f, 0.3f, 0.8f};
  EXPECT_TRUE(relative_equal(a, b, 0.02f));
  EXPECT_TRUE((equal<relative_policy>(a, b, 0.02f)));
}

TEST(RelativeCompareTest, OutsideRelative) {
  rgbf_t a{0.5f, 0.3f, 0.8f};
  rgbf_t b{0.55f, 0.3f, 0.8f};
  EXPECT_FALSE(relative_equal(a, b, 0.001f));
}

// ============================================================================
// Perceptual comparison tests
// ============================================================================

TEST(PerceptualCompareTest, IdenticalColors) {
  rgb8_t a{128, 64, 32};
  rgb8_t b{128, 64, 32};
  EXPECT_TRUE(perceptual_equal(a, b, 0.01f));
  EXPECT_TRUE((equal<perceptual_policy>(a, b, 0.01f)));
}

TEST(PerceptualCompareTest, SimilarColors) {
  rgb8_t a{128, 64, 32};
  rgb8_t b{130, 65, 33};
  EXPECT_TRUE(perceptual_equal(a, b, 5.0f));
}

TEST(PerceptualCompareTest, DifferentColors) {
  rgb8_t a{255, 0, 0};
  rgb8_t b{0, 255, 0};
  EXPECT_FALSE(perceptual_equal(a, b, 1.0f));
}

TEST(PerceptualCompareTest, CrossTypeDeltaE2000Comparison) {
  rgb8_t a{128, 64, 32};
  rgbaf_t b{128.0f / 255.0f, 64.0f / 255.0f, 32.0f / 255.0f, 1.0f};

  EXPECT_TRUE(perceptual_equal(a, b, 0.05f));
  EXPECT_TRUE(perceptual_equal_de2000(a, b, 0.05f));
  EXPECT_TRUE((equal<perceptual_policy>(a, b, 0.05f)));
}

TEST(PerceptualCompareTest, CrossSpaceDeltaE2000Comparison) {
  rgb8_t source{128, 64, 32};
  auto converted = conversion::color_cast<oklab_t>(source);

  EXPECT_TRUE(perceptual_equal(source, converted, 0.001f));
  EXPECT_TRUE(perceptual_equal_de2000(source, converted, 0.001f));
  EXPECT_TRUE((equal<perceptual_policy>(source, converted, 0.001f)));
}

TEST(PerceptualCompareTest, AlphaIsIgnoredButExactComparisonStillDiffers) {
  rgbaf_t opaque{0.5f, 0.25f, 0.75f, 1.0f};
  rgbaf_t transparent{0.5f, 0.25f, 0.75f, 0.1f};

  EXPECT_TRUE(perceptual_equal(opaque, transparent, 0.001f));
  EXPECT_TRUE(perceptual_equal_de2000(opaque, transparent, 0.001f));
  EXPECT_TRUE((equal<perceptual_policy>(opaque, transparent, 0.001f)));
  EXPECT_FALSE(exact_equal(opaque, transparent));
  EXPECT_FALSE(equal(opaque, transparent));
}

// ============================================================================
// Cross-type tests
// ============================================================================

TEST(PerceptualCompareTest, ThresholdEqualsDistanceIsSame) {
  // Boundary is inclusive: distance == threshold must count as equal.
  rgb8_t a{128, 64, 32};
  rgb8_t b{130, 65, 33};
  float d = algorithms::delta_e::delta_e_2000(a, b);
  ASSERT_GT(d, 0.0f);
  EXPECT_TRUE(perceptual_equal(a, b, d));
}

TEST(PerceptualCompareTest, OklabMetricUsesJndScale) {
  rgb8_t a{128, 64, 32};
  rgb8_t b{130, 65, 33};
  float d = algorithms::delta_e::delta_e_ok(a, b);
  ASSERT_GT(d, 0.0f);
  EXPECT_TRUE(perceptual_equal_ok(a, b, d));
  EXPECT_TRUE(equal<perceptual_oklab_policy>(a, b, d));
  EXPECT_FALSE(perceptual_equal_ok(a, b, d * 0.5f));
}

TEST(PerceptualCompareTest, OklabMetricDefaultThresholdIsJnd) {
  rgb8_t a{128, 64, 32};
  rgb8_t b{129, 64, 32};  // sub-JND difference
  float d = algorithms::delta_e::delta_e_ok(a, b);
  if (d < algorithms::delta_e::oklab_jnd_typical) {
    EXPECT_TRUE(perceptual_equal_ok(a, b));  // default threshold = oklab_jnd_typical
  }
}

TEST(PerceptualCompareTest, AlphaExactModeDetectsDifference) {
  rgbaf_t opaque{0.5f, 0.25f, 0.75f, 1.0f};
  rgbaf_t transparent{0.5f, 0.25f, 0.75f, 0.1f};
  // Default stays alpha-ignoring (backward compatible) ...
  EXPECT_TRUE(perceptual_equal(opaque, transparent, 1.0f));
  EXPECT_TRUE(perceptual_equal(opaque, transparent, 1.0f, alpha_mode::ignore));
  // ... but alpha_mode::exact must reject the alpha difference.
  EXPECT_FALSE(perceptual_equal(opaque, transparent, 1.0f, alpha_mode::exact));
}

TEST(PerceptualCompareTest, AlphaExactModeAcceptsMatchingAlpha) {
  rgbaf_t a{0.5f, 0.25f, 0.75f, 1.0f};
  rgbaf_t b{0.4f, 0.2f, 0.7f, 1.0f};
  EXPECT_TRUE(perceptual_equal(a, b, 10.0f, alpha_mode::exact));
}

TEST(PerceptualCompareTest, WeightsPassthrough) {
  rgb8_t a{128, 64, 32};
  rgb8_t b{130, 65, 33};
  float base = algorithms::delta_e::delta_e_2000(a, b);
  // Extreme lightness weight shrinks the L term's contribution.
  float heavy_kl = algorithms::delta_e::delta_e_2000(a, b, 10.0f, 1.0f, 1.0f);
  ASSERT_GT(base, 0.0f);
  ASSERT_LT(heavy_kl, base);
  EXPECT_TRUE(perceptual_equal(a, b, heavy_kl, alpha_mode::ignore, 10.0f, 1.0f, 1.0f));
  EXPECT_FALSE(perceptual_equal(a, b, heavy_kl * 0.5f, alpha_mode::ignore, 10.0f, 1.0f, 1.0f));
}

TEST(CompareCrossTypeTest, Rgb8VsRgbaf) {
  rgb8_t a{128, 64, 32};
  rgbaf_t b{128.0f / 255.0f, 64.0f / 255.0f, 32.0f / 255.0f, 1.0f};
  // Different types - should use conversion
  auto b_rgb8 = conversion::color_cast<rgb8_t>(b);
  EXPECT_TRUE(exact_equal(a, b_rgb8));
}

// ============================================================================
// Constexpr tests
// ============================================================================

TEST(ExactCompareTest, HueWraparoundEqual) {
  // 0° and 360° are the same hue by library convention; the compare layer
  // must use the circular distance instead of the linear one.
  hsl_float_t a{0.0f, 0.8f, 0.5f};
  hsl_float_t b{360.0f, 0.8f, 0.5f};
  EXPECT_TRUE(exact_equal(a, b));
  EXPECT_TRUE(equal(a, b));
}

TEST(ExactCompareTest, OklchHueWraparoundEqual) {
  oklch_t a{0.7f, 0.1f, 0.0f};
  oklch_t b{0.7f, 0.1f, 360.0f};
  EXPECT_TRUE(exact_equal(a, b));
}

TEST(EpsilonCompareTest, HueWraparoundEqual) {
  hsl_float_t a{0.0f, 0.8f, 0.5f};
  hsl_float_t b{360.0f, 0.8f, 0.5f};
  EXPECT_TRUE(epsilon_equal(a, b, 0.01f));
}

TEST(EpsilonCompareTest, HueShortestArc) {
  // 359.9° and 0.0° are 0.1° apart on the ring, not 359.9°.
  hsl_float_t a{359.9f, 0.8f, 0.5f};
  hsl_float_t b{0.0f, 0.8f, 0.5f};
  EXPECT_TRUE(epsilon_equal(a, b, 0.2f));
}

TEST(EpsilonCompareTest, HueShortestArcRejectsWideGap) {
  hsl_float_t a{0.0f, 0.8f, 0.5f};
  hsl_float_t b{180.0f, 0.8f, 0.5f};
  EXPECT_FALSE(epsilon_equal(a, b, 1.0f));
}

TEST(RelativeCompareTest, HueWraparoundEqual) {
  hsl_float_t a{0.0f, 0.8f, 0.5f};
  hsl_float_t b{360.0f, 0.8f, 0.5f};
  EXPECT_TRUE(relative_equal(a, b, 1e-3f));
}

TEST(ConstexprCompareTest, ExactConstexpr) {
  constexpr rgb8_t a{100, 50, 200};
  constexpr rgb8_t b{100, 50, 200};
  static_assert(exact_equal(a, b), "Should be constexpr");
}

TEST(ConstexprCompareTest, EpsilonConstexpr) {
  constexpr rgbf_t a{0.5f, 0.3f, 0.7f};
  constexpr rgbf_t b{0.5f, 0.3f, 0.7f};
  static_assert(epsilon_equal(a, b, 0.001f), "Should be constexpr");
}

}  // namespace colorcpp::operations::test
