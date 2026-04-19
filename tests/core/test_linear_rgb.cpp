/**
 * @file test_linear_rgb.cpp
 * @brief Tests for Linear RGB color model and conversion.
 */

#include <gtest/gtest.h>

#include <colorcpp/core/linear_rgb.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::core::test {

using namespace colorcpp::operations::conversion;

// Construction

TEST(LinearRGBTest, DefaultConstruction) {
  linear_rgbf_t rgb;
  EXPECT_FLOAT_EQ(rgb.r(), 0.0f);
  EXPECT_FLOAT_EQ(rgb.g(), 0.0f);
  EXPECT_FLOAT_EQ(rgb.b(), 0.0f);
}

TEST(LinearRGBTest, ParameterizedConstruction) {
  linear_rgbf_t rgb(0.2f, 0.4f, 0.6f);
  EXPECT_FLOAT_EQ(rgb.r(), 0.2f);
  EXPECT_FLOAT_EQ(rgb.g(), 0.4f);
  EXPECT_FLOAT_EQ(rgb.b(), 0.6f);
}

TEST(LinearRGBATest, ParameterizedWithAlpha) {
  linear_rgbaf_t rgba(0.2f, 0.4f, 0.6f, 0.8f);
  EXPECT_FLOAT_EQ(rgba.r(), 0.2f);
  EXPECT_FLOAT_EQ(rgba.g(), 0.4f);
  EXPECT_FLOAT_EQ(rgba.b(), 0.6f);
  EXPECT_FLOAT_EQ(rgba.a(), 0.8f);
}

// Boundary values

TEST(LinearRGBTest, BoundaryValuesMin) {
  linear_rgbf_t rgb(0.0f, 0.0f, 0.0f);
  EXPECT_FLOAT_EQ(rgb.r(), 0.0f);
  EXPECT_FLOAT_EQ(rgb.g(), 0.0f);
  EXPECT_FLOAT_EQ(rgb.b(), 0.0f);
}

TEST(LinearRGBTest, BoundaryValuesMax) {
  linear_rgbf_t rgb(1.0f, 1.0f, 1.0f);
  EXPECT_FLOAT_EQ(rgb.r(), 1.0f);
  EXPECT_FLOAT_EQ(rgb.g(), 1.0f);
  EXPECT_FLOAT_EQ(rgb.b(), 1.0f);
}

TEST(LinearRGBTest, OutOfRangeThrows) {
  EXPECT_THROW(linear_rgbf_t(-0.1f, 0.5f, 0.5f), std::out_of_range);
  EXPECT_THROW(linear_rgbf_t(1.1f, 0.5f, 0.5f), std::out_of_range);
  EXPECT_THROW(linear_rgbf_t(0.5f, -0.1f, 0.5f), std::out_of_range);
  EXPECT_THROW(linear_rgbf_t(0.5f, 1.1f, 0.5f), std::out_of_range);
  EXPECT_THROW(linear_rgbf_t(0.5f, 0.5f, -0.1f), std::out_of_range);
  EXPECT_THROW(linear_rgbf_t(0.5f, 0.5f, 1.1f), std::out_of_range);
}

// Member and indexed access

TEST(LinearRGBTest, MutableMemberAccess) {
  linear_rgbf_t rgb(0.1f, 0.2f, 0.3f);
  rgb.r() = 0.4f;
  rgb.g() = 0.5f;
  rgb.b() = 0.6f;

  EXPECT_FLOAT_EQ(rgb.r(), 0.4f);
  EXPECT_FLOAT_EQ(rgb.g(), 0.5f);
  EXPECT_FLOAT_EQ(rgb.b(), 0.6f);
}

TEST(LinearRGBTest, IndexAccess) {
  linear_rgbf_t rgb(0.2f, 0.4f, 0.6f);
  EXPECT_FLOAT_EQ(rgb.get_index<0>(), 0.2f);
  EXPECT_FLOAT_EQ(rgb.get_index<1>(), 0.4f);
  EXPECT_FLOAT_EQ(rgb.get_index<2>(), 0.6f);
}

TEST(LinearRGBTest, ConstCorrectness) {
  const linear_rgbf_t rgb(0.3f, 0.5f, 0.7f);
  EXPECT_FLOAT_EQ(rgb.r(), 0.3f);
  EXPECT_FLOAT_EQ(rgb.g(), 0.5f);
  EXPECT_FLOAT_EQ(rgb.b(), 0.7f);
}

