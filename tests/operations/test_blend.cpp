
#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

namespace colorcpp::operations::test {

using namespace core;
using namespace blend;
using namespace conversion;

// Helper: blend two rgbaf colors with given mode and return rgbaf result
static core::rgbaf_t do_blend(float dr, float dg, float db, float da, float sr, float sg, float sb, float sa,
                              blend_mode mode, float opacity = 1.0f) {
  core::rgbaf_t dst(dr, dg, db, da);
  core::rgbaf_t src(sr, sg, sb, sa);
  return blend::blend(dst, src, mode, opacity);
}

// Normal mode

TEST(BlendTest, NormalFullOpacityReturnsSrc) {
  // With both alpha=1, normal blend returns src
  auto result = do_blend(0.2f, 0.4f, 0.6f, 1.0f, 0.8f, 0.5f, 0.1f, 1.0f, blend_mode::normal);
  EXPECT_NEAR(result.r(), 0.8f, 1e-4f);
  EXPECT_NEAR(result.g(), 0.5f, 1e-4f);
  EXPECT_NEAR(result.b(), 0.1f, 1e-4f);
  EXPECT_NEAR(result.a(), 1.0f, 1e-4f);
}

TEST(BlendTest, NormalZeroOpacityReturnsDst) {
  auto result = do_blend(0.2f, 0.4f, 0.6f, 1.0f, 0.8f, 0.5f, 0.1f, 1.0f, blend_mode::normal, 0.0f);
  EXPECT_NEAR(result.r(), 0.2f, 1e-4f);
  EXPECT_NEAR(result.g(), 0.4f, 1e-4f);
  EXPECT_NEAR(result.b(), 0.6f, 1e-4f);
}

// Multiply

TEST(BlendTest, MultiplyHalfByHalf) {
  // multiply(0.5, 0.5) = 0.25
  auto result = do_blend(0.5f, 0.5f, 0.5f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f, blend_mode::multiply);
  EXPECT_NEAR(result.r(), 0.25f, 1e-4f);
  EXPECT_NEAR(result.g(), 0.25f, 1e-4f);
  EXPECT_NEAR(result.b(), 0.25f, 1e-4f);
}

TEST(BlendTest, MultiplyByBlackGivesBlack) {
  auto result = do_blend(0.8f, 0.6f, 0.4f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, blend_mode::multiply);
  EXPECT_NEAR(result.r(), 0.0f, 1e-4f);
  EXPECT_NEAR(result.g(), 0.0f, 1e-4f);
  EXPECT_NEAR(result.b(), 0.0f, 1e-4f);
}

TEST(BlendTest, MultiplyByWhiteIdentity) {
  // multiply(x, 1) = x
  auto result = do_blend(0.7f, 0.3f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, blend_mode::multiply);
  EXPECT_NEAR(result.r(), 0.7f, 1e-4f);
  EXPECT_NEAR(result.g(), 0.3f, 1e-4f);
  EXPECT_NEAR(result.b(), 0.5f, 1e-4f);
}

// Screen

TEST(BlendTest, ScreenHalfByHalf) {
  // screen(0.5, 0.5) = 1 - (1-0.5)*(1-0.5) = 0.75
  auto result = do_blend(0.5f, 0.5f, 0.5f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f, blend_mode::screen);
  EXPECT_NEAR(result.r(), 0.75f, 1e-4f);
  EXPECT_NEAR(result.g(), 0.75f, 1e-4f);
  EXPECT_NEAR(result.b(), 0.75f, 1e-4f);
}

TEST(BlendTest, ScreenByWhiteGivesWhite) {
  auto result = do_blend(0.3f, 0.5f, 0.7f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, blend_mode::screen);
  EXPECT_NEAR(result.r(), 1.0f, 1e-4f);
  EXPECT_NEAR(result.g(), 1.0f, 1e-4f);
  EXPECT_NEAR(result.b(), 1.0f, 1e-4f);
}

// Darken / Lighten

TEST(BlendTest, DarkenPicksMinChannel) {
  auto result = do_blend(0.3f, 0.7f, 0.5f, 1.0f, 0.6f, 0.4f, 0.8f, 1.0f, blend_mode::darken);
  EXPECT_NEAR(result.r(), 0.3f, 1e-4f);  // min(0.3, 0.6)
  EXPECT_NEAR(result.g(), 0.4f, 1e-4f);  // min(0.7, 0.4)
  EXPECT_NEAR(result.b(), 0.5f, 1e-4f);  // min(0.5, 0.8)
}

TEST(BlendTest, LightenPicksMaxChannel) {
  auto result = do_blend(0.3f, 0.7f, 0.5f, 1.0f, 0.6f, 0.4f, 0.8f, 1.0f, blend_mode::lighten);
  EXPECT_NEAR(result.r(), 0.6f, 1e-4f);  // max(0.3, 0.6)
  EXPECT_NEAR(result.g(), 0.7f, 1e-4f);  // max(0.7, 0.4)
  EXPECT_NEAR(result.b(), 0.8f, 1e-4f);  // max(0.5, 0.8)
}

// Addition / Subtraction

TEST(BlendTest, AdditionClampedAt1) {
  // addition(0.7, 0.5) = min(1, 1.2) = 1
  auto result = do_blend(0.7f, 0.7f, 0.7f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f, blend_mode::addition);
  EXPECT_NEAR(result.r(), 1.0f, 1e-4f);
}

TEST(BlendTest, SubtractionClampedAt0) {
  // subtraction: dst - src = 0.3 - 0.7 = -0.4 → clamped to 0
  auto result = do_blend(0.3f, 0.3f, 0.3f, 1.0f, 0.7f, 0.7f, 0.7f, 1.0f, blend_mode::subtraction);
  EXPECT_NEAR(result.r(), 0.0f, 1e-4f);
}

// Difference / Exclusion

TEST(BlendTest, DifferenceIsAbsValue) {
  auto result = do_blend(0.3f, 0.3f, 0.3f, 1.0f, 0.8f, 0.8f, 0.8f, 1.0f, blend_mode::difference);
  EXPECT_NEAR(result.r(), 0.5f, 1e-4f);  // |0.3 - 0.8|
}

TEST(BlendTest, DifferenceSymmetric) {
  auto r1 = do_blend(0.3f, 0.3f, 0.3f, 1.0f, 0.7f, 0.7f, 0.7f, 1.0f, blend_mode::difference);
  auto r2 = do_blend(0.7f, 0.7f, 0.7f, 1.0f, 0.3f, 0.3f, 0.3f, 1.0f, blend_mode::difference);
  EXPECT_NEAR(r1.r(), r2.r(), 1e-4f);
}

TEST(BlendTest, ExclusionFormula) {
  // exclusion(a, b) = a + b - 2ab
  float a = 0.6f, b = 0.4f;
  float expected = a + b - 2.0f * a * b;  // = 0.52
  auto result = do_blend(a, a, a, 1.0f, b, b, b, 1.0f, blend_mode::exclusion);
  EXPECT_NEAR(result.r(), expected, 1e-4f);
}

// Alpha compositing

TEST(BlendTest, AlphaCompositing) {
  // Blending transparent src over opaque dst → dst dominates
  auto result = do_blend(0.8f, 0.4f, 0.2f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, blend_mode::normal);
  EXPECT_NEAR(result.r(), 0.8f, 1e-4f);
  EXPECT_NEAR(result.g(), 0.4f, 1e-4f);
  EXPECT_NEAR(result.b(), 0.2f, 1e-4f);
  EXPECT_NEAR(result.a(), 1.0f, 1e-4f);
}

TEST(BlendTest, AlphaOutIsUnion) {
  // out_alpha = sa + da * (1 - sa)
  auto result = do_blend(0.5f, 0.5f, 0.5f, 0.6f, 0.5f, 0.5f, 0.5f, 0.4f, blend_mode::normal);
  float expected_a = 0.4f + 0.6f * (1.0f - 0.4f);  // = 0.76
  EXPECT_NEAR(result.a(), expected_a, 1e-4f);
}

// Opacity

TEST(BlendTest, HalfOpacityBlend) {
  // Half opacity: src alpha is halved
  auto result = do_blend(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, blend_mode::normal, 0.5f);
  // out_a = 0.5 + 1 * (1-0.5) = 1.0; mixed = 0.5 * 0 + 0 * 0.5 + 0.5 * 1 = 0.5 / 1 = 0.5
  EXPECT_NEAR(result.r(), 0.5f, 1e-3f);
}

// Non-separable modes

TEST(BlendTest, HueModeDoesNotCrash) {
  auto result = do_blend(0.5f, 0.2f, 0.8f, 1.0f, 0.8f, 0.4f, 0.1f, 1.0f, blend_mode::hue);
  EXPECT_GE(result.r(), 0.0f);
  EXPECT_LE(result.r(), 1.0f);
  EXPECT_GE(result.g(), 0.0f);
  EXPECT_LE(result.g(), 1.0f);
  EXPECT_GE(result.b(), 0.0f);
  EXPECT_LE(result.b(), 1.0f);
}

TEST(BlendTest, LuminosityModeDoesNotCrash) {
  auto result = do_blend(0.3f, 0.6f, 0.9f, 1.0f, 0.1f, 0.5f, 0.7f, 1.0f, blend_mode::luminosity);
  EXPECT_GE(result.r(), 0.0f);
  EXPECT_LE(result.r(), 1.0f);
}

// type generality

TEST(BlendTest, WorksWithRgba8Input) {
  rgba8_t dst(100, 200, 50, 255);
  rgba8_t src(50, 100, 150, 255);
  auto result = blend::blend(dst, src, blend_mode::multiply);
  EXPECT_GE(result.r(), 0);
  EXPECT_LE(result.r(), 255);
}

}  // namespace colorcpp::operations::test
