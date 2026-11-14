/**
 * @file test_harmony.cpp
 * @brief Unit tests for color harmony module.
 */

#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

namespace colorcpp::algorithms::test {

using namespace core;
using namespace harmony;
using namespace operations::palette;
using operations::conversion::color_cast;

namespace {

// Build a palette from exact HSL hues (S=1, L=0.5) so tests control the hue
// geometry precisely, independent of raw-RGB construction quirks.
palette_set<rgbaf_t> Hues(std::initializer_list<float> hues) {
  palette_set<rgbaf_t> p;
  for (float h : hues) {
    p.add(color_cast<rgbaf_t>(hsla_float_t{h, 1.0f, 0.5f, 1.0f}));
  }
  return p;
}

}  // namespace

// ============================================================================
// Harmony assessment tests
// ============================================================================

TEST(HarmonyAssessTest, ComplementaryPalette) {
  // Create a complementary palette (0° and 180°)
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});  // Red (0°)
  palette.add(rgbaf_t{0.0f, 1.0f, 1.0f, 1.0f});  // Cyan (180°)

  auto result = assess(palette);
  EXPECT_EQ(result.scheme, harmony_scheme::complementary);
  EXPECT_GT(result.score, 80.0f);
}

TEST(HarmonyRulesTest, TriadicRuleHasCanonicalOffsets) {
  auto rule = rule_for(harmony_scheme::triadic);
  ASSERT_EQ(rule.color_count, 3u);
  ASSERT_EQ(rule.ideal_steps.size(), 2u);
  ASSERT_EQ(rule.generation_offsets.size(), 3u);
  EXPECT_FLOAT_EQ(rule.ideal_steps[0], 120.0f);
  EXPECT_FLOAT_EQ(rule.ideal_steps[1], 120.0f);
  EXPECT_FLOAT_EQ(rule.generation_offsets[0], 0.0f);
  EXPECT_FLOAT_EQ(rule.generation_offsets[1], 120.0f);
  EXPECT_FLOAT_EQ(rule.generation_offsets[2], 240.0f);
}

TEST(HarmonyRulesTest, DetectSquareScheme) {
  const std::vector<float> diffs{90.0f, 90.0f, 90.0f, 90.0f};
  auto [scheme, steps] = detect_scheme(4, diffs);
  EXPECT_EQ(scheme, harmony_scheme::square);
  ASSERT_EQ(steps.size(), 3u);
  EXPECT_FLOAT_EQ(steps[0], 90.0f);
}

TEST(HarmonyAssessTest, TriadicPalette) {
  // Create a triadic palette (0°, 120°, 240°)
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});  // Red (0°)
  palette.add(rgbaf_t{0.0f, 1.0f, 0.0f, 1.0f});  // Green (120°)
  palette.add(rgbaf_t{0.0f, 0.0f, 1.0f, 1.0f});  // Blue (240°)

  auto result = assess(palette);
  EXPECT_EQ(result.scheme, harmony_scheme::triadic);
  EXPECT_GT(result.score, 80.0f);
}

TEST(HarmonyAssessTest, AnalogousPalette) {
  // Create an analogous palette (0°, 30°, 60°)
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});  // Red (0°)
  palette.add(rgbaf_t{1.0f, 0.5f, 0.0f, 1.0f});  // Orange (30°)
  palette.add(rgbaf_t{1.0f, 1.0f, 0.0f, 1.0f});  // Yellow (60°)

  auto result = assess(palette);
  EXPECT_EQ(result.scheme, harmony_scheme::analogous);
  EXPECT_GT(result.score, 70.0f);
}

TEST(HarmonyAssessTest, SingleColorPalette) {
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});

  auto result = assess(palette);
  EXPECT_EQ(result.scheme, harmony_scheme::single);
  EXPECT_EQ(result.score, 100.0f);
}

// ============================================================================
// Harmony correction tests
// ============================================================================

