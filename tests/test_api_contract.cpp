#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

TEST(PublicApiContract, CanonicalNamespacesWorkFromUmbrellaHeader) {
  using namespace colorcpp::io::literals;

  colorcpp::core::rgba8_t red{255, 0, 0, 255};
  auto hsv = colorcpp::operations::conversion::color_cast<colorcpp::core::hsva_float_t>(red);
  auto parsed = colorcpp::io::css::parse_css_color_rgba8("coral");
  auto ratio = colorcpp::algorithms::accessibility::contrast_ratio(red, colorcpp::constants::white);
  auto clipped = colorcpp::algorithms::gamut::gamut_clip(colorcpp::core::oklch_t{0.6f, 0.4f, 180.0f});
  auto random = colorcpp::operations::random_color<colorcpp::core::rgbf_t>(123u);
  auto literal = 0xFF6347_rgb;

  ASSERT_TRUE(parsed);
  EXPECT_EQ(literal.r(), 255);
  EXPECT_EQ(parsed->r(), 255);
  EXPECT_GE(hsv.h(), 0.0f);
  EXPECT_GT(ratio, 1.0f);
  EXPECT_TRUE(colorcpp::algorithms::gamut::is_in_srgb_gamut(clipped));
  EXPECT_GE(random.r(), 0.0f);
  EXPECT_LE(random.r(), 1.0f);
}

TEST(PublicApiContract, ConvenienceReexportsRemainAvailable) {
  colorcpp::rgba8_t red{255, 0, 0, 255};
  auto hsv = colorcpp::conversion::color_cast<colorcpp::hsva_float_t>(red);
  auto ratio = colorcpp::accessibility::contrast_ratio(red, colorcpp::constants::white);
  auto clipped = colorcpp::gamut::gamut_clip(colorcpp::oklch_t{0.6f, 0.4f, 180.0f});

  EXPECT_GE(hsv.h(), 0.0f);
  EXPECT_GT(ratio, 1.0f);
  EXPECT_TRUE(colorcpp::gamut::is_in_srgb_gamut(clipped));
}
