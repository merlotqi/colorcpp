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
  // Create a palette with colors not quite complementary
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});  // Red (0°)
  palette.add(rgbaf_t{0.0f, 0.9f, 0.9f, 1.0f});  // Slightly off cyan

  auto corrected = correct(palette, harmony_scheme::complementary);

  // Verify the corrected palette is more harmonious
  auto result = assess(corrected);
  EXPECT_EQ(result.scheme, harmony_scheme::complementary);
  EXPECT_GT(result.score, 90.0f);
}

TEST(HarmonyCorrectTest, CorrectToTriadic) {
  // Create a palette with colors not quite triadic
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});  // Red (0°)
  palette.add(rgbaf_t{0.1f, 1.0f, 0.1f, 1.0f});  // Slightly off green
  palette.add(rgbaf_t{0.1f, 0.1f, 1.0f, 1.0f});  // Slightly off blue

  auto corrected = correct(palette, harmony_scheme::triadic);

  // Verify the corrected palette is more harmonious
  auto result = assess(corrected);
  EXPECT_EQ(result.scheme, harmony_scheme::triadic);
  EXPECT_GT(result.score, 90.0f);
}

TEST(HarmonyCorrectTest, SingleColorUnchanged) {
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});

  auto corrected = correct(palette);

  // Single color should remain unchanged
  EXPECT_EQ(corrected.size(), 1);
  EXPECT_FLOAT_EQ(corrected[0].r(), 1.0f);
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

  auto result = assess(corrected);
  EXPECT_EQ(result.scheme, harmony_scheme::golden);
  EXPECT_GE(result.score, 85.0f);

  auto second_h = color_cast<hsla_float_t>(corrected.at(1));
  EXPECT_NEAR(second_h.h(), 137.5f, 1.0f);
}

}  // namespace colorcpp::algorithms::test
