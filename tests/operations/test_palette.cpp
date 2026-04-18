/**
 * @file test_palette.cpp
 * @brief Unit tests for palette operations.
 */

#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>
#include <cstddef>

namespace colorcpp::operations::test {

using namespace core;
using namespace palette;

// ============================================================================
// palette_set tests
// ============================================================================

TEST(PaletteSetTest, DefaultConstructor) {
  palette_set<rgb8_t> p;
  EXPECT_TRUE(p.empty());
  EXPECT_EQ(p.size(), 0u);
}

TEST(PaletteSetTest, InitializerListConstructor) {
  palette_set<rgb8_t> p{rgb8_t{255, 0, 0}, rgb8_t{0, 255, 0}, rgb8_t{0, 0, 255}};
  EXPECT_EQ(p.size(), 3u);
  EXPECT_FALSE(p.empty());
}

TEST(PaletteSetTest, AddColors) {
  palette_set<rgb8_t> p;
  p.add(rgb8_t{255, 0, 0});
  p.add(rgb8_t{0, 255, 0});
  EXPECT_EQ(p.size(), 2u);
}

TEST(PaletteSetTest, AtBoundsCheck) {
  palette_set<rgb8_t> p{rgb8_t{255, 0, 0}};
  EXPECT_NO_THROW(p.at(0));
  // volatile: stop constant propagation so GCC -Warray-bounds does not false-positive on EXPECT_THROW.
  volatile std::size_t out_of_range = 1;
  EXPECT_THROW(p.at(out_of_range), std::out_of_range);
}

TEST(PaletteSetTest, OperatorWraparound) {
  palette_set<rgb8_t> p{rgb8_t{255, 0, 0}, rgb8_t{0, 255, 0}, rgb8_t{0, 0, 255}};
  // Index 5 should wrap to 5 % 3 = 2
  EXPECT_EQ(p[5].r(), p[2].r());
  EXPECT_EQ(p[5].g(), p[2].g());
  EXPECT_EQ(p[5].b(), p[2].b());
}

TEST(PaletteSetTest, OperatorEmptySet) {
  palette_set<rgb8_t> p;
  // Should return default color for empty set
  rgb8_t c = p[0];
  EXPECT_EQ(c.r(), 0);
  EXPECT_EQ(c.g(), 0);
  EXPECT_EQ(c.b(), 0);
}

TEST(PaletteSetTest, ConstIteration) {
  const palette_set<rgb8_t> p{rgb8_t{255, 0, 0}, rgb8_t{0, 255, 0}};
  size_t count = 0;
  for (auto it = p.begin(); it != p.end(); ++it) {
    ++count;
  }
  EXPECT_EQ(count, 2u);
}

// ============================================================================
// details tests
// ============================================================================

TEST(DetailsTest, RotateHueNormal) {
  EXPECT_NEAR(details::rotate_hue(0.0f, 90.0f), 90.0f, 1e-4f);
  EXPECT_NEAR(details::rotate_hue(90.0f, 90.0f), 180.0f, 1e-4f);
  EXPECT_NEAR(details::rotate_hue(270.0f, 90.0f), 0.0f, 1e-4f);
}

TEST(DetailsTest, RotateHueNegative) {
  EXPECT_NEAR(details::rotate_hue(0.0f, -90.0f), 270.0f, 1e-4f);
  EXPECT_NEAR(details::rotate_hue(90.0f, -180.0f), 270.0f, 1e-4f);
}

TEST(DetailsTest, RotateHueNaN) {
  EXPECT_EQ(details::rotate_hue(std::numeric_limits<float>::quiet_NaN(), 90.0f), 0.0f);
  EXPECT_EQ(details::rotate_hue(90.0f, std::numeric_limits<float>::quiet_NaN()), 0.0f);
}

TEST(DetailsTest, RotateHueInf) {
  EXPECT_EQ(details::rotate_hue(std::numeric_limits<float>::infinity(), 90.0f), 0.0f);
  EXPECT_EQ(details::rotate_hue(90.0f, std::numeric_limits<float>::infinity()), 0.0f);
}

// ============================================================================
// Scales tests
// ============================================================================

TEST(ScalesTest, LinearScaleZero) {
  auto p = linear_scale(rgb8_t{255, 0, 0}, rgb8_t{0, 0, 255}, 0);
  EXPECT_TRUE(p.empty());
}

TEST(ScalesTest, LinearScaleOne) {
  auto p = linear_scale(rgb8_t{255, 0, 0}, rgb8_t{0, 0, 255}, 1);
  EXPECT_EQ(p.size(), 1u);
  EXPECT_EQ(p[0].r(), 255);
  EXPECT_EQ(p[0].g(), 0);
  EXPECT_EQ(p[0].b(), 0);
}

TEST(ScalesTest, LinearScaleTwo) {
  auto p = linear_scale(rgb8_t{255, 0, 0}, rgb8_t{0, 0, 255}, 2);
  EXPECT_EQ(p.size(), 2u);
  EXPECT_EQ(p[0].r(), 255);
  EXPECT_EQ(p[0].g(), 0);
  EXPECT_EQ(p[0].b(), 0);
  EXPECT_EQ(p[1].r(), 0);
  EXPECT_EQ(p[1].g(), 0);
  EXPECT_EQ(p[1].b(), 255);
}

TEST(ScalesTest, VisualScaleZero) {
  auto p = visual_scale(rgb8_t{255, 0, 0}, rgb8_t{0, 0, 255}, 0);
  EXPECT_TRUE(p.empty());
}

TEST(ScalesTest, PerceptualScaleZero) {
  auto p = perceptual_scale(rgb8_t{255, 0, 0}, rgb8_t{0, 0, 255}, 0);
  EXPECT_TRUE(p.empty());
}

// ============================================================================
// Harmony tests
// ============================================================================

TEST(HarmonyTest, Complementary) {
  rgb8_t base{255, 0, 0};  // Red
  auto p = schemes::complementary(base);
  EXPECT_EQ(p.size(), 2u);
  EXPECT_EQ(p[0].r(), base.r());
  EXPECT_EQ(p[0].g(), base.g());
  EXPECT_EQ(p[0].b(), base.b());
  // Second color should be cyan (complementary of red)
}

TEST(HarmonyTest, Analogous) {
  rgb8_t base{255, 0, 0};
  auto p = schemes::analogous(base);
  EXPECT_EQ(p.size(), 3u);
  EXPECT_EQ(p[1].r(), base.r());
  EXPECT_EQ(p[1].g(), base.g());
  EXPECT_EQ(p[1].b(), base.b());
}

TEST(HarmonyTest, AnalogousAngleZero) {
  rgb8_t base{255, 0, 0};
  auto p = schemes::analogous(base, 0.0f);
  EXPECT_EQ(p.size(), 1u);
  EXPECT_EQ(p[0].r(), base.r());
  EXPECT_EQ(p[0].g(), base.g());
  EXPECT_EQ(p[0].b(), base.b());
}

TEST(HarmonyTest, AnalogousAngleSmall) {
  rgb8_t base{255, 0, 0};
  auto p = schemes::analogous(base, 0.001f);
  EXPECT_EQ(p.size(), 1u);
  EXPECT_EQ(p[0].r(), base.r());
  EXPECT_EQ(p[0].g(), base.g());
  EXPECT_EQ(p[0].b(), base.b());
}

TEST(HarmonyTest, Triadic) {
  rgb8_t base{255, 0, 0};
  auto p = schemes::triadic(base);
  EXPECT_EQ(p.size(), 3u);
  EXPECT_EQ(p[0].r(), base.r());
  EXPECT_EQ(p[0].g(), base.g());
  EXPECT_EQ(p[0].b(), base.b());
}

TEST(HarmonyTest, SplitComplementary) {
  rgb8_t base{255, 0, 0};
  auto p = schemes::split_complementary(base);
  EXPECT_EQ(p.size(), 3u);
  EXPECT_EQ(p[0].r(), base.r());
  EXPECT_EQ(p[0].g(), base.g());
  EXPECT_EQ(p[0].b(), base.b());
}

TEST(HarmonyTest, Tetradic) {
  rgb8_t base{255, 0, 0};
  auto p = schemes::tetradic(base);
  EXPECT_EQ(p.size(), 4u);
  EXPECT_EQ(p[0].r(), base.r());
  EXPECT_EQ(p[0].g(), base.g());
  EXPECT_EQ(p[0].b(), base.b());
}

TEST(HarmonyTest, Square) {
  rgb8_t base{255, 0, 0};
  auto p = schemes::square(base);
  EXPECT_EQ(p.size(), 4u);
  EXPECT_EQ(p[0].r(), base.r());
  EXPECT_EQ(p[0].g(), base.g());
  EXPECT_EQ(p[0].b(), base.b());
}

TEST(HarmonyTest, Monochromatic) {
  rgb8_t base{128, 128, 128};
  auto p = schemes::monochromatic(base, 5);
  EXPECT_EQ(p.size(), 5u);
}

TEST(HarmonyTest, MonochromaticCustomRange) {
  rgb8_t base{128, 128, 128};
  auto p = schemes::monochromatic(base, 3, 0.0f, 1.0f);
  EXPECT_EQ(p.size(), 3u);
}

TEST(HarmonyTest, MonochromaticReversedRange) {
  // min > max should be swapped
  rgb8_t base{128, 128, 128};
  auto p = schemes::monochromatic(base, 3, 0.9f, 0.2f);
  EXPECT_EQ(p.size(), 3u);
}

TEST(HarmonyTest, MonochromaticClampedRange) {
  // Values outside [0, 1] should be clamped
  rgb8_t base{128, 128, 128};
  auto p = schemes::monochromatic(base, 3, -0.5f, 1.5f);
  EXPECT_EQ(p.size(), 3u);
}

// ============================================================================
// Rainbow tests
// ============================================================================

TEST(RainbowTest, DefaultCount) {
  auto p = rainbow<rgb8_t>();
  EXPECT_EQ(p.size(), 7u);
}

TEST(RainbowTest, CustomCount) {
  auto p = rainbow<rgb8_t>(12);
  EXPECT_EQ(p.size(), 12u);
}

TEST(RainbowTest, ZeroCount) {
  auto p = rainbow<rgb8_t>(0);
  EXPECT_TRUE(p.empty());
}

TEST(RainbowTest, SingleColor) {
  auto p = rainbow<rgb8_t>(1);
  EXPECT_EQ(p.size(), 1u);
}

TEST(RainbowTest, CustomSaturationLightness) {
  auto p = rainbow_custom<rgb8_t>(5, 0.8f, 0.6f);
  EXPECT_EQ(p.size(), 5u);
}

// ============================================================================
// Temperature tests
// ============================================================================

TEST(TemperatureTest, WarmPalette) {
  rgb8_t base{200, 100, 50};
  auto p = families::warm(base, 5);
  EXPECT_EQ(p.size(), 5u);
}

TEST(TemperatureTest, CoolPalette) {
  rgb8_t base{50, 100, 200};
  auto p = families::cool(base, 5);
  EXPECT_EQ(p.size(), 5u);
}

TEST(TemperatureTest, NeutralPalette) {
  rgb8_t base{128, 128, 128};
  auto p = families::neutral(base, 5);
  EXPECT_EQ(p.size(), 5u);
}

TEST(TemperatureTest, WarmPaletteDefaultCount) {
  rgb8_t base{255, 0, 0};
  auto p = families::warm(base);
  EXPECT_EQ(p.size(), 5u);
}

TEST(TemperatureTest, CoolPaletteDefaultCount) {
  rgb8_t base{0, 0, 255};
  auto p = families::cool(base);
  EXPECT_EQ(p.size(), 5u);
}

// ============================================================================
// Material Design tests
// ============================================================================

TEST(MaterialTest, Primary) {
  rgb8_t base{100, 50, 200};
  auto p = material_primary(base, 10);
  EXPECT_EQ(p.size(), 10u);
}

TEST(MaterialTest, Secondary) {
  rgb8_t base{100, 50, 200};
  auto p = material_secondary(base, 10);
  EXPECT_EQ(p.size(), 10u);
}

TEST(MaterialTest, Tertiary) {
  rgb8_t base{100, 50, 200};
  auto p = material_tertiary(base, 10);
  EXPECT_EQ(p.size(), 10u);
}

TEST(MaterialTest, Neutral) {
  rgb8_t base{100, 50, 200};
  auto p = material_neutral(base, 10);
  EXPECT_EQ(p.size(), 10u);
}

TEST(MaterialTest, Error) {
  auto p = material_error<rgb8_t>(10);
  EXPECT_EQ(p.size(), 10u);
}

TEST(MaterialTest, DesignSystem) {
  rgb8_t base{100, 50, 200};
  // Test individual components instead of full system
  auto primary = material_primary(base, 10);
  auto secondary = material_secondary(base, 10);
  auto tertiary = material_tertiary(base, 10);
  auto neutral = material_neutral(base, 10);
  auto error = material_error<rgb8_t>(10);

  EXPECT_EQ(primary.size(), 10u);
  EXPECT_EQ(secondary.size(), 10u);
  EXPECT_EQ(tertiary.size(), 10u);
  EXPECT_EQ(neutral.size(), 10u);
  EXPECT_EQ(error.size(), 10u);
}

// ============================================================================
// Advanced scales tests
// ============================================================================

TEST(AdvancedScalesTest, EasingScaleLinear) {
  auto p = easing_scale(rgb8_t{255, 0, 0}, rgb8_t{0, 0, 255}, 5, easing_type::linear);
  EXPECT_EQ(p.size(), 5u);
  // Linear should have equal spacing
  EXPECT_EQ(p[0].r(), 255);
  EXPECT_EQ(p[4].b(), 255);
}

TEST(AdvancedScalesTest, EasingScaleEaseIn) {
  auto p = easing_scale(rgb8_t{255, 0, 0}, rgb8_t{0, 0, 255}, 5, easing_type::ease_in);
  EXPECT_EQ(p.size(), 5u);
  // Ease-in should be slower at start
  auto p_linear = easing_scale(rgb8_t{255, 0, 0}, rgb8_t{0, 0, 255}, 5, easing_type::linear);
  // First step should be closer to start (higher r value) than linear
  EXPECT_GT(p[1].r(), p_linear[1].r());
}

TEST(AdvancedScalesTest, EasingScaleEaseOut) {
  auto p = easing_scale(rgb8_t{255, 0, 0}, rgb8_t{0, 0, 255}, 5, easing_type::ease_out);
  EXPECT_EQ(p.size(), 5u);
}

TEST(AdvancedScalesTest, EasingScaleBounce) {
  auto p = easing_scale(rgb8_t{255, 0, 0}, rgb8_t{0, 0, 255}, 10, easing_type::bounce);
  EXPECT_EQ(p.size(), 10u);
}

TEST(AdvancedScalesTest, SplineScaleBasic) {
  std::vector<rgb8_t> control;
  control.push_back(rgb8_t{255, 0, 0});    // Red
  control.push_back(rgb8_t{255, 255, 0});  // Yellow
  control.push_back(rgb8_t{0, 255, 0});    // Green
  control.push_back(rgb8_t{0, 0, 255});    // Blue
  auto p = spline_scale(control, 10);
  EXPECT_EQ(p.size(), 10u);
}

TEST(AdvancedScalesTest, SplineScaleCentripetal) {
  std::vector<rgb8_t> control;
  control.push_back(rgb8_t{255, 0, 0});
  control.push_back(rgb8_t{255, 255, 0});
  control.push_back(rgb8_t{0, 255, 0});
  control.push_back(rgb8_t{0, 0, 255});
  auto p = spline_scale(control, 10, interpolate::cr_mode::centripetal);
  EXPECT_EQ(p.size(), 10u);
}

TEST(AdvancedScalesTest, MultiScaleThreeColors) {
  std::vector<rgb8_t> colors;
  colors.push_back(rgb8_t{255, 0, 0});  // Red
  colors.push_back(rgb8_t{0, 255, 0});  // Green
  colors.push_back(rgb8_t{0, 0, 255});  // Blue
  auto p = multi_scale(colors, 9);
  EXPECT_EQ(p.size(), 9u);
}

TEST(AdvancedScalesTest, MultiScaleHSL) {
  std::vector<rgb8_t> colors;
  colors.push_back(rgb8_t{255, 0, 0});
  colors.push_back(rgb8_t{0, 0, 255});
  auto p = multi_scale(colors, 5, lerp_method::hsl);
  EXPECT_EQ(p.size(), 5u);
}

TEST(AdvancedScalesTest, EmptyInputs) {
  auto p1 = easing_scale(rgb8_t{255, 0, 0}, rgb8_t{0, 0, 255}, 0, easing_type::linear);
  EXPECT_TRUE(p1.empty());

  std::vector<rgb8_t> empty;
  auto p2 = spline_scale(empty, 5);
  EXPECT_TRUE(p2.empty());

  auto p3 = multi_scale(empty, 5);
  EXPECT_TRUE(p3.empty());
}

}  // namespace colorcpp::operations::test
