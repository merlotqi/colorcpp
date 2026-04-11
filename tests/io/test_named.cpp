/**
 * @file test_named.cpp
 * @brief Unit tests for CSS named color literal operator.
 */

#include <gtest/gtest.h>

#include <colorcpp/io/css/named_colors.hpp>

using namespace colorcpp::io::css;

// ===== get_named_color tests =====

TEST(NamedColor, BasicColors) {
  auto red = get_named_color("red");
  ASSERT_TRUE(red.has_value());
  EXPECT_EQ(red->r(), 255);
  EXPECT_EQ(red->g(), 0);
  EXPECT_EQ(red->b(), 0);
  EXPECT_EQ(red->a(), 255);

  auto green = get_named_color("green");
  ASSERT_TRUE(green.has_value());
  EXPECT_EQ(green->r(), 0);
  EXPECT_EQ(green->g(), 128);
  EXPECT_EQ(green->b(), 0);

  auto blue = get_named_color("blue");
  ASSERT_TRUE(blue.has_value());
  EXPECT_EQ(blue->r(), 0);
  EXPECT_EQ(blue->g(), 0);
  EXPECT_EQ(blue->b(), 255);
}

TEST(NamedColor, CaseInsensitive) {
  auto lower = get_named_color("coral");
  auto upper = get_named_color("CORAL");
  auto mixed = get_named_color("CoRaL");

  ASSERT_TRUE(lower.has_value());
  ASSERT_TRUE(upper.has_value());
  ASSERT_TRUE(mixed.has_value());

  EXPECT_EQ(*lower, *upper);
  EXPECT_EQ(*lower, *mixed);
  EXPECT_EQ(lower->r(), 255);
  EXPECT_EQ(lower->g(), 127);
  EXPECT_EQ(lower->b(), 80);
}

TEST(NamedColor, InvalidName) {
  EXPECT_FALSE(get_named_color("notacolor").has_value());
  EXPECT_FALSE(get_named_color("").has_value());
  EXPECT_FALSE(get_named_color("redd").has_value());
}

TEST(NamedColor, TransparentKeyword) {
  auto transparent = get_named_color("transparent");
  auto transparent_upper = get_named_color("TRANSPARENT");
  ASSERT_TRUE(transparent.has_value());
  ASSERT_TRUE(transparent_upper.has_value());
  EXPECT_EQ(*transparent, *transparent_upper);
  EXPECT_EQ(transparent->r(), 0);
  EXPECT_EQ(transparent->g(), 0);
  EXPECT_EQ(transparent->b(), 0);
  EXPECT_EQ(transparent->a(), 0);
}

TEST(NamedColor, IsNamedColor) {
  EXPECT_TRUE(is_named_color("red"));
  EXPECT_TRUE(is_named_color("CORAL"));
  EXPECT_TRUE(is_named_color("steelblue"));
  EXPECT_TRUE(is_named_color("transparent"));
  EXPECT_FALSE(is_named_color("notacolor"));
  EXPECT_FALSE(is_named_color(""));
}

// ===== _color literal tests =====

TEST(NamedColor, LiteralBasic) {
  using namespace colorcpp::io::css::named_literal;
  auto c = "red"_color;
  EXPECT_EQ(c.r(), 255);
  EXPECT_EQ(c.g(), 0);
  EXPECT_EQ(c.b(), 0);
  EXPECT_EQ(c.a(), 255);
}

TEST(NamedColor, LiteralCoral) {
  using namespace colorcpp::io::css::named_literal;
  auto c = "coral"_color;
  EXPECT_EQ(c.r(), 255);
  EXPECT_EQ(c.g(), 127);
  EXPECT_EQ(c.b(), 80);
  EXPECT_EQ(c.a(), 255);
}

TEST(NamedColor, LiteralCaseInsensitive) {
  using namespace colorcpp::io::css::named_literal;
  auto a = "steelblue"_color;
  auto b = "SteelBlue"_color;
  auto c = "STEELBLUE"_color;

  EXPECT_EQ(a, b);
  EXPECT_EQ(a, c);
  EXPECT_EQ(a.r(), 70);
  EXPECT_EQ(a.g(), 130);
  EXPECT_EQ(a.b(), 180);
}

TEST(NamedColor, LiteralWhite) {
  using namespace colorcpp::io::css::named_literal;
  auto c = "white"_color;
  EXPECT_EQ(c.r(), 255);
  EXPECT_EQ(c.g(), 255);
  EXPECT_EQ(c.b(), 255);
  EXPECT_EQ(c.a(), 255);
}

TEST(NamedColor, LiteralBlack) {
  using namespace colorcpp::io::css::named_literal;
  auto c = "black"_color;
  EXPECT_EQ(c.r(), 0);
  EXPECT_EQ(c.g(), 0);
  EXPECT_EQ(c.b(), 0);
  EXPECT_EQ(c.a(), 255);
}

TEST(NamedColor, LiteralInvalidThrows) {
  using namespace colorcpp::io::css::named_literal;
  EXPECT_THROW("notacolor"_color, std::invalid_argument);
}

TEST(NamedColor, LiteralTransparent) {
  using namespace colorcpp::io::css::named_literal;
  auto c = "transparent"_color;
  EXPECT_EQ(c.r(), 0);
  EXPECT_EQ(c.g(), 0);
  EXPECT_EQ(c.b(), 0);
  EXPECT_EQ(c.a(), 0);
}

// ===== Additional named colors =====

TEST(NamedColor, ExtendedColors) {
  EXPECT_TRUE(is_named_color("cornflowerblue"));
  EXPECT_TRUE(is_named_color("mediumaquamarine"));
  EXPECT_TRUE(is_named_color("lightgoldenrodyellow"));
  EXPECT_TRUE(is_named_color("papayawhip"));
  using namespace colorcpp::io::css::named_literal;

  auto cf = "cornflowerblue"_color;
  EXPECT_EQ(cf.r(), 100);
  EXPECT_EQ(cf.g(), 149);
  EXPECT_EQ(cf.b(), 237);
}