TEST(HarmonyCorrectTest, CorrectToComplementary) {
  // Create a palette with hues off the complementary relationship (0° and 170°)
  palette_set<rgbaf_t> palette;
  palette.add(color_cast<rgbaf_t>(hsla_float_t{0.0f, 1.0f, 0.5f, 1.0f}));
  palette.add(color_cast<rgbaf_t>(hsla_float_t{170.0f, 1.0f, 0.5f, 1.0f}));

  auto corrected = correct(palette, harmony_scheme::complementary);
  EXPECT_TRUE(corrected.applied());
  EXPECT_EQ(corrected.status, correction_status::corrected);
  EXPECT_EQ(corrected.detected_scheme, harmony_scheme::complementary);
  EXPECT_EQ(corrected.target_scheme, harmony_scheme::complementary);

  // Verify the corrected palette is more harmonious
  auto result = assess(corrected.palette);
  EXPECT_EQ(result.scheme, harmony_scheme::complementary);
  EXPECT_GT(result.score, 90.0f);
}

TEST(HarmonyCorrectTest, CorrectToTriadic) {
  // Create a palette with hues off the triadic relationship (0°, 130°, 250°)
  palette_set<rgbaf_t> palette;
  palette.add(color_cast<rgbaf_t>(hsla_float_t{0.0f, 1.0f, 0.5f, 1.0f}));
  palette.add(color_cast<rgbaf_t>(hsla_float_t{130.0f, 1.0f, 0.5f, 1.0f}));
  palette.add(color_cast<rgbaf_t>(hsla_float_t{250.0f, 1.0f, 0.5f, 1.0f}));

  auto corrected = correct(palette, harmony_scheme::triadic);
  EXPECT_TRUE(corrected.applied());
  EXPECT_EQ(corrected.status, correction_status::corrected);
  EXPECT_EQ(corrected.detected_scheme, harmony_scheme::triadic);
  EXPECT_EQ(corrected.target_scheme, harmony_scheme::triadic);

  // Verify the corrected palette is more harmonious
  auto result = assess(corrected.palette);
  EXPECT_EQ(result.scheme, harmony_scheme::triadic);
  EXPECT_GT(result.score, 90.0f);
}

TEST(HarmonyCorrectTest, SingleColorUnchanged) {
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});

  auto corrected = correct(palette);

  // Single color should remain unchanged
  EXPECT_FALSE(corrected.applied());
  EXPECT_EQ(corrected.status, correction_status::insufficient_colors);
  EXPECT_EQ(corrected.palette.size(), 1);
  EXPECT_FLOAT_EQ(corrected.palette[0].r(), 1.0f);
}

TEST(HarmonyCorrectTest, AlreadyAlignedPaletteReportsUnchanged) {
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});
  palette.add(rgbaf_t{0.0f, 1.0f, 1.0f, 1.0f});

  auto corrected = correct(palette, harmony_scheme::complementary);

  EXPECT_FALSE(corrected.applied());
  EXPECT_EQ(corrected.status, correction_status::unchanged);
  EXPECT_EQ(corrected.target_scheme, harmony_scheme::complementary);
}

TEST(HarmonyCorrectTest, UnsupportedSchemeReportsStatus) {
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});
  palette.add(rgbaf_t{1.0f, 0.5f, 0.0f, 1.0f});
  palette.add(rgbaf_t{1.0f, 1.0f, 0.0f, 1.0f});
  palette.add(rgbaf_t{0.0f, 1.0f, 0.0f, 1.0f});
  palette.add(rgbaf_t{0.0f, 0.0f, 1.0f, 1.0f});
  palette.add(rgbaf_t{0.5f, 0.0f, 1.0f, 1.0f});

  auto corrected = correct(palette);

  EXPECT_FALSE(corrected.applied());
  EXPECT_EQ(corrected.status, correction_status::unsupported_scheme);
  EXPECT_EQ(corrected.detected_scheme, harmony_scheme::custom);
}

// ============================================================================
// Harmony suggestion tests
// ============================================================================

TEST(HarmonySuggestTest, SuggestForBadHarmony) {
  // Create a palette with poor harmony
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});  // Red (0°)
  palette.add(rgbaf_t{0.8f, 0.2f, 0.0f, 1.0f});  // Red-orange (15°)

  auto suggestions = suggest(palette);

  // Should suggest adjustments
  EXPECT_FALSE(suggestions.empty());
}

