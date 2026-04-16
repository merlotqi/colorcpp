/**
 * @file test_display_p3.cpp
 * @brief Tests for Display P3 color model and conversion.
 */

#include <gtest/gtest.h>

#include <colorcpp/core/display_p3.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::core::test {

using namespace colorcpp::operations::conversion;

// Construction

TEST(DisplayP3Test, DefaultConstruction) {
  display_p3f_t p3;
  EXPECT_FLOAT_EQ(p3.r(), 0.0f);
  EXPECT_FLOAT_EQ(p3.g(), 0.0f);
  EXPECT_FLOAT_EQ(p3.b(), 0.0f);
}

TEST(DisplayP3Test, ParameterizedConstruction) {
  display_p3f_t p3(0.2f, 0.4f, 0.6f);
  EXPECT_FLOAT_EQ(p3.r(), 0.2f);
  EXPECT_FLOAT_EQ(p3.g(), 0.4f);
  EXPECT_FLOAT_EQ(p3.b(), 0.6f);
}

TEST(DisplayP3ATest, ParameterizedWithAlpha) {
  display_p3af_t p3a(0.2f, 0.4f, 0.6f, 0.8f);
  EXPECT_FLOAT_EQ(p3a.r(), 0.2f);
  EXPECT_FLOAT_EQ(p3a.g(), 0.4f);
  EXPECT_FLOAT_EQ(p3a.b(), 0.6f);
  EXPECT_FLOAT_EQ(p3a.a(), 0.8f);
}

// Boundary values

TEST(DisplayP3Test, BoundaryValuesMin) {
  display_p3f_t p3(0.0f, 0.0f, 0.0f);
  EXPECT_FLOAT_EQ(p3.r(), 0.0f);
  EXPECT_FLOAT_EQ(p3.g(), 0.0f);
  EXPECT_FLOAT_EQ(p3.b(), 0.0f);
}

TEST(DisplayP3Test, BoundaryValuesMax) {
  display_p3f_t p3(1.0f, 1.0f, 1.0f);
  EXPECT_FLOAT_EQ(p3.r(), 1.0f);
  EXPECT_FLOAT_EQ(p3.g(), 1.0f);
  EXPECT_FLOAT_EQ(p3.b(), 1.0f);
}

TEST(DisplayP3Test, OutOfRangeThrows) {
  EXPECT_THROW(display_p3f_t(-0.1f, 0.5f, 0.5f), std::out_of_range);
  EXPECT_THROW(display_p3f_t(1.1f, 0.5f, 0.5f), std::out_of_range);
  EXPECT_THROW(display_p3f_t(0.5f, -0.1f, 0.5f), std::out_of_range);
  EXPECT_THROW(display_p3f_t(0.5f, 1.1f, 0.5f), std::out_of_range);
  EXPECT_THROW(display_p3f_t(0.5f, 0.5f, -0.1f), std::out_of_range);
  EXPECT_THROW(display_p3f_t(0.5f, 0.5f, 1.1f), std::out_of_range);
}

// Member and indexed access

TEST(DisplayP3Test, MutableMemberAccess) {
  display_p3f_t p3(0.1f, 0.2f, 0.3f);
  p3.r() = 0.4f;
  p3.g() = 0.5f;
  p3.b() = 0.6f;

  EXPECT_FLOAT_EQ(p3.r(), 0.4f);
  EXPECT_FLOAT_EQ(p3.g(), 0.5f);
  EXPECT_FLOAT_EQ(p3.b(), 0.6f);
}

TEST(DisplayP3Test, IndexAccess) {
  display_p3f_t p3(0.2f, 0.4f, 0.6f);
  EXPECT_FLOAT_EQ(p3.get_index<0>(), 0.2f);
  EXPECT_FLOAT_EQ(p3.get_index<1>(), 0.4f);
  EXPECT_FLOAT_EQ(p3.get_index<2>(), 0.6f);
}

TEST(DisplayP3Test, ConstCorrectness) {
  const display_p3f_t p3(0.3f, 0.5f, 0.7f);
  EXPECT_FLOAT_EQ(p3.r(), 0.3f);
  EXPECT_FLOAT_EQ(p3.g(), 0.5f);
  EXPECT_FLOAT_EQ(p3.b(), 0.7f);
}

// Copy and move semantics

TEST(DisplayP3Test, CopyConstructors) {
  display_p3f_t original(0.2f, 0.4f, 0.6f);
  display_p3f_t copy(original);

  EXPECT_FLOAT_EQ(copy.r(), 0.2f);
  EXPECT_FLOAT_EQ(copy.g(), 0.4f);
  EXPECT_FLOAT_EQ(copy.b(), 0.6f);
}

