
#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

namespace colorcpp::operations::test {

using namespace core;
using namespace interpolate;
using namespace conversion;

// ── lerp (RGB space) ──────────────────────────────────────────────────────────

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

// ── lerp_hsl (HSL space) ──────────────────────────────────────────────────────

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
  //   forward arc = 240° long, backward arc = 120° long
  //   shortest arc goes backward: 0° → 300° → 240° (through magenta)
  //   midpoint at t=0.5 should be near 300°
  core::rgbf_t red(1.0f, 0.0f, 0.0f);
  core::rgbf_t blue(0.0f, 0.0f, 1.0f);
  auto mid = lerp_hsl(red, blue, 0.5f);
  auto hsl = color_cast<core::hsl_float_t>(mid);
  EXPECT_NEAR(hsl.h(), 300.0f, 5.0f);
}

// ── lerp_oklab (OKLab space) ──────────────────────────────────────────────────

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

// ── lerp_hsv (HSV space) ──────────────────────────────────────────────────────

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

// ── Easing functions ──────────────────────────────────────────────────────────

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

TEST(EasingTest, SmoothstepMidpoint) {
  EXPECT_NEAR(easing::smoothstep(0.5f), 0.5f, 1e-6f);
}

TEST(EasingTest, SmootherStepEndpoints) {
  EXPECT_NEAR(easing::smootherstep(0.0f), 0.0f, 1e-6f);
  EXPECT_NEAR(easing::smootherstep(1.0f), 1.0f, 1e-6f);
}

TEST(EasingTest, InOutQuadEndpoints) {
  EXPECT_NEAR(easing::in_out_quad(0.0f), 0.0f, 1e-6f);
  EXPECT_NEAR(easing::in_out_quad(1.0f), 1.0f, 1e-6f);
}

TEST(EasingTest, SineEasingsEndpoints) {
  EXPECT_NEAR(easing::in_sine(0.0f),     0.0f, 1e-5f);
  EXPECT_NEAR(easing::in_sine(1.0f),     1.0f, 1e-5f);
  EXPECT_NEAR(easing::out_sine(0.0f),    0.0f, 1e-5f);
  EXPECT_NEAR(easing::out_sine(1.0f),    1.0f, 1e-5f);
  EXPECT_NEAR(easing::in_out_sine(0.0f), 0.0f, 1e-5f);
  EXPECT_NEAR(easing::in_out_sine(1.0f), 1.0f, 1e-5f);
}

TEST(EasingTest, ExpoEasingsEndpoints) {
  EXPECT_NEAR(easing::in_expo(0.0f),  0.0f, 1e-6f);
  EXPECT_NEAR(easing::in_expo(1.0f),  1.0f, 1e-5f);
  EXPECT_NEAR(easing::out_expo(0.0f), 0.0f, 1e-5f);
  EXPECT_NEAR(easing::out_expo(1.0f), 1.0f, 1e-6f);
}

// ── lerp_lab (CIELAB space) ───────────────────────────────────────────────────

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

}  // namespace colorcpp::operations::test
