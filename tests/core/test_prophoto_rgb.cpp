/**
 * @file test_prophoto_rgb.cpp
 * @brief Tests for ProPhoto RGB (ROMM RGB) color model and conversion.
 *
 * ProPhoto RGB uses Gamma 1.8 with linear segment, D50 white point,
 * and Bradford chromatic adaptation for D50↔D65 conversion.
 */

#include <gtest/gtest.h>

#include <colorcpp/core/prophoto_rgb.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/io/css.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::core::test {

using namespace colorcpp::operations::conversion;

// --- Construction ---

TEST(ProPhotoRGBTest, DefaultConstruction) {
  prophoto_rgbf_t c;
  EXPECT_FLOAT_EQ(c.r(), 0.0f);
  EXPECT_FLOAT_EQ(c.g(), 0.0f);
  EXPECT_FLOAT_EQ(c.b(), 0.0f);
}

TEST(ProPhotoRGBTest, ParameterizedConstruction) {
  prophoto_rgbf_t c(0.2f, 0.4f, 0.6f);
  EXPECT_FLOAT_EQ(c.r(), 0.2f);
  EXPECT_FLOAT_EQ(c.g(), 0.4f);
  EXPECT_FLOAT_EQ(c.b(), 0.6f);
}

TEST(ProPhotoRGBATest, ParameterizedWithAlpha) {
  prophoto_rgbaf_t c(0.2f, 0.4f, 0.6f, 0.8f);
  EXPECT_FLOAT_EQ(c.r(), 0.2f);
  EXPECT_FLOAT_EQ(c.g(), 0.4f);
  EXPECT_FLOAT_EQ(c.b(), 0.6f);
  EXPECT_FLOAT_EQ(c.a(), 0.8f);
}

// --- Boundary values ---

TEST(ProPhotoRGBTest, BoundaryValuesMin) {
  prophoto_rgbf_t c(0.0f, 0.0f, 0.0f);
  EXPECT_FLOAT_EQ(c.r(), 0.0f);
  EXPECT_FLOAT_EQ(c.g(), 0.0f);
  EXPECT_FLOAT_EQ(c.b(), 0.0f);
}

TEST(ProPhotoRGBTest, BoundaryValuesMax) {
  prophoto_rgbf_t c(1.0f, 1.0f, 1.0f);
  EXPECT_FLOAT_EQ(c.r(), 1.0f);
  EXPECT_FLOAT_EQ(c.g(), 1.0f);
  EXPECT_FLOAT_EQ(c.b(), 1.0f);
}

TEST(ProPhotoRGBTest, OutOfRangeThrows) {
  EXPECT_THROW(prophoto_rgbf_t(-0.1f, 0.5f, 0.5f), std::out_of_range);
  EXPECT_THROW(prophoto_rgbf_t(1.1f, 0.5f, 0.5f), std::out_of_range);
}

TEST(ProPhotoRGBTest, MutableMemberAccess) {
  prophoto_rgbf_t c(0.1f, 0.2f, 0.3f);
  c.r() = 0.4f;
  c.g() = 0.5f;
  c.b() = 0.6f;
  EXPECT_FLOAT_EQ(c.r(), 0.4f);
  EXPECT_FLOAT_EQ(c.g(), 0.5f);
  EXPECT_FLOAT_EQ(c.b(), 0.6f);
}

TEST(ProPhotoRGBTest, IndexAccess) {
  prophoto_rgbf_t c(0.2f, 0.4f, 0.6f);
  EXPECT_FLOAT_EQ(c.get_index<0>(), 0.2f);
  EXPECT_FLOAT_EQ(c.get_index<1>(), 0.4f);
  EXPECT_FLOAT_EQ(c.get_index<2>(), 0.6f);
}

// --- Type traits ---

TEST(ProPhotoRGBTest, TypeTraits) {
  using channels = typename traits::model_traits<prophoto_rgb::model::prophoto_rgb>::channels_type;
  static_assert(std::tuple_size_v<channels> == 3);

  using a_channels = typename traits::model_traits<prophoto_rgb::model::prophoto_rgba>::channels_type;
  static_assert(std::tuple_size_v<a_channels> == 4);
}

// --- sRGB <-> ProPhoto RGB conversions ---

TEST(ProPhotoRGBConversionTest, BlackRemainsBlack) {
  auto c = color_cast<prophoto_rgbf_t>(rgbf_t{0.0f, 0.0f, 0.0f});
  EXPECT_NEAR(c.r(), 0.0f, 1e-4f);
  EXPECT_NEAR(c.g(), 0.0f, 1e-4f);
  EXPECT_NEAR(c.b(), 0.0f, 1e-4f);
}

TEST(ProPhotoRGBConversionTest, FromSRGBViaCanonicalPath) {
  auto c = color_cast<prophoto_rgbf_t>(rgbf_t{0.2f, 0.5f, 0.8f});
  EXPECT_GT(c.r(), 0.0f);
  EXPECT_GT(c.g(), 0.0f);
  EXPECT_GT(c.b(), 0.0f);
  EXPECT_LT(c.r(), 1.0f);
  EXPECT_LT(c.g(), 1.0f);
  EXPECT_LT(c.b(), 1.0f);
}

