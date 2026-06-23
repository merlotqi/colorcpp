/**
 * @file test_rec2020.cpp
 * @brief Tests for Rec.2020 (BT.2020) color model and conversion.
 *
 * Rec.2020 uses BT.709-style piecewise transfer function and D65 white point.
 */

#include <gtest/gtest.h>

#include <colorcpp/core/rec2020.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/io/css.hpp>
#include <colorcpp/io/css/format.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::core::test {

using namespace colorcpp::operations::conversion;
using colorcpp::io::css::parse_css_color;
using colorcpp::io::css::to_css_color_string;

// --- Construction ---

TEST(Rec2020Test, DefaultConstruction) {
  rec2020_rgbf_t c;
  EXPECT_FLOAT_EQ(c.r(), 0.0f);
  EXPECT_FLOAT_EQ(c.g(), 0.0f);
  EXPECT_FLOAT_EQ(c.b(), 0.0f);
}

TEST(Rec2020Test, ParameterizedConstruction) {
  rec2020_rgbf_t c(0.2f, 0.4f, 0.6f);
  EXPECT_FLOAT_EQ(c.r(), 0.2f);
  EXPECT_FLOAT_EQ(c.g(), 0.4f);
  EXPECT_FLOAT_EQ(c.b(), 0.6f);
}

TEST(Rec2020ATest, ParameterizedWithAlpha) {
  rec2020_rgbaf_t c(0.2f, 0.4f, 0.6f, 0.8f);
  EXPECT_FLOAT_EQ(c.r(), 0.2f);
  EXPECT_FLOAT_EQ(c.g(), 0.4f);
  EXPECT_FLOAT_EQ(c.b(), 0.6f);
  EXPECT_FLOAT_EQ(c.a(), 0.8f);
}

// --- Boundary values ---

TEST(Rec2020Test, BoundaryValuesMin) {
  rec2020_rgbf_t c(0.0f, 0.0f, 0.0f);
  EXPECT_FLOAT_EQ(c.r(), 0.0f);
  EXPECT_FLOAT_EQ(c.g(), 0.0f);
  EXPECT_FLOAT_EQ(c.b(), 0.0f);
}

TEST(Rec2020Test, BoundaryValuesMax) {
  rec2020_rgbf_t c(1.0f, 1.0f, 1.0f);
  EXPECT_FLOAT_EQ(c.r(), 1.0f);
  EXPECT_FLOAT_EQ(c.g(), 1.0f);
  EXPECT_FLOAT_EQ(c.b(), 1.0f);
}

TEST(Rec2020Test, OutOfRangeThrows) {
  EXPECT_THROW(rec2020_rgbf_t(-0.1f, 0.5f, 0.5f), std::out_of_range);
  EXPECT_THROW(rec2020_rgbf_t(1.1f, 0.5f, 0.5f), std::out_of_range);
  EXPECT_THROW(rec2020_rgbf_t(0.5f, -0.1f, 0.5f), std::out_of_range);
  EXPECT_THROW(rec2020_rgbf_t(0.5f, 1.1f, 0.5f), std::out_of_range);
  EXPECT_THROW(rec2020_rgbf_t(0.5f, 0.5f, -0.1f), std::out_of_range);
  EXPECT_THROW(rec2020_rgbf_t(0.5f, 0.5f, 1.1f), std::out_of_range);
}

// --- Member and indexed access ---

TEST(Rec2020Test, MutableMemberAccess) {
  rec2020_rgbf_t c(0.1f, 0.2f, 0.3f);
  c.r() = 0.4f;
  c.g() = 0.5f;
  c.b() = 0.6f;
  EXPECT_FLOAT_EQ(c.r(), 0.4f);
  EXPECT_FLOAT_EQ(c.g(), 0.5f);
  EXPECT_FLOAT_EQ(c.b(), 0.6f);
}

