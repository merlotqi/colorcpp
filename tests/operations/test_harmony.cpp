/**
 * @file test_harmony.cpp
 * @brief Unit tests for color harmony module.
 */

#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

namespace colorcpp::operations::test {

using namespace core;
using namespace harmony;
using namespace palette;

// ============================================================================
// Harmony assessment tests
// ============================================================================

TEST(HarmonyAssessTest, ComplementaryPalette) {
  // Create a complementary palette (0° and 180°)
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});  // Red (0°)
  palette.add(rgbaf_t{0.0f, 1.0f, 1.0f, 1.0f});  // Cyan (180°)

  auto result = assess(palette);
  EXPECT_EQ(result.scheme, "complementary");
  EXPECT_GT(result.score, 80.0f);
}

TEST(HarmonyAssessTest, TriadicPalette) {
  // Create a triadic palette (0°, 120°, 240°)
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});  // Red (0°)
  palette.add(rgbaf_t{0.0f, 1.0f, 0.0f, 1.0f});  // Green (120°)
  palette.add(rgbaf_t{0.0f, 0.0f, 1.0f, 1.0f});  // Blue (240°)

  auto result = assess(palette);
  EXPECT_EQ(result.scheme, "triadic");
  EXPECT_GT(result.score, 80.0f);
}

TEST(HarmonyAssessTest, AnalogousPalette) {
  // Create an analogous palette (0°, 30°, 60°)
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});  // Red (0°)
  palette.add(rgbaf_t{1.0f, 0.5f, 0.0f, 1.0f});  // Orange (30°)
  palette.add(rgbaf_t{1.0f, 1.0f, 0.0f, 1.0f});  // Yellow (60°)

  auto result = assess(palette);
  EXPECT_EQ(result.scheme, "analogous");
  EXPECT_GT(result.score, 70.0f);
}

TEST(HarmonyAssessTest, SingleColorPalette) {
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});

  auto result = assess(palette);
  EXPECT_EQ(result.scheme, "single");
  EXPECT_EQ(result.score, 100.0f);
}

// ============================================================================
// Harmony correction tests
// ============================================================================

TEST(HarmonyCorrectTest, CorrectToComplementary) {
  // Create a palette with colors not quite complementary
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});   // Red (0°)
  palette.add(rgbaf_t{0.0f, 0.9f, 0.9f, 1.0f});   // Slightly off cyan

  auto corrected = correct(palette, "complementary");

  // Verify the corrected palette is more harmonious
  auto result = assess(corrected);
  EXPECT_EQ(result.scheme, "complementary");
  EXPECT_GT(result.score, 90.0f);
}

TEST(HarmonyCorrectTest, CorrectToTriadic) {
  // Create a palette with colors not quite triadic
  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});  // Red (0°)
  palette.add(rgbaf_t{0.1f, 1.0f, 0.1f, 1.0f});  // Slightly off green
  palette.add(rgbaf_t{0.1f, 0.1f, 1.0f, 1.0f});  // Slightly off blue

  auto corrected = correct(palette, "triadic");

  // Verify the corrected palette is more harmonious
  auto result = assess(corrected);
  EXPECT_EQ(result.scheme, "triadic");
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

}  // namespace colorcpp::operations::test
