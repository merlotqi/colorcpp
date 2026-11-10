/**
 * @file test_compare.cpp
 * @brief Unit tests for color comparison module.
 */

#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>
#include <functional>
#include <limits>
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

TEST(OperatorCompareTest, HueRingEquality) {
  // 0° and 360° are one hue by library convention: operator== must agree
  // with exact_equal and std::hash on the ring (issue #88).
  hsl_float_t a{0.0f, 0.8f, 0.5f};
  hsl_float_t b{360.0f, 0.8f, 0.5f};
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);
  oklch_t x{0.7f, 0.1f, 0.0f};
  oklch_t y{0.7f, 0.1f, 360.0f};
  EXPECT_TRUE(x == y);
}

TEST(OperatorCompareTest, PowerlessHueEquality) {
  // Both sides achromatic: the stored hue is atan2 noise, so == must not split
  // on it (issue #88) - the same equivalence exact_equal already has.
  oklch_t a{0.7f, 1.2e-7f, 90.0f};
  oklch_t b{0.7f, 1.2e-7f, 200.0f};
  EXPECT_TRUE(a == b);
  hsl_float_t x{0.0f, 0.0f, 0.5f};
  hsl_float_t y{180.0f, 0.0f, 0.5f};
  EXPECT_TRUE(x == y);
}

TEST(OperatorCompareTest, DistinctHuesStillDiffer) {
  // Guard: the ring rule must not merge genuinely different hues.
  hsl_float_t a{0.0f, 0.8f, 0.5f};
  hsl_float_t b{90.0f, 0.8f, 0.5f};
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
}