TEST(Rec2020Test, IndexAccess) {
  rec2020_rgbf_t c(0.2f, 0.4f, 0.6f);
  EXPECT_FLOAT_EQ(c.get_index<0>(), 0.2f);
  EXPECT_FLOAT_EQ(c.get_index<1>(), 0.4f);
  EXPECT_FLOAT_EQ(c.get_index<2>(), 0.6f);
}

TEST(Rec2020Test, ConstCorrectness) {
  const rec2020_rgbf_t c(0.3f, 0.5f, 0.7f);
  EXPECT_FLOAT_EQ(c.r(), 0.3f);
  EXPECT_FLOAT_EQ(c.g(), 0.5f);
  EXPECT_FLOAT_EQ(c.b(), 0.7f);
}

// --- Copy and move semantics ---

TEST(Rec2020Test, CopyConstruction) {
  rec2020_rgbf_t original(0.2f, 0.4f, 0.6f);
  rec2020_rgbf_t copy(original);
  EXPECT_FLOAT_EQ(copy.r(), 0.2f);
  EXPECT_FLOAT_EQ(copy.g(), 0.4f);
  EXPECT_FLOAT_EQ(copy.b(), 0.6f);
}

TEST(Rec2020Test, CopyAssignment) {
  rec2020_rgbf_t original(0.2f, 0.4f, 0.6f);
  rec2020_rgbf_t copy;
  copy = original;
  EXPECT_FLOAT_EQ(copy.r(), 0.2f);
  EXPECT_FLOAT_EQ(copy.g(), 0.4f);
  EXPECT_FLOAT_EQ(copy.b(), 0.6f);
}

