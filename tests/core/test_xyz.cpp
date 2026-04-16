/**
 * @file test_xyz.cpp
 * @brief Tests for CIE XYZ color model and conversion.
 */

#include <gtest/gtest.h>

#include <colorcpp/core/cielab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::core::test {

using namespace colorcpp::operations::conversion;

// Construction

TEST(XYZTest, DefaultConstruction) {
  xyz_t xyz;
  EXPECT_FLOAT_EQ(xyz.x(), 0.0f);
  EXPECT_FLOAT_EQ(xyz.y(), 0.0f);
  EXPECT_FLOAT_EQ(xyz.z(), 0.0f);
}

TEST(XYZTest, ParameterizedConstruction) {
  xyz_t xyz(0.4f, 0.5f, 0.3f);
  EXPECT_FLOAT_EQ(xyz.x(), 0.4f);
  EXPECT_FLOAT_EQ(xyz.y(), 0.5f);
  EXPECT_FLOAT_EQ(xyz.z(), 0.3f);
}

// Boundary values

TEST(XYZTest, BoundaryValuesMin) {
  xyz_t xyz(0.0f, 0.0f, 0.0f);
  EXPECT_FLOAT_EQ(xyz.x(), 0.0f);
  EXPECT_FLOAT_EQ(xyz.y(), 0.0f);
  EXPECT_FLOAT_EQ(xyz.z(), 0.0f);
}

TEST(XYZTest, BoundaryValuesMax) {
  xyz_t xyz(2.0f, 2.0f, 2.0f);
  EXPECT_FLOAT_EQ(xyz.x(), 2.0f);
  EXPECT_FLOAT_EQ(xyz.y(), 2.0f);
  EXPECT_FLOAT_EQ(xyz.z(), 2.0f);
}

TEST(XYZTest, OutOfRangeThrows) {
  EXPECT_THROW(xyz_t(-0.1f, 1.0f, 1.0f), std::out_of_range);
  EXPECT_THROW(xyz_t(2.1f, 1.0f, 1.0f), std::out_of_range);
  EXPECT_THROW(xyz_t(1.0f, -0.1f, 1.0f), std::out_of_range);
  EXPECT_THROW(xyz_t(1.0f, 2.1f, 1.0f), std::out_of_range);
  EXPECT_THROW(xyz_t(1.0f, 1.0f, -0.1f), std::out_of_range);
  EXPECT_THROW(xyz_t(1.0f, 1.0f, 2.1f), std::out_of_range);
}

// Member and indexed access

TEST(XYZTest, MutableMemberAccess) {
  xyz_t xyz(0.3f, 0.4f, 0.5f);
  xyz.x() = 0.6f;
  xyz.y() = 0.7f;
  xyz.z() = 0.8f;

  EXPECT_FLOAT_EQ(xyz.x(), 0.6f);
  EXPECT_FLOAT_EQ(xyz.y(), 0.7f);
  EXPECT_FLOAT_EQ(xyz.z(), 0.8f);
}

TEST(XYZTest, IndexAccess) {
  xyz_t xyz(0.2f, 0.5f, 0.7f);
  EXPECT_FLOAT_EQ(xyz.get_index<0>(), 0.2f);
  EXPECT_FLOAT_EQ(xyz.get_index<1>(), 0.5f);
  EXPECT_FLOAT_EQ(xyz.get_index<2>(), 0.7f);
}

TEST(XYZTest, ConstCorrectness) {
  const xyz_t xyz(0.3f, 0.6f, 0.4f);
  EXPECT_FLOAT_EQ(xyz.x(), 0.3f);
  EXPECT_FLOAT_EQ(xyz.y(), 0.6f);
  EXPECT_FLOAT_EQ(xyz.z(), 0.4f);
}

// Copy and move semantics

TEST(XYZTest, CopyConstructors) {
  xyz_t original(0.4f, 0.5f, 0.3f);
  xyz_t copy(original);

  EXPECT_FLOAT_EQ(copy.x(), 0.4f);
  EXPECT_FLOAT_EQ(copy.y(), 0.5f);
  EXPECT_FLOAT_EQ(copy.z(), 0.3f);
}

