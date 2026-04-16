
#include <gtest/gtest.h>

#include <colorcpp/core/rgb.hpp>
#include <type_traits>

namespace colorcpp::core::test {

TEST(RGBATest, Construction) {
  rgb8_t rgb_default;
  EXPECT_EQ(rgb_default.r(), 0);
  EXPECT_EQ(rgb_default.g(), 0);
  EXPECT_EQ(rgb_default.b(), 0);

  rgb8_t rgb(255, 128, 64);
  EXPECT_EQ(rgb.r(), 255);
  EXPECT_EQ(rgb.g(), 128);
  EXPECT_EQ(rgb.b(), 64);

  rgba8_t rgba(255, 128, 64, 255);
  EXPECT_EQ(rgba.r(), 255);
  EXPECT_EQ(rgba.g(), 128);
  EXPECT_EQ(rgba.b(), 64);
  EXPECT_EQ(rgba.a(), 255);

  rgbf_t rgbf(0.5f, 0.75f, 1.0f);
  EXPECT_FLOAT_EQ(rgbf.r(), 0.5f);
  EXPECT_FLOAT_EQ(rgbf.g(), 0.75f);
  EXPECT_FLOAT_EQ(rgbf.b(), 1.0f);

  rgbaf_t rgbaf(0.5f, 0.75f, 1.0f, 0.25f);
  EXPECT_FLOAT_EQ(rgbaf.r(), 0.5f);
  EXPECT_FLOAT_EQ(rgbaf.g(), 0.75f);
  EXPECT_FLOAT_EQ(rgbaf.b(), 1.0f);
  EXPECT_FLOAT_EQ(rgbaf.a(), 0.25f);
}

TEST(RGBATest, BoundaryValues) {
  rgb8_t rgb_min(0, 0, 0);
  EXPECT_EQ(rgb_min.r(), 0);
  EXPECT_EQ(rgb_min.g(), 0);
  EXPECT_EQ(rgb_min.b(), 0);

  rgb8_t rgb_max(255, 255, 255);
  EXPECT_EQ(rgb_max.r(), 255);
  EXPECT_EQ(rgb_max.g(), 255);
  EXPECT_EQ(rgb_max.b(), 255);

  rgbf_t rgbf_min(0.0f, 0.0f, 0.0f);
  EXPECT_FLOAT_EQ(rgbf_min.r(), 0.0f);
  EXPECT_FLOAT_EQ(rgbf_min.g(), 0.0f);
  EXPECT_FLOAT_EQ(rgbf_min.b(), 0.0f);

  rgbf_t rgbf_max(1.0f, 1.0f, 1.0f);
  EXPECT_FLOAT_EQ(rgbf_max.r(), 1.0f);
  EXPECT_FLOAT_EQ(rgbf_max.g(), 1.0f);
  EXPECT_FLOAT_EQ(rgbf_max.b(), 1.0f);
}

TEST(RGBATest, OutOfRangeThrows) {
  EXPECT_THROW(rgb8_t(256, 0, 0), std::out_of_range);
  EXPECT_THROW(rgb8_t(0, 256, 0), std::out_of_range);
  EXPECT_THROW(rgb8_t(0, 0, 256), std::out_of_range);
  EXPECT_THROW(rgba8_t(0, 0, 0, 256), std::out_of_range);

  EXPECT_THROW(rgbf_t(1.1f, 0.0f, 0.0f), std::out_of_range);
  EXPECT_THROW(rgbf_t(0.0f, 1.1f, 0.0f), std::out_of_range);
  EXPECT_THROW(rgbf_t(0.0f, 0.0f, 1.1f), std::out_of_range);
  EXPECT_THROW(rgbaf_t(0.0f, 0.0f, 0.0f, 1.1f), std::out_of_range);

  EXPECT_THROW(rgbf_t(-0.1f, 0.0f, 0.0f), std::out_of_range);
}

TEST(RGBATest, TagAccess) {
  rgba8_t rgba(100, 150, 200, 250);

  EXPECT_EQ(rgba.get<rgb::channel::r_tag>(), 100);
  EXPECT_EQ(rgba.get<rgb::channel::g_tag>(), 150);
  EXPECT_EQ(rgba.get<rgb::channel::b_tag>(), 200);
  EXPECT_EQ(rgba.get<rgb::channel::a_tag>(), 250);

  rgba.get<rgb::channel::r_tag>() = 50;
  EXPECT_EQ(rgba.r(), 50);
}

TEST(RGBATest, IndexAccess) {
  rgba8_t rgba(100, 150, 200, 250);

  EXPECT_EQ(rgba.get_index<0>(), 100);
  EXPECT_EQ(rgba.get_index<1>(), 150);
  EXPECT_EQ(rgba.get_index<2>(), 200);
  EXPECT_EQ(rgba.get_index<3>(), 250);

  rgba.get_index<1>() = 75;
  EXPECT_EQ(rgba.g(), 75);
}

TEST(RGBATest, MemberAccess) {
  rgba8_t rgba(100, 150, 200, 250);

  EXPECT_EQ(rgba.r(), 100);
  EXPECT_EQ(rgba.g(), 150);
  EXPECT_EQ(rgba.b(), 200);
  EXPECT_EQ(rgba.a(), 250);

  rgba.r() = 50;
  rgba.g() = 75;
  rgba.b() = 125;
  rgba.a() = 200;

  EXPECT_EQ(rgba.r(), 50);
  EXPECT_EQ(rgba.g(), 75);
  EXPECT_EQ(rgba.b(), 125);
  EXPECT_EQ(rgba.a(), 200);
}

TEST(RGBATest, GlobalGetter) {
  rgba8_t rgba(100, 150, 200, 250);

  EXPECT_EQ(colorcpp::get<rgb::channel::r_tag>(rgba), 100);
  EXPECT_EQ(colorcpp::get<rgb::channel::g_tag>(rgba), 150);
  EXPECT_EQ(colorcpp::get<rgb::channel::b_tag>(rgba), 200);
  EXPECT_EQ(colorcpp::get<rgb::channel::a_tag>(rgba), 250);

  EXPECT_EQ(colorcpp::get<0>(rgba), 100);
  EXPECT_EQ(colorcpp::get<1>(rgba), 150);
  EXPECT_EQ(colorcpp::get<2>(rgba), 200);
  EXPECT_EQ(colorcpp::get<3>(rgba), 250);

  colorcpp::get<0>(rgba) = 50;
  EXPECT_EQ(rgba.r(), 50);
}

TEST(RGBATest, StdGetter) {
  rgba8_t rgba(100, 150, 200, 250);

  EXPECT_EQ(std::get<0>(rgba), 100);
  EXPECT_EQ(std::get<1>(rgba), 150);
  EXPECT_EQ(std::get<2>(rgba), 200);
  EXPECT_EQ(std::get<3>(rgba), 250);

  EXPECT_EQ(std::get<rgb::channel::r_tag>(rgba), 100);
  EXPECT_EQ(std::get<rgb::channel::g_tag>(rgba), 150);
  EXPECT_EQ(std::get<rgb::channel::b_tag>(rgba), 200);
  EXPECT_EQ(std::get<rgb::channel::a_tag>(rgba), 250);

  std::get<1>(rgba) = 75;
  EXPECT_EQ(rgba.g(), 75);
}

TEST(RGBATest, ConstCorrectness) {
  const rgba8_t rgba(100, 150, 200, 250);

  EXPECT_EQ(rgba.r(), 100);
  EXPECT_EQ(rgba.g(), 150);
  EXPECT_EQ(rgba.b(), 200);
  EXPECT_EQ(rgba.a(), 250);
}

TEST(RGBATest, TypeTraits) {
  using rgb8_channels = typename traits::model_traits<rgb::model::rgb8>::channels_type;
  static_assert(std::tuple_size_v<rgb8_channels> == 3);

  using first_channel = std::tuple_element_t<0, rgb8_channels>;
  static_assert(std::is_same_v<first_channel::value_type, uint8_t>);
  static_assert(first_channel::min == 0);
  static_assert(first_channel::max == 255);

  using rgba8_channels = typename traits::model_traits<rgb::model::rgba8>::channels_type;
  static_assert(std::tuple_size_v<rgba8_channels> == 4);

  using rgbf_channels = typename traits::model_traits<rgb::model::rgb_float>::channels_type;
  static_assert(std::tuple_size_v<rgbf_channels> == 3);

  using float_channel = std::tuple_element_t<0, rgbf_channels>;
  static_assert(std::is_same_v<float_channel::value_type, float>);
  static_assert(float_channel::min == 0.0f);
  static_assert(float_channel::max == 1.0f);
}

TEST(RGBATest, ConstexprSupport) {
  constexpr rgb8_t rgb(100, 150, 200);
  static_assert(rgb.r() == 100);
  static_assert(rgb.g() == 150);
  static_assert(rgb.b() == 200);

  constexpr rgba8_t rgba(100, 150, 200, 250);
  static_assert(rgba.a() == 250);

  constexpr rgbf_t rgbf(0.25f, 0.5f, 0.75f);
  static_assert(rgbf.r() == 0.25f);
  static_assert(rgbf.g() == 0.5f);
  static_assert(rgbf.b() == 0.75f);
}

TEST(RGBATest, CombinationOperations) {
  rgba8_t rgba(100, 150, 200, 250);

  rgba.r() = 50;
  rgba.get<rgb::channel::g_tag>() = 75;
  rgba.get_index<2>() = 125;
  colorcpp::get<3>(rgba) = 200;

  EXPECT_EQ(rgba.r(), 50);
  EXPECT_EQ(rgba.g(), 75);
  EXPECT_EQ(rgba.b(), 125);
  EXPECT_EQ(rgba.a(), 200);
}

TEST(RGBATest, MultipleColors) {
  rgb8_t rgb1(100, 150, 200);
  rgb8_t rgb2(50, 75, 125);

  auto rgb3 = rgb1;
  rgb3 = rgb2;

  EXPECT_EQ(rgb3.r(), 50);
  EXPECT_EQ(rgb3.g(), 75);
  EXPECT_EQ(rgb3.b(), 125);
}

TEST(RGBATest, AlphaExistence) {
  static_assert(!traits::has_channel_tag_v<rgb::model::rgb8, rgb::channel::a_tag>);

  static_assert(traits::has_channel_tag_v<rgb::model::rgba8, rgb::channel::a_tag>);

  static_assert(!traits::has_channel_tag_v<rgb::model::rgb_float, rgb::channel::a_tag>);
  static_assert(traits::has_channel_tag_v<rgb::model::rgba_float, rgb::channel::a_tag>);
}

TEST(RGBATest, CopyConstructors) {
  rgba8_t original(100, 150, 200, 250);
  rgba8_t copy(original);

  EXPECT_EQ(copy.r(), 100);
  EXPECT_EQ(copy.g(), 150);
  EXPECT_EQ(copy.b(), 200);
  EXPECT_EQ(copy.a(), 250);
}

TEST(RGBATest, CopyAssignment) {
  rgba8_t original(100, 150, 200, 250);
  rgba8_t copy(0, 0, 0, 0);
  copy = original;

  EXPECT_EQ(copy.r(), 100);
  EXPECT_EQ(copy.g(), 150);
  EXPECT_EQ(copy.b(), 200);
  EXPECT_EQ(copy.a(), 250);
}

TEST(RGBATest, MoveSemantics) {
  rgba8_t original(100, 150, 200, 250);
  rgba8_t moved(std::move(original));

  EXPECT_EQ(moved.r(), 100);
  EXPECT_EQ(moved.g(), 150);
  EXPECT_EQ(moved.b(), 200);
  EXPECT_EQ(moved.a(), 250);
}

TEST(RGBATest, MoveAssignment) {
  rgba8_t original(100, 150, 200, 250);
  rgba8_t moved(0, 0, 0, 0);
  moved = std::move(original);

  EXPECT_EQ(moved.r(), 100);
  EXPECT_EQ(moved.g(), 150);
  EXPECT_EQ(moved.b(), 200);
  EXPECT_EQ(moved.a(), 250);
}

TEST(RGBATest, EqualityComparison) {
  rgba8_t color1(100, 150, 200, 250);
  rgba8_t color2(100, 150, 200, 250);
  rgba8_t color3(100, 150, 200, 240);

  EXPECT_TRUE(color1 == color2);
  EXPECT_FALSE(color1 == color3);
  EXPECT_TRUE(color1 != color3);
}

TEST(RGBATest, FloatPrecision) {
  rgbaf_t color(0.1f, 0.2f, 0.3f, 0.4f);

  EXPECT_NEAR(color.r(), 0.1f, 0.00001f);
  EXPECT_NEAR(color.g(), 0.2f, 0.00001f);
  EXPECT_NEAR(color.b(), 0.3f, 0.00001f);
  EXPECT_NEAR(color.a(), 0.4f, 0.00001f);
}

TEST(RGBATest, ConstexprBoundaryValues) {
  constexpr rgb8_t min_color(0, 0, 0);
  static_assert(min_color.r() == 0);
  static_assert(min_color.g() == 0);
  static_assert(min_color.b() == 0);

  constexpr rgb8_t max_color(255, 255, 255);
  static_assert(max_color.r() == 255);
  static_assert(max_color.g() == 255);
  static_assert(max_color.b() == 255);

  constexpr rgbf_t float_min(0.0f, 0.0f, 0.0f);
  static_assert(float_min.r() == 0.0f);
  static_assert(float_min.g() == 0.0f);
  static_assert(float_min.b() == 0.0f);

  constexpr rgbf_t float_max(1.0f, 1.0f, 1.0f);
  static_assert(float_max.r() == 1.0f);
  static_assert(float_max.g() == 1.0f);
  static_assert(float_max.b() == 1.0f);
}

}  // namespace colorcpp::core::test
