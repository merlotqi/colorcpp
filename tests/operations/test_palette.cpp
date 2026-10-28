/**
 * @file test_palette.cpp
 * @brief Unit tests for palette operations.
 */

#include <gtest/gtest.h>

#include <cmath>
#include <colorcpp/colorcpp.hpp>
#include <colorcpp/operations/palette/ios.hpp>
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace colorcpp::operations::test {

using namespace core;
using namespace operations::palette;

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
  // Wrap indexing is meaningless on an empty set: accessing must throw, not
  // fabricate a default color that is not part of any palette.
  // volatile: stop constant propagation so GCC -Warray-bounds does not false-positive on EXPECT_THROW.
  volatile std::size_t index = 0;
  EXPECT_THROW(p[index], std::out_of_range);
}

TEST(PaletteSetTest, VectorConversionIsExplicit) {
  const palette_set<rgb8_t> p{rgb8_t{1, 2, 3}};
  // Implicit conversion is disabled on purpose (it silently copies); spell the cast.
  const std::vector<rgb8_t> v = static_cast<std::vector<rgb8_t>>(p);
  EXPECT_EQ(v.size(), 1u);
  EXPECT_EQ(v[0].r(), 1);
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
  auto p = warm(base, 5);
  EXPECT_EQ(p.size(), 5u);
}

TEST(TemperatureTest, CoolPalette) {
  rgb8_t base{50, 100, 200};
  auto p = cool(base, 5);
  EXPECT_EQ(p.size(), 5u);
}

TEST(TemperatureTest, NeutralPalette) {
  rgb8_t base{128, 128, 128};
  auto p = neutral(base, 5);
  EXPECT_EQ(p.size(), 5u);
}

TEST(TemperatureTest, WarmPaletteDefaultCount) {
  rgb8_t base{255, 0, 0};
  auto p = warm(base);
  EXPECT_EQ(p.size(), 5u);
}