TEST(DisplayP3Test, CopyAssignment) {
  display_p3f_t original(0.2f, 0.4f, 0.6f);
  display_p3f_t copy(0.0f, 0.0f, 0.0f);
  copy = original;

  EXPECT_FLOAT_EQ(copy.r(), 0.2f);
  EXPECT_FLOAT_EQ(copy.g(), 0.4f);
  EXPECT_FLOAT_EQ(copy.b(), 0.6f);
}

TEST(DisplayP3Test, MoveSemantics) {
  display_p3f_t original(0.2f, 0.4f, 0.6f);
  display_p3f_t moved(std::move(original));

  EXPECT_FLOAT_EQ(moved.r(), 0.2f);
  EXPECT_FLOAT_EQ(moved.g(), 0.4f);
  EXPECT_FLOAT_EQ(moved.b(), 0.6f);
}

TEST(DisplayP3Test, MoveAssignment) {
  display_p3f_t original(0.2f, 0.4f, 0.6f);
  display_p3f_t moved(0.0f, 0.0f, 0.0f);
  moved = std::move(original);

  EXPECT_FLOAT_EQ(moved.r(), 0.2f);
  EXPECT_FLOAT_EQ(moved.g(), 0.4f);
  EXPECT_FLOAT_EQ(moved.b(), 0.6f);
}

TEST(DisplayP3Test, EqualityComparison) {
  display_p3f_t color1(0.2f, 0.4f, 0.6f);
  display_p3f_t color2(0.2f, 0.4f, 0.6f);
  display_p3f_t color3(0.3f, 0.4f, 0.6f);

  EXPECT_TRUE(color1 == color2);
  EXPECT_FALSE(color1 == color3);
  EXPECT_TRUE(color1 != color3);
}

TEST(DisplayP3Test, ConstexprBoundaryValues) {
  constexpr display_p3f_t min_color(0.0f, 0.0f, 0.0f);
  static_assert(min_color.r() == 0.0f);
  static_assert(min_color.g() == 0.0f);
  static_assert(min_color.b() == 0.0f);

  constexpr display_p3f_t max_color(1.0f, 1.0f, 1.0f);
  static_assert(max_color.r() == 1.0f);
  static_assert(max_color.g() == 1.0f);
  static_assert(max_color.b() == 1.0f);
}

// DisplayP3A tests

TEST(DisplayP3ATest, CopyConstructors) {
  display_p3af_t original(0.2f, 0.4f, 0.6f, 0.8f);
  display_p3af_t copy(original);

  EXPECT_FLOAT_EQ(copy.r(), 0.2f);
  EXPECT_FLOAT_EQ(copy.g(), 0.4f);
  EXPECT_FLOAT_EQ(copy.b(), 0.6f);
  EXPECT_FLOAT_EQ(copy.a(), 0.8f);
}

TEST(DisplayP3ATest, CopyAssignment) {
  display_p3af_t original(0.2f, 0.4f, 0.6f, 0.8f);
  display_p3af_t copy(0.0f, 0.0f, 0.0f, 0.0f);
  copy = original;

  EXPECT_FLOAT_EQ(copy.r(), 0.2f);
  EXPECT_FLOAT_EQ(copy.g(), 0.4f);
  EXPECT_FLOAT_EQ(copy.b(), 0.6f);
  EXPECT_FLOAT_EQ(copy.a(), 0.8f);
}

TEST(DisplayP3ATest, MoveSemantics) {
  display_p3af_t original(0.2f, 0.4f, 0.6f, 0.8f);
  display_p3af_t moved(std::move(original));

  EXPECT_FLOAT_EQ(moved.r(), 0.2f);
  EXPECT_FLOAT_EQ(moved.g(), 0.4f);
  EXPECT_FLOAT_EQ(moved.b(), 0.6f);
  EXPECT_FLOAT_EQ(moved.a(), 0.8f);
}

TEST(DisplayP3ATest, MoveAssignment) {
  display_p3af_t original(0.2f, 0.4f, 0.6f, 0.8f);
  display_p3af_t moved(0.0f, 0.0f, 0.0f, 0.0f);
  moved = std::move(original);

  EXPECT_FLOAT_EQ(moved.r(), 0.2f);
  EXPECT_FLOAT_EQ(moved.g(), 0.4f);
  EXPECT_FLOAT_EQ(moved.b(), 0.6f);
  EXPECT_FLOAT_EQ(moved.a(), 0.8f);
}