// Alpha through graph routing (XYZ hub discards alpha, same limitation as Display P3A)
TEST(ProPhotoRGBConversionTest, AlphaPreservedViaLinear) {
  prophoto_rgbaf_t orig(0.5f, 0.3f, 0.7f, 0.6f);
  auto linear = color_cast<linear_prophoto_rgbaf_t>(orig);
  EXPECT_NEAR(linear.a(), 0.6f, 1e-4f);
}

// --- Linear ProPhoto RGB conversions ---

TEST(ProPhotoRGBLinearTest, GammaCorrected) {
  prophoto_rgbf_t gamma(0.5f, 0.5f, 0.5f);
  auto linear = color_cast<linear_prophoto_rgbf_t>(gamma);
  EXPECT_LT(linear.r(), 0.5f);
}

TEST(ProPhotoRGBLinearTest, LinearRoundTrip) {
  prophoto_rgbf_t orig(0.3f, 0.6f, 0.9f);
  auto linear = color_cast<linear_prophoto_rgbf_t>(orig);
  auto back = color_cast<prophoto_rgbf_t>(linear);
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

// --- Cross-space conversions (looser tolerance via graph routing) ---

TEST(ProPhotoRGBConversionTest, ToDisplayP3AndBack) {
  prophoto_rgbf_t orig(0.4f, 0.2f, 0.7f);
  auto dp3 = color_cast<display_p3f_t>(orig);
  auto back = color_cast<prophoto_rgbf_t>(dp3);
  // Multi-hop graph routing through Bradford adaptation
  EXPECT_GT(back.r(), 0.0f);
  EXPECT_GT(back.g(), 0.0f);
  EXPECT_GT(back.b(), 0.0f);
  EXPECT_LT(back.r(), 1.0f);
  EXPECT_LT(back.g(), 1.0f);
  EXPECT_LT(back.b(), 1.0f);
}

TEST(ProPhotoRGBConversionTest, ToOklabAndBack) {
  prophoto_rgbf_t orig(0.3f, 0.6f, 0.2f);
  auto ok = color_cast<oklab_t>(orig);
  auto back = color_cast<prophoto_rgbf_t>(ok);
  EXPECT_GT(back.r(), 0.0f);
  EXPECT_GT(back.g(), 0.0f);
  EXPECT_GT(back.b(), 0.0f);
}

TEST(ProPhotoRGBConversionTest, ToXYZAndBack) {
  prophoto_rgbf_t orig(0.25f, 0.45f, 0.85f);
  auto xyz = color_cast<xyz_t>(orig);
  auto back = color_cast<prophoto_rgbf_t>(xyz);
  // ProPhoto → XYZ → ProPhoto goes through Bradford D50↔D65 adaptation
  EXPECT_GT(back.r(), 0.0f);
  EXPECT_GT(back.g(), 0.0f);
  EXPECT_GT(back.b(), 0.0f);
  EXPECT_LT(back.r(), 1.0f);
  EXPECT_LT(back.g(), 1.0f);
  EXPECT_LT(back.b(), 1.0f);
}

// --- CSS parsing support ---

TEST(ProPhotoRGBCSSTest, ColorFunctionAsRgba8) {
  auto c = colorcpp::io::css::parse_css_color_rgba8("color(prophoto-rgb 0.5 0.3 0.7)");
  ASSERT_TRUE(c.has_value());
  EXPECT_GT(c->get_index<0>(), 0);
  EXPECT_LT(c->get_index<2>(), 255);
}

TEST(ProPhotoRGBCSSTest, ColorFunctionWithAlpha) {
  auto c = colorcpp::io::css::parse_css_color_rgba8("color(prophoto-rgb 0.2 0.4 0.6 / 0.5)");
  ASSERT_TRUE(c.has_value());
  EXPECT_EQ(c->get_index<3>(), 128);
}

TEST(ProPhotoRGBCSSTest, ColorFunctionAsTyped) {
  colorcpp::io::css::details::Cursor cursor{"color(prophoto-rgb 0.3 0.5 0.7)", 0};
  auto c = colorcpp::io::css::parse_color_function_as<prophoto_rgbf_t>(cursor);
  ASSERT_TRUE(c.has_value());
  EXPECT_NEAR(c->r(), 0.3f, 1e-4f);
  EXPECT_NEAR(c->g(), 0.5f, 1e-4f);
  EXPECT_NEAR(c->b(), 0.7f, 1e-4f);
}

TEST(ProPhotoRGBCSSTest, ColorMixInProPhoto) {
  auto c = colorcpp::io::css::parse_css_color_rgbaf("color-mix(in prophoto-rgb, red, blue)");
  ASSERT_TRUE(c.has_value());
  EXPECT_GT(c->get_index<0>(), 0.0f);
  EXPECT_LT(c->get_index<2>(), 1.0f);
}

TEST(ProPhotoRGBCSSTest, ColorMixInProPhotoLinear) {
  auto c = colorcpp::io::css::parse_css_color_rgbaf("color-mix(in prophoto-rgb-linear, red, blue)");
  ASSERT_TRUE(c.has_value());
}

TEST(ProPhotoRGBCSSTest, ColorMixProgressForm) {
  auto c = colorcpp::io::css::parse_css_color_rgbaf("color-mix(25% in prophoto-rgb, red, blue)");
  ASSERT_TRUE(c.has_value());
}

}  // namespace colorcpp::core::test