TEST(OperatorCompareTest, EqualityAgreesWithExactEqual) {
  // One equivalence everywhere: exact_equal(a, b) <=> a == b for same-type
  // colors (issue #88).
  const hsl_float_t pairs[][2] = {
      {hsl_float_t{0.0f, 0.8f, 0.5f}, hsl_float_t{360.0f, 0.8f, 0.5f}},
      {hsl_float_t{359.9f, 0.8f, 0.5f}, hsl_float_t{0.1f, 0.8f, 0.5f}},
      {hsl_float_t{0.0f, 0.0f, 0.5f}, hsl_float_t{200.0f, 0.0f, 0.5f}},
      {hsl_float_t{0.0f, 0.8f, 0.5f}, hsl_float_t{90.0f, 0.8f, 0.5f}},
      {hsl_float_t{400.0f, 0.8f, 0.5f}, hsl_float_t{40.0f, 0.8f, 0.5f}},
  };
  for (const auto& p : pairs) EXPECT_EQ(p[0] == p[1], exact_equal(p[0], p[1]));
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

TEST(ColorHashTest, HueRingHashMatches) {
  // Ring convention (issue #88): equal keys must hash equally, including the
  // 0° / 360° pair exact_equal already calls one hue.
  hsl_float_t a{0.0f, 0.8f, 0.5f};
  hsl_float_t b{360.0f, 0.8f, 0.5f};
  EXPECT_EQ(std::hash<hsl_float_t>{}(a), std::hash<hsl_float_t>{}(b));
  oklch_t x{0.7f, 0.1f, 0.0f};
  oklch_t y{0.7f, 0.1f, 360.0f};
  EXPECT_EQ(std::hash<oklch_t>{}(x), std::hash<oklch_t>{}(y));
}

TEST(ColorHashTest, PowerlessHueHashMatches) {
  // Achromatic hue is not part of the key (issue #88).
  oklch_t a{0.7f, 1.2e-7f, 90.0f};
  oklch_t b{0.7f, 1.2e-7f, 200.0f};
  EXPECT_EQ(std::hash<oklch_t>{}(a), std::hash<oklch_t>{}(b));
}

TEST(ColorHashTest, UnorderedMapFindsAcrossHueRepresentatives) {
  struct exact_key_eq {
    bool operator()(const hsl_float_t& a, const hsl_float_t& b) const { return exact_equal(a, b); }
  };
  std::unordered_map<hsl_float_t, int, std::hash<hsl_float_t>, exact_key_eq> exact_keyed;
  exact_keyed[hsl_float_t{0.0f, 0.8f, 0.5f}] = 42;
  auto it = exact_keyed.find(hsl_float_t{360.0f, 0.8f, 0.5f});
  ASSERT_NE(it, exact_keyed.end());
  EXPECT_EQ(it->second, 42);

  std::unordered_map<hsl_float_t, int> defaulted;
  defaulted[hsl_float_t{0.0f, 0.8f, 0.5f}] = 7;
  auto it2 = defaulted.find(hsl_float_t{360.0f, 0.8f, 0.5f});
  ASSERT_NE(it2, defaulted.end());
  EXPECT_EQ(it2->second, 7);
}

TEST(ColorOrderingTest, HueRingOrderingTies) {
  // std::map equivalence derives from <, so 0° and 360° must tie (issue #88).
  hsl_float_t a{0.0f, 0.8f, 0.5f};
  hsl_float_t b{360.0f, 0.8f, 0.5f};
  EXPECT_FALSE(a < b);
  EXPECT_FALSE(b < a);
}

TEST(ColorOrderingTest, OrderedMapFindsAcrossHueRepresentatives) {
  std::map<hsl_float_t, int> m;
  m[hsl_float_t{0.0f, 0.8f, 0.5f}] = 7;
  auto it = m.find(hsl_float_t{360.0f, 0.8f, 0.5f});
  ASSERT_NE(it, m.end());
  EXPECT_EQ(it->second, 7);
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

TEST(RelativeCompareTest, HueToleranceIsPositionInvariant) {
  // Issue #89: the hue bound used to scale with the raw hue magnitudes
  // (rel * max(|ha|, |hb|)), so the same 0.2° gap passed near 360 and failed
  // near 0 — the verdict depended on where the pair sat on the ring. The
  // relative part must scale with the ring span (rel * 360) instead.
  const hsl_float_t pairs[][2] = {
      {hsl_float_t{359.9f, 0.8f, 0.5f}, hsl_float_t{0.1f, 0.8f, 0.5f}},  // 0.2° across the seam
      {hsl_float_t{1.0f, 0.8f, 0.5f}, hsl_float_t{1.2f, 0.8f, 0.5f}},    // 0.2° near zero
      {hsl_float_t{90.0f, 0.8f, 0.5f}, hsl_float_t{90.2f, 0.8f, 0.5f}},  // 0.2° mid-ring
  };
  for (const auto& p : pairs) {
    // rel * 360 = 0.36 covers the 0.2° gap at every ring position ...
    EXPECT_TRUE(relative_equal(p[0], p[1], 1e-3f));
    // ... and rel * 360 = 0.0036 covers none of it, at any position.
    EXPECT_FALSE(relative_equal(p[0], p[1], 1e-5f));
    // The absolute floor keeps dominating when larger (isclose-style).
    EXPECT_TRUE(relative_equal(p[0], p[1], 0.0f, 0.3f));
  }
  // The 0.36 bound is tight on the top side too: a 0.5° gap must not sneak in.
  EXPECT_FALSE(relative_equal(hsl_float_t{90.0f, 0.8f, 0.5f}, hsl_float_t{90.5f, 0.8f, 0.5f}, 1e-3f));
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

TEST(ExactCompareTest, NearZeroChromaHueIsIgnored) {
  // Issue #51: real conversions leave ~1e-7 of chroma noise on grays (white's
  // OkLCH C is 6e-8, hue 90° from atan2 rounding). Exact-zero missed it, so
  // the same gray at different stored hues compared unequal. Chroma within
  // the shared epsilon must count as achromatic too. (Chroma values must be
  // equal here: the chroma channel itself is still compared exactly.)
  oklch_t a{0.7f, 1e-7f, 90.0f};
  oklch_t b{0.7f, 1e-7f, 200.0f};
  EXPECT_TRUE(exact_equal(a, b));
  EXPECT_TRUE(equal(a, b));
}

TEST(EpsilonCompareTest, NearZeroChromaHueIsIgnored) {
  // HSL gray from a conversion carries the same kind of near-zero saturation.
  hsl_float_t a{0.0f, 1e-6f, 0.5f};
  hsl_float_t b{180.0f, 5e-6f, 0.5f};
  EXPECT_TRUE(epsilon_equal(a, b, 0.01f));
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

// core::is_achromatic — the shared eps-based check (issue #51). Compare and
// every polar interpolator must agree on when a hue is powerless.

TEST(AchromaticHelperTest, EpsBasedChromaCheck) {
  // Real conversions leave atan2 noise on neutrals: white's OkLCH C is 6e-8
  // with a bogus 90° hue, mid-gray's C is ~1.2e-7.
  EXPECT_TRUE(core::is_achromatic(conversion::color_cast<oklch_t>(rgbf_t{1.0f, 1.0f, 1.0f})));
  EXPECT_TRUE(core::is_achromatic(oklch_t{0.7f, 1.2e-7f, 90.0f}));
  EXPECT_TRUE(core::is_achromatic(hsl_float_t{240.0f, 1e-6f, 0.5f}));
  EXPECT_FALSE(core::is_achromatic(oklch_t{0.452f, 0.313f, 264.0f}));
  EXPECT_FALSE(core::is_achromatic(hsl_float_t{240.0f, 0.5f, 0.5f}));
  // Models without a chroma channel are never achromatic.
  EXPECT_FALSE(core::is_achromatic(rgbf_t{0.5f, 0.5f, 0.5f}));
  // Caller-supplied epsilon overrides the default.
  EXPECT_TRUE(core::is_achromatic(oklch_t{0.7f, 0.01f, 90.0f}, 0.02f));
  EXPECT_FALSE(core::is_achromatic(oklch_t{0.7f, 0.01f, 90.0f}, 1e-3f));
}

TEST(EpsilonCompareTest, InfiniteHueTerminatesAndIsNotEqual) {
  // circular_diff used to wrap via `while (d > range) d -= range;`, which
  // never terminates for d = inf (inf - range is still inf).
  hsl_float_t a{std::numeric_limits<float>::infinity(), 0.8f, 0.5f};
  hsl_float_t b{0.0f, 0.8f, 0.5f};
  EXPECT_FALSE(epsilon_equal(a, b, 1.0f));
}

TEST(EpsilonCompareTest, HugeHueTerminatesAndIsNotEqual) {
  // A hue thousands of turns out is meaningless input; the reduction must
  // not take ~forever, and the colors must not compare equal.
  hsl_float_t a{1e38f, 0.8f, 0.5f};
  hsl_float_t b{0.0f, 0.8f, 0.5f};
  EXPECT_FALSE(epsilon_equal(a, b, 1.0f));
}

TEST(EpsilonCompareTest, MultiTurnHueStillWrapsExactly) {
  // Regression guard: in-range magnitudes keep the exact subtraction wrap.
  hsl_float_t a{725.5f, 0.8f, 0.5f};
  hsl_float_t b{5.5f, 0.8f, 0.5f};
  EXPECT_TRUE(epsilon_equal(a, b, 1e-3f));
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