TEST(HarmonySuggestTest, NoSuggestionForGoodHarmony) {
  // Create a harmonious complementary palette
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});  // Red (0°)
  palette.add(rgbaf_t{0.0f, 1.0f, 1.0f, 1.0f});  // Cyan (180°)

  auto suggestions = suggest(palette);

  // Should have no suggestions for harmonious palette
  EXPECT_TRUE(suggestions.empty());
}

TEST(HarmonySuggestTest, SuggestComplementary) {
  // Create a pair without complementary relationship
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});  // Red (0°)
  palette.add(rgbaf_t{1.0f, 1.0f, 0.0f, 1.0f});  // Yellow (60°)

  auto suggestions = suggest(palette);

  // Should suggest making it complementary
  EXPECT_FALSE(suggestions.empty());
  EXPECT_EQ(suggestions[0].color_index, 1);
  EXPECT_GT(suggestions[0].hue_adjustment, 100.0f);  // Should suggest adding ~120°
}

// ============================================================================
// New harmony scheme tests
// ============================================================================

TEST(HarmonyAssessTest, GoldenPalette2) {
  // Golden angle pair: 0° and 137.5° (build via HSL so RGB matches assess()'s rgb_to_hsl)
  palette_set<rgbaf_t> palette;
  palette.add(color_cast<rgbaf_t>(hsla_float_t{0.0f, 1.0f, 0.5f, 1.0f}));
  palette.add(color_cast<rgbaf_t>(hsla_float_t{137.5f, 1.0f, 0.5f, 1.0f}));

  auto result = assess(palette);
  EXPECT_EQ(result.scheme, harmony_scheme::golden);
  EXPECT_GT(result.score, 80.0f);
}

TEST(HarmonyAssessTest, GoldenPalette3) {
  // 3-stop golden: 0°, 137.5°, 275° (137.5 + 137.5 = 275)
  palette_set<rgbaf_t> palette;
  palette.add(color_cast<rgbaf_t>(hsla_float_t{0.0f, 1.0f, 0.5f, 1.0f}));
  palette.add(color_cast<rgbaf_t>(hsla_float_t{137.5f, 1.0f, 0.5f, 1.0f}));
  palette.add(color_cast<rgbaf_t>(hsla_float_t{275.0f, 1.0f, 0.5f, 1.0f}));

  auto result = assess(palette);
  EXPECT_EQ(result.scheme, harmony_scheme::golden);
  EXPECT_GT(result.score, 80.0f);
}

TEST(HarmonyAssessTest, SplitAnalogousPalette) {
  // Create a split analogous palette (0°, 60°, 90°)
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});  // Red (0°)
  palette.add(rgbaf_t{1.0f, 1.0f, 0.0f, 1.0f});  // Yellow (60°)
  palette.add(rgbaf_t{0.5f, 1.0f, 0.0f, 1.0f});  // Yellow-green (90°)

  auto result = assess(palette);
  EXPECT_EQ(result.scheme, harmony_scheme::split_analogous);
  EXPECT_GT(result.score, 70.0f);
}

TEST(HarmonyAssessTest, AnalogousWidePalette) {
  // Create a wide analogous palette (0°, 20°, 40°, 60°)
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});   // Red (0°)
  palette.add(rgbaf_t{1.0f, 0.33f, 0.0f, 1.0f});  // Orange (~20°)
  palette.add(rgbaf_t{1.0f, 0.67f, 0.0f, 1.0f});  // Orange (~40°)
  palette.add(rgbaf_t{1.0f, 1.0f, 0.0f, 1.0f});   // Yellow (60°)

  auto result = assess(palette);
  EXPECT_EQ(result.scheme, harmony_scheme::analogous_wide);
  EXPECT_GT(result.score, 70.0f);
}

TEST(HarmonyAssessTest, CompoundPalette) {
  // Create a compound palette (0°, 30°, 180°, 210°)
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});  // Red (0°)
  palette.add(rgbaf_t{1.0f, 0.5f, 0.0f, 1.0f});  // Orange (30°)
  palette.add(rgbaf_t{0.0f, 1.0f, 1.0f, 1.0f});  // Cyan (180°)
  palette.add(rgbaf_t{0.0f, 0.5f, 1.0f, 1.0f});  // Cyan-blue (210°)

  auto result = assess(palette);
  EXPECT_EQ(result.scheme, harmony_scheme::compound);
  EXPECT_GT(result.score, 70.0f);
}

