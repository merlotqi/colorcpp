#include <colorcpp/colorcpp.hpp>
#include <gtest/gtest.h>

using namespace colorcpp::core;

static void expect_rgba(const rgba8_t& c, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  EXPECT_EQ(c.r(), r);
  EXPECT_EQ(c.g(), g);
  EXPECT_EQ(c.b(), b);
  EXPECT_EQ(c.a(), a);
}

TEST(CssColor, Hex) {
  ASSERT_TRUE(parse_css_color_rgba8("#f63").has_value());
  expect_rgba(*parse_css_color_rgba8("#f63"), 255, 102, 51, 255);
  ASSERT_TRUE(parse_css_color_rgba8("  #FF6347  ").has_value());
  expect_rgba(*parse_css_color_rgba8("  #FF6347  "), 255, 99, 71, 255);
  ASSERT_TRUE(parse_css_color_rgba8("#FF634780").has_value());
  expect_rgba(*parse_css_color_rgba8("#FF634780"), 255, 99, 71, 128);
  EXPECT_FALSE(parse_css_color_rgba8("#gg0000").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("#12").has_value());
}

TEST(CssColor, RgbComma) {
  auto c = parse_css_color_rgba8("rgb(255, 99, 71)");
  ASSERT_TRUE(c);
  expect_rgba(*c, 255, 99, 71, 255);

  ASSERT_TRUE(parse_css_color_rgba8("RGB(0, 0, 0)").has_value());
  expect_rgba(*parse_css_color_rgba8("RGB(0, 0, 0)"), 0, 0, 0, 255);
  ASSERT_TRUE(parse_css_color_rgba8("rgba(255, 0, 0, 0.5)").has_value());
  EXPECT_EQ(parse_css_color_rgba8("rgba(255, 0, 0, 0.5)")->a(), 128);
}

TEST(CssColor, RgbSpaceSlash) {
  ASSERT_TRUE(parse_css_color_rgba8("rgb(255 99 71)").has_value());
  expect_rgba(*parse_css_color_rgba8("rgb(255 99 71)"), 255, 99, 71, 255);
  auto c = parse_css_color_rgba8("rgb(255 99 71 / 50%)");
  ASSERT_TRUE(c);
  EXPECT_EQ(c->r(), 255);
  EXPECT_EQ(c->g(), 99);
  EXPECT_EQ(c->b(), 71);
  EXPECT_EQ(c->a(), 128);
}

TEST(CssColor, RgbPercent) {
  ASSERT_TRUE(parse_css_color_rgba8("rgb(100% 0% 0%)").has_value());
  expect_rgba(*parse_css_color_rgba8("rgb(100% 0% 0%)"), 255, 0, 0, 255);
  ASSERT_TRUE(parse_css_color_rgba8("rgb(50% 50% 50%)").has_value());
  expect_rgba(*parse_css_color_rgba8("rgb(50% 50% 50%)"), 128, 128, 128, 255);
}

TEST(CssColor, HslComma) {
  auto c = parse_css_color_rgba8("hsl(0, 100%, 50%)");
  ASSERT_TRUE(c);
  EXPECT_EQ(c->r(), 255);
  EXPECT_EQ(c->g(), 0);
  EXPECT_EQ(c->b(), 0);
}

TEST(CssColor, HslSpaceSlash) {
  auto c = parse_css_color_rgba8("hsl(120 100% 50% / 80%)");
  ASSERT_TRUE(c);
  EXPECT_GT(c->g(), 200);
  EXPECT_EQ(c->a(), 204);
}

TEST(CssColor, HslHueUnits) {
  auto a = parse_css_color_rgba8("hsl(0.5turn 100% 50%)");
  auto b = parse_css_color_rgba8("hsl(180deg 100% 50%)");
  ASSERT_TRUE(a);
  ASSERT_TRUE(b);
  EXPECT_EQ(a->r(), b->r());
  EXPECT_EQ(a->g(), b->g());
  EXPECT_EQ(a->b(), b->b());
}

TEST(CssColor, ParseTemplate) {
  auto hsl = parse_css_color<hsla_float_t>("hsl(0, 100%, 50%)");
  ASSERT_TRUE(hsl);
  EXPECT_NEAR(hsl->h(), 0.0f, 1.0f);
  EXPECT_NEAR(hsl->s(), 1.0f, 0.05f);
  EXPECT_NEAR(hsl->l(), 0.5f, 0.05f);
}

TEST(CssColor, Invalid) {
  EXPECT_FALSE(parse_css_color_rgba8("").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("red").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("rgb(1 2)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("rgb(1,,2)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color(srgb 1 0 0)").has_value());
}