TEST(XYZTest, CopyAssignment) {
  xyz_t original(0.4f, 0.5f, 0.3f);
  xyz_t copy(0.0f, 0.0f, 0.0f);
  copy = original;

  EXPECT_FLOAT_EQ(copy.x(), 0.4f);
  EXPECT_FLOAT_EQ(copy.y(), 0.5f);
  EXPECT_FLOAT_EQ(copy.z(), 0.3f);
}

TEST(XYZTest, MoveSemantics) {
  xyz_t original(0.4f, 0.5f, 0.3f);
  xyz_t moved(std::move(original));

  EXPECT_FLOAT_EQ(moved.x(), 0.4f);
  EXPECT_FLOAT_EQ(moved.y(), 0.5f);
  EXPECT_FLOAT_EQ(moved.z(), 0.3f);
}

TEST(XYZTest, MoveAssignment) {
  xyz_t original(0.4f, 0.5f, 0.3f);
  xyz_t moved(0.0f, 0.0f, 0.0f);
  moved = std::move(original);

  EXPECT_FLOAT_EQ(moved.x(), 0.4f);
  EXPECT_FLOAT_EQ(moved.y(), 0.5f);
  EXPECT_FLOAT_EQ(moved.z(), 0.3f);
}

TEST(XYZTest, EqualityComparison) {
  xyz_t color1(0.4f, 0.5f, 0.3f);
  xyz_t color2(0.4f, 0.5f, 0.3f);
  xyz_t color3(0.5f, 0.5f, 0.3f);

  EXPECT_TRUE(color1 == color2);
  EXPECT_FALSE(color1 == color3);
  EXPECT_TRUE(color1 != color3);
}

TEST(XYZTest, ConstexprBoundaryValues) {
  constexpr xyz_t min_color(0.0f, 0.0f, 0.0f);
  static_assert(min_color.x() == 0.0f);
  static_assert(min_color.y() == 0.0f);
  static_assert(min_color.z() == 0.0f);

  constexpr xyz_t max_color(2.0f, 2.0f, 2.0f);
  static_assert(max_color.x() == 2.0f);
  static_assert(max_color.y() == 2.0f);
  static_assert(max_color.z() == 2.0f);
}

// Type traits

TEST(XYZTest, TypeTraits) {
  using xyz_channels = typename traits::model_traits<xyz::model::xyze>::channels_type;
  static_assert(std::tuple_size_v<xyz_channels> == 3);

  using x_ch = std::tuple_element_t<0, xyz_channels>;
  static_assert(std::is_same_v<x_ch::value_type, float>);
  static_assert(x_ch::min == 0.0f);
  static_assert(x_ch::max == 2.0f);

  static_assert(traits::has_channel_tag_v<xyz::model::xyze, xyz::channel::x_tag>);
  static_assert(traits::has_channel_tag_v<xyz::model::xyze, xyz::channel::y_tag>);
  static_assert(traits::has_channel_tag_v<xyz::model::xyze, xyz::channel::z_tag>);
}

// Reference points

TEST(XYZConversionTest, D65WhitePoint) {
  // D65 white point XYZ values
  auto xyz = color_cast<xyz_t>(rgbf_t{1.0f, 1.0f, 1.0f});
  EXPECT_NEAR(xyz.y(), 1.0f, 1e-2f);  // Y = 1 for white
}

TEST(XYZConversionTest, BlackPoint) {
  auto xyz = color_cast<xyz_t>(rgbf_t{0.0f, 0.0f, 0.0f});
  EXPECT_NEAR(xyz.x(), 0.0f, 1e-4f);
  EXPECT_NEAR(xyz.y(), 0.0f, 1e-4f);
  EXPECT_NEAR(xyz.z(), 0.0f, 1e-4f);
}

// Round-trip conversions

TEST(XYZConversionTest, RGBRoundTrip) {
  rgbf_t orig(0.3f, 0.6f, 0.8f);
  auto back = color_cast<rgbf_t>(color_cast<xyz_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-3f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-3f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-3f);
}

TEST(XYZConversionTest, CIELABRoundTrip) {
  xyz_t orig(0.4f, 0.5f, 0.3f);
  auto back = color_cast<xyz_t>(color_cast<cielab_t>(orig));
  EXPECT_NEAR(back.x(), orig.x(), 1e-3f);
  EXPECT_NEAR(back.y(), orig.y(), 1e-3f);
  EXPECT_NEAR(back.z(), orig.z(), 1e-3f);
}

}  // namespace colorcpp::core::test