TEST(TemperatureTest, CoolPaletteDefaultCount) {
  rgb8_t base{0, 0, 255};
  auto p = cool(base);
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

// ============================================================================
// Boundary-condition regression guards
// ============================================================================

TEST(AdvancedScalesTest, MultiScaleCountSmallerThanSegmentsTerminates) {
  // count < colors.size()-1 used to make the last segment's sample count
  // underflow (0 - 1 -> SIZE_MAX), hanging the process.
  std::vector<rgb8_t> colors{rgb8_t(255, 0, 0), rgb8_t(255, 255, 0), rgb8_t(0, 255, 0), rgb8_t(0, 0, 255)};
  auto p = multi_scale(colors, 2);
  EXPECT_EQ(p.size(), 2u);
  for (const auto& c : p) {
    EXPECT_TRUE(std::isfinite(static_cast<float>(c.r())));
  }
}

TEST(AdvancedScalesTest, SplineScaleCountSmallerThanSegmentsTerminates) {
  std::vector<rgb8_t> colors{rgb8_t(255, 0, 0), rgb8_t(255, 255, 0), rgb8_t(0, 255, 0), rgb8_t(0, 0, 255)};
  auto p = spline_scale(colors, 2);
  EXPECT_EQ(p.size(), 2u);
}

TEST(AdvancedScalesTest, SplineScaleCountOneIsFirstControlPoint) {
  // The 2-point linear fallback computed t = 0/(1-1) = NaN; >=3 points hung.
  std::vector<rgb8_t> two{rgb8_t(255, 0, 0), rgb8_t(0, 0, 255)};
  auto p1 = spline_scale(two, 1);
  ASSERT_EQ(p1.size(), 1u);
  EXPECT_EQ(p1.at(0).r(), 255);

  std::vector<rgb8_t> four{rgb8_t(255, 0, 0), rgb8_t(255, 255, 0), rgb8_t(0, 255, 0), rgb8_t(0, 0, 255)};
  auto p2 = spline_scale(four, 1);
  ASSERT_EQ(p2.size(), 1u);
  EXPECT_EQ(p2.at(0).r(), 255);
}

TEST(AdvancedScalesTest, ApplyEasingElasticStaysInUnitInterval) {
  // Elastic overshoots t>1 (~1.35), but every lerp clamps t to [0, 1], so the
  // overshoot silently collapsed onto the end color. apply_easing must honor
  // its [0, 1] contract itself.
  for (int i = 0; i <= 20; ++i) {
    float t = static_cast<float>(i) / 20.0f;
    float e = details::apply_easing(t, easing_type::elastic);
    EXPECT_GE(e, 0.0f);
    EXPECT_LE(e, 1.0f);
  }
}

TEST(FamilyScaleTest, NeutralPreservesBaseAlpha) {
  // warm()/cool() keep the base alpha; neutral() used to hardcode 1.0.
  rgbaf_t base(1.0f, 0.5f, 0.2f, 0.42f);
  auto n = neutral(base);
  ASSERT_FALSE(n.empty());
  EXPECT_NEAR(n[0].a(), 0.42f, 1e-3f);
  EXPECT_NEAR(n[n.size() - 1].a(), 0.42f, 1e-3f);
}

TEST(MaterialTest, DesignSystemAcceptsFloatColor) {
  // material_design<Color> is a template but assigns palette_set<Color> into
  // palette_set<rgba8_t> members; any non-rgba8 Color must still work.
  rgbaf_t base(0.2f, 0.4f, 0.8f, 1.0f);
  auto palette = material_design(base, 3);
  EXPECT_EQ(palette.primary.size(), 3u);
  EXPECT_EQ(palette.secondary.size(), 3u);
  EXPECT_EQ(palette.error.size(), 3u);
}

TEST(HarmonyTest, SchemesMakeThrowsOnMismatchedCount) {
  // make() with a count that has no canonical rule used to return an empty
  // palette with no diagnostics.
  EXPECT_THROW(schemes::make(rgb8_t(255, 0, 0), algorithms::harmony::harmony_scheme::complementary, 5),
               std::invalid_argument);
  EXPECT_THROW(schemes::make(rgb8_t(255, 0, 0), algorithms::harmony::harmony_scheme::custom), std::invalid_argument);
  // Valid calls still succeed.
  EXPECT_NO_THROW(schemes::make(rgb8_t(255, 0, 0), algorithms::harmony::harmony_scheme::complementary));
  EXPECT_EQ(schemes::complementary(rgb8_t(255, 0, 0)).size(), 2u);
}

// ============================================================================
// iOS palette tests
// ============================================================================

TEST(IOSTest, LightModePaletteComplete) {
  auto p = ios_light_mode();

  // System primary colors
  EXPECT_EQ(p.system_red, (rgba8_t{0xFF, 0x3B, 0x30, 0xFF}));
  EXPECT_EQ(p.system_orange, (rgba8_t{0xFF, 0x95, 0x00, 0xFF}));
  EXPECT_EQ(p.system_yellow, (rgba8_t{0xFF, 0xCC, 0x00, 0xFF}));
  EXPECT_EQ(p.system_green, (rgba8_t{0x34, 0xC7, 0x59, 0xFF}));
  EXPECT_EQ(p.system_mint, (rgba8_t{0x00, 0xC7, 0xBE, 0xFF}));
  EXPECT_EQ(p.system_teal, (rgba8_t{0x30, 0xB0, 0xC7, 0xFF}));
  EXPECT_EQ(p.system_cyan, (rgba8_t{0x32, 0xAD, 0xE6, 0xFF}));
  EXPECT_EQ(p.system_blue, (rgba8_t{0x00, 0x7A, 0xFF, 0xFF}));
  EXPECT_EQ(p.system_indigo, (rgba8_t{0x58, 0x56, 0xD6, 0xFF}));
  EXPECT_EQ(p.system_purple, (rgba8_t{0xAF, 0x52, 0xDE, 0xFF}));
  EXPECT_EQ(p.system_pink, (rgba8_t{0xFF, 0x2D, 0x55, 0xFF}));
  EXPECT_EQ(p.system_brown, (rgba8_t{0xA2, 0x84, 0x5E, 0xFF}));
}

TEST(IOSTest, LightModeGrayScale) {
  auto p = ios_light_mode();

  EXPECT_EQ(p.system_gray, (rgba8_t{0x8E, 0x8E, 0x93, 0xFF}));
  EXPECT_EQ(p.system_gray2, (rgba8_t{0xAE, 0xAE, 0xB2, 0xFF}));
  EXPECT_EQ(p.system_gray3, (rgba8_t{0xC7, 0xC7, 0xCC, 0xFF}));
  EXPECT_EQ(p.system_gray4, (rgba8_t{0xD1, 0xD1, 0xD6, 0xFF}));
  EXPECT_EQ(p.system_gray5, (rgba8_t{0xE5, 0xE5, 0xEA, 0xFF}));
  EXPECT_EQ(p.system_gray6, (rgba8_t{0xF2, 0xF2, 0xF7, 0xFF}));
}

TEST(IOSTest, LightModeLabels) {
  auto p = ios_light_mode();

  EXPECT_EQ(p.label, (rgba8_t{0x00, 0x00, 0x00, 0xFF}));
  EXPECT_EQ(p.secondary_label, (rgba8_t{0x3C, 0x3C, 0x43, 0x99}));
  EXPECT_EQ(p.tertiary_label, (rgba8_t{0x3C, 0x3C, 0x43, 0x4C}));
  EXPECT_EQ(p.quaternary_label, (rgba8_t{0x3C, 0x3C, 0x43, 0x2E}));
}

TEST(IOSTest, LightModeFills) {
  auto p = ios_light_mode();

  EXPECT_EQ(p.system_fill, (rgba8_t{0x78, 0x78, 0x80, 0x33}));
  EXPECT_EQ(p.secondary_system_fill, (rgba8_t{0x78, 0x78, 0x80, 0x28}));
  EXPECT_EQ(p.tertiary_system_fill, (rgba8_t{0x76, 0x76, 0x80, 0x1E}));
  EXPECT_EQ(p.quaternary_system_fill, (rgba8_t{0x74, 0x74, 0x80, 0x14}));
}

TEST(IOSTest, LightModeBackgrounds) {
  auto p = ios_light_mode();

  EXPECT_EQ(p.system_background, (rgba8_t{0xFF, 0xFF, 0xFF, 0xFF}));
  EXPECT_EQ(p.secondary_system_background, (rgba8_t{0xF2, 0xF2, 0xF7, 0xFF}));
  EXPECT_EQ(p.tertiary_system_background, (rgba8_t{0xFF, 0xFF, 0xFF, 0xFF}));
  EXPECT_EQ(p.system_grouped_background, (rgba8_t{0xF2, 0xF2, 0xF7, 0xFF}));
  EXPECT_EQ(p.secondary_system_grouped_background, (rgba8_t{0xFF, 0xFF, 0xFF, 0xFF}));
  EXPECT_EQ(p.tertiary_system_grouped_background, (rgba8_t{0xF2, 0xF2, 0xF7, 0xFF}));
}

TEST(IOSTest, LightModeSeparators) {
  auto p = ios_light_mode();

  EXPECT_EQ(p.separator, (rgba8_t{0x3C, 0x3C, 0x43, 0x49}));
  EXPECT_EQ(p.opaque_separator, (rgba8_t{0xC6, 0xC6, 0xC8, 0xFF}));
}

TEST(IOSTest, DarkModePaletteComplete) {
  auto p = ios_dark_mode();

  // System primary colors (dark mode adjustments)
  EXPECT_EQ(p.system_red, (rgba8_t{0xFF, 0x45, 0x3A, 0xFF}));
  EXPECT_EQ(p.system_orange, (rgba8_t{0xFF, 0x9F, 0x0A, 0xFF}));
  EXPECT_EQ(p.system_yellow, (rgba8_t{0xFF, 0xD6, 0x0A, 0xFF}));
  EXPECT_EQ(p.system_green, (rgba8_t{0x30, 0xD1, 0x58, 0xFF}));
  EXPECT_EQ(p.system_mint, (rgba8_t{0x63, 0xDA, 0xC0, 0xFF}));
  EXPECT_EQ(p.system_teal, (rgba8_t{0x40, 0xCD, 0xC8, 0xFF}));
  EXPECT_EQ(p.system_cyan, (rgba8_t{0x64, 0xD2, 0xFF, 0xFF}));
  EXPECT_EQ(p.system_blue, (rgba8_t{0x0A, 0x84, 0xFF, 0xFF}));
  EXPECT_EQ(p.system_indigo, (rgba8_t{0x5E, 0x5C, 0xE6, 0xFF}));
  EXPECT_EQ(p.system_purple, (rgba8_t{0xBF, 0x5A, 0xF2, 0xFF}));
  EXPECT_EQ(p.system_pink, (rgba8_t{0xFF, 0x37, 0x5F, 0xFF}));
  EXPECT_EQ(p.system_brown, (rgba8_t{0xAC, 0x8E, 0x68, 0xFF}));
}

TEST(IOSTest, DarkModeGrayScale) {
  auto p = ios_dark_mode();

  EXPECT_EQ(p.system_gray, (rgba8_t{0x8E, 0x8E, 0x93, 0xFF}));
  EXPECT_EQ(p.system_gray2, (rgba8_t{0x63, 0x63, 0x66, 0xFF}));
  EXPECT_EQ(p.system_gray3, (rgba8_t{0x48, 0x48, 0x4A, 0xFF}));
  EXPECT_EQ(p.system_gray4, (rgba8_t{0x3A, 0x3A, 0x3C, 0xFF}));
  EXPECT_EQ(p.system_gray5, (rgba8_t{0x2C, 0x2C, 0x2E, 0xFF}));
  EXPECT_EQ(p.system_gray6, (rgba8_t{0x1C, 0x1C, 0x1E, 0xFF}));
}

TEST(IOSTest, DarkModeLabels) {
  auto p = ios_dark_mode();

  EXPECT_EQ(p.label, (rgba8_t{0xFF, 0xFF, 0xFF, 0xFF}));
  EXPECT_EQ(p.secondary_label, (rgba8_t{0xEB, 0xEB, 0xF5, 0x99}));
  EXPECT_EQ(p.tertiary_label, (rgba8_t{0xEB, 0xEB, 0xF5, 0x4C}));
  EXPECT_EQ(p.quaternary_label, (rgba8_t{0xEB, 0xEB, 0xF5, 0x2E}));
}

TEST(IOSTest, DarkModeFills) {
  auto p = ios_dark_mode();

  EXPECT_EQ(p.system_fill, (rgba8_t{0x78, 0x78, 0x80, 0x5C}));
  EXPECT_EQ(p.secondary_system_fill, (rgba8_t{0x78, 0x78, 0x80, 0x51}));
  EXPECT_EQ(p.tertiary_system_fill, (rgba8_t{0x76, 0x76, 0x80, 0x3D}));
  EXPECT_EQ(p.quaternary_system_fill, (rgba8_t{0x74, 0x74, 0x80, 0x2E}));
}

TEST(IOSTest, DarkModeBackgrounds) {
  auto p = ios_dark_mode();

  EXPECT_EQ(p.system_background, (rgba8_t{0x00, 0x00, 0x00, 0xFF}));
  EXPECT_EQ(p.secondary_system_background, (rgba8_t{0x1C, 0x1C, 0x1E, 0xFF}));
  EXPECT_EQ(p.tertiary_system_background, (rgba8_t{0x2C, 0x2C, 0x2E, 0xFF}));
  EXPECT_EQ(p.system_grouped_background, (rgba8_t{0x00, 0x00, 0x00, 0xFF}));
  EXPECT_EQ(p.secondary_system_grouped_background, (rgba8_t{0x1C, 0x1C, 0x1E, 0xFF}));
  EXPECT_EQ(p.tertiary_system_grouped_background, (rgba8_t{0x2C, 0x2C, 0x2E, 0xFF}));
}

TEST(IOSTest, DarkModeSeparators) {
  auto p = ios_dark_mode();

  EXPECT_EQ(p.separator, (rgba8_t{0x54, 0x54, 0x58, 0x99}));
  EXPECT_EQ(p.opaque_separator, (rgba8_t{0x38, 0x38, 0x3A, 0xFF}));
}

TEST(IOSTest, FromSeedLightMode) {
  rgb8_t seed{100, 150, 200};
  auto p = ios_from_seed(seed, false);

  // Grayscale should match light mode values
  EXPECT_EQ(p.system_gray, (rgba8_t{0x8E, 0x8E, 0x93, 0xFF}));
  EXPECT_EQ(p.system_gray6, (rgba8_t{0xF2, 0xF2, 0xF7, 0xFF}));

  // Labels should match light mode values
  EXPECT_EQ(p.label, (rgba8_t{0x00, 0x00, 0x00, 0xFF}));
  EXPECT_EQ(p.secondary_label, (rgba8_t{0x3C, 0x3C, 0x43, 0x99}));

  // Backgrounds should match light mode values
  EXPECT_EQ(p.system_background, (rgba8_t{0xFF, 0xFF, 0xFF, 0xFF}));
  EXPECT_EQ(p.secondary_system_background, (rgba8_t{0xF2, 0xF2, 0xF7, 0xFF}));
}

TEST(IOSTest, FromSeedDarkMode) {
  rgb8_t seed{100, 150, 200};
  auto p = ios_from_seed(seed, true);

  // Grayscale should match dark mode values
  EXPECT_EQ(p.system_gray, (rgba8_t{0x8E, 0x8E, 0x93, 0xFF}));
  EXPECT_EQ(p.system_gray6, (rgba8_t{0x1C, 0x1C, 0x1E, 0xFF}));

  // Labels should match dark mode values
  EXPECT_EQ(p.label, (rgba8_t{0xFF, 0xFF, 0xFF, 0xFF}));
  EXPECT_EQ(p.secondary_label, (rgba8_t{0xEB, 0xEB, 0xF5, 0x99}));

  // Backgrounds should match dark mode values
  EXPECT_EQ(p.system_background, (rgba8_t{0x00, 0x00, 0x00, 0xFF}));
  EXPECT_EQ(p.secondary_system_background, (rgba8_t{0x1C, 0x1C, 0x1E, 0xFF}));
}

TEST(IOSTest, FromSeedWarmColors) {
  rgb8_t seed{255, 0, 0};
  auto p = ios_from_seed(seed, false);

  // Warm colors should have valid values
  EXPECT_GT(p.system_red.r(), 0);
  EXPECT_GT(p.system_orange.r(), 128);
  EXPECT_GT(p.system_yellow.r(), 128);
}

TEST(IOSTest, FromSeedCoolColors) {
  rgb8_t seed{0, 0, 255};
  auto p = ios_from_seed(seed, false);

  // Cool colors should have valid values
  EXPECT_GT(p.system_green.g(), 100);
  EXPECT_GT(p.system_cyan.g(), 100);
  EXPECT_GT(p.system_blue.b(), 100);
}

TEST(IOSTest, FromSeedDifferentColorTypes) {
  // Test with different color type inputs
  rgb8_t rgb_seed{100, 150, 200};
  hsla_float_t hsl_seed{210.0f, 0.5f, 0.6f, 1.0f};

  auto p_rgb = ios_from_seed(rgb_seed);
  auto p_hsl = ios_from_seed(hsl_seed);

  // Both should generate valid palettes
  EXPECT_GT(p_rgb.system_blue.b(), 0);
  EXPECT_GT(p_hsl.system_blue.b(), 0);
}

}  // namespace colorcpp::operations::test
