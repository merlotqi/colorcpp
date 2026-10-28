/**
 * @file test_compare.cpp
 * @brief Unit tests for color comparison module.
 */

#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>
#include <functional>
#include <map>
#include <unordered_map>

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
// Ordering and hashing (std::map / std::unordered_map usability)
// ============================================================================

TEST(ColorOrderingTest, LexicographicLess) {
  // Strict weak ordering, channel order: first differing channel decides.
  rgb8_t a{1, 2, 3};
  rgb8_t b{1, 2, 4};
  rgb8_t c{1, 3, 0};
  EXPECT_TRUE(a < b);
  EXPECT_FALSE(b < a);
  EXPECT_TRUE(a < c);
  EXPECT_FALSE(a < a);
}

TEST(ColorOrderingTest, OrderedMapFindWorks) {
  std::map<rgb8_t, int> m;
  m[rgb8_t{1, 2, 3}] = 1;
  m[rgb8_t{2, 0, 0}] = 2;
  ASSERT_NE(m.find(rgb8_t{1, 2, 3}), m.end());
  EXPECT_EQ(m.find(rgb8_t{1, 2, 3})->second, 1);
  ASSERT_NE(m.find(rgb8_t{2, 0, 0}), m.end());
  EXPECT_EQ(m.find(rgb8_t{2, 0, 0})->second, 2);
  EXPECT_EQ(m.find(rgb8_t{1, 2, 4}), m.end());
}

TEST(ColorHashTest, EqualColorsHaveEqualHash) {
  // std::unordered_map contract: a == b implies hash(a) == hash(b).
  rgb8_t a{128, 64, 32};
  rgb8_t b{128, 64, 32};
  EXPECT_EQ(std::hash<rgb8_t>{}(a), std::hash<rgb8_t>{}(b));
  rgbf_t x{0.5f, 0.3f, 0.7f};
  rgbf_t y{0.5f, 0.3f, 0.7f};
  EXPECT_EQ(std::hash<rgbf_t>{}(x), std::hash<rgbf_t>{}(y));
}

TEST(ColorHashTest, PositiveNegativeZeroHashMatch) {
  // +0.0 == -0.0 channel-wise, so their hashes must agree too.
  rgbf_t pos{0.0f, 0.3f, 0.7f};
  rgbf_t neg{-0.0f, 0.3f, 0.7f};
  EXPECT_TRUE(pos == neg);
  EXPECT_EQ(std::hash<rgbf_t>{}(pos), std::hash<rgbf_t>{}(neg));
}

TEST(ColorHashTest, UnorderedMapFindWorks) {
  std::unordered_map<rgb8_t, int> m;
  m[rgb8_t{10, 20, 30}] = 42;
  auto it = m.find(rgb8_t{10, 20, 30});
  ASSERT_NE(it, m.end());
  EXPECT_EQ(it->second, 42);
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

TEST(RelativeCompareTest, IntegerSubPercentTolerance) {
  // Before the fix the tolerance parameter was Color::value_type:
  // 0.02f truncated to uint8_t was 0, so integer relative could not express
  // any ratio below 100% and degenerated to exact equality.
  rgb8_t a{100, 50, 200};
  rgb8_t b{101, 50, 200};
  EXPECT_TRUE(relative_equal(a, b, 0.02f));
  EXPECT_FALSE(relative_equal(a, b, 0.001f));
}

TEST(RelativeCompareTest, NearZeroUsesAbsFloor) {
  // Pure relative fails near zero: rel * max(|a|,|b|) = 1e-7 cannot cover 1e-4.
  // The absolute floor (isclose-style) does.
  rgbf_t a{0.0f, 0.5f, 0.5f};
  rgbf_t b{1e-4f, 0.5f, 0.5f};
  EXPECT_TRUE(relative_equal(a, b, 1e-3f, 1e-3f));
  EXPECT_FALSE(relative_equal(a, b, 1e-3f));
}

TEST(RelativeCompareTest, NegativeRelativeNeverMatches) {
  // Before the fix the `a == b` early return made identical floats "equal"
  // even with a negative tolerance (the integer path rejects that).
  rgbf_t a{0.5f, 0.3f, 0.7f};
  EXPECT_FALSE(relative_equal(a, a, -0.1f));
}

TEST(RelativeCompareTest, DefaultRgb8StaysEffectiveExact) {
  // Default rel is tiny; for integer channels that still means "equal or not".
  rgb8_t a{100, 50, 200};
  rgb8_t b{101, 50, 200};
  EXPECT_FALSE(relative_equal(a, b));
}

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

TEST(PerceptualCompareTest, DistanceApiMatchesDeltaE) {
  rgb8_t a{128, 64, 32};
  rgb8_t b{130, 65, 33};
  EXPECT_NEAR(perceptual_distance(a, b), algorithms::delta_e::delta_e_2000(a, b), 1e-6f);
  EXPECT_NEAR(perceptual_distance_de2000(a, b), algorithms::delta_e::delta_e_2000(a, b), 1e-6f);
  EXPECT_NEAR(perceptual_distance_ok(a, b), algorithms::delta_e::delta_e_ok(a, b), 1e-6f);
}

TEST(PerceptualCompareTest, DistanceApiWeightsPassthrough) {
  rgb8_t a{128, 64, 32};
  rgb8_t b{130, 65, 33};
  EXPECT_NEAR(perceptual_distance_de2000(a, b, 10.0f, 1.0f, 1.0f),
              algorithms::delta_e::delta_e_2000(a, b, 10.0f, 1.0f, 1.0f), 1e-6f);
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

TEST(ExactCompareTest, AchromaticHueIsIgnored) {
  // Saturation/chroma is zero on both sides: the stored hue is meaningless,
  // so a gray at h = 0 and the same gray at h = 180 must compare equal.
  hsl_float_t a{0.0f, 0.0f, 0.5f};
  hsl_float_t b{180.0f, 0.0f, 0.5f};
  EXPECT_TRUE(exact_equal(a, b));
  EXPECT_TRUE(equal(a, b));
}

TEST(ExactCompareTest, AchromaticHueIsIgnoredOklch) {
  oklch_t a{0.7f, 0.0f, 0.0f};
  oklch_t b{0.7f, 0.0f, 200.0f};
  EXPECT_TRUE(exact_equal(a, b));
}

TEST(ExactCompareTest, OneAchromaticStillDiffersOnChroma) {
  // Hue is only skipped when BOTH sides are achromatic; here saturation differs.
  hsl_float_t gray{0.0f, 0.0f, 0.5f};
  hsl_float_t tinted{0.0f, 0.5f, 0.5f};
  EXPECT_FALSE(exact_equal(gray, tinted));
}

TEST(EpsilonCompareTest, AchromaticHueIsIgnored) {
  hsl_float_t a{0.0f, 0.0f, 0.5f};
  hsl_float_t b{180.0f, 0.0f, 0.5f};
  EXPECT_TRUE(epsilon_equal(a, b, 0.01f));
}

TEST(RelativeCompareTest, AchromaticHueIsIgnored) {
  hsl_float_t a{0.0f, 0.0f, 0.5f};
  hsl_float_t b{180.0f, 0.0f, 0.5f};
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
