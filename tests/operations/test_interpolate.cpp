
#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

namespace colorcpp::operations::test {

using namespace core;
using namespace interpolate;
using namespace conversion;

static bool near(float a, float b, float eps = 1e-4f) { return std::fabs(a - b) < eps; }

static void expect_rgb_near(const core::rgbf_t& c, float r, float g, float b, float eps = 1e-3f) {
  EXPECT_TRUE(near(c.template get_index<0>(), r, eps));
  EXPECT_TRUE(near(c.template get_index<1>(), g, eps));
  EXPECT_TRUE(near(c.template get_index<2>(), b, eps));
}

// lerp (RGB space)

TEST(LerpTest, TZeroReturnsFirst) {
  core::rgbf_t black(0.0f, 0.0f, 0.0f);
  core::rgbf_t white(1.0f, 1.0f, 1.0f);
  auto result = lerp(black, white, 0.0f);
  EXPECT_NEAR(result.r(), 0.0f, 1e-4f);
  EXPECT_NEAR(result.g(), 0.0f, 1e-4f);
  EXPECT_NEAR(result.b(), 0.0f, 1e-4f);
}

TEST(LerpTest, TOneReturnsSecond) {
  core::rgbf_t black(0.0f, 0.0f, 0.0f);
  core::rgbf_t white(1.0f, 1.0f, 1.0f);
  auto result = lerp(black, white, 1.0f);
  EXPECT_NEAR(result.r(), 1.0f, 1e-4f);
  EXPECT_NEAR(result.g(), 1.0f, 1e-4f);
  EXPECT_NEAR(result.b(), 1.0f, 1e-4f);
}

TEST(LerpTest, THalfIsMidpoint) {
  core::rgbf_t black(0.0f, 0.0f, 0.0f);
  core::rgbf_t white(1.0f, 1.0f, 1.0f);
  auto result = lerp(black, white, 0.5f);
  EXPECT_NEAR(result.r(), 0.5f, 1e-4f);
  EXPECT_NEAR(result.g(), 0.5f, 1e-4f);
  EXPECT_NEAR(result.b(), 0.5f, 1e-4f);
}

TEST(LerpTest, LinearInterpolation) {
  core::rgbf_t a(0.2f, 0.4f, 0.6f);
  core::rgbf_t b(0.8f, 0.6f, 0.4f);
  auto result = lerp(a, b, 0.25f);
  // lerp(0.2, 0.8, 0.25) = 0.2 + 0.6 * 0.25 = 0.35
  EXPECT_NEAR(result.r(), 0.35f, 1e-4f);
  // lerp(0.4, 0.6, 0.25) = 0.4 + 0.2 * 0.25 = 0.45
  EXPECT_NEAR(result.g(), 0.45f, 1e-4f);
  // lerp(0.6, 0.4, 0.25) = 0.6 - 0.2 * 0.25 = 0.55
  EXPECT_NEAR(result.b(), 0.55f, 1e-4f);
}

TEST(LerpTest, TClampedBelowZero) {
  core::rgbf_t a(0.0f, 0.0f, 0.0f);
  core::rgbf_t b(1.0f, 1.0f, 1.0f);
  auto result = lerp(a, b, -1.0f);  // clamped to 0
  EXPECT_NEAR(result.r(), 0.0f, 1e-4f);
}

TEST(LerpTest, TClampedAboveOne) {
  core::rgbf_t a(0.0f, 0.0f, 0.0f);
  core::rgbf_t b(1.0f, 1.0f, 1.0f);
  auto result = lerp(a, b, 2.0f);  // clamped to 1
  EXPECT_NEAR(result.r(), 1.0f, 1e-4f);
}

TEST(LerpTest, WorksWithRgb8) {
  core::rgb8_t black(0, 0, 0);
  core::rgb8_t white(255, 255, 255);
  auto result = lerp(black, white, 0.5f);
  EXPECT_GE(result.r(), 0);
  EXPECT_LE(result.r(), 255);
}

// lerp_hsl (HSL space)

TEST(LerpHslTest, TZeroReturnsFirst) {
  core::rgbf_t red(1.0f, 0.0f, 0.0f);
  core::rgbf_t blue(0.0f, 0.0f, 1.0f);
  auto result = lerp_hsl(red, blue, 0.0f);
  EXPECT_NEAR(result.r(), red.r(), 5e-3f);
  EXPECT_NEAR(result.g(), red.g(), 5e-3f);
  EXPECT_NEAR(result.b(), red.b(), 5e-3f);
}

TEST(LerpHslTest, TOneReturnsSecond) {
  core::rgbf_t red(1.0f, 0.0f, 0.0f);
  core::rgbf_t blue(0.0f, 0.0f, 1.0f);
  auto result = lerp_hsl(red, blue, 1.0f);
  EXPECT_NEAR(result.r(), blue.r(), 5e-3f);
  EXPECT_NEAR(result.g(), blue.g(), 5e-3f);
  EXPECT_NEAR(result.b(), blue.b(), 5e-3f);
}

TEST(LerpHslTest, HueShortestArc) {
  // From red (hue=0°) to blue (hue=240°):
  // forward arc = 240° long, backward arc = 120° long
  // shortest arc goes backward: 0° → 300° → 240° (through magenta)
  // midpoint at t=0.5 should be near 300°
  core::rgbf_t red(1.0f, 0.0f, 0.0f);
  core::rgbf_t blue(0.0f, 0.0f, 1.0f);
  auto mid = lerp_hsl(red, blue, 0.5f);
  auto hsl = color_cast<core::hsl_float_t>(mid);
  EXPECT_NEAR(hsl.h(), 300.0f, 5.0f);
}

// lerp_oklab (OKLab space)

TEST(LerpOklabTest, TZeroReturnsFirst) {
  core::rgbf_t a(0.2f, 0.8f, 0.4f);
  core::rgbf_t b(0.9f, 0.1f, 0.6f);
  auto result = lerp_oklab(a, b, 0.0f);
  EXPECT_NEAR(result.r(), a.r(), 1e-3f);
  EXPECT_NEAR(result.g(), a.g(), 1e-3f);
  EXPECT_NEAR(result.b(), a.b(), 1e-3f);
}

TEST(LerpOklabTest, TOneReturnsSecond) {
  core::rgbf_t a(0.2f, 0.8f, 0.4f);
  core::rgbf_t b(0.9f, 0.1f, 0.6f);
  auto result = lerp_oklab(a, b, 1.0f);
  EXPECT_NEAR(result.r(), b.r(), 1e-3f);
  EXPECT_NEAR(result.g(), b.g(), 1e-3f);
  EXPECT_NEAR(result.b(), b.b(), 1e-3f);
}

TEST(LerpOklabTest, BlackToWhiteMidpointIsGray) {
  // OKLab midpoint between black and white should be perceptual mid-gray
  core::rgbf_t black(0.0f, 0.0f, 0.0f);
  core::rgbf_t white(1.0f, 1.0f, 1.0f);
  auto mid = lerp_oklab(black, white, 0.5f);
  // Result should be achromatic (r≈g≈b)
  EXPECT_NEAR(mid.r(), mid.g(), 5e-3f);
  EXPECT_NEAR(mid.g(), mid.b(), 5e-3f);
}

// lerp_hsv (HSV space)

TEST(LerpHsvTest, TZeroReturnsFirst) {
  core::rgbf_t a(1.0f, 0.0f, 0.0f);
  core::rgbf_t b(0.0f, 0.0f, 1.0f);
  auto result = lerp_hsv(a, b, 0.0f);
  EXPECT_NEAR(result.r(), a.r(), 5e-3f);
  EXPECT_NEAR(result.b(), a.b(), 5e-3f);
}

TEST(LerpHsvTest, TOneReturnsSecond) {
  core::rgbf_t a(1.0f, 0.0f, 0.0f);
  core::rgbf_t b(0.0f, 0.0f, 1.0f);
  auto result = lerp_hsv(a, b, 1.0f);
  EXPECT_NEAR(result.r(), b.r(), 5e-3f);
  EXPECT_NEAR(result.b(), b.b(), 5e-3f);
}

// Easing functions

TEST(EasingTest, LinearIsIdentity) {
  EXPECT_NEAR(easing::linear(0.0f), 0.0f, 1e-6f);
  EXPECT_NEAR(easing::linear(0.5f), 0.5f, 1e-6f);
  EXPECT_NEAR(easing::linear(1.0f), 1.0f, 1e-6f);
}

TEST(EasingTest, InQuadEndpoints) {
  EXPECT_NEAR(easing::in_quad(0.0f), 0.0f, 1e-6f);
  EXPECT_NEAR(easing::in_quad(1.0f), 1.0f, 1e-6f);
}

TEST(EasingTest, InQuadSlowAtStart) {
  // in_quad(t) = t², which is slower than linear near 0
  EXPECT_LT(easing::in_quad(0.3f), 0.3f);
}

TEST(EasingTest, OutQuadEndpoints) {
  EXPECT_NEAR(easing::out_quad(0.0f), 0.0f, 1e-6f);
  EXPECT_NEAR(easing::out_quad(1.0f), 1.0f, 1e-6f);
}

TEST(EasingTest, SmoothstepEndpoints) {
  EXPECT_NEAR(easing::smoothstep(0.0f), 0.0f, 1e-6f);
  EXPECT_NEAR(easing::smoothstep(1.0f), 1.0f, 1e-6f);
}

TEST(EasingTest, SmoothstepMidpoint) { EXPECT_NEAR(easing::smoothstep(0.5f), 0.5f, 1e-6f); }

TEST(EasingTest, SmootherStepEndpoints) {
  EXPECT_NEAR(easing::smootherstep(0.0f), 0.0f, 1e-6f);
  EXPECT_NEAR(easing::smootherstep(1.0f), 1.0f, 1e-6f);
}

TEST(EasingTest, InOutQuadEndpoints) {
  EXPECT_NEAR(easing::in_out_quad(0.0f), 0.0f, 1e-6f);
  EXPECT_NEAR(easing::in_out_quad(1.0f), 1.0f, 1e-6f);
}

TEST(EasingTest, SineEasingsEndpoints) {
  EXPECT_NEAR(easing::in_sine(0.0f), 0.0f, 1e-5f);
  EXPECT_NEAR(easing::in_sine(1.0f), 1.0f, 1e-5f);
  EXPECT_NEAR(easing::out_sine(0.0f), 0.0f, 1e-5f);
  EXPECT_NEAR(easing::out_sine(1.0f), 1.0f, 1e-5f);
  EXPECT_NEAR(easing::in_out_sine(0.0f), 0.0f, 1e-5f);
  EXPECT_NEAR(easing::in_out_sine(1.0f), 1.0f, 1e-5f);
}

TEST(EasingTest, ExpoEasingsEndpoints) {
  EXPECT_NEAR(easing::in_expo(0.0f), 0.0f, 1e-6f);
  EXPECT_NEAR(easing::in_expo(1.0f), 1.0f, 1e-5f);
  EXPECT_NEAR(easing::out_expo(0.0f), 0.0f, 1e-5f);
  EXPECT_NEAR(easing::out_expo(1.0f), 1.0f, 1e-6f);
}

// lerp_lab (CIELAB space)

TEST(LerpLabTest, BlackToWhiteMidpointIsAchromatic) {
  core::rgbf_t black(0.0f, 0.0f, 0.0f);
  core::rgbf_t white(1.0f, 1.0f, 1.0f);
  auto mid = lerp_lab(black, white, 0.5f);
  // Result should be achromatic
  EXPECT_NEAR(mid.r(), mid.g(), 5e-3f);
  EXPECT_NEAR(mid.g(), mid.b(), 5e-3f);
}

TEST(LerpLabTest, TZeroReturnsFirst) {
  core::rgbf_t a(0.3f, 0.7f, 0.2f);
  core::rgbf_t b(0.8f, 0.1f, 0.9f);
  auto result = lerp_lab(a, b, 0.0f);
  EXPECT_NEAR(result.r(), a.r(), 1e-3f);
  EXPECT_NEAR(result.g(), a.g(), 1e-3f);
  EXPECT_NEAR(result.b(), a.b(), 1e-3f);
}

// lerp_catmull_rom (Catmull-Rom cubic interpolation)

TEST(LerpCatmullRomTest, TZeroReturnsP1) {
  core::rgbf_t p0(0.0f, 0.0f, 0.0f);
  core::rgbf_t p1(1.0f, 0.0f, 0.0f);
  core::rgbf_t p2(0.0f, 1.0f, 0.0f);
  core::rgbf_t p3(0.0f, 0.0f, 1.0f);
  auto result = lerp_catmull_rom(p0, p1, p2, p3, 0.0f);
  EXPECT_NEAR(result.r(), p1.r(), 1e-3f);
  EXPECT_NEAR(result.g(), p1.g(), 1e-3f);
  EXPECT_NEAR(result.b(), p1.b(), 1e-3f);
}

TEST(LerpCatmullRomTest, TOneReturnsP2) {
  core::rgbf_t p0(0.0f, 0.0f, 0.0f);
  core::rgbf_t p1(1.0f, 0.0f, 0.0f);
  core::rgbf_t p2(0.0f, 1.0f, 0.0f);
  core::rgbf_t p3(0.0f, 0.0f, 1.0f);
  auto result = lerp_catmull_rom(p0, p1, p2, p3, 1.0f);
  EXPECT_NEAR(result.r(), p2.r(), 1e-3f);
  EXPECT_NEAR(result.g(), p2.g(), 1e-3f);
  EXPECT_NEAR(result.b(), p2.b(), 1e-3f);
}

TEST(LerpCatmullRomTest, THalfIsSmoothMidpoint) {
  core::rgbf_t p0(0.0f, 0.0f, 0.0f);
  core::rgbf_t p1(1.0f, 0.0f, 0.0f);
  core::rgbf_t p2(0.0f, 1.0f, 0.0f);
  core::rgbf_t p3(0.0f, 0.0f, 1.0f);
  auto result = lerp_catmull_rom(p0, p1, p2, p3, 0.5f);
  // At t=0.5, the curve should be between p1 and p2
  EXPECT_GT(result.r(), 0.0f);
  EXPECT_GT(result.g(), 0.0f);
  EXPECT_LT(result.r(), 1.0f);
  EXPECT_LT(result.g(), 1.0f);
  EXPECT_NEAR(result.b(), 0.0f, 1e-3f);
}

TEST(LerpCatmullRomTest, QuarterPoint) {
  core::rgbf_t p0(0.0f, 0.0f, 0.0f);
  core::rgbf_t p1(1.0f, 0.0f, 0.0f);
  core::rgbf_t p2(0.0f, 1.0f, 0.0f);
  core::rgbf_t p3(0.0f, 0.0f, 1.0f);
  auto result = lerp_catmull_rom(p0, p1, p2, p3, 0.25f);
  // At t=0.25, should be closer to p1 than p2
  EXPECT_GT(result.r(), result.g());
  EXPECT_NEAR(result.b(), 0.0f, 1e-3f);
}

TEST(LerpCatmullRomTest, ThreeQuarterPoint) {
  core::rgbf_t p0(0.0f, 0.0f, 0.0f);
  core::rgbf_t p1(1.0f, 0.0f, 0.0f);
  core::rgbf_t p2(0.0f, 1.0f, 0.0f);
  core::rgbf_t p3(0.0f, 0.0f, 1.0f);
  auto result = lerp_catmull_rom(p0, p1, p2, p3, 0.75f);
  // At t=0.75, should be closer to p2 than p1
  EXPECT_GT(result.g(), result.r());
  EXPECT_NEAR(result.b(), 0.0f, 1e-3f);
}

//
// Basic through-point test (must hold)
// Catmull-Rom should pass through p1 and p2
//
TEST(LerpCatmullRomTest, PassThroughPoints) {
  core::rgbf_t p0(0.2f, 0.1f, 0.7f);
  core::rgbf_t p1(0.9f, 0.2f, 0.3f);
  core::rgbf_t p2(0.1f, 0.8f, 0.6f);
  core::rgbf_t p3(0.3f, 0.4f, 0.9f);

  auto at0 = operations::interpolate::lerp_catmull_rom(p0, p1, p2, p3, 0.0f);
  auto at1 = operations::interpolate::lerp_catmull_rom(p0, p1, p2, p3, 1.0f);

  expect_rgb_near(at0, 0.9f, 0.2f, 0.3f);
  expect_rgb_near(at1, 0.1f, 0.8f, 0.6f);
}

//
// Unconventional colors: high purple <-> teal (testing perceptual space smoothness)
//
TEST(LerpCatmullRomTest, PurpleToTealCurve) {
  core::rgbf_t p0(0.8f, 0.0f, 0.9f);  // purple
  core::rgbf_t p1(0.5f, 0.0f, 0.7f);
  core::rgbf_t p2(0.0f, 0.7f, 0.6f);  // teal
  core::rgbf_t p3(0.0f, 0.9f, 0.4f);

  auto mid = operations::interpolate::lerp_catmull_rom(p0, p1, p2, p3, 0.5f);

  // Should not produce NaN or out-of-bounds values
  EXPECT_GE(mid.template get_index<0>(), 0.0f);
  EXPECT_GE(mid.template get_index<1>(), 0.0f);
  EXPECT_GE(mid.template get_index<2>(), 0.0f);

  EXPECT_LE(mid.template get_index<0>(), 1.2f);  // Allow slight overshoot
  EXPECT_LE(mid.template get_index<1>(), 1.2f);
  EXPECT_LE(mid.template get_index<2>(), 1.2f);
}

//
// Very dark <-> very bright (testing L channel stability)
//
TEST(LerpCatmullRomTest, DarkToBright) {
  core::rgbf_t p0(0.01f, 0.01f, 0.02f);
  core::rgbf_t p1(0.05f, 0.02f, 0.1f);
  core::rgbf_t p2(0.9f, 0.95f, 0.8f);
  core::rgbf_t p3(1.0f, 1.0f, 0.9f);

  auto mid = operations::interpolate::lerp_catmull_rom(p0, p1, p2, p3, 0.5f);

  // Should not produce negative values (common overshoot bug)
  EXPECT_GE(mid.template get_index<0>(), -0.1f);
  EXPECT_GE(mid.template get_index<1>(), -0.1f);
  EXPECT_GE(mid.template get_index<2>(), -0.1f);
}

//
// Low saturation <-> high saturation (testing chroma behavior)
//
TEST(LerpCatmullRomTest, LowToHighChroma) {
  core::rgbf_t p0(0.5f, 0.5f, 0.5f);  // gray
  core::rgbf_t p1(0.6f, 0.6f, 0.6f);
  core::rgbf_t p2(1.0f, 0.2f, 0.0f);  // high-saturation orange
  core::rgbf_t p3(0.9f, 0.1f, 0.0f);

  auto mid = operations::interpolate::lerp_catmull_rom(p0, p1, p2, p3, 0.5f);

  // At least one channel should deviate significantly from gray (chroma was properly introduced)
  float r = mid.template get_index<0>();
  float g = mid.template get_index<1>();
  float b = mid.template get_index<2>();

  EXPECT_FALSE(near(r, g) && near(g, b));  // Should not remain gray
}

//
// Zigzag variation (testing overshoot & curve stability)
//
TEST(LerpCatmullRomTest, ZigZagControlPoints) {
  core::rgbf_t p0(1.0f, 0.0f, 0.0f);  // red
  core::rgbf_t p1(0.0f, 1.0f, 0.0f);  // green
  core::rgbf_t p2(1.0f, 0.0f, 0.0f);  // red
  core::rgbf_t p3(0.0f, 1.0f, 0.0f);  // green

  auto mid = operations::interpolate::lerp_catmull_rom(p0, p1, p2, p3, 0.5f);

  // Should not explode (NaN or extreme values)
  EXPECT_FALSE(std::isnan(mid.template get_index<0>()));
  EXPECT_FALSE(std::isnan(mid.template get_index<1>()));
  EXPECT_FALSE(std::isnan(mid.template get_index<2>()));
}

//
// Alpha curve test (Catmull-Rom used for alpha)
//
TEST(LerpCatmullRomTest, AlphaInterpolation) {
  core::rgbaf_t p0(0.2f, 0.3f, 0.4f, 0.0f);
  core::rgbaf_t p1(0.3f, 0.4f, 0.5f, 0.2f);
  core::rgbaf_t p2(0.6f, 0.7f, 0.8f, 0.8f);
  core::rgbaf_t p3(0.9f, 1.0f, 0.9f, 1.0f);

  auto mid = operations::interpolate::lerp_catmull_rom(p0, p1, p2, p3, 0.5f);

  float a = mid.template get_index<3>();

  EXPECT_GE(a, 0.0f);
  EXPECT_LE(a, 1.2f);  // Allow overshoot
}

//
// Hue wrap (implicitly tests OkLab stability)
// Red → blue → purple path
//
TEST(LerpCatmullRomTest, HueWrapBehavior) {
  core::rgbf_t p0(1.0f, 0.0f, 0.0f);  // red
  core::rgbf_t p1(1.0f, 0.5f, 0.0f);  // orange
  core::rgbf_t p2(0.0f, 0.0f, 1.0f);  // blue
  core::rgbf_t p3(0.5f, 0.0f, 1.0f);  // purple

  auto mid = operations::interpolate::lerp_catmull_rom(p0, p1, p2, p3, 0.5f);

  // Should be blue-purple tones, not gray or muddy colors
  float r = mid.template get_index<0>();
  float g = mid.template get_index<1>();
  float b = mid.template get_index<2>();

  EXPECT_TRUE(b > r || r > g);  // At least not gray
}

// lerp_cubic_hermite (Cubic Hermite interpolation)

TEST(LerpCubicHermiteTest, TZeroReturnsP1) {
  core::rgbf_t p0(0.0f, 0.0f, 0.0f);
  core::rgbf_t p1(1.0f, 0.0f, 0.0f);
  core::rgbf_t p2(0.0f, 1.0f, 0.0f);
  core::rgbf_t p3(0.0f, 0.0f, 1.0f);
  auto result = lerp_cubic_hermite(p0, p1, p2, p3, 0.0f);
  EXPECT_NEAR(result.r(), p1.r(), 1e-3f);
  EXPECT_NEAR(result.g(), p1.g(), 1e-3f);
  EXPECT_NEAR(result.b(), p1.b(), 1e-3f);
}

TEST(LerpCubicHermiteTest, TOneReturnsP2) {
  core::rgbf_t p0(0.0f, 0.0f, 0.0f);
  core::rgbf_t p1(1.0f, 0.0f, 0.0f);
  core::rgbf_t p2(0.0f, 1.0f, 0.0f);
  core::rgbf_t p3(0.0f, 0.0f, 1.0f);
  auto result = lerp_cubic_hermite(p0, p1, p2, p3, 1.0f);
  EXPECT_NEAR(result.r(), p2.r(), 1e-3f);
  EXPECT_NEAR(result.g(), p2.g(), 1e-3f);
  EXPECT_NEAR(result.b(), p2.b(), 1e-3f);
}

// lerp_hue_locked (Hue-locked interpolation)

TEST(LerpHueLockedTest, TZeroReturnsFirst) {
  core::rgbf_t red(1.0f, 0.0f, 0.0f);
  core::rgbf_t blue(0.0f, 0.0f, 1.0f);
  auto result = lerp_hue_locked(red, blue, 0.0f, true);
  EXPECT_NEAR(result.r(), red.r(), 5e-3f);
  EXPECT_NEAR(result.g(), red.g(), 5e-3f);
  EXPECT_NEAR(result.b(), red.b(), 5e-3f);
}

TEST(LerpHueLockedTest, TOneReturnsSecondWithLockedHue) {
  // When hue_locked=true, the hue should stay at the start color's hue
  core::rgbf_t red(1.0f, 0.0f, 0.0f);
  core::rgbf_t blue(0.0f, 0.0f, 1.0f);
  auto result = lerp_hue_locked(red, blue, 1.0f, true);
  auto oklch = color_cast<core::oklch_t>(result);
  auto start_oklch = color_cast<core::oklch_t>(red);
  // Hue should be locked to start color's hue
  EXPECT_NEAR(oklch.h(), start_oklch.h(), 5.0f);
}

// lerp_chroma_preserving (Chroma-preserving interpolation)

TEST(LerpChromaPreservingTest, TZeroReturnsFirst) {
  core::rgbf_t a(1.0f, 0.0f, 0.0f);
  core::rgbf_t b(0.0f, 0.0f, 1.0f);
  auto result = lerp_chroma_preserving(a, b, 0.0f);
  EXPECT_NEAR(result.r(), a.r(), 5e-3f);
  EXPECT_NEAR(result.g(), a.g(), 5e-3f);
  EXPECT_NEAR(result.b(), a.b(), 5e-3f);
}

TEST(LerpChromaPreservingTest, TOneReturnsSecond) {
  core::rgbf_t a(1.0f, 0.0f, 0.0f);
  core::rgbf_t b(0.0f, 0.0f, 1.0f);
  auto result = lerp_chroma_preserving(a, b, 1.0f);
  EXPECT_NEAR(result.r(), b.r(), 5e-3f);
  EXPECT_NEAR(result.g(), b.g(), 5e-3f);
  EXPECT_NEAR(result.b(), b.b(), 5e-3f);
}

// lerp_alpha_mode (Alpha-independent interpolation)

TEST(LerpAlphaModeTest, InterpolateAlpha) {
  core::rgbaf_t a(1.0f, 0.0f, 0.0f, 1.0f);
  core::rgbaf_t b(0.0f, 0.0f, 1.0f, 0.0f);
  auto result = lerp_alpha_mode(a, b, 0.5f, alpha_mode::lerp);  // interpolate alpha
  EXPECT_NEAR(result.a(), 0.5f, 1e-3f);
}

TEST(LerpAlphaModeTest, UseStartAlpha) {
  core::rgbaf_t a(1.0f, 0.0f, 0.0f, 0.8f);
  core::rgbaf_t b(0.0f, 0.0f, 1.0f, 0.2f);
  auto result = lerp_alpha_mode(a, b, 0.5f, alpha_mode::start);  // use start alpha
  EXPECT_NEAR(result.a(), 0.8f, 1e-3f);
}

TEST(LerpAlphaModeTest, UseEndAlpha) {
  core::rgbaf_t a(1.0f, 0.0f, 0.0f, 0.8f);
  core::rgbaf_t b(0.0f, 0.0f, 1.0f, 0.2f);
  auto result = lerp_alpha_mode(a, b, 0.5f, alpha_mode::end);  // use end alpha
  EXPECT_NEAR(result.a(), 0.2f, 1e-3f);
}

TEST(LerpAlphaModeTest, MultiplyAlphas) {
  core::rgbaf_t a(1.0f, 0.0f, 0.0f, 0.8f);
  core::rgbaf_t b(0.0f, 0.0f, 1.0f, 0.5f);
  auto result = lerp_alpha_mode(a, b, 0.5f, alpha_mode::multiply);  // multiply alphas
  EXPECT_NEAR(result.a(), 0.4f, 1e-3f);
}

// Position-based multi_lerp tests

TEST(MultiLerpPositionTest, BasicGradient) {
  using namespace colorcpp::operations::interpolate;
  
  core::rgbf_t red(1.0f, 0.0f, 0.0f);
  core::rgbf_t green(0.0f, 1.0f, 0.0f);
  core::rgbf_t blue(0.0f, 0.0f, 1.0f);
  
  std::vector<stop<core::rgbf_t>> stops = {
    {red, 0.0f},
    {green, 0.5f},
    {blue, 1.0f}
  };
  
  // At t=0, should return red
  auto result0 = multi_lerp(stops, 0.0f);
  EXPECT_NEAR(result0.r(), 1.0f, 1e-4f);
  EXPECT_NEAR(result0.g(), 0.0f, 1e-4f);
  EXPECT_NEAR(result0.b(), 0.0f, 1e-4f);
  
  // At t=0.5, should return green
  auto result50 = multi_lerp(stops, 0.5f);
  EXPECT_NEAR(result50.r(), 0.0f, 1e-4f);
  EXPECT_NEAR(result50.g(), 1.0f, 1e-4f);
  EXPECT_NEAR(result50.b(), 0.0f, 1e-4f);
  
  // At t=1, should return blue
  auto result100 = multi_lerp(stops, 1.0f);
  EXPECT_NEAR(result100.r(), 0.0f, 1e-4f);
  EXPECT_NEAR(result100.g(), 0.0f, 1e-4f);
  EXPECT_NEAR(result100.b(), 1.0f, 1e-4f);
}

TEST(MultiLerpPositionTest, NonUniformStops) {
  using namespace colorcpp::operations::interpolate;
  
  core::rgbf_t black(0.0f, 0.0f, 0.0f);
  core::rgbf_t white(1.0f, 1.0f, 1.0f);
  
  // Non-uniform positions: black at 0, white at 0.25
  std::vector<stop<core::rgbf_t>> stops = {
    {black, 0.0f},
    {white, 0.25f}
  };
  
  // At t=0.125 (halfway between 0 and 0.25), should be gray
  auto result = multi_lerp(stops, 0.125f);
  EXPECT_NEAR(result.r(), 0.5f, 1e-4f);
  EXPECT_NEAR(result.g(), 0.5f, 1e-4f);
  EXPECT_NEAR(result.b(), 0.5f, 1e-4f);
}

TEST(MultiLerpPositionTest, MultipleStops) {
  using namespace colorcpp::operations::interpolate;
  
  core::rgbf_t red(1.0f, 0.0f, 0.0f);
  core::rgbf_t yellow(1.0f, 1.0f, 0.0f);
  core::rgbf_t green(0.0f, 1.0f, 0.0f);
  core::rgbf_t cyan(0.0f, 1.0f, 1.0f);
  core::rgbf_t blue(0.0f, 0.0f, 1.0f);
  
  std::vector<stop<core::rgbf_t>> stops = {
    {red, 0.0f},
    {yellow, 0.25f},
    {green, 0.5f},
    {cyan, 0.75f},
    {blue, 1.0f}
  };
  
  // Test at various positions
  auto result25 = multi_lerp(stops, 0.25f);
  EXPECT_NEAR(result25.r(), 1.0f, 1e-4f);
  EXPECT_NEAR(result25.g(), 1.0f, 1e-4f);
  EXPECT_NEAR(result25.b(), 0.0f, 1e-4f);
  
  auto result75 = multi_lerp(stops, 0.75f);
  EXPECT_NEAR(result75.r(), 0.0f, 1e-4f);
  EXPECT_NEAR(result75.g(), 1.0f, 1e-4f);
  EXPECT_NEAR(result75.b(), 1.0f, 1e-4f);
}

TEST(MultiLerpPositionTest, EdgeCases) {
  using namespace colorcpp::operations::interpolate;
  
  core::rgbf_t red(1.0f, 0.0f, 0.0f);
  core::rgbf_t blue(0.0f, 0.0f, 1.0f);
  
  std::vector<stop<core::rgbf_t>> stops = {
    {red, 0.0f},
    {blue, 1.0f}
  };
  
  // t < 0 should clamp to first stop
  auto result_neg = multi_lerp(stops, -0.5f);
  EXPECT_NEAR(result_neg.r(), 1.0f, 1e-4f);
  
  // t > 1 should clamp to last stop
  auto result_over = multi_lerp(stops, 1.5f);
  EXPECT_NEAR(result_over.b(), 1.0f, 1e-4f);
}

TEST(MultiLerpPositionTest, HslVariant) {
  using namespace colorcpp::operations::interpolate;
  
  core::rgbf_t red(1.0f, 0.0f, 0.0f);
  core::rgbf_t blue(0.0f, 0.0f, 1.0f);
  
  std::vector<stop<core::rgbf_t>> stops = {
    {red, 0.0f},
    {blue, 1.0f}
  };
  
  auto result = multi_lerp_hsl(stops, 0.5f);
  // Should be somewhere between red and blue
  EXPECT_GT(result.b(), 0.0f);
  EXPECT_GT(result.r(), 0.0f);
}

TEST(MultiLerpPositionTest, OklabVariant) {
  using namespace colorcpp::operations::interpolate;
  
  core::rgbf_t black(0.0f, 0.0f, 0.0f);
  core::rgbf_t white(1.0f, 1.0f, 1.0f);
  
  std::vector<stop<core::rgbf_t>> stops = {
    {black, 0.0f},
    {white, 1.0f}
  };
  
  auto result = multi_lerp_oklab(stops, 0.5f);
  // OkLab midpoint should be perceptually uniform gray
  EXPECT_NEAR(result.r(), result.g(), 1e-3f);
  EXPECT_NEAR(result.g(), result.b(), 1e-3f);
}

TEST(MultiLerpPositionTest, OklchVariant) {
  using namespace colorcpp::operations::interpolate;
  
  core::rgbf_t red(1.0f, 0.0f, 0.0f);
  core::rgbf_t blue(0.0f, 0.0f, 1.0f);
  
  std::vector<stop<core::rgbf_t>> stops = {
    {red, 0.0f},
    {blue, 1.0f}
  };
  
  auto result = multi_lerp_oklch(stops, 0.5f);
  // Should interpolate in OkLCH space
  EXPECT_GT(result.b(), 0.0f);
}

TEST(MultiLerpPositionTest, ThrowsOnTooFewStops) {
  using namespace colorcpp::operations::interpolate;
  
  core::rgbf_t red(1.0f, 0.0f, 0.0f);
  std::vector<stop<core::rgbf_t>> stops = {{red, 0.0f}};
  
  EXPECT_THROW(multi_lerp(stops, 0.5f), std::invalid_argument);
}

}  // namespace colorcpp::operations::test