// Copy and move semantics

TEST(LinearRGBTest, CopyConstructors) {
  linear_rgbf_t original(0.2f, 0.4f, 0.6f);
  linear_rgbf_t copy(original);

  EXPECT_FLOAT_EQ(copy.r(), 0.2f);
  EXPECT_FLOAT_EQ(copy.g(), 0.4f);
  EXPECT_FLOAT_EQ(copy.b(), 0.6f);
}

TEST(LinearRGBTest, CopyAssignment) {
  linear_rgbf_t original(0.2f, 0.4f, 0.6f);
  linear_rgbf_t copy(0.0f, 0.0f, 0.0f);
  copy = original;

  EXPECT_FLOAT_EQ(copy.r(), 0.2f);
  EXPECT_FLOAT_EQ(copy.g(), 0.4f);
  EXPECT_FLOAT_EQ(copy.b(), 0.6f);
}

TEST(LinearRGBTest, MoveSemantics) {
  linear_rgbf_t original(0.2f, 0.4f, 0.6f);
  linear_rgbf_t moved(std::move(original));

  EXPECT_FLOAT_EQ(moved.r(), 0.2f);
  EXPECT_FLOAT_EQ(moved.g(), 0.4f);
  EXPECT_FLOAT_EQ(moved.b(), 0.6f);
}

TEST(LinearRGBTest, MoveAssignment) {
  linear_rgbf_t original(0.2f, 0.4f, 0.6f);
  linear_rgbf_t moved(0.0f, 0.0f, 0.0f);
  moved = std::move(original);

  EXPECT_FLOAT_EQ(moved.r(), 0.2f);
  EXPECT_FLOAT_EQ(moved.g(), 0.4f);
  EXPECT_FLOAT_EQ(moved.b(), 0.6f);
}

TEST(LinearRGBTest, EqualityComparison) {
  linear_rgbf_t color1(0.2f, 0.4f, 0.6f);
  linear_rgbf_t color2(0.2f, 0.4f, 0.6f);
  linear_rgbf_t color3(0.3f, 0.4f, 0.6f);

  EXPECT_TRUE(color1 == color2);
  EXPECT_FALSE(color1 == color3);
  EXPECT_TRUE(color1 != color3);
}

TEST(LinearRGBTest, ConstexprBoundaryValues) {
  constexpr linear_rgbf_t min_color(0.0f, 0.0f, 0.0f);
  static_assert(min_color.r() == 0.0f);
  static_assert(min_color.g() == 0.0f);
  static_assert(min_color.b() == 0.0f);

  constexpr linear_rgbf_t max_color(1.0f, 1.0f, 1.0f);
  static_assert(max_color.r() == 1.0f);
  static_assert(max_color.g() == 1.0f);
  static_assert(max_color.b() == 1.0f);
}

// Linear RGBA tests

TEST(LinearRGBATest, CopyConstructors) {
  linear_rgbaf_t original(0.2f, 0.4f, 0.6f, 0.8f);
  linear_rgbaf_t copy(original);

  EXPECT_FLOAT_EQ(copy.r(), 0.2f);
  EXPECT_FLOAT_EQ(copy.g(), 0.4f);
  EXPECT_FLOAT_EQ(copy.b(), 0.6f);
  EXPECT_FLOAT_EQ(copy.a(), 0.8f);
}

TEST(LinearRGBATest, CopyAssignment) {
  linear_rgbaf_t original(0.2f, 0.4f, 0.6f, 0.8f);
  linear_rgbaf_t copy(0.0f, 0.0f, 0.0f, 0.0f);
  copy = original;

  EXPECT_FLOAT_EQ(copy.r(), 0.2f);
  EXPECT_FLOAT_EQ(copy.g(), 0.4f);
  EXPECT_FLOAT_EQ(copy.b(), 0.6f);
  EXPECT_FLOAT_EQ(copy.a(), 0.8f);
}

TEST(LinearRGBATest, MoveSemantics) {
  linear_rgbaf_t original(0.2f, 0.4f, 0.6f, 0.8f);
  linear_rgbaf_t moved(std::move(original));

  EXPECT_FLOAT_EQ(moved.r(), 0.2f);
  EXPECT_FLOAT_EQ(moved.g(), 0.4f);
  EXPECT_FLOAT_EQ(moved.b(), 0.6f);
  EXPECT_FLOAT_EQ(moved.a(), 0.8f);
}

