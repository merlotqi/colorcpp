/**
 * @file test_adobe_rgb.cpp
 * @brief Tests for Adobe RGB (A98-RGB) color model and conversion.
 *
 * Adobe RGB uses pure Gamma 2.2 transfer function and D65 white point.
 */

#include <gtest/gtest.h>

#include <colorcpp/core/adobe_rgb.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/io/css.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::core::test {

using namespace colorcpp::operations::conversion;

// --- Construction ---

TEST(AdobeRGBTest, DefaultConstruction) {
  adobe_rgbf_t c;
  EXPECT_FLOAT_EQ(c.r(), 0.0f);
  EXPECT_FLOAT_EQ(c.g(), 0.0f);
  EXPECT_FLOAT_EQ(c.b(), 0.0f);
}

TEST(AdobeRGBTest, ParameterizedConstruction) {
  adobe_rgbf_t c(0.2f, 0.4f, 0.6f);
  EXPECT_FLOAT_EQ(c.r(), 0.2f);
  EXPECT_FLOAT_EQ(c.g(), 0.4f);
  EXPECT_FLOAT_EQ(c.b(), 0.6f);
}

TEST(AdobeRGBATest, ParameterizedWithAlpha) {
  adobe_rgbaf_t c(0.2f, 0.4f, 0.6f, 0.8f);
  EXPECT_FLOAT_EQ(c.r(), 0.2f);
  EXPECT_FLOAT_EQ(c.g(), 0.4f);
  EXPECT_FLOAT_EQ(c.b(), 0.6f);
  EXPECT_FLOAT_EQ(c.a(), 0.8f);
}

// --- Boundary values ---

TEST(AdobeRGBTest, BoundaryValuesMin) {
  adobe_rgbf_t c(0.0f, 0.0f, 0.0f);
  EXPECT_FLOAT_EQ(c.r(), 0.0f);
  EXPECT_FLOAT_EQ(c.g(), 0.0f);
  EXPECT_FLOAT_EQ(c.b(), 0.0f);
}

TEST(AdobeRGBTest, BoundaryValuesMax) {
  adobe_rgbf_t c(1.0f, 1.0f, 1.0f);
  EXPECT_FLOAT_EQ(c.r(), 1.0f);
  EXPECT_FLOAT_EQ(c.g(), 1.0f);
  EXPECT_FLOAT_EQ(c.b(), 1.0f);
}

TEST(AdobeRGBTest, OutOfRangeThrows) {
  EXPECT_THROW(adobe_rgbf_t(-0.1f, 0.5f, 0.5f), std::out_of_range);
  EXPECT_THROW(adobe_rgbf_t(1.1f, 0.5f, 0.5f), std::out_of_range);
  EXPECT_THROW(adobe_rgbf_t(0.5f, -0.1f, 0.5f), std::out_of_range);
  EXPECT_THROW(adobe_rgbf_t(0.5f, 1.1f, 0.5f), std::out_of_range);
  EXPECT_THROW(adobe_rgbf_t(0.5f, 0.5f, -0.1f), std::out_of_range);
  EXPECT_THROW(adobe_rgbf_t(0.5f, 0.5f, 1.1f), std::out_of_range);
}

// --- Member and indexed access ---

TEST(AdobeRGBTest, MutableMemberAccess) {
  adobe_rgbf_t c(0.1f, 0.2f, 0.3f);
  c.r() = 0.4f;
  c.g() = 0.5f;
  c.b() = 0.6f;
  EXPECT_FLOAT_EQ(c.r(), 0.4f);
  EXPECT_FLOAT_EQ(c.g(), 0.5f);
  EXPECT_FLOAT_EQ(c.b(), 0.6f);
}

TEST(AdobeRGBTest, IndexAccess) {
  adobe_rgbf_t c(0.2f, 0.4f, 0.6f);
  EXPECT_FLOAT_EQ(c.get_index<0>(), 0.2f);
  EXPECT_FLOAT_EQ(c.get_index<1>(), 0.4f);
  EXPECT_FLOAT_EQ(c.get_index<2>(), 0.6f);
}