TEST(HarmonyAssessTest, DoubleComplementaryPalette) {
  // Create a double complementary palette (0°, 60°, 180°, 240°)
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});  // Red (0°)
  palette.add(rgbaf_t{1.0f, 1.0f, 0.0f, 1.0f});  // Yellow (60°)
  palette.add(rgbaf_t{0.0f, 1.0f, 1.0f, 1.0f});  // Cyan (180°)
  palette.add(rgbaf_t{0.0f, 0.0f, 1.0f, 1.0f});  // Blue (240°)

  auto result = assess(palette);
  EXPECT_EQ(result.scheme, harmony_scheme::double_complementary);
  EXPECT_GT(result.score, 70.0f);
}

TEST(HarmonyCorrectTest, CorrectToGolden) {
  // Create a palette with colors not quite at golden angle
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});   // Red (0°)
  palette.add(rgbaf_t{0.0f, 0.25f, 1.0f, 1.0f});  // Slightly off golden angle

  auto corrected = correct(palette, harmony_scheme::golden);
  EXPECT_TRUE(corrected.applied());
  EXPECT_EQ(corrected.status, correction_status::corrected);

  auto result = assess(corrected.palette);
  EXPECT_EQ(result.scheme, harmony_scheme::golden);
  EXPECT_GE(result.score, 85.0f);

  auto second_h = color_cast<hsla_float_t>(corrected.palette.at(1));
  EXPECT_NEAR(second_h.h(), 137.5f, 1.0f);
}

TEST(HarmonyAssessTest, ComplementaryDeviationNotDilutedByWrap) {
  // A pair 10° off complementary has average deviation 10° -> score 90.
  // The wrap-around diff used to inflate the divisor, reporting dev=5, score=95.
  auto palette = Hues({0.0f, 170.0f});
  auto result = assess(palette);
  EXPECT_EQ(result.scheme, harmony_scheme::complementary);
  EXPECT_NEAR(result.deviation, 10.0f, 0.5f);
  EXPECT_NEAR(result.score, 90.0f, 0.5f);
}

TEST(HarmonyAssessTest, DetectionIsInsertionOrderIndependent) {
  // The same three hues in two insertion orders must detect the same scheme.
  auto sorted_order = Hues({0.0f, 30.0f, 60.0f});
  auto shuffled = Hues({0.0f, 60.0f, 30.0f});
  EXPECT_EQ(assess(sorted_order).scheme, harmony_scheme::analogous);
  EXPECT_EQ(assess(shuffled).scheme, harmony_scheme::analogous);
  EXPECT_EQ(assess_weighted(shuffled, harmony_weights::angle_only()).scheme, harmony_scheme::analogous);
}

TEST(HarmonySuggestTest, FallbackSuggestionReportsTrueHueDelta) {
  // Red (0°) + yellow (60°): pair, so the fallback targets complementary.
  // The needed adjustment is target(180°) - current(60°) = +120°, not the
  // absolute generation offset 180.
  auto palette = Hues({0.0f, 60.0f});
  auto suggestions = suggest(palette);
  ASSERT_FALSE(suggestions.empty());
  EXPECT_EQ(suggestions[0].color_index, 1);
  EXPECT_NEAR(suggestions[0].hue_adjustment, 120.0f, 1.0f);
  EXPECT_NE(suggestions[0].action.find("+120"), std::string::npos);
}

TEST(HarmonySuggestTest, TriadicFallbackReportsTrueHueDelta) {
  // {0°, 100°, 200°}: recognized as 'triad' (no named scheme), so the
  // fallback targets triadic. Needed deltas from base 0°: color 1 -> +20°
  // (120 - 100), color 2 -> +40° (240 - 200) — not the raw offsets 120/240.
  auto palette = Hues({0.0f, 100.0f, 200.0f});
  auto suggestions = suggest(palette);
  ASSERT_EQ(suggestions.size(), 2u);
  EXPECT_EQ(suggestions[0].color_index, 1);
  EXPECT_NEAR(suggestions[0].hue_adjustment, 20.0f, 1.0f);
  EXPECT_EQ(suggestions[1].color_index, 2);
  EXPECT_NEAR(suggestions[1].hue_adjustment, 40.0f, 1.0f);
}

