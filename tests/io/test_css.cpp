#include <gtest/gtest.h>

#include <chrono>
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

TEST(Css, ParseNumberRejectsNaN) {
  // rgb(nan ...) — strtod would parse "nan" as NaN, but parse_number rejects it
  EXPECT_FALSE(parse_css_color_rgba8("rgb(nan, 0, 0)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("rgb(-nan, 0, 0)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("rgb(NAN, 0, 0)").has_value());
}

TEST(Css, ParseNumberRejectsInf) {
  EXPECT_FALSE(parse_css_color_rgba8("rgb(inf, 0, 0)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("rgb(-inf, 0, 0)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("rgb(infinity, 0, 0)").has_value());
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
  // lab() is CIE L*a*b* with the D50 white point (CSS Color 4 §9.1); the
  // parser adapts to the D65-based core::cielab_t via Bradford, so expected
  // values are the D65 equivalents (computed independently in double precision).
  // lab(L a b) with numbers — direct lab() parse (no rgba8 hub)
  auto c1 = parse_css_color<cielab_t>("lab(50 25 -30)");
  ASSERT_TRUE(c1.has_value());
  EXPECT_NEAR(c1->l(), 50.2149f, 0.01f);
  EXPECT_NEAR(c1->a(), 27.7958f, 0.01f);
  EXPECT_NEAR(c1->b(), -29.8948f, 0.01f);

  // lab(L a b) with percentage for L
  auto c2 = parse_css_color<cielab_t>("lab(75% 20 -25)");
  ASSERT_TRUE(c2.has_value());
  EXPECT_NEAR(c2->l(), 75.1601f, 0.01f);
  EXPECT_NEAR(c2->a(), 22.3285f, 0.01f);
  EXPECT_NEAR(c2->b(), -24.9638f, 0.01f);

  // lab(L a b / alpha) - alpha is parsed but ignored
  auto c3 = parse_css_color<cielab_t>("lab(60 30 -40 / 0.5)");
  ASSERT_TRUE(c3.has_value());
  EXPECT_NEAR(c3->l(), 60.3283f, 0.01f);
  EXPECT_NEAR(c3->a(), 33.8153f, 0.01f);
  EXPECT_NEAR(c3->b(), -39.7658f, 0.01f);
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
  // lch() is D50-based like lab(); expectations are the Bradford-adapted
  // D65 equivalents of the parsed values (independent double-precision
  // computation) — note the hue shift the white-point change induces.
  // lch(L C H) with numbers — direct lch() parse (no rgba8 hub)
  auto c1 = parse_css_color<cielch_t>("lch(70 30 180)");
  ASSERT_TRUE(c1.has_value());
  EXPECT_NEAR(c1->l(), 70.1745f, 0.01f);
  EXPECT_NEAR(c1->c(), 29.7571f, 0.01f);
  EXPECT_NEAR(c1->h(), 179.1885f, 0.01f);

  // lch(L C H) with percentage for L and C
  auto c2 = parse_css_color<cielch_t>("lch(75% 20% 90deg)");
  ASSERT_TRUE(c2.has_value());
  EXPECT_NEAR(c2->l(), 74.7517f, 0.01f);
  EXPECT_NEAR(c2->c(), 29.9620f, 0.01f);
  EXPECT_NEAR(c2->h(), 94.4713f, 0.01f);

  // lch(L C H / alpha) - alpha is parsed but ignored
  auto c3 = parse_css_color<cielch_t>("lch(50 40 270 / 0.8)");
  ASSERT_TRUE(c3.has_value());
  EXPECT_NEAR(c3->l(), 50.5554f, 0.01f);
  EXPECT_NEAR(c3->c(), 39.6064f, 0.01f);
  EXPECT_NEAR(c3->h(), 277.3211f, 0.01f);
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

TEST(Css, LabLchD50RendersCssExample) {
  // D50 lab/lch of peru (#CD853F) — CSS Color 4 worked example numbers for
  // lch(62.2532 54.0114 63.6769). Both forms must render to peru in sRGB;
  // storing the D50 channels unadapted in the D65 core type breaks this.
  auto from_lch = parse_css_color_rgbaf("lch(62.2532 54.0114 63.6769)");
  ASSERT_TRUE(from_lch);
  expect_rgbaf_near(*from_lch, 0.803922f, 0.521569f, 0.247059f, 1.0f, 0.003f);

  auto from_lab = parse_css_color_rgbaf("lab(62.2532 23.9504 48.4108)");
  ASSERT_TRUE(from_lab);
  expect_rgbaf_near(*from_lab, 0.803922f, 0.521569f, 0.247059f, 1.0f, 0.003f);
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
  expect_leaf("color(rec2020 0.47428 0.52316 0.41425)");
}

TEST(Css, ColorFunctionRec2020MatchesSpecSameColor) {
  // CSS Color 4 section 11.1 (#ex-same-color): color(rec2020 0.522 0.557 0.892) is #6c88ea.
  // Verifies the rec2020 transfer function (gamma 2.4 from BT.1886, not the BT.2020 OETF).
  auto c = parse_css_color<rgbaf_t>("color(rec2020 0.522 0.557 0.892)");
  ASSERT_TRUE(c);
  expect_rgbaf_near(*c, 0.423529f, 0.533333f, 0.917647f, 1.0f, 0.003f);
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

TEST(Css, RelativeColorAstEvaluateSupportsVar) {
  auto ast = parse_css_color_ast("rgb(from var(--theme-primary) r g calc(b * 1.2) / 0.8)");
  ASSERT_TRUE(ast);
  EXPECT_TRUE(ast->is_relative());

  auto result = evaluate<rgbaf_t>(*ast, [](std::string_view name) -> std::optional<rgbaf_t> {
    if (name == "--theme-primary") return rgbaf_t{0.9f, 0.2f, 0.4f, 1.0f};
    return std::nullopt;
  });
  ASSERT_TRUE(result);
  expect_rgbaf_near(*result, 0.9f, 0.2f, 0.48f, 0.8f, 0.01f);

  auto concrete = parse_css_color_ast("red");
  ASSERT_TRUE(concrete);
  EXPECT_TRUE(concrete->is_concrete());
  auto concrete_eval =
      evaluate<rgba8_t>(*concrete, [](std::string_view) -> std::optional<rgbaf_t> { return std::nullopt; });
  ASSERT_TRUE(concrete_eval);
  expect_rgba(*concrete_eval, 255, 0, 0, 255);
}

TEST(Css, RelativeColorParsingUsesContextVariableResolver) {
  parse_css_color_context context;
  context.variable_resolver = [](std::string_view name) -> std::optional<rgbaf_t> {
    if (name == "--theme-primary") return rgbaf_t{0.9f, 0.2f, 0.4f, 1.0f};
    return std::nullopt;
  };

  auto rgb_relative = parse_css_color_rgbaf("rgb(from var(--theme-primary) r g calc(b * 1.2) / 0.8)", context);
  ASSERT_TRUE(rgb_relative);
  expect_rgbaf_near(*rgb_relative, 0.9f, 0.2f, 0.48f, 0.8f, 0.01f);

  auto color_relative =
      parse_css_color_rgbaf("color(from var(--theme-primary) srgb calc(r * 0.5) g b / alpha)", context);
  ASSERT_TRUE(color_relative);
  expect_rgbaf_near(*color_relative, 0.45f, 0.2f, 0.4f, 1.0f, 0.01f);

  EXPECT_FALSE(parse_css_color_rgbaf("rgb(from var(--missing) r g b)", context).has_value());
  EXPECT_FALSE(parse_css_color_rgbaf("rgb(from var(--theme-primary) q g b)", context).has_value());
  EXPECT_FALSE(parse_css_color_rgbaf("rgb(from var(--theme-primary) r g b)").has_value());
}

TEST(Css, RelativeColorTypedParsingPreservesTargetSpace) {
  auto relative_p3 = parse_css_color<display_p3f_t>("color(from color(display-p3 1 0 0) display-p3 r g b)");
  ASSERT_TRUE(relative_p3);
  EXPECT_NEAR(relative_p3->r(), 1.0f, 0.001f);
  EXPECT_NEAR(relative_p3->g(), 0.0f, 0.001f);
  EXPECT_NEAR(relative_p3->b(), 0.0f, 0.001f);

  auto relative_xyz = parse_css_color<xyz_t>("color(from color(display-p3 1 0 0) xyz-d65 x y z / 25%)");
  ASSERT_TRUE(relative_xyz);
  auto expected_xyz = parse_css_color<xyz_t>("color(display-p3 1 0 0 / 25%)");
  ASSERT_TRUE(expected_xyz);
  EXPECT_NEAR(relative_xyz->x(), expected_xyz->x(), 0.002f);
  EXPECT_NEAR(relative_xyz->y(), expected_xyz->y(), 0.002f);
  EXPECT_NEAR(relative_xyz->z(), expected_xyz->z(), 0.002f);
}

// ---------------------------------------------------------------------------
// CSS Color 5 relative color syntax: oklab(from ...)/oklch(from ...)
// Expected sRGB values computed independently in double precision from the
// spec's resulting oklab/oklch channel values (not via colorcpp).
// ---------------------------------------------------------------------------

TEST(Css, RelativeOklabSpecExample) {
  // CSS Color 5: oklab(from var(--bluegreen) calc(1.0 - l) calc(a * 0.8) b)
  // with --bluegreen: oklab(54.3% -22.5% -5%) => result oklab(0.457 -0.072 -0.02)
  auto c = parse_css_color_rgbaf("oklab(from oklab(0.543 -0.09 -0.02) calc(1 - l) calc(a * 0.8) b)");
  ASSERT_TRUE(c);
  expect_rgbaf_near(*c, 0.04696f, 0.391693f, 0.393793f, 1.0f, 0.003f);
}

TEST(Css, RelativeOklchHueRotate) {
  // oklch(from oklch(0.526 0.115 44.6deg) l c calc(h + 90)) => oklch(0.526 0.115 134.6)
  auto c = parse_css_color_rgbaf("oklch(from oklch(0.526 0.115 44.6deg) l c calc(h + 90))");
  ASSERT_TRUE(c);
  expect_rgbaf_near(*c, 0.300863f, 0.473593f, 0.184649f, 1.0f, 0.003f);
}

TEST(Css, RelativeOklchIdentity) {
  auto c = parse_css_color_rgbaf("oklch(from oklch(0.526 0.115 44.6deg) l c h)");
  ASSERT_TRUE(c);
  expect_rgbaf_near(*c, 0.626653f, 0.321147f, 0.17627f, 1.0f, 0.003f);
}

TEST(Css, RelativeOklchAlphaClamped) {
  // CSS Color 5: alpha is clamped to [0, 1]; channels are not scaled.
  auto c = parse_css_color_rgbaf("oklch(from rgb(255 0 0 / 0.7) l c h / calc(alpha * 2))");
  ASSERT_TRUE(c);
  EXPECT_NEAR(c->a(), 1.0f, 0.0001f);
  // channels: red's oklch round-trip must stay red
  expect_rgbaf_near(*c, 1.0f, 0.0f, 0.0f, 1.0f, 0.003f);
}

TEST(Css, RelativeOklchDefaultAlphaIsSource) {
  auto c = parse_css_color_rgbaf("oklch(from rgb(255 0 0 / 0.7) l c h)");
  ASSERT_TRUE(c);
  EXPECT_NEAR(c->a(), 0.7f, 0.003f);
}

TEST(Css, RelativeOklchPercentageScales) {
  // oklch percentages: l 100% = 1, c 100% = 0.4.
  // oklch(from white 0% 100% 0) => oklch(0 0.4 0) — out of sRGB gamut, clamps to ~black-red edge.
  auto c = parse_css_color<oklch_t>("oklch(from white 50% 50% 0)");
  ASSERT_TRUE(c);
  EXPECT_NEAR(c->l(), 0.5f, 0.0001f);
  EXPECT_NEAR(c->c(), 0.2f, 0.0001f);
  EXPECT_NEAR(c->h(), 0.0f, 0.0001f);
}

TEST(Css, RelativeColorRejectsMalformed) {
  EXPECT_FALSE(parse_css_color_rgbaf("oklch(from red 1 2)").has_value());      // missing channel
  EXPECT_FALSE(parse_css_color_rgbaf("oklch(from red 1 2 3 4)").has_value());  // extra channel
  EXPECT_FALSE(parse_css_color_rgbaf("oklch(from red w g b)").has_value());    // wrong keywords
  EXPECT_FALSE(parse_css_color_rgbaf("oklab(from red l c h)").has_value());    // c not in oklab
  // CSS Color 5: non-absolute colors are not permitted as relative sources
  EXPECT_FALSE(parse_css_color_rgbaf("rgb(from light-dark(red, blue) r g b)").has_value());
  EXPECT_FALSE(parse_css_color_rgbaf("rgb(from contrast-color(white) r g b)").has_value());
}

// ---------------------------------------------------------------------------
// CSS Color 5 relative color syntax: hsl(from ...)/hwb(from ...)
// Expected sRGB values computed independently in double precision.
// ---------------------------------------------------------------------------

TEST(Css, RelativeHslIdentity) {
  // peru = #CD853F = hsl(29.5775 58.6777% 52.549%); identity must round-trip.
  auto c = parse_css_color_rgbaf("hsl(from peru h s l)");
  ASSERT_TRUE(c);
  expect_rgbaf_near(*c, 0.803922f, 0.521569f, 0.247059f, 1.0f, 0.002f);
}

TEST(Css, RelativeHslHueRotate) {
  // CSS Color 5: hsl(from var(--accent) calc(h + 180) s l) — complement.
  // lightseagreen #20B2AA = hsl(176.7123 69.5238% 41.1765%) => hsl(356.7123 ...)
  auto c = parse_css_color_rgbaf("hsl(from lightseagreen calc(h + 180) s l)");
  ASSERT_TRUE(c);
  expect_rgbaf_near(*c, 0.698039f, 0.125490f, 0.156863f, 1.0f, 0.003f);
}

TEST(Css, RelativeHslAngleUnitsInCalc) {
  // Angle units resolve to degrees: calc(h + 90deg) on peru.
  auto c = parse_css_color_rgbaf("hsl(from peru calc(h + 90deg) s l)");
  ASSERT_TRUE(c);
  expect_rgbaf_near(*c, 0.250980f, 0.803922f, 0.247059f, 1.0f, 0.003f);

  auto turn = parse_css_color_rgbaf("hsl(from peru calc(h + 0.25turn) s l)");
  ASSERT_TRUE(turn);
  expect_rgbaf_near(*turn, 0.250980f, 0.803922f, 0.247059f, 1.0f, 0.003f);
}

TEST(Css, RelativeHslPercentageScales) {
  // hsl percentages: s and l, 100% = 100. hsl(from peru h 100% 50%) = hsl(29.5775 100% 50%).
  auto c = parse_css_color_rgbaf("hsl(from peru h 100% 50%)");
  ASSERT_TRUE(c);
  // full-saturation mid-lightness of peru's hue: r=1, g=x, b=0 with m=0
  expect_rgbaf_near(*c, 1.0f, 0.492958f, 0.0f, 1.0f, 0.003f);
}

TEST(Css, RelativeHwbIdentity) {
  // peru as HWB: whiteness = min channel = 24.7059%, blackness = 1 - max = 19.6078%.
  auto c = parse_css_color_rgbaf("hwb(from peru h w b)");
  ASSERT_TRUE(c);
  expect_rgbaf_near(*c, 0.803922f, 0.521569f, 0.247059f, 1.0f, 0.002f);
}

TEST(Css, RelativeHwbSpecFormula) {
  // CSS Color 4/5: result = C * (1 - W - B) + W on the full-saturation hue color.
  auto c = parse_css_color_rgbaf("hwb(from peru h 30% 20%)");
  ASSERT_TRUE(c);
  expect_rgbaf_near(*c, 0.8f, 0.546479f, 0.3f, 1.0f, 0.003f);

  auto whiteish = parse_css_color_rgbaf("hwb(from peru h 100% 0%)");
  ASSERT_TRUE(whiteish);
  expect_rgbaf_near(*whiteish, 1.0f, 1.0f, 1.0f, 1.0f, 0.0001f);
}

TEST(Css, RelativeHslHwbRejectWrongKeywords) {
  EXPECT_FALSE(parse_css_color_rgbaf("hsl(from red r g b)").has_value());
  EXPECT_FALSE(parse_css_color_rgbaf("hwb(from red r g b)").has_value());
  EXPECT_FALSE(parse_css_color_rgbaf("hsl(from red h s)").has_value());
}

// ---------------------------------------------------------------------------
// CSS Color 5 relative color syntax: lab(from ...)/lch(from ...)
// Channels resolve in D50 (CSS lab/lch white point) and are adapted to D65
// for the core types/sRGB. Expected values computed independently.
// ---------------------------------------------------------------------------

TEST(Css, RelativeLabIdentity) {
  auto c = parse_css_color_rgbaf("lab(from peru l a b)");
  ASSERT_TRUE(c);
  expect_rgbaf_near(*c, 0.803922f, 0.521569f, 0.247059f, 1.0f, 0.003f);
}

TEST(Css, RelativeLabLighten) {
  // CSS Color 5 pattern: lab(from <color> calc(l + 10) a b).
  auto c = parse_css_color_rgbaf("lab(from peru calc(l + 10) a b)");
  ASSERT_TRUE(c);
  expect_rgbaf_near(*c, 0.9210f, 0.6250f, 0.3453f, 1.0f, 0.003f);
}

TEST(Css, RelativeLabPercentageScales) {
  // lab percentages: l 100% = 100; a/b 100% = ±125. White origin: l=100, a=b=0.
  auto typed = parse_css_color<cielab_t>("lab(from white 50% 25% -50%)");
  ASSERT_TRUE(typed);
  EXPECT_NEAR(typed->l(), 50.8066f, 0.01f);
  EXPECT_NEAR(typed->a(), 38.0474f, 0.01f);
  EXPECT_NEAR(typed->b(), -61.5119f, 0.01f);

  auto c = parse_css_color_rgbaf("lab(from white 50% 25% -50%)");
  ASSERT_TRUE(c);
  expect_rgbaf_near(*c, 0.4682f, 0.4019f, 0.8909f, 1.0f, 0.003f);
}

TEST(Css, RelativeLchIdentity) {
  auto c = parse_css_color_rgbaf("lch(from peru l c h)");
  ASSERT_TRUE(c);
  expect_rgbaf_near(*c, 0.803922f, 0.521569f, 0.247059f, 1.0f, 0.003f);
}

TEST(Css, RelativeLchChromaScale) {
  auto c = parse_css_color_rgbaf("lch(from peru l calc(c * 1.5) h)");
  ASSERT_TRUE(c);
  expect_rgbaf_near(*c, 0.8799f, 0.4807f, 0.0f, 1.0f, 0.003f);
}

TEST(Css, RelativeLchPercentageScales) {
  // lch percentages: l 100% = 100; c 100% = 150. lch(from white 50% 50% 0) = D50 lch(50 75 0).
  auto c = parse_css_color_rgbaf("lch(from white 50% 50% 0)");
  ASSERT_TRUE(c);
  expect_rgbaf_near(*c, 0.8811f, 0.0806f, 0.4814f, 1.0f, 0.003f);
}

TEST(Css, RelativeLabAlphaDefaultsToSource) {
  auto c = parse_css_color_rgbaf("lab(from rgb(255 0 0 / 0.7) l a b)");
  ASSERT_TRUE(c);
  expect_rgbaf_near(*c, 1.0f, 0.0f, 0.0f, 0.7f, 0.005f);

  auto clamped = parse_css_color_rgbaf("lab(from rgb(255 0 0 / 0.7) l a b / calc(alpha * 2))");
  ASSERT_TRUE(clamped);
  EXPECT_NEAR(clamped->a(), 1.0f, 0.001f);
}

TEST(Css, RelativeLabLchRejectWrongKeywords) {
  EXPECT_FALSE(parse_css_color_rgbaf("lab(from red l c h)").has_value());
  EXPECT_FALSE(parse_css_color_rgbaf("lch(from red l a b)").has_value());
  EXPECT_FALSE(parse_css_color_rgbaf("lab(from red l a)").has_value());
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
  EXPECT_EQ(normalized->a(), 102);
}

TEST(Css, ColorMixDefaultsToOklabAndAllowsSingleItem) {
  auto implicit = parse_css_color_rgbaf("color-mix(red, blue)");
  auto explicit_oklab = parse_css_color_rgbaf("color-mix(in oklab, red, blue)");
  ASSERT_TRUE(implicit);
  ASSERT_TRUE(explicit_oklab);
  expect_rgbaf_near(*implicit, explicit_oklab->r(), explicit_oklab->g(), explicit_oklab->b(), explicit_oklab->a(),
                    0.01f);

  auto single = parse_css_color_rgbaf("color-mix(in srgb, red)");
  ASSERT_TRUE(single);
  expect_rgbaf_near(*single, 1.0f, 0.0f, 0.0f, 1.0f, 0.01f);
}

TEST(Css, ColorMixSupportsMultiItemLists) {
  auto equal = parse_css_color_rgba8("color-mix(in srgb, red, green, blue)");
  ASSERT_TRUE(equal);
  expect_rgba(*equal, 85, 43, 85, 255);

  auto weighted = parse_css_color_rgba8("color-mix(in srgb, 25% red, blue, white 25%)");
  ASSERT_TRUE(weighted);
  expect_rgba(*weighted, 128, 64, 191, 255);

  auto folded = parse_css_color_rgbaf(
      "color-mix(in oklch longer hue, oklch(0.7 0.04 30), oklch(0.7 0.04 90), oklch(0.7 0.04 150))");
  auto manual = parse_css_color_rgbaf(
      "color-mix(in oklch longer hue, color-mix(in oklch longer hue, oklch(0.7 0.04 30), oklch(0.7 0.04 90)), "
      "oklch(0.7 0.04 150) 33.3333%)");
  ASSERT_TRUE(folded);
  ASSERT_TRUE(manual);
  expect_rgbaf_near(*folded, manual->r(), manual->g(), manual->b(), manual->a(), 0.02f);
}

TEST(Css, ColorMixSupportsProgressForm) {
  auto simple = parse_css_color_rgbaf("color-mix(25%, red, blue)");
  auto equivalent = parse_css_color_rgbaf("color-mix(red 75%, blue 25%)");
  ASSERT_TRUE(simple);
  ASSERT_TRUE(equivalent);
  expect_rgbaf_near(*simple, equivalent->r(), equivalent->g(), equivalent->b(), equivalent->a(), 0.01f);

  auto method_first = parse_css_color_rgbaf("color-mix(in srgb 25%, red, blue)");
  auto method_equivalent = parse_css_color_rgbaf("color-mix(in srgb, red 75%, blue 25%)");
  ASSERT_TRUE(method_first);
  ASSERT_TRUE(method_equivalent);
  expect_rgbaf_near(*method_first, method_equivalent->r(), method_equivalent->g(), method_equivalent->b(),
                    method_equivalent->a(), 0.01f);

  auto progress_first = parse_css_color_rgbaf("color-mix(25% in srgb, red, blue)");
  ASSERT_TRUE(progress_first);
  expect_rgbaf_near(*progress_first, method_equivalent->r(), method_equivalent->g(), method_equivalent->b(),
                    method_equivalent->a(), 0.01f);
}

TEST(Css, ColorMixRejectsMalformedExtendedForms) {
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(25%, red)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(in srgb 25%, red)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(in srgb, red,, blue)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(in srgb, red 25% 50%, blue)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(in srgb longer hue, red, blue)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(in srgb, red 0%, blue 0%, white 0%)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(25% in srgb, red, blue) trailing").has_value());
}

TEST(Css, ColorMixNestedAndInvalidForms) {
  auto nested = parse_css_color_rgba8("color-mix(in srgb, color(srgb 1 0 0), color(display-p3 0 0 1) 50%)");
  ASSERT_TRUE(nested);
  EXPECT_GT(nested->r(), 100);
  EXPECT_GT(nested->b(), 100);

  auto transparent_mix = parse_css_color_rgba8("color-mix(in srgb, transparent, red 50%)");
  ASSERT_TRUE(transparent_mix);
  expect_rgba(*transparent_mix, 255, 0, 0, 128);

  auto semi_transparent = parse_css_color_rgbaf("color-mix(in srgb, rgb(100% 0% 0% / 70%) 25%, rgb(0% 100% 0% / 20%))");
  ASSERT_TRUE(semi_transparent);
  expect_rgbaf_near(*semi_transparent, 0.53846f, 0.46154f, 0.0f, 0.325f, 0.01f);

  // CSS Color 5: non-absolute colors are not permitted inside color-mix().
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(in srgb, red, light-dark(blue, green))").has_value());
  EXPECT_FALSE(parse_css_color_rgbaf("color-mix(in srgb, LIGHT-DARK(blue, green), red)").has_value());

  // Relative colors remain legal color-mix items.
  auto relative_item = parse_css_color_rgbaf("color-mix(in srgb, rgb(from red r g b), blue)");
  ASSERT_TRUE(relative_item);

  auto oklab_mix = parse_css_color_rgbaf("color-mix(in oklab, red, blue)");
  ASSERT_TRUE(oklab_mix);
  auto expected_oklab = operations::interpolate::lerp_oklab<rgbaf_t>(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f},
                                                                     rgbaf_t{0.0f, 0.0f, 1.0f, 1.0f}, 0.5f);
  expect_rgbaf_near(*oklab_mix, expected_oklab.r(), expected_oklab.g(), expected_oklab.b(), expected_oklab.a(), 0.02f);

  auto linear_mix = parse_css_color_rgbaf("color-mix(in srgb-linear, red, blue)");
  ASSERT_TRUE(linear_mix);

  auto p3_mix = parse_css_color_rgbaf("color-mix(in display-p3, red, blue)");
  ASSERT_TRUE(p3_mix);

  EXPECT_FALSE(parse_css_color_rgba8("color-mix(in srgb red, blue)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(in srgb, red blue)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(in unknown-space, red, blue)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(in srgb, red 0%, blue 0%)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(in srgb longer hue, red, blue)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(in oklch longer, red, blue)").has_value());
}

TEST(Css, ColorMixPowerlessHueTakesOtherEndpoint) {
  // Issue #51: white's OkLCH hue is atan2 noise (H = 90°, C = 6e-8). Without
  // powerless-hue substitution, color-mix(in oklch, white, blue) blended that
  // noise toward 264° and produced teal rgb(0 197 165). Per CSS Color 4 the
  // powerless hue is missing and takes blue's 264.05°: the spec midpoint is
  // oklch(0.726 0.157 264.05) → rgb(0.453, 0.638, 1).
  auto c = parse_css_color_rgbaf("color-mix(in oklch, white, blue)");
  ASSERT_TRUE(c);
  EXPECT_NEAR(c->r(), 0.4530f, 5e-3f);
  EXPECT_NEAR(c->g(), 0.6381f, 5e-3f);
  EXPECT_NEAR(c->b(), 1.0f, 5e-3f);

  // Mid-gray is just as powerless as white. Spec midpoint:
  // oklch((0.598 + 0.452)/2, 0.157, 264.05) → rgb(0.229, 0.392, 0.767).
  auto g = parse_css_color_rgbaf("color-mix(in oklch, gray, blue)");
  ASSERT_TRUE(g);
  EXPECT_NEAR(g->r(), 0.2286f, 5e-3f);
  EXPECT_NEAR(g->g(), 0.3919f, 5e-3f);
  EXPECT_NEAR(g->b(), 0.7666f, 5e-3f);

  // CIELCH gets the same rule (blue's LCH hue is 306.29°).
  auto l = parse_css_color_rgbaf("color-mix(in lch, white, blue)");
  ASSERT_TRUE(l);
  EXPECT_NEAR(l->r(), 0.7014f, 5e-3f);
  EXPECT_NEAR(l->g(), 0.5458f, 5e-3f);
  EXPECT_NEAR(l->b(), 1.0f, 5e-3f);
}

TEST(Css, ColorMixHueInterpolationKeywords) {
  auto shorter = parse_css_color_rgbaf("color-mix(in oklch shorter hue, oklch(0.7 0.04 30), oklch(0.7 0.04 90))");
  auto shorter_expected = parse_css_color_rgbaf("oklch(0.7 0.04 60)");
  ASSERT_TRUE(shorter);
  ASSERT_TRUE(shorter_expected);
  expect_rgbaf_near(*shorter, shorter_expected->r(), shorter_expected->g(), shorter_expected->b(),
                    shorter_expected->a(), 0.01f);

  auto longer = parse_css_color_rgbaf("color-mix(in oklch longer hue, oklch(0.7 0.04 30), oklch(0.7 0.04 90))");
  auto longer_expected = parse_css_color_rgbaf("oklch(0.7 0.04 240)");
  ASSERT_TRUE(longer);
  ASSERT_TRUE(longer_expected);
  expect_rgbaf_near(*longer, longer_expected->r(), longer_expected->g(), longer_expected->b(), longer_expected->a(),
                    0.01f);

  auto increasing =
      parse_css_color_rgbaf("color-mix(in oklch increasing hue, oklch(0.7 0.04 300), oklch(0.7 0.04 30))");
  auto increasing_expected = parse_css_color_rgbaf("oklch(0.7 0.04 345)");
  ASSERT_TRUE(increasing);
  ASSERT_TRUE(increasing_expected);
  expect_rgbaf_near(*increasing, increasing_expected->r(), increasing_expected->g(), increasing_expected->b(),
                    increasing_expected->a(), 0.01f);

  auto decreasing =
      parse_css_color_rgbaf("color-mix(in oklch decreasing hue, oklch(0.7 0.04 300), oklch(0.7 0.04 30))");
  auto decreasing_expected = parse_css_color_rgbaf("oklch(0.7 0.04 165)");
  ASSERT_TRUE(decreasing);
  ASSERT_TRUE(decreasing_expected);
  expect_rgbaf_near(*decreasing, decreasing_expected->r(), decreasing_expected->g(), decreasing_expected->b(),
                    decreasing_expected->a(), 0.01f);

  auto exact_half_turn = parse_css_color_rgbaf("color-mix(in oklch, oklch(0.7 0.04 30), oklch(0.7 0.04 210))");
  auto exact_half_turn_expected = parse_css_color_rgbaf("oklch(0.7 0.04 120)");
  ASSERT_TRUE(exact_half_turn);
  ASSERT_TRUE(exact_half_turn_expected);
  expect_rgbaf_near(*exact_half_turn, exact_half_turn_expected->r(), exact_half_turn_expected->g(),
                    exact_half_turn_expected->b(), exact_half_turn_expected->a(), 0.01f);
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
  expect_rgba(*mixed, 255, 170, 170, 191);

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

TEST(Css, ToCssColorStringRgbafRoundTrip) {
  // rgb() numeric channels are 0-255 in CSS; serializing unit floats as bare
  // numbers made "rgb(1.0 0 0)" parse back as r = 1/255.
  const rgbaf_t src{1.0f, 0.4f, 0.2f, 0.5f};
  auto s = to_css_color_string(src);
  auto back = parse_css_color<rgbaf_t>(s);
  ASSERT_TRUE(back);
  expect_rgbaf_near(*back, 1.0f, 0.4f, 0.2f, 0.5f, 0.002f);
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
      {"color-mix(in srgb, red)", 255, 0, 0, 255},
      {"color-mix(25% in srgb, red, blue)", 191, 0, 64, 255},
      {"color-mix(in srgb, red, green, blue)", 85, 43, 85, 255},
      {"color-mix(in srgb, red 20%, blue 20%)", 128, 0, 128, 102},
      {"color-mix(in srgb, transparent, transparent)", 0, 0, 0, 0},
      {"color-mix(in srgb, transparent, red 50%)", 255, 0, 0, 128},
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
      {"color-mix(25%, red)"},
      {"color-mix(in srgb 25%, red)"},
      {"color-mix(in srgb, red,, blue)"},
      {"color-mix(in srgb, red 25% 50%, blue)"},
      {"color-mix(in srgb, red 0%, blue 0%, white 0%)"},
      {"color-mix(25% in srgb, red, blue) trailing"},
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

// contrast-color(<color>) — CSS Color 5 §8: resolves to white or black,
// whichever contrasts more (WCAG luminance); equal contrast → white.

TEST(Css, ContrastColorPicksWhiteOnDark) {
  auto c = parse_css_color_rgbaf("contrast-color(black)");
  ASSERT_TRUE(c);
  expect_rgbaf_near(*c, 1.0f, 1.0f, 1.0f, 1.0f, 0.002f);
}

TEST(Css, ContrastColorPicksBlackOnLight) {
  auto c = parse_css_color_rgbaf("contrast-color(white)");
  ASSERT_TRUE(c);
  expect_rgbaf_near(*c, 0.0f, 0.0f, 0.0f, 1.0f, 0.002f);
  // WCAG luminance of gray 128 is ~0.2159 > tie (~0.17913) → black
  auto g = parse_css_color_rgbaf("rgb(128 128 128)");
  ASSERT_TRUE(g);
  auto r = parse_css_color_rgbaf("contrast-color(rgb(128 128 128))");
  ASSERT_TRUE(r);
  expect_rgbaf_near(*r, 0.0f, 0.0f, 0.0f, 1.0f, 0.002f);
}

TEST(Css, ContrastColorTieBoundary) {
  // Grays straddling the WCAG tie luminance sqrt(0.0525) - 0.05 ≈ 0.1791287847
  // (independent double-precision reference; ±1e-5 in luminance).
  auto below = parse_css_color_rgbaf("color(srgb 0.460301333 0.460301333 0.460301333)");
  ASSERT_TRUE(below);
  auto rb = parse_css_color_rgbaf("contrast-color(color(srgb 0.460301333 0.460301333 0.460301333))");
  ASSERT_TRUE(rb);
  expect_rgbaf_near(*rb, 1.0f, 1.0f, 1.0f, 1.0f, 0.002f);

  auto above = parse_css_color_rgbaf("color(srgb 0.460325306 0.460325306 0.460325306)");
  ASSERT_TRUE(above);
  auto ra = parse_css_color_rgbaf("contrast-color(color(srgb 0.460325306 0.460325306 0.460325306))");
  ASSERT_TRUE(ra);
  expect_rgbaf_near(*ra, 0.0f, 0.0f, 0.0f, 1.0f, 0.002f);
}

TEST(Css, ContrastColorIgnoresAlpha) {
  // Background alpha does not affect the luminance decision; output is opaque.
  auto c = parse_css_color_rgbaf("contrast-color(rgba(255 255 255 / 0))");
  ASSERT_TRUE(c);
  expect_rgbaf_near(*c, 0.0f, 0.0f, 0.0f, 1.0f, 0.002f);
}

TEST(Css, ContrastColorAcceptsFullColorArgument) {
  // The argument is a full <color>: relative syntax and color-mix() recurse.
  auto rel = parse_css_color_rgbaf("contrast-color(rgb(from black r g b))");
  ASSERT_TRUE(rel);
  expect_rgbaf_near(*rel, 1.0f, 1.0f, 1.0f, 1.0f, 0.002f);

  auto mix = parse_css_color_rgbaf("contrast-color(color-mix(in srgb, black 50%, white))");
  ASSERT_TRUE(mix);
  expect_rgbaf_near(*mix, 0.0f, 0.0f, 0.0f, 1.0f, 0.002f);

  // CSS Color 5: contrast-color() is non-absolute and not permitted in color-mix().
  EXPECT_FALSE(parse_css_color_rgbaf("color-mix(in srgb, contrast-color(black), black)").has_value());
}

TEST(Css, ContrastColorContextAwareArgument) {
  // light-dark() inside the argument resolves with the ambient context.
  auto light = parse_css_color_rgbaf("contrast-color(light-dark(black, white))");
  ASSERT_TRUE(light);
  expect_rgbaf_near(*light, 1.0f, 1.0f, 1.0f, 1.0f, 0.002f);

  parse_css_color_context ctx;
  ctx.dark_theme = true;
  auto dark = parse_css_color_rgbaf("contrast-color(light-dark(black, white))", ctx);
  ASSERT_TRUE(dark);
  expect_rgbaf_near(*dark, 0.0f, 0.0f, 0.0f, 1.0f, 0.002f);
}

TEST(Css, ContrastColorTypedAndRgba8) {
  auto lab = parse_css_color<core::cielab_t>("contrast-color(white)");
  ASSERT_TRUE(lab);
  EXPECT_NEAR(lab->l(), 0.0f, 0.02f);
  EXPECT_NEAR(lab->a(), 0.0f, 0.02f);
  EXPECT_NEAR(lab->b(), 0.0f, 0.02f);

  auto rgbf = parse_css_color<core::rgbf_t>("contrast-color(black)");
  ASSERT_TRUE(rgbf);
  EXPECT_NEAR(rgbf->r(), 1.0f, 0.002f);
  EXPECT_NEAR(rgbf->g(), 1.0f, 0.002f);
  EXPECT_NEAR(rgbf->b(), 1.0f, 0.002f);

  auto rgba = parse_css_color_rgba8("contrast-color(white)");
  ASSERT_TRUE(rgba);
  expect_rgba(*rgba, 0, 0, 0, 255);
}

TEST(Css, ContrastColorRejectsMalformed) {
  const std::vector<InvalidCssCase> cases = {
      {"contrast-color()"},
      {"contrast-color(white black)"},
      {"contrast-color(nope)"},
      {"contrast-color(white"},
      {"contrast-color(white))"},
      {"contrast-color white)"},
      {"contrast-color(rgb(1 2 3)"},
      // relative sources resolve through the atomic parser: non-absolute
      // contrast-color() is not accepted there
      {"rgb(from contrast-color(white) r g b)"},
  };
  for (const auto& row : cases) {
    SCOPED_TRACE(row.input);
    EXPECT_FALSE(parse_css_color_rgbaf(row.input).has_value());
    EXPECT_FALSE(parse_css_color_rgba8(row.input).has_value());
  }
}

// lab()/lch()/oklab()/oklch() used to parse "/ alpha" and then discard it, so
// every rgba8/rgbaf conversion came back fully opaque. The slash alpha must
// survive the cast into the alpha-carrying targets.
TEST(Css, LabFunctionAlphaSurvivesRgba8Cast) {
  auto c = parse_css_color_rgba8("lab(50% 40 59.5 / 0.5)");
  ASSERT_TRUE(c);
  EXPECT_EQ(c->a(), 128);
}

TEST(Css, LchFunctionAlphaSurvivesRgba8Cast) {
  auto c = parse_css_color_rgba8("lch(50% 40 270 / 50%)");
  ASSERT_TRUE(c);
  EXPECT_EQ(c->a(), 128);
}

TEST(Css, OklabFunctionAlphaSurvivesRgba8Cast) {
  auto c = parse_css_color_rgba8("oklab(0.5 0.1 -0.2 / 25%)");
  ASSERT_TRUE(c);
  EXPECT_EQ(c->a(), 64);
}

TEST(Css, OklchFunctionAlphaSurvivesRgba8Cast) {
  auto c = parse_css_color_rgba8("oklch(0.6 0.1 30 / 0)");
  ASSERT_TRUE(c);
  EXPECT_EQ(c->a(), 0);
}

TEST(Css, OklchFunctionAlphaSurvivesRgbafCast) {
  auto c = parse_css_color<rgbaf_t>("oklch(0.6 0.1 30 / 0.25)");
  ASSERT_TRUE(c);
  EXPECT_NEAR(c->a(), 0.25f, 0.002f);
}

TEST(Css, LabFunctionDefaultsToOpaqueWithoutAlpha) {
  auto c = parse_css_color_rgba8("lab(50% 40 59.5)");
  ASSERT_TRUE(c);
  EXPECT_EQ(c->a(), 255);
}

// Channel-expression parsing recurses through calc() and parenthesized
// groups; without a depth limit, ~20000 levels of nesting overflow the stack
// (segfault). The parser must fail cleanly (nullopt) instead.
TEST(Css, DeepCalcNestingFailsCleanlyInsteadOfCrashing) {
  std::string s = "rgb(from red ";
  for (int i = 0; i < 20000; ++i) s += "calc(";
  s += "1";
  for (int i = 0; i < 20000; ++i) s += ")";
  s += " g b)";
  EXPECT_FALSE(parse_css_color_rgba8(s).has_value());
}

TEST(Css, DeepParenNestingFailsCleanlyInsteadOfCrashing) {
  std::string s = "rgb(from red ";
  for (int i = 0; i < 20000; ++i) s += "(";
  s += "1";
  for (int i = 0; i < 20000; ++i) s += ")";
  s += " g b)";
  EXPECT_FALSE(parse_css_color_rgba8(s).has_value());
}

// A flat 1+1+1+... chain parses iteratively but builds a left-deep AST;
// evaluate_expression then recurses once per operator and overflows.
TEST(Css, LongAdditiveChainFailsCleanlyInsteadOfCrashing) {
  std::string s = "rgb(from red calc(1";
  for (int i = 0; i < 50000; ++i) s += "+1";
  s += ") g b)";
  EXPECT_FALSE(parse_css_color_rgba8(s).has_value());
}

TEST(Css, ModerateNestingStillParses) {
  // Depth well under the limit must keep working.
  std::string s = "rgb(from red calc(calc(calc(1 + 2))) g b)";
  auto c = parse_css_color_rgba8(s);
  ASSERT_TRUE(c);
}

// Issue #63: the #53 depth budget lives only in the calc() parser, so every
// other recursive entry (color-mix, light-dark, relative from) still overflows
// the stack at ~20000 levels. Each form must fail cleanly with nullopt instead.
// Uniform nesting makes the deep input cheap to build: prefix*n + core + suffix*n.

TEST(Css, DeepColorMixNestingFailsCleanlyInsteadOfCrashing) {
  std::string s;
  for (int i = 0; i < 20000; ++i) s += "color-mix(in srgb, ";
  s += "red";
  for (int i = 0; i < 20000; ++i) s += ", blue)";
  const auto t0 = std::chrono::steady_clock::now();
  const auto r = parse_css_color_rgba8(s);
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0);
  EXPECT_FALSE(r.has_value());
  EXPECT_LT(ms.count(), 2000);
}

TEST(Css, DeepLightDarkNestingFailsCleanlyInsteadOfCrashing) {
  std::string s;
  for (int i = 0; i < 20000; ++i) s += "light-dark(";
  s += "red";
  for (int i = 0; i < 20000; ++i) s += ", blue)";
  EXPECT_FALSE(parse_css_color_rgba8(s).has_value());
}

TEST(Css, DeepRgbFromNestingFailsCleanlyInsteadOfCrashing) {
  std::string s;
  for (int i = 0; i < 20000; ++i) s += "rgb(from ";
  s += "red";
  for (int i = 0; i < 20000; ++i) s += " r g b)";
  EXPECT_FALSE(parse_css_color_rgba8(s).has_value());
}

TEST(Css, DeepHslFromNestingFailsCleanlyInsteadOfCrashing) {
  std::string s;
  for (int i = 0; i < 20000; ++i) s += "hsl(from ";
  s += "red";
  for (int i = 0; i < 20000; ++i) s += " h s l)";
  EXPECT_FALSE(parse_css_color_rgba8(s).has_value());
}

TEST(Css, DeepOklchFromNestingFailsCleanlyInsteadOfCrashing) {
  std::string s;
  for (int i = 0; i < 20000; ++i) s += "oklch(from ";
  s += "red";
  for (int i = 0; i < 20000; ++i) s += " l c h)";
  EXPECT_FALSE(parse_css_color_rgba8(s).has_value());
}

// Polar from() resolves its origin twice per level (alpha + channels), so the
// cost doubles per level: depth 22 took ~1.7s at -O2 before the fix. One
// shared resolution per level makes it linear.
TEST(Css, PolarFromNestingRunsInBoundedTime) {
  std::string s;
  for (int i = 0; i < 22; ++i) s += "hsl(from ";
  s += "red";
  for (int i = 0; i < 22; ++i) s += " h s l)";
  const auto t0 = std::chrono::steady_clock::now();
  const auto r = parse_css_color_rgba8(s);
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0);
  EXPECT_TRUE(r.has_value());  // depth 22 is under the cap
  EXPECT_LT(ms.count(), 500);
}

// Nesting beyond the shared budget fails with nullopt; pre-fix this input
// parses fine (quadratic but fast), so the missing limit shows up here.
TEST(Css, ColorMixBeyondDepthBudgetFailsCleanly) {
  std::string s;
  for (int i = 0; i < 100; ++i) s += "color-mix(in srgb, ";
  s += "red";
  for (int i = 0; i < 100; ++i) s += ", blue)";
  EXPECT_FALSE(parse_css_color_rgba8(s).has_value());
}

// Depth well under the budget must keep parsing for every recursive form.
TEST(Css, ModerateFromNestingStillParses) {
  std::string s;
  for (int i = 0; i < 16; ++i) s += "hsl(from ";
  s += "red";
  for (int i = 0; i < 16; ++i) s += " h s l)";
  EXPECT_TRUE(parse_css_color_rgba8(s).has_value());
}

}  // namespace colorcpp::io::test