TEST(AdobeRGBTest, ConstCorrectness) {
  const adobe_rgbf_t c(0.3f, 0.5f, 0.7f);
  EXPECT_FLOAT_EQ(c.r(), 0.3f);
  EXPECT_FLOAT_EQ(c.g(), 0.5f);
  EXPECT_FLOAT_EQ(c.b(), 0.7f);
}

// --- Copy and move semantics ---

TEST(AdobeRGBTest, CopyConstructors) {
  adobe_rgbf_t original(0.2f, 0.4f, 0.6f);
  adobe_rgbf_t copy(original);
  EXPECT_FLOAT_EQ(copy.r(), 0.2f);
  EXPECT_FLOAT_EQ(copy.g(), 0.4f);
  EXPECT_FLOAT_EQ(copy.b(), 0.6f);
}

TEST(AdobeRGBTest, CopyAssignment) {
  adobe_rgbf_t original(0.2f, 0.4f, 0.6f);
  adobe_rgbf_t copy(0.0f, 0.0f, 0.0f);
  copy = original;
  EXPECT_FLOAT_EQ(copy.r(), 0.2f);
  EXPECT_FLOAT_EQ(copy.g(), 0.4f);
  EXPECT_FLOAT_EQ(copy.b(), 0.6f);
}

TEST(AdobeRGBTest, EqualityComparison) {
  adobe_rgbf_t c1(0.2f, 0.4f, 0.6f);
  adobe_rgbf_t c2(0.2f, 0.4f, 0.6f);
  adobe_rgbf_t c3(0.3f, 0.4f, 0.6f);
  EXPECT_TRUE(c1 == c2);
  EXPECT_FALSE(c1 == c3);
  EXPECT_TRUE(c1 != c3);
}

// --- Type traits ---

TEST(AdobeRGBTest, TypeTraits) {
  using channels = typename traits::model_traits<adobe_rgb::model::adobe_rgb>::channels_type;
  static_assert(std::tuple_size_v<channels> == 3);

  using a_channels = typename traits::model_traits<adobe_rgb::model::adobe_rgba>::channels_type;
  static_assert(std::tuple_size_v<a_channels> == 4);

  static_assert(!traits::has_channel_tag_v<adobe_rgb::model::adobe_rgb, adobe_rgb::channel::a_tag>);
  static_assert(traits::has_channel_tag_v<adobe_rgb::model::adobe_rgba, adobe_rgb::channel::a_tag>);
}

// --- sRGB <-> Adobe RGB conversions ---

TEST(AdobeRGBConversionTest, BlackRemainsBlack) {
  auto a = color_cast<adobe_rgbf_t>(rgbf_t{0.0f, 0.0f, 0.0f});
  EXPECT_NEAR(a.r(), 0.0f, 1e-4f);
  EXPECT_NEAR(a.g(), 0.0f, 1e-4f);
  EXPECT_NEAR(a.b(), 0.0f, 1e-4f);
}

TEST(AdobeRGBConversionTest, FromSRGBViaCanonicalPath) {
  auto a = color_cast<adobe_rgbf_t>(rgbf_t{0.2f, 0.5f, 0.8f});
  // Verify conversion produces expected non-zero results
  EXPECT_GT(a.r(), 0.0f);
  EXPECT_GT(a.g(), 0.0f);
  EXPECT_GT(a.b(), 0.0f);
  EXPECT_LT(a.r(), 1.0f);
  EXPECT_LT(a.g(), 1.0f);
  EXPECT_LT(a.b(), 1.0f);
}

// Alpha through graph routing uses direct edge via linear_adobe_rgbaf_t -> rgbaf_t
// (XYZ hub discards alpha, same limitation as Display P3A)
TEST(AdobeRGBConversionTest, AlphaPreservedViaLinear) {
  adobe_rgbaf_t orig(0.5f, 0.3f, 0.7f, 0.6f);
  auto linear = color_cast<linear_adobe_rgbaf_t>(orig);
  EXPECT_NEAR(linear.a(), 0.6f, 1e-4f);
}

