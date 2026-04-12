
#include <gtest/gtest.h>

#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::core::test {

using namespace colorcpp::operations::conversion;

// Construction

TEST(HSLTest, DefaultConstruction) {
  hsl_float_t hsl;
  EXPECT_FLOAT_EQ(hsl.h(), 0.0f);
  EXPECT_FLOAT_EQ(hsl.s(), 0.0f);
  EXPECT_FLOAT_EQ(hsl.l(), 0.0f);
}

TEST(HSLTest, ParameterizedConstruction) {
  hsl_float_t hsl(120.0f, 1.0f, 0.5f);
  EXPECT_FLOAT_EQ(hsl.h(), 120.0f);
  EXPECT_FLOAT_EQ(hsl.s(), 1.0f);
  EXPECT_FLOAT_EQ(hsl.l(), 0.5f);
}

TEST(HSLATest, ParameterizedWithAlpha) {
  hsla_float_t hsla(240.0f, 0.8f, 0.6f, 0.75f);
  EXPECT_FLOAT_EQ(hsla.h(), 240.0f);
  EXPECT_FLOAT_EQ(hsla.s(), 0.8f);
  EXPECT_FLOAT_EQ(hsla.l(), 0.6f);
  EXPECT_FLOAT_EQ(hsla.a(), 0.75f);
}

// Boundary values

TEST(HSLTest, BoundaryValuesMin) {
  hsl_float_t hsl(0.0f, 0.0f, 0.0f);
  EXPECT_FLOAT_EQ(hsl.h(), 0.0f);
  EXPECT_FLOAT_EQ(hsl.s(), 0.0f);
  EXPECT_FLOAT_EQ(hsl.l(), 0.0f);
}

TEST(HSLTest, BoundaryValuesMax) {
  hsl_float_t hsl(360.0f, 1.0f, 1.0f);
  EXPECT_FLOAT_EQ(hsl.h(), 360.0f);
  EXPECT_FLOAT_EQ(hsl.s(), 1.0f);
  EXPECT_FLOAT_EQ(hsl.l(), 1.0f);
}

TEST(HSLTest, OutOfRangeThrows) {
  EXPECT_THROW(hsl_float_t(-1.0f, 0.5f, 0.5f), std::out_of_range);
  EXPECT_THROW(hsl_float_t(361.0f, 0.5f, 0.5f), std::out_of_range);
  EXPECT_THROW(hsl_float_t(180.0f, -0.1f, 0.5f), std::out_of_range);
  EXPECT_THROW(hsl_float_t(180.0f, 1.1f, 0.5f), std::out_of_range);
  EXPECT_THROW(hsl_float_t(180.0f, 0.5f, -0.1f), std::out_of_range);
  EXPECT_THROW(hsl_float_t(180.0f, 0.5f, 1.1f), std::out_of_range);
}

// Mutable and indexed access

TEST(HSLTest, MutableMemberAccess) {
  hsl_float_t hsl(180.0f, 0.5f, 0.5f);
  hsl.h() = 90.0f;
  hsl.s() = 0.25f;
  hsl.l() = 0.75f;

  EXPECT_FLOAT_EQ(hsl.h(), 90.0f);
  EXPECT_FLOAT_EQ(hsl.s(), 0.25f);
  EXPECT_FLOAT_EQ(hsl.l(), 0.75f);
}

TEST(HSLTest, IndexAccess) {
  hsl_float_t hsl(120.0f, 0.6f, 0.4f);
  EXPECT_FLOAT_EQ(hsl.get_index<0>(), 120.0f);
  EXPECT_FLOAT_EQ(hsl.get_index<1>(), 0.6f);
  EXPECT_FLOAT_EQ(hsl.get_index<2>(), 0.4f);
}

TEST(HSLTest, ConstCorrectness) {
  const hsl_float_t hsl(30.0f, 0.7f, 0.3f);
  EXPECT_FLOAT_EQ(hsl.h(), 30.0f);
  EXPECT_FLOAT_EQ(hsl.s(), 0.7f);
  EXPECT_FLOAT_EQ(hsl.l(), 0.3f);
}

// Known color conversions

TEST(HSLConversionTest, BlackRgbToHsl) {
  auto hsl = color_cast<hsl_float_t>(rgb8_t{0, 0, 0});
  EXPECT_NEAR(hsl.s(), 0.0f, 1e-4f);
  EXPECT_NEAR(hsl.l(), 0.0f, 1e-4f);
}

TEST(HSLConversionTest, WhiteRgbToHsl) {
  auto hsl = color_cast<hsl_float_t>(rgb8_t{255, 255, 255});
  EXPECT_NEAR(hsl.s(), 0.0f, 1e-4f);
  EXPECT_NEAR(hsl.l(), 1.0f, 1e-4f);
}