TEST(Rec2020Test, Equality) {
  rec2020_rgbf_t a(0.2f, 0.4f, 0.6f);
  rec2020_rgbf_t b(0.2f, 0.4f, 0.6f);
  rec2020_rgbf_t c(0.3f, 0.4f, 0.6f);
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

// --- Linear variants ---

TEST(LinearRec2020Test, Construction) {
  linear_rec2020_rgbf_t c(0.1f, 0.5f, 0.9f);
  EXPECT_FLOAT_EQ(c.r(), 0.1f);
  EXPECT_FLOAT_EQ(c.g(), 0.5f);
  EXPECT_FLOAT_EQ(c.b(), 0.9f);
}

TEST(LinearRec2020ATest, ConstructionWithAlpha) {
  linear_rec2020_rgbaf_t c(0.1f, 0.5f, 0.9f, 0.3f);
  EXPECT_FLOAT_EQ(c.r(), 0.1f);
  EXPECT_FLOAT_EQ(c.g(), 0.5f);
  EXPECT_FLOAT_EQ(c.b(), 0.9f);
  EXPECT_FLOAT_EQ(c.a(), 0.3f);
}

// --- Conversion tests ---

TEST(Rec2020ConversionTest, BlackConversion) {
  auto r2020 = color_cast<rec2020_rgbf_t>(rgbf_t{0.0f, 0.0f, 0.0f});
  EXPECT_NEAR(r2020.r(), 0.0f, 1e-4f);
  EXPECT_NEAR(r2020.g(), 0.0f, 1e-4f);
  EXPECT_NEAR(r2020.b(), 0.0f, 1e-4f);
}

TEST(Rec2020ConversionTest, WhiteConversion) {
  auto r2020 = color_cast<rec2020_rgbf_t>(rgbf_t{1.0f, 1.0f, 1.0f});
  EXPECT_NEAR(r2020.r(), 1.0f, 1e-4f);
  EXPECT_NEAR(r2020.g(), 1.0f, 1e-4f);
  EXPECT_NEAR(r2020.b(), 1.0f, 1e-4f);
}

TEST(Rec2020ConversionTest, AlphaPreserved) {
  rec2020_rgbaf_t original(0.5f, 0.3f, 0.7f, 0.6f);
  auto rgb = color_cast<rgbaf_t>(original);
  EXPECT_NEAR(rgb.a(), 0.6f, 1e-4f);
}

TEST(Rec2020ConversionTest, FloatRoundTrip) {
  rec2020_rgbf_t orig(0.6f, 0.2f, 0.8f);
  auto back = color_cast<rec2020_rgbf_t>(color_cast<rgbf_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

TEST(Rec2020ConversionTest, AlphaRoundTrip) {
  rec2020_rgbaf_t orig(0.3f, 0.5f, 0.7f, 0.4f);
  auto back = color_cast<rec2020_rgbaf_t>(color_cast<rgbaf_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
  EXPECT_NEAR(back.a(), orig.a(), 1e-4f);
}

// Linear conversion
TEST(Rec2020LinearTest, GammaDirection) {
  rec2020_rgbf_t gamma(0.5f, 0.5f, 0.5f);
  auto linear = color_cast<linear_rec2020_rgbf_t>(gamma);
  // Non-linear gamma -- linear values are lower for mid-tones
  EXPECT_LT(linear.r(), 0.5f);
  EXPECT_LT(linear.g(), 0.5f);
  EXPECT_LT(linear.b(), 0.5f);
}

TEST(Rec2020LinearTest, LinearRoundTrip) {
  rec2020_rgbf_t orig(0.3f, 0.6f, 0.9f);
  auto linear = color_cast<linear_rec2020_rgbf_t>(orig);
  auto back = color_cast<rec2020_rgbf_t>(linear);
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

// Cross-space conversions
TEST(Rec2020ConversionTest, DisplayP3CrossConvert) {
  rec2020_rgbf_t orig(0.5f, 0.3f, 0.7f);
  auto dp3 = color_cast<display_p3f_t>(orig);
  auto back = color_cast<rec2020_rgbf_t>(dp3);
  EXPECT_NEAR(back.r(), orig.r(), 1e-2f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-2f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-2f);
}

TEST(Rec2020ConversionTest, AdobeRGBCrossConvert) {
  rec2020_rgbf_t orig(0.5f, 0.3f, 0.7f);
  auto argb = color_cast<adobe_rgbf_t>(orig);
  auto back = color_cast<rec2020_rgbf_t>(argb);
  EXPECT_NEAR(back.r(), orig.r(), 1e-2f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-2f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-2f);
}

TEST(Rec2020ConversionTest, XYZCrossConvert) {
  rec2020_rgbf_t orig(0.5f, 0.3f, 0.7f);
  auto xyz = color_cast<xyz_t>(orig);
  auto back = color_cast<rec2020_rgbf_t>(xyz);
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

TEST(Rec2020ConversionTest, OKLabCrossConvert) {
  rec2020_rgbf_t orig(0.5f, 0.3f, 0.7f);
  auto oklab = color_cast<oklab_t>(orig);
  auto back = color_cast<rec2020_rgbf_t>(oklab);
  EXPECT_NEAR(back.r(), orig.r(), 1e-2f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-2f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-2f);
}

// CSS parsing
TEST(Rec2020CSSTest, ParseColorFunction) {
  auto c = parse_css_color<rec2020_rgbaf_t>("color(rec2020 0.5 0.3 0.7 / 0.8)");
  ASSERT_TRUE(c.has_value());
  EXPECT_NEAR(c->r(), 0.5f, 1e-4f);
  EXPECT_NEAR(c->g(), 0.3f, 1e-4f);
  EXPECT_NEAR(c->b(), 0.7f, 1e-4f);
  EXPECT_NEAR(c->a(), 0.8f, 1e-4f);
}

TEST(Rec2020CSSTest, ParseColorFunctionRoundTrip) {
  auto c = parse_css_color<rec2020_rgbaf_t>("color(rec2020 0.64 0.33 0.21)");
  ASSERT_TRUE(c.has_value());
  auto back = parse_css_color<rec2020_rgbaf_t>(to_css_color_string(*c));
  ASSERT_TRUE(back.has_value());
  EXPECT_NEAR(back->r(), c->r(), 1e-4f);
  EXPECT_NEAR(back->g(), c->g(), 1e-4f);
  EXPECT_NEAR(back->b(), c->b(), 1e-4f);
}

}  // namespace colorcpp::core::test
