#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

namespace colorcpp::io::test {

using namespace colorcpp::core;
using namespace colorcpp::io::css;

static void expect_rgba(const rgba8_t& c, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  EXPECT_EQ(c.r(), r);
  EXPECT_EQ(c.g(), g);
  EXPECT_EQ(c.b(), b);
  EXPECT_EQ(c.a(), a);
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

TEST(Css, ParseTemplate) {
  auto hsl = parse_css_color<hsla_float_t>("hsl(0, 100%, 50%)");
  ASSERT_TRUE(hsl);
  EXPECT_NEAR(hsl->h(), 0.0f, 1.0f);
  EXPECT_NEAR(hsl->s(), 1.0f, 0.05f);
  EXPECT_NEAR(hsl->l(), 0.5f, 0.05f);
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

  // Wrong color space
  EXPECT_FALSE(parse_css_color<display_p3f_t>("color(srgb 0.5 0.3 0.2)").has_value());
}

}  // namespace colorcpp::io::test