TEST(HSLConversionTest, RedRgbToHsl) {
  auto hsl = color_cast<hsl_float_t>(rgbf_t{1.0f, 0.0f, 0.0f});
  // Red: h≈0° (or 360°), s=1, l=0.5
  EXPECT_NEAR(hsl.s(), 1.0f, 1e-4f);
  EXPECT_NEAR(hsl.l(), 0.5f, 1e-4f);
}

TEST(HSLConversionTest, GreenRgbToHsl) {
  auto hsl = color_cast<hsl_float_t>(rgbf_t{0.0f, 1.0f, 0.0f});
  EXPECT_NEAR(hsl.h(), 120.0f, 1e-3f);
  EXPECT_NEAR(hsl.s(), 1.0f, 1e-4f);
  EXPECT_NEAR(hsl.l(), 0.5f, 1e-4f);
}

TEST(HSLConversionTest, BlueRgbToHsl) {
  auto hsl = color_cast<hsl_float_t>(rgbf_t{0.0f, 0.0f, 1.0f});
  EXPECT_NEAR(hsl.h(), 240.0f, 1e-3f);
  EXPECT_NEAR(hsl.s(), 1.0f, 1e-4f);
  EXPECT_NEAR(hsl.l(), 0.5f, 1e-4f);
}

TEST(HSLConversionTest, YellowRgbToHsl) {
  auto hsl = color_cast<hsl_float_t>(rgbf_t{1.0f, 1.0f, 0.0f});
  EXPECT_NEAR(hsl.h(), 60.0f, 1e-3f);
  EXPECT_NEAR(hsl.s(), 1.0f, 1e-4f);
  EXPECT_NEAR(hsl.l(), 0.5f, 1e-4f);
}

// Round-trip tests