TEST(HarmonyRulesTest, FiveColorAnalogousWideCheckedBeforeAnalogous) {
  // Canonical wide steps are 15°, canonical analogous steps are 22.5°.
  // The wide check must run first, otherwise 15° palettes match the
  // looser analogous threshold (<35) and analogous_wide is unreachable.
  auto wide = detect_scheme(5, {15.0f, 15.0f, 15.0f, 15.0f});
  EXPECT_EQ(wide.first, harmony_scheme::analogous_wide);

  auto narrow = detect_scheme(5, {22.5f, 22.5f, 22.5f, 22.5f});
  EXPECT_EQ(narrow.first, harmony_scheme::analogous);
}

TEST(HarmonyCorrectTest, ToleranceLeavesNearAlignedPaletteUnchanged) {
  // Hues 10° and 185°: complementary target is 190°, so the only needed
  // adjustment is +5°. With tolerance 30 the palette must stay untouched
  // (the old code assigned target hues regardless of tolerance).
  auto palette = Hues({10.0f, 185.0f});
  auto corrected = correct(palette, harmony_scheme::complementary, 30.0f);
  EXPECT_EQ(corrected.status, correction_status::unchanged);
  EXPECT_FALSE(corrected.applied());
  auto hsl = color_cast<hsla_float_t>(corrected.palette.at(1));
  EXPECT_NEAR(hsl.h(), 185.0f, 1.0f);
}

TEST(HarmonyCorrectTest, ToleranceExceededAdjustsOnlyOffendingColors) {
  // Same palette, tight tolerance: the +5° adjustment exceeds tolerance 2,
  // so that color is corrected and the status reports tolerance_exceeded.
  auto palette = Hues({10.0f, 185.0f});
  auto corrected = correct(palette, harmony_scheme::complementary, 2.0f);
  EXPECT_EQ(corrected.status, correction_status::tolerance_exceeded);
  EXPECT_TRUE(corrected.applied());
  auto hsl = color_cast<hsla_float_t>(corrected.palette.at(1));
  EXPECT_NEAR(hsl.h(), 190.0f, 1.0f);
}

TEST(HarmonyWeightedTest, OutOfRangeWeightsAreClampedBeforeScoring) {
  // harmony_weights::validate() clamps to [0, 1] but was never called;
  // out-of-range weights must score the same as their clamped values.
  // Component scores must differ (varied S/L and slightly off-triadic
  // hues), otherwise any weighting yields the same average.
  palette_set<rgbaf_t> palette;
  palette.add(color_cast<rgbaf_t>(hsla_float_t{0.0f, 1.0f, 0.5f, 1.0f}));
  palette.add(color_cast<rgbaf_t>(hsla_float_t{130.0f, 0.3f, 0.9f, 1.0f}));
  palette.add(color_cast<rgbaf_t>(hsla_float_t{250.0f, 0.8f, 0.2f, 1.0f}));
  auto raw = assess_weighted(palette, harmony_weights{2.0f, -1.0f, 0.5f});
  auto clamped = assess_weighted(palette, harmony_weights{1.0f, 0.0f, 0.5f});
  EXPECT_FLOAT_EQ(raw.score, clamped.score);
}

TEST(HarmonyWeightedTest, WeightedAngleScoreIsHueRotationInvariant) {
  // weights.hpp scored ideal_steps positionally just like assess(), so the
  // rotation that scrambled assess()'s score scrambled the weighted one too
  // (issue #103).
  const auto base = assess_weighted(Hues({0.0f, 150.0f, 210.0f}), harmony_weights::angle_only());
  const auto rotated = assess_weighted(Hues({200.0f, 350.0f, 50.0f}), harmony_weights::angle_only());
  EXPECT_EQ(base.scheme, harmony_scheme::split_complementary);
  EXPECT_EQ(rotated.scheme, harmony_scheme::split_complementary);
  EXPECT_NEAR(base.angle_score, rotated.angle_score, 1.0f) << "issue #103: angle score must not depend on rotation";
  EXPECT_GT(rotated.angle_score, 97.0f);
}