TEST(DisplayP3ATest, EqualityComparison) {
  display_p3af_t color1(0.2f, 0.4f, 0.6f, 0.8f);
  display_p3af_t color2(0.2f, 0.4f, 0.6f, 0.8f);
  display_p3af_t color3(0.2f, 0.4f, 0.6f, 0.5f);

  EXPECT_TRUE(color1 == color2);
  EXPECT_FALSE(color1 == color3);
  EXPECT_TRUE(color1 != color3);
}

// Linear Display P3 tests

TEST(LinearDisplayP3Test, DefaultConstruction) {
  linear_display_p3f_t p3;
  EXPECT_FLOAT_EQ(p3.r(), 0.0f);
  EXPECT_FLOAT_EQ(p3.g(), 0.0f);
  EXPECT_FLOAT_EQ(p3.b(), 0.0f);
}

TEST(LinearDisplayP3Test, ParameterizedConstruction) {
  linear_display_p3f_t p3(0.1f, 0.3f, 0.5f);
  EXPECT_FLOAT_EQ(p3.r(), 0.1f);
  EXPECT_FLOAT_EQ(p3.g(), 0.3f);
  EXPECT_FLOAT_EQ(p3.b(), 0.5f);
}

// Type traits

TEST(DisplayP3Test, TypeTraits) {
  using display_p3_channels = typename traits::model_traits<display_p3::model::display_p3>::channels_type;
  static_assert(std::tuple_size_v<display_p3_channels> == 3);

  using r_ch = std::tuple_element_t<0, display_p3_channels>;
  static_assert(std::is_same_v<r_ch::value_type, float>);
  static_assert(r_ch::min == 0.0f);
  static_assert(r_ch::max == 1.0f);

  using display_p3a_channels = typename traits::model_traits<display_p3::model::display_p3a>::channels_type;
  static_assert(std::tuple_size_v<display_p3a_channels> == 4);

  static_assert(!traits::has_channel_tag_v<display_p3::model::display_p3, display_p3::channel::a_tag>);
  static_assert(traits::has_channel_tag_v<display_p3::model::display_p3a, display_p3::channel::a_tag>);
}

// Known color conversions - Temporarily disabled due to missing conversion implementation
/*
TEST(DisplayP3ConversionTest, BlackConversion) {
  auto p3 = color_cast<display_p3f_t>(rgbf_t{0.0f, 0.0f, 0.0f});
  EXPECT_NEAR(p3.r(), 0.0f, 1e-4f);
  EXPECT_NEAR(p3.g(), 0.0f, 1e-4f);
  EXPECT_NEAR(p3.b(), 0.0f, 1e-4f);
}

TEST(DisplayP3ConversionTest, WhiteConversion) {
  auto p3 = color_cast<display_p3f_t>(rgbf_t{1.0f, 1.0f, 1.0f});
  EXPECT_NEAR(p3.r(), 1.0f, 1e-4f);
  EXPECT_NEAR(p3.g(), 1.0f, 1e-4f);
  EXPECT_NEAR(p3.b(), 1.0f, 1e-4f);
}

TEST(DisplayP3ConversionTest, AlphaPreserved) {
  display_p3af_t original(0.5f, 0.3f, 0.7f, 0.6f);
  auto rgb = color_cast<rgbaf_t>(original);
  EXPECT_NEAR(rgb.a(), 0.6f, 1e-4f);
}

// Round-trip

TEST(DisplayP3ConversionTest, FloatRoundTrip) {
  display_p3f_t orig(0.6f, 0.2f, 0.8f);
  auto back = color_cast<display_p3f_t>(color_cast<rgbf_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

TEST(DisplayP3ConversionTest, AlphaRoundTrip) {
  display_p3af_t orig(0.3f, 0.5f, 0.7f, 0.4f);
  auto back = color_cast<display_p3af_t>(color_cast<rgbaf_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
  EXPECT_NEAR(back.a(), orig.a(), 1e-4f);
}

// Linear conversion

TEST(DisplayP3LinearTest, LinearConversion) {
  display_p3f_t gamma(0.5f, 0.5f, 0.5f);
  auto linear = color_cast<linear_display_p3f_t>(gamma);

  // Verify non-linear gamma curve
  EXPECT_LT(linear.r(), 0.5f);
  EXPECT_LT(linear.g(), 0.5f);
  EXPECT_LT(linear.b(), 0.5f);
}

TEST(DisplayP3LinearTest, LinearRoundTrip) {
  display_p3f_t orig(0.3f, 0.6f, 0.9f);
  auto linear = color_cast<linear_display_p3f_t>(orig);
  auto back = color_cast<display_p3f_t>(linear);

  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}
*/

}  // namespace colorcpp::core::test