TEST(HSLConversionTest, FloatRoundTrip) {
  rgbf_t orig(0.6f, 0.2f, 0.8f);
  auto back = color_cast<rgbf_t>(color_cast<hsl_float_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

TEST(HSLConversionTest, Uint8RoundTrip) {
  rgb8_t orig(200, 80, 40);
  auto back = color_cast<rgb8_t>(color_cast<hsl_float_t>(orig));
  EXPECT_EQ(back.r(), orig.r());
  EXPECT_EQ(back.g(), orig.g());
  EXPECT_EQ(back.b(), orig.b());
}

TEST(HSLAConversionTest, AlphaPreservedRoundTrip) {
  rgbaf_t orig(0.5f, 0.25f, 0.75f, 0.5f);
  auto back = color_cast<rgbaf_t>(color_cast<hsla_float_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
  EXPECT_NEAR(back.a(), orig.a(), 1e-4f);
}

// Achromatic invariant

TEST(HSLConversionTest, GrayHasSaturationZero) {
  for (const int v : {0, 64, 128, 192, 255}) {
    rgb8_t gray(static_cast<uint8_t>(v), static_cast<uint8_t>(v), static_cast<uint8_t>(v));
    auto hsl = color_cast<hsl_float_t>(gray);
    EXPECT_NEAR(hsl.s(), 0.0f, 1e-4f) << "v=" << (int)v;
  }
}

TEST(HSLConversionTest, GrayLightnessMatchesValue) {
  // l = v / 255 for gray (since l = (max+min)/2 = v/255)
  rgb8_t gray128(128, 128, 128);
  auto hsl = color_cast<hsl_float_t>(gray128);
  EXPECT_NEAR(hsl.l(), 128.0f / 255.0f, 1e-3f);
}

TEST(HSLTest, CopyConstructors) {
  hsl_float_t original(120.0f, 0.8f, 0.6f);
  hsl_float_t copy(original);
  
  EXPECT_FLOAT_EQ(copy.h(), 120.0f);
  EXPECT_FLOAT_EQ(copy.s(), 0.8f);
  EXPECT_FLOAT_EQ(copy.l(), 0.6f);
}

TEST(HSLTest, CopyAssignment) {
  hsl_float_t original(120.0f, 0.8f, 0.6f);
  hsl_float_t copy(0.0f, 0.0f, 0.0f);
  copy = original;
  
  EXPECT_FLOAT_EQ(copy.h(), 120.0f);
  EXPECT_FLOAT_EQ(copy.s(), 0.8f);
  EXPECT_FLOAT_EQ(copy.l(), 0.6f);
}

TEST(HSLTest, MoveSemantics) {
  hsl_float_t original(120.0f, 0.8f, 0.6f);
  hsl_float_t moved(std::move(original));
  
  EXPECT_FLOAT_EQ(moved.h(), 120.0f);
  EXPECT_FLOAT_EQ(moved.s(), 0.8f);
  EXPECT_FLOAT_EQ(moved.l(), 0.6f);
}

TEST(HSLTest, MoveAssignment) {
  hsl_float_t original(120.0f, 0.8f, 0.6f);
  hsl_float_t moved(0.0f, 0.0f, 0.0f);
  moved = std::move(original);
  
  EXPECT_FLOAT_EQ(moved.h(), 120.0f);
  EXPECT_FLOAT_EQ(moved.s(), 0.8f);
  EXPECT_FLOAT_EQ(moved.l(), 0.6f);
}

TEST(HSLTest, EqualityComparison) {
  hsl_float_t color1(120.0f, 0.8f, 0.6f);
  hsl_float_t color2(120.0f, 0.8f, 0.6f);
  hsl_float_t color3(130.0f, 0.8f, 0.6f);
  
  EXPECT_TRUE(color1 == color2);
  EXPECT_FALSE(color1 == color3);
  EXPECT_TRUE(color1 != color3);
}

TEST(HSLTest, ConstexprBoundaryValues) {
  constexpr hsl_float_t min_color(0.0f, 0.0f, 0.0f);
  static_assert(min_color.h() == 0.0f);
  static_assert(min_color.s() == 0.0f);
  static_assert(min_color.l() == 0.0f);
  
  constexpr hsl_float_t max_color(360.0f, 1.0f, 1.0f);
  static_assert(max_color.h() == 360.0f);
  static_assert(max_color.s() == 1.0f);
  static_assert(max_color.l() == 1.0f);
}

TEST(HSLATest, CopyConstructors) {
  hsla_float_t original(240.0f, 0.8f, 0.6f, 0.75f);
  hsla_float_t copy(original);
  
  EXPECT_FLOAT_EQ(copy.h(), 240.0f);
  EXPECT_FLOAT_EQ(copy.s(), 0.8f);
  EXPECT_FLOAT_EQ(copy.l(), 0.6f);
  EXPECT_FLOAT_EQ(copy.a(), 0.75f);
}

TEST(HSLATest, CopyAssignment) {
  hsla_float_t original(240.0f, 0.8f, 0.6f, 0.75f);
  hsla_float_t copy(0.0f, 0.0f, 0.0f, 0.0f);
  copy = original;
  
  EXPECT_FLOAT_EQ(copy.h(), 240.0f);
  EXPECT_FLOAT_EQ(copy.s(), 0.8f);
  EXPECT_FLOAT_EQ(copy.l(), 0.6f);
  EXPECT_FLOAT_EQ(copy.a(), 0.75f);
}

TEST(HSLATest, MoveSemantics) {
  hsla_float_t original(240.0f, 0.8f, 0.6f, 0.75f);
  hsla_float_t moved(std::move(original));
  
  EXPECT_FLOAT_EQ(moved.h(), 240.0f);
  EXPECT_FLOAT_EQ(moved.s(), 0.8f);
  EXPECT_FLOAT_EQ(moved.l(), 0.6f);
  EXPECT_FLOAT_EQ(moved.a(), 0.75f);
}

TEST(HSLATest, MoveAssignment) {
  hsla_float_t original(240.0f, 0.8f, 0.6f, 0.75f);
  hsla_float_t moved(0.0f, 0.0f, 0.0f, 0.0f);
  moved = std::move(original);
  
  EXPECT_FLOAT_EQ(moved.h(), 240.0f);
  EXPECT_FLOAT_EQ(moved.s(), 0.8f);
  EXPECT_FLOAT_EQ(moved.l(), 0.6f);
  EXPECT_FLOAT_EQ(moved.a(), 0.75f);
}

TEST(HSLATest, EqualityComparison) {
  hsla_float_t color1(240.0f, 0.8f, 0.6f, 0.75f);
  hsla_float_t color2(240.0f, 0.8f, 0.6f, 0.75f);
  hsla_float_t color3(240.0f, 0.8f, 0.6f, 0.5f);
  
  EXPECT_TRUE(color1 == color2);
  EXPECT_FALSE(color1 == color3);
  EXPECT_TRUE(color1 != color3);
}

TEST(HSLTest, HueAngleEquivalence) {
  // Test 0° and 360° are equivalent for red hue
  hsl_float_t color1(0.0f, 1.0f, 0.5f);
  hsl_float_t color2(360.0f, 1.0f, 0.5f);
  
  EXPECT_NEAR(color1.h(), 0.0f, 1e-6f);
  EXPECT_NEAR(color2.h(), 360.0f, 1e-6f);
}

}  // namespace colorcpp::core::test