TEST(HarmonyCorrectTest, AutoDetectMatchesAssessOnReorderedPalette) {
  // Descending analogous palette: assess() must agree with correct()'s
  // auto-detection (both use shortest-path diffs on sorted hues).
  auto palette = Hues({60.0f, 30.0f, 0.0f});
  auto result = assess(palette);
  EXPECT_EQ(result.scheme, harmony_scheme::analogous);

  auto corrected = correct(palette);
  EXPECT_EQ(corrected.detected_scheme, harmony_scheme::analogous);
  EXPECT_EQ(corrected.target_scheme, harmony_scheme::analogous);
  // 0/30/60 is already a perfect analogous set — nothing to move
  EXPECT_EQ(corrected.status, correction_status::unchanged);
}

// ============================================================================
// Issue #103: detect_scheme matched fixed patterns against diffs[0]/diffs[1]
// and never the wrap entry, so which pattern fired depended on which gap
// sorted first — rotating every hue by a constant cyclically shifted the
// diff vector and changed the classification (e.g. split_complementary
// became an accidental golden match: |150 - 137.508| = 12.49 < tol 15).
// ============================================================================

TEST(HarmonyRulesTest, SplitComplementarySurvivesRotation) {
  // The issue probe: the same relative layout at two absolute rotations.
  auto base = detect_scheme_from_hues({0.0f, 150.0f, 210.0f});
  EXPECT_EQ(base.first, harmony_scheme::split_complementary);
  auto rotated = detect_scheme_from_hues({200.0f, 350.0f, 50.0f});
  EXPECT_EQ(rotated.first, harmony_scheme::split_complementary)
      << "issue #103: +200 deg rotation must not fall into golden";
}

TEST(HarmonyRulesTest, DetectSchemeIsHueRotationInvariant) {
  // Every canonical layout must classify identically at every rotation;
  // the rotation class is exactly the cyclic shifts of the diff vector.
  const std::vector<std::vector<float>> bases = {
      {0.0f, 180.0f},                      // complementary
      {0.0f, 137.5f},                      // golden pair
      {0.0f, 120.0f, 240.0f},              // triadic
      {0.0f, 137.5f, 275.0f},              // golden triad
      {0.0f, 30.0f, 60.0f},                // analogous
      {0.0f, 150.0f, 210.0f},              // split complementary
      {0.0f, 60.0f, 90.0f},                // split analogous
      {0.0f, 90.0f, 180.0f, 270.0f},       // square
      {0.0f, 30.0f, 180.0f, 210.0f},       // compound
      {0.0f, 60.0f, 180.0f, 240.0f},       // double complementary
      {0.0f, 20.0f, 40.0f, 60.0f},         // analogous wide (4)
      {0.0f, 15.0f, 30.0f, 45.0f},         // analogous wide (5)
      {0.0f, 22.5f, 45.0f, 67.5f, 90.0f},  // analogous (5)
  };
  for (const auto& base : bases) {
    const auto expected = detect_scheme_from_hues(base);
    EXPECT_NE(expected.first, harmony_scheme::custom) << "base[0]=" << base[0];
    for (float rot : {37.0f, 123.0f, 200.0f, 319.0f}) {
      std::vector<float> rotated;
      rotated.reserve(base.size());
      for (float h : base) {
        float r = h + rot;
        if (r >= 360.0f) r -= 360.0f;
        rotated.push_back(r);
      }
      const auto got = detect_scheme_from_hues(rotated);
      EXPECT_EQ(got.first, expected.first) << "issue #103: base hue0=" << base[0] << " rot=" << rot;
    }
  }
}

