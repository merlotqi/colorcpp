#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>
#include <string>
#include <vector>

namespace colorcpp::io::test {

using namespace colorcpp::core;
using namespace colorcpp::io::css;

static void expect_rgba(const rgba8_t& c, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  EXPECT_EQ(c.r(), r);
  EXPECT_EQ(c.g(), g);
  EXPECT_EQ(c.b(), b);
  EXPECT_EQ(c.a(), a);
}

static void expect_rgbaf_near(const rgbaf_t& c, float r, float g, float b, float a, float eps = 0.01f) {
  EXPECT_NEAR(c.r(), r, eps);
  EXPECT_NEAR(c.g(), g, eps);
  EXPECT_NEAR(c.b(), b, eps);
  EXPECT_NEAR(c.a(), a, eps);
}

TEST(Css, Hex) {
  ASSERT_TRUE(parse_css_color_rgba8("#f63").has_value());
  expect_rgba(*parse_css_color_rgba8("#f63"), 255, 102, 51, 255);
  ASSERT_TRUE(parse_css_color_rgba8("  #FF6347  ").has_value());
  expect_rgba(*parse_css_color_rgba8("  #FF6347  "), 255, 99, 71, 255);
  ASSERT_TRUE(parse_css_color_rgba8("#FF634780").has_value());
  expect_rgba(*parse_css_color_rgba8("#FF634780"), 255, 99, 71, 128);
  EXPECT_FALSE(parse_css_color_rgba8("#gg0000").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("#12").has_value());
}

TEST(Css, HexShortAlphaAndTransparentBlack) {
  ASSERT_TRUE(parse_css_color_rgba8("#0f08").has_value());
  expect_rgba(*parse_css_color_rgba8("#0f08"), 0, 255, 0, 136);

  ASSERT_TRUE(parse_css_color_rgba8("#0000").has_value());
  expect_rgba(*parse_css_color_rgba8("#0000"), 0, 0, 0, 0);
}

TEST(Css, RgbComma) {
  auto c = parse_css_color_rgba8("rgb(255, 99, 71)");
  ASSERT_TRUE(c);
  expect_rgba(*c, 255, 99, 71, 255);

  ASSERT_TRUE(parse_css_color_rgba8("RGB(0, 0, 0)").has_value());
  expect_rgba(*parse_css_color_rgba8("RGB(0, 0, 0)"), 0, 0, 0, 255);
  ASSERT_TRUE(parse_css_color_rgba8("rgba(255, 0, 0, 0.5)").has_value());
  EXPECT_EQ(parse_css_color_rgba8("rgba(255, 0, 0, 0.5)")->a(), 128);
}

TEST(Css, RgbSpaceSlash) {
  ASSERT_TRUE(parse_css_color_rgba8("rgb(255 99 71)").has_value());
  expect_rgba(*parse_css_color_rgba8("rgb(255 99 71)"), 255, 99, 71, 255);
  auto c = parse_css_color_rgba8("rgb(255 99 71 / 50%)");
  ASSERT_TRUE(c);
  EXPECT_EQ(c->r(), 255);
  EXPECT_EQ(c->g(), 99);
  EXPECT_EQ(c->b(), 71);
  EXPECT_EQ(c->a(), 128);
}

TEST(Css, RgbPercent) {
  ASSERT_TRUE(parse_css_color_rgba8("rgb(100% 0% 0%)").has_value());
  expect_rgba(*parse_css_color_rgba8("rgb(100% 0% 0%)"), 255, 0, 0, 255);
  ASSERT_TRUE(parse_css_color_rgba8("rgb(50% 50% 50%)").has_value());
  expect_rgba(*parse_css_color_rgba8("rgb(50% 50% 50%)"), 128, 128, 128, 255);
}

TEST(Css, RgbClampAndFloatAlphaEdges) {
  auto c = parse_css_color_rgba8("  rgb(-10 300 127.6 / 120%)  ");
  ASSERT_TRUE(c);
  expect_rgba(*c, 0, 255, 128, 255);

  auto f = parse_css_color<rgbaf_t>("rgb(300 -10 50% / -10%)");
  ASSERT_TRUE(f);
  expect_rgbaf_near(*f, 1.0f, 0.0f, 0.5f, 0.0f, 0.002f);
}

TEST(Css, HslComma) {
  auto c = parse_css_color_rgba8("hsl(0, 100%, 50%)");
  ASSERT_TRUE(c);
  EXPECT_EQ(c->r(), 255);
  EXPECT_EQ(c->g(), 0);
  EXPECT_EQ(c->b(), 0);
}

TEST(Css, HslSpaceSlash) {
  auto c = parse_css_color_rgba8("hsl(120 100% 50% / 80%)");
  ASSERT_TRUE(c);
  EXPECT_GT(c->g(), 200);
  EXPECT_EQ(c->a(), 204);
}

TEST(Css, HslHueUnits) {
  auto a = parse_css_color_rgba8("hsl(0.5turn 100% 50%)");
  auto b = parse_css_color_rgba8("hsl(180deg 100% 50%)");
  ASSERT_TRUE(a);
  ASSERT_TRUE(b);
  EXPECT_EQ(a->r(), b->r());
  EXPECT_EQ(a->g(), b->g());
  EXPECT_EQ(a->b(), b->b());
}

TEST(Css, HueNormalizationWrapsNegativeAngles) {
  auto hsl_neg = parse_css_color_rgba8("hsl(-180deg 100% 50%)");
  auto hsl_pos = parse_css_color_rgba8("hsl(180deg 100% 50%)");
  ASSERT_TRUE(hsl_neg);
  ASSERT_TRUE(hsl_pos);
  expect_rgba(*hsl_neg, hsl_pos->r(), hsl_pos->g(), hsl_pos->b(), hsl_pos->a());

  auto oklch_neg = parse_css_color<oklch_t>("oklch(0.5 0.1 -180deg)");
  auto oklch_pos = parse_css_color<oklch_t>("oklch(0.5 0.1 180deg)");
  ASSERT_TRUE(oklch_neg);
  ASSERT_TRUE(oklch_pos);
  EXPECT_NEAR(oklch_neg->h(), oklch_pos->h(), 0.001f);
}

TEST(Css, ParseTemplate) {
  auto hsl = parse_css_color<hsla_float_t>("hsl(0, 100%, 50%)");
  ASSERT_TRUE(hsl);
  EXPECT_TRUE(hsl->h() <= 1.0f || hsl->h() >= 359.0f);
  EXPECT_NEAR(hsl->s(), 1.0f, 0.05f);
  EXPECT_NEAR(hsl->l(), 0.5f, 0.05f);
}

TEST(Css, ParseTemplateSpecializationsFallbackToGenericInputs) {
  auto named_oklab = parse_css_color<oklab_t>("red");
  ASSERT_TRUE(named_oklab);

  auto srgb_display_p3 = parse_css_color<display_p3f_t>("color(srgb 0.5 0.3 0.2)");
  ASSERT_TRUE(srgb_display_p3);

  auto expected_rgbaf = parse_css_color_rgbaf("color(srgb 0.5 0.3 0.2)");
  ASSERT_TRUE(expected_rgbaf);
  auto expected_display_p3 = operations::conversion::color_cast<display_p3f_t>(*expected_rgbaf);

  EXPECT_NEAR(srgb_display_p3->r(), expected_display_p3.r(), 0.001f);
  EXPECT_NEAR(srgb_display_p3->g(), expected_display_p3.g(), 0.001f);
  EXPECT_NEAR(srgb_display_p3->b(), expected_display_p3.b(), 0.001f);
}

TEST(Css, GenericColorFunctionPreservesWideGamutForTypedTargets) {
  auto xyz_from_p3 = parse_css_color<xyz_t>("color(display-p3 1 0 0)");
  ASSERT_TRUE(xyz_from_p3);
  auto expected_xyz_from_p3 = operations::conversion::color_cast<xyz_t>(display_p3f_t{1.0f, 0.0f, 0.0f});
  EXPECT_NEAR(xyz_from_p3->x(), expected_xyz_from_p3.x(), 0.002f);
  EXPECT_NEAR(xyz_from_p3->y(), expected_xyz_from_p3.y(), 0.002f);
  EXPECT_NEAR(xyz_from_p3->z(), expected_xyz_from_p3.z(), 0.002f);

  auto xyz_from_rec2020 = parse_css_color<xyz_t>("color(rec2020 1 0 0)");
  ASSERT_TRUE(xyz_from_rec2020);
  EXPECT_NEAR(xyz_from_rec2020->x(), 0.6369580f, 0.002f);
  EXPECT_NEAR(xyz_from_rec2020->y(), 0.2627002f, 0.002f);
  EXPECT_NEAR(xyz_from_rec2020->z(), 0.0f, 0.002f);
}

TEST(Css, GenericColorFunctionPreservesAlphaForAlphaAwareTargets) {
  auto p3_alpha = parse_css_color<display_p3af_t>("color(display-p3 0.7 0.5 0.3 / 0.5)");
  ASSERT_TRUE(p3_alpha);
  EXPECT_NEAR(p3_alpha->r(), 0.7f, 0.01f);
  EXPECT_NEAR(p3_alpha->g(), 0.5f, 0.01f);
  EXPECT_NEAR(p3_alpha->b(), 0.3f, 0.01f);
  EXPECT_NEAR(p3_alpha->a(), 0.5f, 0.002f);

  auto rec2020_alpha = parse_css_color<display_p3af_t>("color(rec2020 1 0 0 / 25%)");
  ASSERT_TRUE(rec2020_alpha);
  auto expected = operations::conversion::color_cast<display_p3af_t>(xyz_t{0.6369580f, 0.2627002f, 0.0f});
  EXPECT_NEAR(rec2020_alpha->r(), expected.r(), 0.02f);
  EXPECT_NEAR(rec2020_alpha->g(), expected.g(), 0.02f);
  EXPECT_NEAR(rec2020_alpha->b(), expected.b(), 0.02f);
  EXPECT_NEAR(rec2020_alpha->a(), 0.25f, 0.002f);
}

TEST(Css, Invalid) {
  EXPECT_FALSE(parse_css_color_rgba8("").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("rgb(1 2)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("rgb(1,,2)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("notacolor").has_value());
}

TEST(Css, NamedColors) {
  // Basic named colors
  auto red = parse_css_color_rgba8("red");
  ASSERT_TRUE(red.has_value());
  expect_rgba(*red, 255, 0, 0, 255);

  auto blue = parse_css_color_rgba8("blue");
  ASSERT_TRUE(blue.has_value());
  expect_rgba(*blue, 0, 0, 255, 255);

  auto green = parse_css_color_rgba8("green");
  ASSERT_TRUE(green.has_value());
  expect_rgba(*green, 0, 128, 0, 255);

  // Case insensitive
  auto white = parse_css_color_rgba8("WHITE");
  ASSERT_TRUE(white.has_value());
  expect_rgba(*white, 255, 255, 255, 255);

  auto black = parse_css_color_rgba8("Black");
  ASSERT_TRUE(black.has_value());
  expect_rgba(*black, 0, 0, 0, 255);

  // With whitespace
  auto yellow = parse_css_color_rgba8("  yellow  ");
  ASSERT_TRUE(yellow.has_value());
  expect_rgba(*yellow, 255, 255, 0, 255);
}

TEST(Css, TransparentKeyword) {
  auto rgba = parse_css_color_rgba8("transparent");
  ASSERT_TRUE(rgba);
  expect_rgba(*rgba, 0, 0, 0, 0);

  auto rgbaf = parse_css_color<rgbaf_t>("TRANSPARENT");
  ASSERT_TRUE(rgbaf);
  expect_rgbaf_near(*rgbaf, 0.0f, 0.0f, 0.0f, 0.0f);

  auto spaced = parse_css_color_rgba8(" \n\tTrAnSpArEnT \r");
  ASSERT_TRUE(spaced);
  expect_rgba(*spaced, 0, 0, 0, 0);
}

TEST(Css, LabBasic) {
  // lab(L a b) with numbers — direct lab() parse (no rgba8 hub)
  auto c1 = parse_css_color<cielab_t>("lab(50 25 -30)");
  ASSERT_TRUE(c1.has_value());
  EXPECT_NEAR(c1->l(), 50.0f, 0.01f);
  EXPECT_NEAR(c1->a(), 25.0f, 0.01f);
  EXPECT_NEAR(c1->b(), -30.0f, 0.01f);

  // lab(L a b) with percentage for L
  auto c2 = parse_css_color<cielab_t>("lab(75% 20 -25)");
  ASSERT_TRUE(c2.has_value());
  EXPECT_NEAR(c2->l(), 75.0f, 0.01f);
  EXPECT_NEAR(c2->a(), 20.0f, 0.01f);
  EXPECT_NEAR(c2->b(), -25.0f, 0.01f);

  // lab(L a b / alpha) - alpha is parsed but ignored
  auto c3 = parse_css_color<cielab_t>("lab(60 30 -40 / 0.5)");
  ASSERT_TRUE(c3.has_value());
  EXPECT_NEAR(c3->l(), 60.0f, 0.01f);
  EXPECT_NEAR(c3->a(), 30.0f, 0.01f);
  EXPECT_NEAR(c3->b(), -40.0f, 0.01f);
}

TEST(Css, LabEdgeCases) {
  // Black (L=0)
  auto black = parse_css_color<cielab_t>("lab(0 0 0)");
  ASSERT_TRUE(black.has_value());
  EXPECT_NEAR(black->l(), 0.0f, 0.01f);

  // White (L=100)
  auto white = parse_css_color<cielab_t>("lab(100 0 0)");
  ASSERT_TRUE(white.has_value());
  EXPECT_NEAR(white->l(), 100.0f, 0.01f);

  // Case insensitive
  auto upper = parse_css_color<cielab_t>("LAB(50 25 -30)");
  ASSERT_TRUE(upper.has_value());
}

TEST(Css, LabInvalid) {
  // Missing components
  EXPECT_FALSE(parse_css_color<cielab_t>("lab()").has_value());
  EXPECT_FALSE(parse_css_color<cielab_t>("lab(50)").has_value());
  EXPECT_FALSE(parse_css_color<cielab_t>("lab(50 25)").has_value());

  // Invalid format
  EXPECT_FALSE(parse_css_color<cielab_t>("lab(50, 25, -30)").has_value());
}

TEST(Css, LChBasic) {
  // lch(L C H) with numbers — direct lch() parse (no rgba8 hub)
  auto c1 = parse_css_color<cielch_t>("lch(70 30 180)");
  ASSERT_TRUE(c1.has_value());
  EXPECT_NEAR(c1->l(), 70.0f, 0.01f);
  EXPECT_NEAR(c1->c(), 30.0f, 0.01f);
  EXPECT_NEAR(c1->h(), 180.0f, 0.01f);

  // lch(L C H) with percentage for L and C
  auto c2 = parse_css_color<cielch_t>("lch(75% 20% 90deg)");
  ASSERT_TRUE(c2.has_value());
  EXPECT_NEAR(c2->l(), 75.0f, 0.01f);
  EXPECT_NEAR(c2->c(), 30.0f, 0.01f);
  EXPECT_NEAR(c2->h(), 90.0f, 0.01f);

  // lch(L C H / alpha) - alpha is parsed but ignored
  auto c3 = parse_css_color<cielch_t>("lch(50 40 270 / 0.8)");
  ASSERT_TRUE(c3.has_value());
  EXPECT_NEAR(c3->l(), 50.0f, 0.01f);
  EXPECT_NEAR(c3->c(), 40.0f, 0.01f);
  EXPECT_NEAR(c3->h(), 270.0f, 0.01f);
}

TEST(Css, LChHueUnits) {
  // lch with different hue units
  auto deg = parse_css_color<cielch_t>("lch(50 30 180deg)");
  auto grad = parse_css_color<cielch_t>("lch(50 30 200grad)");
  auto rad = parse_css_color<cielch_t>("lch(50 30 3.14159rad)");
  auto turn = parse_css_color<cielch_t>("lch(50 30 0.5turn)");

  ASSERT_TRUE(deg.has_value());
  ASSERT_TRUE(grad.has_value());
  ASSERT_TRUE(rad.has_value());
  ASSERT_TRUE(turn.has_value());

  // All should be approximately 180 degrees
  EXPECT_NEAR(deg->h(), 180.0f, 1.0f);
  EXPECT_NEAR(grad->h(), 180.0f, 1.0f);
  EXPECT_NEAR(rad->h(), 180.0f, 1.0f);
  EXPECT_NEAR(turn->h(), 180.0f, 1.0f);
}

TEST(Css, LChInvalid) {
  // Missing components
  EXPECT_FALSE(parse_css_color<cielch_t>("lch()").has_value());
  EXPECT_FALSE(parse_css_color<cielch_t>("lch(50)").has_value());
  EXPECT_FALSE(parse_css_color<cielch_t>("lch(50 30)").has_value());

  // Invalid format
  EXPECT_FALSE(parse_css_color<cielch_t>("lch(50, 30, 180)").has_value());
}

TEST(Css, OkLabBasic) {
  // oklab(L a b) with numbers
  auto c1 = parse_css_color<oklab_t>("oklab(0.5 0.1 -0.2)");
  ASSERT_TRUE(c1.has_value());
  EXPECT_NEAR(c1->l(), 0.5f, 0.01f);
  EXPECT_NEAR(c1->a(), 0.1f, 0.01f);
  EXPECT_NEAR(c1->b(), -0.2f, 0.01f);

  // oklab(L a b) with percentages
  auto c2 = parse_css_color<oklab_t>("oklab(50% 20% -40%)");
  ASSERT_TRUE(c2.has_value());
  EXPECT_NEAR(c2->l(), 0.5f, 0.01f);
  EXPECT_NEAR(c2->a(), 0.1f, 0.01f);
  EXPECT_NEAR(c2->b(), -0.2f, 0.01f);

  // oklab(L a b / alpha) - alpha is parsed but ignored
  auto c3 = parse_css_color<oklab_t>("oklab(0.7 0.0 -0.1 / 0.5)");
  ASSERT_TRUE(c3.has_value());
  EXPECT_NEAR(c3->l(), 0.7f, 0.01f);
  EXPECT_NEAR(c3->a(), 0.0f, 0.01f);
  EXPECT_NEAR(c3->b(), -0.1f, 0.01f);
}

TEST(Css, OkLabEdgeCases) {
  // Black (L=0)
  auto black = parse_css_color<oklab_t>("oklab(0 0 0)");
  ASSERT_TRUE(black.has_value());
  EXPECT_NEAR(black->l(), 0.0f, 0.01f);

  // White (L=1)
  auto white = parse_css_color<oklab_t>("oklab(1 0 0)");
  ASSERT_TRUE(white.has_value());
  EXPECT_NEAR(white->l(), 1.0f, 0.01f);

  // Case insensitive
  auto upper = parse_css_color<oklab_t>("OKLAB(0.5 0.1 -0.2)");
  ASSERT_TRUE(upper.has_value());
}

TEST(Css, OkLChBasic) {
  // oklch(L C H) with numbers
  auto c1 = parse_css_color<oklch_t>("oklch(0.7 0.15 200)");
  ASSERT_TRUE(c1.has_value());
  EXPECT_NEAR(c1->l(), 0.7f, 0.01f);
  EXPECT_NEAR(c1->c(), 0.15f, 0.01f);
  EXPECT_NEAR(c1->h(), 200.0f, 0.5f);

  // oklch(L C H) with percentage for L and C
  auto c2 = parse_css_color<oklch_t>("oklch(70% 37.5% 180deg)");
  ASSERT_TRUE(c2.has_value());
  EXPECT_NEAR(c2->l(), 0.7f, 0.01f);
  EXPECT_NEAR(c2->c(), 0.15f, 0.01f);
  EXPECT_NEAR(c2->h(), 180.0f, 0.5f);

  // oklch(L C H / alpha) - alpha is parsed but ignored
  auto c3 = parse_css_color<oklch_t>("oklch(0.5 0.2 90 / 0.8)");
  ASSERT_TRUE(c3.has_value());
  EXPECT_NEAR(c3->l(), 0.5f, 0.01f);
  EXPECT_NEAR(c3->c(), 0.2f, 0.01f);
  EXPECT_NEAR(c3->h(), 90.0f, 0.5f);
}

TEST(Css, OkLChHueUnits) {
  // oklch with different hue units
  auto deg = parse_css_color<oklch_t>("oklch(0.5 0.1 180deg)");
  auto grad = parse_css_color<oklch_t>("oklch(0.5 0.1 200grad)");
  auto rad = parse_css_color<oklch_t>("oklch(0.5 0.1 3.14159rad)");
  auto turn = parse_css_color<oklch_t>("oklch(0.5 0.1 0.5turn)");

  ASSERT_TRUE(deg.has_value());
  ASSERT_TRUE(grad.has_value());
  ASSERT_TRUE(rad.has_value());
  ASSERT_TRUE(turn.has_value());

  // All should be approximately 180 degrees
  EXPECT_NEAR(deg->h(), 180.0f, 1.0f);
  EXPECT_NEAR(grad->h(), 180.0f, 1.0f);
  EXPECT_NEAR(rad->h(), 180.0f, 1.0f);
  EXPECT_NEAR(turn->h(), 180.0f, 1.0f);
}

TEST(Css, OkLabInvalid) {
  // Missing components
  EXPECT_FALSE(parse_css_color<oklab_t>("oklab()").has_value());
  EXPECT_FALSE(parse_css_color<oklab_t>("oklab(0.5)").has_value());
  EXPECT_FALSE(parse_css_color<oklab_t>("oklab(0.5 0.1)").has_value());

  // Invalid format
  EXPECT_FALSE(parse_css_color<oklab_t>("oklab(0.5, 0.1, -0.2)").has_value());
}

TEST(Css, OkLChInvalid) {
  // Missing components
  EXPECT_FALSE(parse_css_color<oklch_t>("oklch()").has_value());
  EXPECT_FALSE(parse_css_color<oklch_t>("oklch(0.5)").has_value());
  EXPECT_FALSE(parse_css_color<oklch_t>("oklch(0.5 0.1)").has_value());

  // Invalid format
  EXPECT_FALSE(parse_css_color<oklch_t>("oklch(0.5, 0.1, 180)").has_value());
}

TEST(Css, DisplayP3Basic) {
  // color(display-p3 r g b) with numbers
  auto c1 = parse_css_color<display_p3f_t>("color(display-p3 0.5 0.3 0.2)");
  ASSERT_TRUE(c1.has_value());
  EXPECT_NEAR(c1->r(), 0.5f, 0.01f);
  EXPECT_NEAR(c1->g(), 0.3f, 0.01f);
  EXPECT_NEAR(c1->b(), 0.2f, 0.01f);

  // color(display-p3 r g b) with percentages
  auto c2 = parse_css_color<display_p3f_t>("color(display-p3 50% 30% 20%)");
  ASSERT_TRUE(c2.has_value());
  EXPECT_NEAR(c2->r(), 0.5f, 0.01f);
  EXPECT_NEAR(c2->g(), 0.3f, 0.01f);
  EXPECT_NEAR(c2->b(), 0.2f, 0.01f);

  // color(display-p3 r g b / alpha) - alpha is parsed but ignored
  auto c3 = parse_css_color<display_p3f_t>("color(display-p3 0.7 0.5 0.3 / 0.5)");
  ASSERT_TRUE(c3.has_value());
  EXPECT_NEAR(c3->r(), 0.7f, 0.01f);
  EXPECT_NEAR(c3->g(), 0.5f, 0.01f);
  EXPECT_NEAR(c3->b(), 0.3f, 0.01f);
}

TEST(Css, DisplayP3EdgeCases) {
  // Black (all zeros)
  auto black = parse_css_color<display_p3f_t>("color(display-p3 0 0 0)");
  ASSERT_TRUE(black.has_value());
  EXPECT_NEAR(black->r(), 0.0f, 0.01f);
  EXPECT_NEAR(black->g(), 0.0f, 0.01f);
  EXPECT_NEAR(black->b(), 0.0f, 0.01f);

  // White (all ones)
  auto white = parse_css_color<display_p3f_t>("color(display-p3 1 1 1)");
  ASSERT_TRUE(white.has_value());
  EXPECT_NEAR(white->r(), 1.0f, 0.01f);
  EXPECT_NEAR(white->g(), 1.0f, 0.01f);
  EXPECT_NEAR(white->b(), 1.0f, 0.01f);

  // Case insensitive
  auto upper = parse_css_color<display_p3f_t>("COLOR(DISPLAY-P3 0.5 0.3 0.2)");
  ASSERT_TRUE(upper.has_value());
}

TEST(Css, DisplayP3Invalid) {
  // Missing components
  EXPECT_FALSE(parse_css_color<display_p3f_t>("color(display-p3)").has_value());
  EXPECT_FALSE(parse_css_color<display_p3f_t>("color(display-p3 0.5)").has_value());
  EXPECT_FALSE(parse_css_color<display_p3f_t>("color(display-p3 0.5 0.3)").has_value());

  // Invalid format
  EXPECT_FALSE(parse_css_color<display_p3f_t>("color(display-p3 0.5, 0.3, 0.2)").has_value());

  // Generic fallback still works for non-display-p3 CSS colors.
  auto generic = parse_css_color<display_p3f_t>("color(srgb 0.5 0.3 0.2)");
  ASSERT_TRUE(generic);
}

TEST(Css, ColorFunctionSrgbAndLinear) {
  auto c = parse_css_color_rgba8("color(srgb 1 0 0)");
  ASSERT_TRUE(c);
  expect_rgba(*c, 255, 0, 0, 255);

  auto lin = parse_css_color_rgba8("color(srgb-linear 1 0 0)");
  ASSERT_TRUE(lin);
  expect_rgba(*lin, 255, 0, 0, 255);
}

TEST(Css, ColorFunctionSupportsNoneAndAlphaClamp) {
  auto srgb = parse_css_color_rgba8("color(srgb none 50% 25% / 150%)");
  ASSERT_TRUE(srgb);
  expect_rgba(*srgb, 0, 128, 64, 255);

  auto d50 = parse_css_color<rgbaf_t>("color(xyz-d50 96.422% 100% 82.521% / -10%)");
  ASSERT_TRUE(d50);
  expect_rgbaf_near(*d50, 1.0f, 1.0f, 1.0f, 0.0f, 0.01f);

  auto linear_p3 = parse_css_color_rgba8("color(display-p3-linear none none none / 0)");
  ASSERT_TRUE(linear_p3);
  expect_rgba(*linear_p3, 0, 0, 0, 0);
}

TEST(Css, ColorFunctionXyzD65) {
  auto c = parse_css_color_rgba8("color(xyz-d65 0.4 0.45 0.05)");
  ASSERT_TRUE(c);
}

TEST(Css, ColorFunctionAdditionalPredefinedSpaces) {
  auto expect_leaf = [](std::string_view input) {
    SCOPED_TRACE(std::string(input));
    auto c = parse_css_color<rgbaf_t>(input);
    ASSERT_TRUE(c) << input;
    expect_rgbaf_near(*c, 0.41587f, 0.50367f, 0.36664f, 1.0f, 0.02f);
  };

  expect_leaf("color(a98-rgb 0.44091 0.49971 0.37408)");
  expect_leaf("color(prophoto-rgb 0.36589 0.41717 0.31333)");
}

TEST(Css, ColorFunctionRec2020) {
  auto white = parse_css_color_rgba8("color(rec2020 1 1 1 / 25%)");
  ASSERT_TRUE(white);
  expect_rgba(*white, 255, 255, 255, 64);

  auto rgbaf = parse_css_color<rgbaf_t>("color(rec2020 0.25 0.5 0.75)");
  ASSERT_TRUE(rgbaf);
  EXPECT_GT(rgbaf->g(), 0.0f);
  EXPECT_GT(rgbaf->b(), 0.0f);
}

TEST(Css, ColorFunctionXyzAliasesAndD50) {
  auto alias = parse_css_color<rgbaf_t>("color(xyz 0.3 0.4 0.5)");
  auto explicit_d65 = parse_css_color<rgbaf_t>("color(xyz-d65 0.3 0.4 0.5)");
  ASSERT_TRUE(alias);
  ASSERT_TRUE(explicit_d65);
  expect_rgbaf_near(*alias, explicit_d65->r(), explicit_d65->g(), explicit_d65->b(), explicit_d65->a(), 0.002f);

  auto white_d50 = parse_css_color<rgbaf_t>("color(xyz-d50 0.96422 1 0.82521)");
  ASSERT_TRUE(white_d50);
  expect_rgbaf_near(*white_d50, 1.0f, 1.0f, 1.0f, 1.0f, 0.01f);
}

TEST(Css, ColorFunctionDisplayP3Linear) {
  auto white = parse_css_color_rgba8("color(display-p3-linear 1 1 1 / 50%)");
  ASSERT_TRUE(white);
  expect_rgba(*white, 255, 255, 255, 128);

  auto black = parse_css_color_rgba8("color(display-p3-linear 0 0 0)");
  ASSERT_TRUE(black);
  expect_rgba(*black, 0, 0, 0, 255);
}

TEST(Css, ColorFunctionInvalidForms) {
  EXPECT_FALSE(parse_css_color_rgba8("color(rec2020 0.1 0.2)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color(xyz-d50 1 2 3 /)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color(unknown-space 1 2 3)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color(srgb 1 0 0) trailing").has_value());
}

TEST(Css, ColorFunctionAlpha) {
  auto c = parse_css_color_rgba8("color(srgb 1 0 0 / 50%)");
  ASSERT_TRUE(c);
  EXPECT_EQ(c->r(), 255);
  EXPECT_EQ(c->a(), 128);
}

TEST(Css, ParseRgbafWideGamut) {
  auto f = parse_css_color<rgbaf_t>("color(display-p3 1 0 0)");
  ASSERT_TRUE(f);
  EXPECT_GT(f->r(), 0.99f);
}

TEST(Css, ColorMixInSrgb) {
  auto c = parse_css_color_rgba8("color-mix(in srgb, red, blue 50%)");
  ASSERT_TRUE(c);
  EXPECT_NEAR(static_cast<double>(c->r()), 128.0, 2.0);
  EXPECT_NEAR(static_cast<double>(c->b()), 128.0, 2.0);

  auto weighted = parse_css_color_rgba8("color-mix(in srgb, red 25%, blue)");
  ASSERT_TRUE(weighted);
  EXPECT_NEAR(static_cast<double>(weighted->r()), 64.0, 2.0);
  EXPECT_NEAR(static_cast<double>(weighted->b()), 191.0, 2.0);

  auto normalized = parse_css_color_rgba8("color-mix(in srgb, red 20%, blue 20%)");
  ASSERT_TRUE(normalized);
  EXPECT_NEAR(static_cast<double>(normalized->r()), 128.0, 2.0);
  EXPECT_NEAR(static_cast<double>(normalized->b()), 128.0, 2.0);
}

TEST(Css, ColorMixNestedAndInvalidForms) {
  auto nested = parse_css_color_rgba8("color-mix(in srgb, color(srgb 1 0 0), color(display-p3 0 0 1) 50%)");
  ASSERT_TRUE(nested);
  EXPECT_GT(nested->r(), 100);
  EXPECT_GT(nested->b(), 100);

  auto transparent_mix = parse_css_color_rgba8("color-mix(in srgb, transparent, red 50%)");
  ASSERT_TRUE(transparent_mix);
  EXPECT_GT(transparent_mix->r(), 100);
  EXPECT_GT(transparent_mix->a(), 100);

  auto nested_second = parse_css_color_rgba8("color-mix(in srgb, red, light-dark(blue, green))");
  ASSERT_TRUE(nested_second);
  EXPECT_GT(nested_second->b(), 100);

  auto oklab_mix = parse_css_color_rgbaf("color-mix(in oklab, red, blue)");
  ASSERT_TRUE(oklab_mix);
  auto expected_oklab =
      operations::interpolate::lerp_oklab<rgbaf_t>(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f}, rgbaf_t{0.0f, 0.0f, 1.0f, 1.0f}, 0.5f);
  expect_rgbaf_near(*oklab_mix, expected_oklab.r(), expected_oklab.g(), expected_oklab.b(), expected_oklab.a(), 0.02f);

  auto linear_mix = parse_css_color_rgbaf("color-mix(in srgb-linear, red, blue)");
  ASSERT_TRUE(linear_mix);

  auto p3_mix = parse_css_color_rgbaf("color-mix(in display-p3, red, blue)");
  ASSERT_TRUE(p3_mix);

  EXPECT_FALSE(parse_css_color_rgba8("color-mix(in srgb red, blue)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(in srgb, red blue)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(in unknown-space, red, blue)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(in srgb, red 0%, blue 0%)").has_value());
}

TEST(Css, LightDarkExplicit) {
  auto light = parse_css_color_light_dark_rgba8("light-dark(red, blue)", false);
  ASSERT_TRUE(light);
  expect_rgba(*light, 255, 0, 0, 255);
  auto dark = parse_css_color_light_dark_rgba8("light-dark(red, blue)", true);
  ASSERT_TRUE(dark);
  expect_rgba(*dark, 0, 0, 255, 255);
}

TEST(Css, LightDarkNestedAndInvalidForms) {
  auto light = parse_css_color_light_dark_rgba8(
      "light-dark(color-mix(in srgb, red, white 50%), color(display-p3 0 0 1))", false);
  ASSERT_TRUE(light);
  EXPECT_GT(light->r(), 200);

  auto dark =
      parse_css_color_light_dark_rgba8("light-dark(color-mix(in srgb, red, white 50%), color(display-p3 0 0 1))", true);
  ASSERT_TRUE(dark);
  EXPECT_GT(dark->b(), 100);

  EXPECT_FALSE(parse_css_color_light_dark_rgba8("light-dark(red)", false).has_value());
  EXPECT_FALSE(parse_css_color_light_dark_rgba8("light-dark(red blue)", false).has_value());
  EXPECT_FALSE(parse_css_color_light_dark_rgba8("light-dark(red, blue) trailing", false).has_value());
}

TEST(Css, ContextAwareParsingSupportsCurrentColorAndSystemColors) {
  parse_css_color_context context;
  context.current_color = rgbaf_t{0.25f, 0.5f, 0.75f, 0.5f};
  context.canvas = rgbaf_t{1.0f, 1.0f, 1.0f, 1.0f};
  context.canvas_text = rgbaf_t{0.0f, 0.0f, 0.0f, 1.0f};
  context.link_text = rgbaf_t{0.0f, 0.2f, 1.0f, 1.0f};

  auto current = parse_css_color_rgba8("currentColor", context);
  ASSERT_TRUE(current);
  expect_rgba(*current, 64, 128, 191, 128);

  auto canvas = parse_css_color_rgba8("Canvas", context);
  ASSERT_TRUE(canvas);
  expect_rgba(*canvas, 255, 255, 255, 255);

  auto current_oklab = parse_css_color<oklab_t>("currentColor", context);
  ASSERT_TRUE(current_oklab);
  EXPECT_GT(current_oklab->l(), 0.0f);

  auto link = parse_css_color_rgbaf("LinkText", context);
  ASSERT_TRUE(link);
  expect_rgbaf_near(*link, 0.0f, 0.2f, 1.0f, 1.0f, 0.01f);

  EXPECT_FALSE(parse_css_color_rgba8("currentColor").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("CanvasText").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("AccentColor", context).has_value());
}

TEST(Css, ContextAwareLightDarkAndColorMixUseAmbientValues) {
  parse_css_color_context light_context;
  light_context.dark_theme = false;
  light_context.current_color = rgbaf_t{1.0f, 0.0f, 0.0f, 0.5f};
  light_context.canvas = rgbaf_t{1.0f, 1.0f, 1.0f, 1.0f};
  light_context.canvas_text = rgbaf_t{0.0f, 0.0f, 1.0f, 1.0f};

  auto light = parse_css_color_rgba8("light-dark(currentColor, CanvasText)", light_context);
  ASSERT_TRUE(light);
  expect_rgba(*light, 255, 0, 0, 128);

  auto mixed = parse_css_color_rgba8("color-mix(in srgb, currentColor, Canvas 50%)", light_context);
  ASSERT_TRUE(mixed);
  expect_rgba(*mixed, 255, 128, 128, 191);

  auto lab_mixed = parse_css_color_rgba8("color-mix(in lab, currentColor 25%, Canvas)", light_context);
  ASSERT_TRUE(lab_mixed);
  EXPECT_GT(lab_mixed->r(), 150);

  auto dark_context = light_context;
  dark_context.dark_theme = true;
  auto dark = parse_css_color_rgba8("light-dark(currentColor, CanvasText)", dark_context);
  ASSERT_TRUE(dark);
  expect_rgba(*dark, 0, 0, 255, 255);
}

TEST(Css, DeviceCmyk) {
  auto c = parse_css_color_rgba8("device-cmyk(0 1 1 0)");
  ASSERT_TRUE(c);
}

TEST(Css, DeviceCmykEdgeCases) {
  auto red = parse_css_color_rgba8("device-cmyk(0% 100% 100% 0% / 25%)");
  ASSERT_TRUE(red);
  expect_rgba(*red, 255, 0, 0, 64);

  auto redf = parse_css_color_rgbaf("device-cmyk(0% 100% 100% 0% / 25%)");
  ASSERT_TRUE(redf);
  expect_rgbaf_near(*redf, 1.0f, 0.0f, 0.0f, 0.25f, 0.01f);

  auto black = parse_css_color_rgba8("device-cmyk(none none none 100%)");
  ASSERT_TRUE(black);
  expect_rgba(*black, 0, 0, 0, 255);

  EXPECT_FALSE(parse_css_color_rgba8("device-cmyk(0, 1, 1, 0)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("device-cmyk(0 1 1)").has_value());
}

TEST(Css, NoneComponents) {
  auto c = parse_css_color_rgba8("rgb(none 255 0)");
  ASSERT_TRUE(c);
  expect_rgba(*c, 0, 255, 0, 255);
  ASSERT_TRUE(parse_css_color_rgba8("lab(none 10 10)").has_value());
  ASSERT_TRUE(parse_css_color_rgba8("hsl(none 100% 50%)").has_value());
  ASSERT_TRUE(parse_css_color_rgba8("color(srgb none none none)").has_value());
}

TEST(Css, ToCssColorStringRoundTrip) {
  rgba8_t tomato{255, 99, 71, 255};
  auto s = to_css_color_string(tomato);
  auto back = parse_css_color_rgba8(s);
  ASSERT_TRUE(back);
  expect_rgba(*back, 255, 99, 71, 255);
}

struct CssTableCase {
  std::string input;
  uint8_t r, g, b, a;
};

TEST(Css, WptStyleExactCorpus) {
  const std::vector<CssTableCase> cases = {
      {"#00ff00", 0, 255, 0, 255},
      {"#0000", 0, 0, 0, 0},
      {"transparent", 0, 0, 0, 0},
      {"rebeccapurple", 102, 51, 153, 255},
      {"rgb(255 0 153 / 50%)", 255, 0, 153, 128},
      {"rgb(-10 300 127.6 / 120%)", 0, 255, 128, 255},
      {"hsl(240deg 100% 50%)", 0, 0, 255, 255},
      {"hsl(-120deg 100% 50%)", 0, 0, 255, 255},
      {"hwb(120 0% 0%)", 0, 255, 0, 255},
      {"hwb(0 100% 0%)", 255, 255, 255, 255},
      {"color(srgb 0 1 0)", 0, 255, 0, 255},
      {"color(srgb-linear 1 0 0)", 255, 0, 0, 255},
      {"color(display-p3-linear 0 0 0 / 0)", 0, 0, 0, 0},
      {"color(xyz-d50 0.96422 1 0.82521)", 255, 255, 255, 255},
      {"color(xyz 0 0 0 / 25%)", 0, 0, 0, 64},
      {"color(rec2020 1 1 1 / 25%)", 255, 255, 255, 64},
      {"device-cmyk(0% 100% 100% 0%)", 255, 0, 0, 255},
      {"device-cmyk(none none none 100%)", 0, 0, 0, 255},
      {"color-mix(in srgb, red, red)", 255, 0, 0, 255},
      {"color-mix(in srgb, black, white)", 128, 128, 128, 255},
      {"color-mix(in srgb, red 20%, blue 20%)", 128, 0, 128, 255},
      {"color-mix(in srgb, transparent, transparent)", 0, 0, 0, 0},
      {"color-mix(in srgb, red, transparent 0%)", 255, 0, 0, 255},
      {" \n color(srgb 1 0 0 / 50%) \t", 255, 0, 0, 128},
  };
  for (const auto& row : cases) {
    SCOPED_TRACE(row.input);
    auto c = parse_css_color_rgba8(row.input);
    ASSERT_TRUE(c) << row.input;
    expect_rgba(*c, row.r, row.g, row.b, row.a);
  }
}

struct InvalidCssCase {
  std::string input;
};

TEST(Css, InvalidCorpusRejectsMalformedInputs) {
  const std::vector<InvalidCssCase> cases = {
      {""},
      {"notacolor"},
      {"#12"},
      {"#gg0000"},
      {"rgb(1 2)"},
      {"rgb(1,,2)"},
      {"rgb(1, 2 3)"},
      {"rgb(1 2, 3)"},
      {"rgb(1 2 3 /)"},
      {"rgb(1 2 3 / 25% 50%)"},
      {"hsl(120 100%)"},
      {"hsl(120, 100% 50%)"},
      {"lab(50, 25, -30)"},
      {"oklch(0.5, 0.1, 180)"},
      {"color(rec2020 0.1 0.2)"},
      {"color(unknown-space 1 2 3)"},
      {"color(xyz-d50 1 2 3 /)"},
      {"color(srgb 1 0 0) trailing"},
      {"color(srgb 1 0 0 / 0.5 0.2)"},
      {"device-cmyk(0, 1, 1, 0)"},
      {"device-cmyk(0 1 1)"},
      {"device-cmyk(0 1 1 0 /)"},
      {"color-mix(in unknown-space, red, blue)"},
      {"color-mix(in srgb, red 0%, blue 0%)"},
      {"color-mix(in srgb red, blue)"},
      {"color-mix(in srgb, red blue)"},
      {"color-mix(in srgb, red, blue 50%) trailing"},
  };
  for (const auto& row : cases) {
    SCOPED_TRACE(row.input);
    EXPECT_FALSE(parse_css_color_rgba8(row.input).has_value());
    EXPECT_FALSE(parse_css_color_rgbaf(row.input).has_value());
  }
}

struct LightDarkCase {
  std::string input;
  bool dark_theme;
  uint8_t r, g, b, a;
};

TEST(Css, LightDarkTableDriven) {
  const std::vector<LightDarkCase> cases = {
      {"light-dark(red, blue)", false, 255, 0, 0, 255},
      {"light-dark(red, blue)", true, 0, 0, 255, 255},
      {"light-dark(color-mix(in srgb, black, white), transparent)", false, 128, 128, 128, 255},
      {"light-dark(color-mix(in srgb, black, white), transparent)", true, 0, 0, 0, 0},
      {" \nlight-dark( color(srgb 1 0 0 / 50%), color(srgb 0 0 1 / 25%) )\t", false, 255, 0, 0, 128},
      {" \nlight-dark( color(srgb 1 0 0 / 50%), color(srgb 0 0 1 / 25%) )\t", true, 0, 0, 255, 64},
  };
  for (const auto& row : cases) {
    SCOPED_TRACE(row.input);
    auto c = parse_css_color_light_dark_rgba8(row.input, row.dark_theme);
    ASSERT_TRUE(c);
    expect_rgba(*c, row.r, row.g, row.b, row.a);
  }
}

TEST(Css, LightDarkInvalidCorpus) {
  const std::vector<InvalidCssCase> cases = {
      {""},
      {"light-dark(red)"},
      {"light-dark(red blue)"},
      {"light-dark(red, blue) trailing"},
      {"light-dark(, blue)"},
      {"light-dark(red, )"},
  };
  for (const auto& row : cases) {
    SCOPED_TRACE(row.input);
    EXPECT_FALSE(parse_css_color_light_dark_rgba8(row.input, false).has_value());
    EXPECT_FALSE(parse_css_color_light_dark_rgba8(row.input, true).has_value());
  }
}

}  // namespace colorcpp::io::test
