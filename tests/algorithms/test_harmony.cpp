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

}  // namespace colorcpp::algorithms::test