TEST(LinearRGBATest, MoveAssignment) {
  linear_rgbaf_t original(0.2f, 0.4f, 0.6f, 0.8f);
  linear_rgbaf_t moved(0.0f, 0.0f, 0.0f, 0.0f);
  moved = std::move(original);

  EXPECT_FLOAT_EQ(moved.r(), 0.2f);
  EXPECT_FLOAT_EQ(moved.g(), 0.4f);
  EXPECT_FLOAT_EQ(moved.b(), 0.6f);
  EXPECT_FLOAT_EQ(moved.a(), 0.8f);
}

TEST(LinearRGBATest, EqualityComparison) {
  linear_rgbaf_t color1(0.2f, 0.4f, 0.6f, 0.8f);
  linear_rgbaf_t color2(0.2f, 0.4f, 0.6f, 0.8f);
  linear_rgbaf_t color3(0.2f, 0.4f, 0.6f, 0.5f);

  EXPECT_TRUE(color1 == color2);
  EXPECT_FALSE(color1 == color3);
  EXPECT_TRUE(color1 != color3);
}

// Type traits

TEST(LinearRGBTest, TypeTraits) {
  using linear_rgb_channels = typename traits::model_traits<linear_rgb::model::linear_rgbf>::channels_type;
  static_assert(std::tuple_size_v<linear_rgb_channels> == 3);

  using r_ch = std::tuple_element_t<0, linear_rgb_channels>;
  static_assert(std::is_same_v<r_ch::value_type, float>);
  static_assert(r_ch::min == 0.0f);
  static_assert(r_ch::max == 1.0f);

  using linear_rgba_channels = typename traits::model_traits<linear_rgb::model::linear_rgbaf>::channels_type;
  static_assert(std::tuple_size_v<linear_rgba_channels> == 4);

  static_assert(!traits::has_channel_tag_v<linear_rgb::model::linear_rgbf, linear_rgb::channel::a_tag>);
  static_assert(traits::has_channel_tag_v<linear_rgb::model::linear_rgbaf, linear_rgb::channel::a_tag>);
}

// Gamma conversion tests

TEST(LinearRGBConversionTest, GammaCurveDirection) {
  // Gamma encoded values are higher than linear values for values < 1.0
  float linear = 0.5f;
  auto gamma = color_cast<rgbf_t>(linear_rgbf_t{linear, linear, linear});

  EXPECT_GT(gamma.r(), linear);
  EXPECT_GT(gamma.g(), linear);
  EXPECT_GT(gamma.b(), linear);
}

TEST(LinearRGBConversionTest, RoundTripConversion) {
  linear_rgbf_t orig(0.3f, 0.5f, 0.7f);
  auto gamma = color_cast<rgbf_t>(orig);
  auto back = color_cast<linear_rgbf_t>(gamma);

  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

TEST(LinearRGBConversionTest, AlphaPreserved) {
  linear_rgbaf_t original(0.3f, 0.5f, 0.7f, 0.6f);
  auto gamma = color_cast<rgbaf_t>(original);
  EXPECT_NEAR(gamma.a(), 0.6f, 1e-4f);

  auto back = color_cast<linear_rgbaf_t>(gamma);
  EXPECT_NEAR(back.a(), 0.6f, 1e-4f);
}

TEST(LinearRGBConversionTest, BlackConversion) {
  auto linear = color_cast<linear_rgbf_t>(rgbf_t{0.0f, 0.0f, 0.0f});
  EXPECT_NEAR(linear.r(), 0.0f, 1e-4f);
  EXPECT_NEAR(linear.g(), 0.0f, 1e-4f);
  EXPECT_NEAR(linear.b(), 0.0f, 1e-4f);
}

TEST(LinearRGBConversionTest, WhiteConversion) {
  auto linear = color_cast<linear_rgbf_t>(rgbf_t{1.0f, 1.0f, 1.0f});
  EXPECT_NEAR(linear.r(), 1.0f, 1e-4f);
  EXPECT_NEAR(linear.g(), 1.0f, 1e-4f);
  EXPECT_NEAR(linear.b(), 1.0f, 1e-4f);
}

}  // namespace colorcpp::core::test