TEST(HarmonyAssessTest, ScoreIsHueRotationInvariant) {
  // assess() scores positionally against ideal_angles, so even a fixed
  // detect_scheme would still report rotation-dependent scores.
  const auto base = assess(Hues({0.0f, 150.0f, 210.0f}));
  const auto rotated = assess(Hues({200.0f, 350.0f, 50.0f}));
  EXPECT_EQ(base.scheme, harmony_scheme::split_complementary);
  EXPECT_EQ(rotated.scheme, harmony_scheme::split_complementary);
  EXPECT_NEAR(base.score, rotated.score, 1.0f) << "issue #103: score must not depend on rotation";
  EXPECT_GT(rotated.score, 97.0f);
}

// ============================================================================
// Issue #104: tetradic detection unreachable - rule_for(tetradic,4) is
// byte-identical to double_complementary. Chosen resolution: tetradic is a
// generation-only alias of the rectangle layout (two complementary pairs
// 0°/60°/180°/240°); detect_scheme must never claim tetradic, and the
// canonical rectangle always classifies as double_complementary.
// ============================================================================

TEST(HarmonyRulesTest, TetradicRuleIsExactlyDoubleComplementary) {
  // The two names describe one geometry; the rules must stay identical so
  // generation through either enum produces the same palette (issue #104).
  const auto tetradic = rule_for(harmony_scheme::tetradic, 4);
  const auto dbl = rule_for(harmony_scheme::double_complementary, 4);
  EXPECT_EQ(canonical_color_count(harmony_scheme::tetradic), 4u);
  EXPECT_EQ(tetradic.ideal_steps, dbl.ideal_steps);
  EXPECT_EQ(tetradic.generation_offsets, dbl.generation_offsets);
}

TEST(HarmonyRulesTest, RectangleDetectsAsDoubleComplementaryNeverTetradic) {
  // Detection must not distinguish what the rules cannot distinguish: the
  // rectangle layout (at any rotation) is double_complementary, never
  // tetradic, and an unmatched 4-hue palette falls back to the generic
  // "tetrad" label - also not the tetradic enum (issue #104).
  const std::vector<std::vector<float>> rectangles = {
      {0.0f, 60.0f, 180.0f, 240.0f},
      {13.0f, 73.0f, 193.0f, 253.0f},
      {200.0f, 260.0f, 20.0f, 80.0f},
  };
  for (const auto& rect : rectangles) {
    auto [scheme, steps] = detect_scheme_from_hues(rect);
    EXPECT_EQ(scheme, harmony_scheme::double_complementary);
    EXPECT_NE(scheme, harmony_scheme::tetradic);
    EXPECT_EQ(steps, rule_for(harmony_scheme::double_complementary, 4).ideal_steps);
  }
  // Gaps {50, 90, 120, 100} match no 4-color pattern -> generic tetrad
  // fallback, not the tetradic enum.
  auto unmatched = detect_scheme_from_hues({0.0f, 50.0f, 140.0f, 260.0f});
  EXPECT_EQ(unmatched.first, harmony_scheme::tetrad);
  EXPECT_NE(unmatched.first, harmony_scheme::tetradic);
}

TEST(HarmonyRulesTest, TetradicGenerationUsesRectangleOffsets) {
  // Generation through the tetradic enum must remain reachable and produce
  // the double_complementary offsets {0, 60, 180, 240} (issue #104).
  const auto base = color_cast<rgbaf_t>(hsla_float_t{40.0f, 1.0f, 0.5f, 1.0f});
  auto p = schemes::tetradic(base);
  ASSERT_EQ(p.size(), 4u);
  const std::vector<float> expected_hues = {40.0f, 100.0f, 220.0f, 280.0f};
  for (size_t i = 0; i < 4; ++i) {
    auto hsl = color_cast<hsla_float_t>(p[i]);
    EXPECT_NEAR(hsl.h(), expected_hues[i], 0.5f) << "index " << i;
  }
  // Round-trip: the generated palette classifies as double_complementary.
  std::vector<float> generated_hues;
  for (const auto& c : p) {
    generated_hues.push_back(color_cast<hsla_float_t>(c).h());
  }
  auto detected = detect_scheme_from_hues(generated_hues);
  EXPECT_EQ(detected.first, harmony_scheme::double_complementary);
}

}  // namespace colorcpp::algorithms::test
