/**
 * @file test_hwb.cpp
 * @brief Tests for HWB color model and conversion.
 */

#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

using colorcpp::operations::conversion::color_cast;

namespace colorcpp::core::test {

using namespace colorcpp::io::css;

TEST(HWBTest, BasicConstruction) {
  hwb_float_t hwb;
  EXPECT_FLOAT_EQ(hwb.h(), 0.0f);
  EXPECT_FLOAT_EQ(hwb.w(), 0.0f);
  EXPECT_FLOAT_EQ(hwb.b(), 0.0f);

  hwb_float_t hwb2{120.0f, 0.2f, 0.3f};
  EXPECT_FLOAT_EQ(hwb2.h(), 120.0f);
  EXPECT_FLOAT_EQ(hwb2.w(), 0.2f);
  EXPECT_FLOAT_EQ(hwb2.b(), 0.3f);

  hwba_float_t hwba{180.0f, 0.1f, 0.2f, 0.5f};
  EXPECT_FLOAT_EQ(hwba.h(), 180.0f);
  EXPECT_FLOAT_EQ(hwba.w(), 0.1f);
  EXPECT_FLOAT_EQ(hwba.b(), 0.2f);
  EXPECT_FLOAT_EQ(hwba.a(), 0.5f);
}

TEST(HWBTest, HwbToRgbConversion) {
  // Pure red (h=0, w=0, b=0)
  hwb_float_t hwb_red{0.0f, 0.0f, 0.0f};
  auto rgb_red = color_cast<rgbaf_t>(hwb_red);
  EXPECT_NEAR(rgb_red.r(), 1.0f, 0.01f);
  EXPECT_NEAR(rgb_red.g(), 0.0f, 0.01f);
  EXPECT_NEAR(rgb_red.b(), 0.0f, 0.01f);

  // Pure green (h=120, w=0, b=0)
  hwb_float_t hwb_green{120.0f, 0.0f, 0.0f};
  auto rgb_green = color_cast<rgbaf_t>(hwb_green);
  EXPECT_NEAR(rgb_green.r(), 0.0f, 0.01f);
  EXPECT_NEAR(rgb_green.g(), 1.0f, 0.01f);
  EXPECT_NEAR(rgb_green.b(), 0.0f, 0.01f);

  // Pure blue (h=240, w=0, b=0)
  hwb_float_t hwb_blue{240.0f, 0.0f, 0.0f};
  auto rgb_blue = color_cast<rgbaf_t>(hwb_blue);
  EXPECT_NEAR(rgb_blue.r(), 0.0f, 0.01f);
  EXPECT_NEAR(rgb_blue.g(), 0.0f, 0.01f);
  EXPECT_NEAR(rgb_blue.b(), 1.0f, 0.01f);

  // White (w=1)
  hwb_float_t hwb_white{0.0f, 1.0f, 0.0f};
  auto rgb_white = color_cast<rgbaf_t>(hwb_white);
  EXPECT_NEAR(rgb_white.r(), 1.0f, 0.01f);
  EXPECT_NEAR(rgb_white.g(), 1.0f, 0.01f);
  EXPECT_NEAR(rgb_white.b(), 1.0f, 0.01f);

  // Black (b=1)
  hwb_float_t hwb_black{0.0f, 0.0f, 1.0f};
  auto rgb_black = color_cast<rgbaf_t>(hwb_black);
  EXPECT_NEAR(rgb_black.r(), 0.0f, 0.01f);
  EXPECT_NEAR(rgb_black.g(), 0.0f, 0.01f);
  EXPECT_NEAR(rgb_black.b(), 0.0f, 0.01f);

  // Achromatic (w+b >= 1)
  hwb_float_t hwb_achromatic{0.0f, 0.6f, 0.5f};
  auto rgb_achromatic = color_cast<rgbaf_t>(hwb_achromatic);
  float expected_gray = 0.6f / (0.6f + 0.5f);
  EXPECT_NEAR(rgb_achromatic.r(), expected_gray, 0.01f);
  EXPECT_NEAR(rgb_achromatic.g(), expected_gray, 0.01f);
  EXPECT_NEAR(rgb_achromatic.b(), expected_gray, 0.01f);

  // Mixed color
  hwb_float_t hwb_mixed{90.0f, 0.2f, 0.1f};
  auto rgb_mixed = color_cast<rgbaf_t>(hwb_mixed);
  EXPECT_GT(rgb_mixed.r(), 0.5f);
  EXPECT_GT(rgb_mixed.g(), 0.5f);
  EXPECT_LT(rgb_mixed.b(), 0.5f);
}

TEST(HWBTest, RgbToHwbConversion) {
  // Pure red
  rgbaf_t rgb_red{1.0f, 0.0f, 0.0f, 1.0f};
  auto hwb_red = color_cast<hwb_float_t>(rgb_red);
  EXPECT_NEAR(hwb_red.h(), 0.0f, 0.01f);
  EXPECT_NEAR(hwb_red.w(), 0.0f, 0.01f);
  EXPECT_NEAR(hwb_red.b(), 0.0f, 0.01f);

  // Pure white
  rgbaf_t rgb_white{1.0f, 1.0f, 1.0f, 1.0f};
  auto hwb_white = color_cast<hwb_float_t>(rgb_white);
  EXPECT_NEAR(hwb_white.w(), 1.0f, 0.01f);
  EXPECT_NEAR(hwb_white.b(), 0.0f, 0.01f);

  // Pure black
  rgbaf_t rgb_black{0.0f, 0.0f, 0.0f, 1.0f};
  auto hwb_black = color_cast<hwb_float_t>(rgb_black);
  EXPECT_NEAR(hwb_black.w(), 0.0f, 0.01f);
  EXPECT_NEAR(hwb_black.b(), 1.0f, 0.01f);

  // Gray
  rgbaf_t rgb_gray{0.5f, 0.5f, 0.5f, 1.0f};
  auto hwb_gray = color_cast<hwb_float_t>(rgb_gray);
  EXPECT_NEAR(hwb_gray.w(), 0.5f, 0.01f);
  EXPECT_NEAR(hwb_gray.b(), 0.5f, 0.01f);
}

TEST(HWBTest, RoundTripConversion) {
  // RGB -> HWB -> RGB
  rgbaf_t original{0.3f, 0.7f, 0.2f, 1.0f};
  auto hwb = color_cast<hwb_float_t>(original);
  auto rgb = color_cast<rgbaf_t>(hwb);
  EXPECT_NEAR(rgb.r(), original.r(), 0.01f);
  EXPECT_NEAR(rgb.g(), original.g(), 0.01f);
  EXPECT_NEAR(rgb.b(), original.b(), 0.01f);

  // HWB -> RGB -> HWB
  hwb_float_t original_hwb{180.0f, 0.3f, 0.2f};
  auto rgb2 = color_cast<rgbaf_t>(original_hwb);
  auto hwb2 = color_cast<hwb_float_t>(rgb2);
  EXPECT_NEAR(hwb2.h(), original_hwb.h(), 1.0f);
  EXPECT_NEAR(hwb2.w(), original_hwb.w(), 0.01f);
  EXPECT_NEAR(hwb2.b(), original_hwb.b(), 0.01f);
}

TEST(HWBTest, AlphaHandling) {
  // HWBA preserves alpha
  hwba_float_t hwba{120.0f, 0.1f, 0.2f, 0.7f};
  auto rgb = color_cast<rgbaf_t>(hwba);
  EXPECT_NEAR(rgb.a(), 0.7f, 0.01f);

  // HWB gets default alpha 1.0
  hwb_float_t hwb{60.0f, 0.2f, 0.3f};
  auto rgb2 = color_cast<rgbaf_t>(hwb);
  EXPECT_NEAR(rgb2.a(), 1.0f, 0.01f);
}

TEST(HWBTest, CSSParsing) {
  // hwb() with comma syntax
  auto color1 = parse_css_color<hwb_float_t>("hwb(120, 30%, 20%)");
  ASSERT_TRUE(color1.has_value());
  EXPECT_NEAR(color1->h(), 120.0f, 0.01f);
  EXPECT_NEAR(color1->w(), 0.3f, 0.01f);
  EXPECT_NEAR(color1->b(), 0.2f, 0.01f);

  // hwb() with space syntax
  auto color2 = parse_css_color<hwb_float_t>("hwb(180 40% 10%)");
  ASSERT_TRUE(color2.has_value());
  EXPECT_NEAR(color2->h(), 180.0f, 0.01f);
  EXPECT_NEAR(color2->w(), 0.4f, 0.01f);
  EXPECT_NEAR(color2->b(), 0.1f, 0.01f);

  // hwb() with alpha (comma syntax)
  auto color3 = parse_css_color<hwba_float_t>("hwb(90, 20%, 30%, 0.5)");
  ASSERT_TRUE(color3.has_value());
  EXPECT_NEAR(color3->h(), 90.0f, 0.01f);
  EXPECT_NEAR(color3->w(), 0.2f, 0.01f);
  EXPECT_NEAR(color3->b(), 0.3f, 0.01f);
  EXPECT_NEAR(color3->a(), 0.5f, 0.01f);

  // hwb() with alpha (space syntax)
  auto color4 = parse_css_color<hwba_float_t>("hwb(270 15% 25% / 0.8)");
  ASSERT_TRUE(color4.has_value());
  EXPECT_NEAR(color4->h(), 270.0f, 0.5f);  // Relaxed tolerance due to rgba8_t precision
  EXPECT_NEAR(color4->w(), 0.15f, 0.05f);
  EXPECT_NEAR(color4->b(), 0.25f, 0.05f);
  EXPECT_NEAR(color4->a(), 0.8f, 0.01f);

  // hwb() with degree unit
  auto color5 = parse_css_color<hwb_float_t>("hwb(180deg 50% 10%)");
  ASSERT_TRUE(color5.has_value());
  EXPECT_NEAR(color5->h(), 180.0f, 0.01f);

  // hwb() case insensitive
  auto color6 = parse_css_color<hwb_float_t>("HWB(60 20% 30%)");
  auto color7 = parse_css_color<hwb_float_t>("hwb(60 20% 30%)");
  ASSERT_TRUE(color6.has_value());
  ASSERT_TRUE(color7.has_value());
  EXPECT_FLOAT_EQ(color6->h(), color7->h());

  // CSS Color 4: W+B > 100% scales proportionally (same sRGB as normalized components)
  auto norm_gray = parse_css_color<rgba8_t>("hwb(0 50% 50%)");
  auto sum_gray = parse_css_color<rgba8_t>("hwb(0 80% 80%)");
  ASSERT_TRUE(norm_gray.has_value());
  ASSERT_TRUE(sum_gray.has_value());
  EXPECT_EQ(norm_gray->r(), sum_gray->r());
  EXPECT_EQ(norm_gray->g(), sum_gray->g());
  EXPECT_EQ(norm_gray->b(), sum_gray->b());
  EXPECT_EQ(norm_gray->a(), sum_gray->a());

  // Invalid HWB syntax
  EXPECT_FALSE(parse_css_color<hwb_float_t>("hwb()").has_value());
  EXPECT_FALSE(parse_css_color<hwb_float_t>("hwb(120)").has_value());
  EXPECT_FALSE(parse_css_color<hwb_float_t>("hwb(120 30%)").has_value());
}

TEST(HWBTest, CopyConstructors) {
  hwb_float_t original(120.0f, 0.2f, 0.3f);
  hwb_float_t copy(original);

  EXPECT_FLOAT_EQ(copy.h(), 120.0f);
  EXPECT_FLOAT_EQ(copy.w(), 0.2f);
  EXPECT_FLOAT_EQ(copy.b(), 0.3f);
}

TEST(HWBTest, CopyAssignment) {
  hwb_float_t original(120.0f, 0.2f, 0.3f);
  hwb_float_t copy(0.0f, 0.0f, 0.0f);
  copy = original;

  EXPECT_FLOAT_EQ(copy.h(), 120.0f);
  EXPECT_FLOAT_EQ(copy.w(), 0.2f);
  EXPECT_FLOAT_EQ(copy.b(), 0.3f);
}

TEST(HWBTest, MoveSemantics) {
  hwb_float_t original(120.0f, 0.2f, 0.3f);
  hwb_float_t moved(std::move(original));

  EXPECT_FLOAT_EQ(moved.h(), 120.0f);
  EXPECT_FLOAT_EQ(moved.w(), 0.2f);
  EXPECT_FLOAT_EQ(moved.b(), 0.3f);
}

TEST(HWBTest, MoveAssignment) {
  hwb_float_t original(120.0f, 0.2f, 0.3f);
  hwb_float_t moved(0.0f, 0.0f, 0.0f);
  moved = std::move(original);

  EXPECT_FLOAT_EQ(moved.h(), 120.0f);
  EXPECT_FLOAT_EQ(moved.w(), 0.2f);
  EXPECT_FLOAT_EQ(moved.b(), 0.3f);
}

TEST(HWBTest, EqualityComparison) {
  hwb_float_t color1(120.0f, 0.2f, 0.3f);
  hwb_float_t color2(120.0f, 0.2f, 0.3f);
  hwb_float_t color3(130.0f, 0.2f, 0.3f);

  EXPECT_TRUE(color1 == color2);
  EXPECT_FALSE(color1 == color3);
  EXPECT_TRUE(color1 != color3);
}

TEST(HWBTest, ConstexprBoundaryValues) {
  constexpr hwb_float_t min_color(0.0f, 0.0f, 0.0f);
  static_assert(min_color.h() == 0.0f);
  static_assert(min_color.w() == 0.0f);
  static_assert(min_color.b() == 0.0f);

  constexpr hwb_float_t max_color(360.0f, 1.0f, 1.0f);
  static_assert(max_color.h() == 360.0f);
  static_assert(max_color.w() == 1.0f);
  static_assert(max_color.b() == 1.0f);
}

TEST(HWBATest, CopyConstructors) {
  hwba_float_t original(180.0f, 0.1f, 0.2f, 0.5f);
  hwba_float_t copy(original);

  EXPECT_FLOAT_EQ(copy.h(), 180.0f);
  EXPECT_FLOAT_EQ(copy.w(), 0.1f);
  EXPECT_FLOAT_EQ(copy.b(), 0.2f);
  EXPECT_FLOAT_EQ(copy.a(), 0.5f);
}

TEST(HWBATest, CopyAssignment) {
  hwba_float_t original(180.0f, 0.1f, 0.2f, 0.5f);
  hwba_float_t copy(0.0f, 0.0f, 0.0f, 0.0f);
  copy = original;

  EXPECT_FLOAT_EQ(copy.h(), 180.0f);
  EXPECT_FLOAT_EQ(copy.w(), 0.1f);
  EXPECT_FLOAT_EQ(copy.b(), 0.2f);
  EXPECT_FLOAT_EQ(copy.a(), 0.5f);
}

TEST(HWBATest, MoveSemantics) {
  hwba_float_t original(180.0f, 0.1f, 0.2f, 0.5f);
  hwba_float_t moved(std::move(original));

  EXPECT_FLOAT_EQ(moved.h(), 180.0f);
  EXPECT_FLOAT_EQ(moved.w(), 0.1f);
  EXPECT_FLOAT_EQ(moved.b(), 0.2f);
  EXPECT_FLOAT_EQ(moved.a(), 0.5f);
}

TEST(HWBATest, MoveAssignment) {
  hwba_float_t original(180.0f, 0.1f, 0.2f, 0.5f);
  hwba_float_t moved(0.0f, 0.0f, 0.0f, 0.0f);
  moved = std::move(original);

  EXPECT_FLOAT_EQ(moved.h(), 180.0f);
  EXPECT_FLOAT_EQ(moved.w(), 0.1f);
  EXPECT_FLOAT_EQ(moved.b(), 0.2f);
  EXPECT_FLOAT_EQ(moved.a(), 0.5f);
}

TEST(HWBATest, EqualityComparison) {
  hwba_float_t color1(180.0f, 0.1f, 0.2f, 0.5f);
  hwba_float_t color2(180.0f, 0.1f, 0.2f, 0.5f);
  hwba_float_t color3(180.0f, 0.1f, 0.2f, 0.6f);

  EXPECT_TRUE(color1 == color2);
  EXPECT_FALSE(color1 == color3);
  EXPECT_TRUE(color1 != color3);
}

TEST(HWBTest, BoundaryValues) {
  hwb_float_t min_color(0.0f, 0.0f, 0.0f);
  EXPECT_FLOAT_EQ(min_color.h(), 0.0f);
  EXPECT_FLOAT_EQ(min_color.w(), 0.0f);
  EXPECT_FLOAT_EQ(min_color.b(), 0.0f);

  hwb_float_t max_color(360.0f, 1.0f, 1.0f);
  EXPECT_FLOAT_EQ(max_color.h(), 360.0f);
  EXPECT_FLOAT_EQ(max_color.w(), 1.0f);
  EXPECT_FLOAT_EQ(max_color.b(), 1.0f);
}

TEST(HWBTest, OutOfRangeThrows) {
  EXPECT_THROW(hwb_float_t(-1.0f, 0.5f, 0.5f), std::out_of_range);    // H < 0
  EXPECT_THROW(hwb_float_t(361.0f, 0.5f, 0.5f), std::out_of_range);   // H > 360
  EXPECT_THROW(hwb_float_t(180.0f, -0.1f, 0.5f), std::out_of_range);  // W < 0
  EXPECT_THROW(hwb_float_t(180.0f, 1.1f, 0.5f), std::out_of_range);   // W > 1
  EXPECT_THROW(hwb_float_t(180.0f, 0.5f, -0.1f), std::out_of_range);  // B < 0
  EXPECT_THROW(hwb_float_t(180.0f, 0.5f, 1.1f), std::out_of_range);   // B > 1
}

TEST(HWBTest, MemberAccess) {
  hwb_float_t hwb(120.0f, 0.2f, 0.3f);

  EXPECT_FLOAT_EQ(hwb.h(), 120.0f);
  EXPECT_FLOAT_EQ(hwb.w(), 0.2f);
  EXPECT_FLOAT_EQ(hwb.b(), 0.3f);

  hwb.h() = 150.0f;
  hwb.w() = 0.4f;
  hwb.b() = 0.5f;

  EXPECT_FLOAT_EQ(hwb.h(), 150.0f);
  EXPECT_FLOAT_EQ(hwb.w(), 0.4f);
  EXPECT_FLOAT_EQ(hwb.b(), 0.5f);
}

TEST(HWBTest, IndexAccess) {
  hwb_float_t hwb(120.0f, 0.2f, 0.3f);

  EXPECT_FLOAT_EQ(hwb.get_index<0>(), 120.0f);
  EXPECT_FLOAT_EQ(hwb.get_index<1>(), 0.2f);
  EXPECT_FLOAT_EQ(hwb.get_index<2>(), 0.3f);
}

TEST(HWBTest, ConstCorrectness) {
  const hwb_float_t hwb(120.0f, 0.2f, 0.3f);

  EXPECT_FLOAT_EQ(hwb.h(), 120.0f);
  EXPECT_FLOAT_EQ(hwb.w(), 0.2f);
  EXPECT_FLOAT_EQ(hwb.b(), 0.3f);
}

}  // namespace colorcpp::core::test