// --- Linear Adobe RGB conversions ---

TEST(AdobeRGBLinearTest, GammaCorrected) {
  adobe_rgbf_t gamma(0.5f, 0.5f, 0.5f);
  auto linear = color_cast<linear_adobe_rgbf_t>(gamma);
  EXPECT_LT(linear.r(), 0.5f);
}

TEST(AdobeRGBLinearTest, LinearRoundTrip) {
  adobe_rgbf_t orig(0.3f, 0.6f, 0.9f);
  auto linear = color_cast<linear_adobe_rgbf_t>(orig);
  auto back = color_cast<adobe_rgbf_t>(linear);
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

// --- Cross-space conversions ---

TEST(AdobeRGBConversionTest, ToDisplayP3AndBack) {
  adobe_rgbf_t orig(0.4f, 0.2f, 0.7f);
  auto dp3 = color_cast<display_p3f_t>(orig);
  auto back = color_cast<adobe_rgbf_t>(dp3);
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

TEST(AdobeRGBConversionTest, ToOklabAndBack) {
  adobe_rgbf_t orig(0.3f, 0.6f, 0.2f);
  auto ok = color_cast<oklab_t>(orig);
  auto back = color_cast<adobe_rgbf_t>(ok);
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

TEST(AdobeRGBConversionTest, ToXYZAndBack) {
  adobe_rgbf_t orig(0.25f, 0.45f, 0.85f);
  auto xyz = color_cast<xyz_t>(orig);
  auto back = color_cast<adobe_rgbf_t>(xyz);
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

// --- CSS parsing support ---

TEST(AdobeRGBCSSTest, ColorFunctionAsRgba8) {
  auto c = colorcpp::io::css::parse_css_color_rgba8("color(a98-rgb 0.5 0.3 0.7)");
  ASSERT_TRUE(c.has_value());
  EXPECT_GT(c->get_index<0>(), 0);
  EXPECT_LT(c->get_index<2>(), 255);
}

TEST(AdobeRGBCSSTest, ColorFunctionWithAlpha) {
  auto c = colorcpp::io::css::parse_css_color_rgba8("color(a98-rgb 0.2 0.4 0.6 / 0.5)");
  ASSERT_TRUE(c.has_value());
  EXPECT_EQ(c->get_index<3>(), 128);
}

TEST(AdobeRGBCSSTest, ColorFunctionAsTyped) {
  colorcpp::io::css::details::Cursor cursor{"color(a98-rgb 0.3 0.5 0.7)", 0};
  auto c = colorcpp::io::css::parse_color_function_as<adobe_rgbf_t>(cursor);
  ASSERT_TRUE(c.has_value());
  EXPECT_NEAR(c->r(), 0.3f, 1e-4f);
  EXPECT_NEAR(c->g(), 0.5f, 1e-4f);
  EXPECT_NEAR(c->b(), 0.7f, 1e-4f);
}

TEST(AdobeRGBCSSTest, ColorMixInA98RGB) {
  auto c = colorcpp::io::css::parse_css_color_rgbaf("color-mix(in a98-rgb, red, blue)");
  ASSERT_TRUE(c.has_value());
  EXPECT_GT(c->get_index<0>(), 0.0f);
  EXPECT_LT(c->get_index<2>(), 1.0f);
}

TEST(AdobeRGBCSSTest, ColorMixInA98RGBLinear) {
  auto c = colorcpp::io::css::parse_css_color_rgbaf("color-mix(in a98-rgb-linear, red, blue)");
  ASSERT_TRUE(c.has_value());
}

TEST(AdobeRGBCSSTest, ColorMixProgressForm) {
  auto c = colorcpp::io::css::parse_css_color_rgbaf("color-mix(25% in a98-rgb, red, blue)");
  ASSERT_TRUE(c.has_value());
}

}  // namespace colorcpp::core::test