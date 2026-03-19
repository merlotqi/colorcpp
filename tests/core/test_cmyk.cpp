
#include <gtest/gtest.h>

#include <colorcpp/core/cmyk.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::core::test {

using namespace colorcpp::operations::conversion;

// ── Construction ──────────────────────────────────────────────────────────────

TEST(CMYKTest, DefaultConstruction) {
  cmyk_float_t cmyk;
  EXPECT_FLOAT_EQ(cmyk.c(), 0.0f);
  EXPECT_FLOAT_EQ(cmyk.m(), 0.0f);
  EXPECT_FLOAT_EQ(cmyk.y(), 0.0f);
  EXPECT_FLOAT_EQ(cmyk.k(), 0.0f);
}

TEST(CMYKTest, FloatParameterizedConstruction) {
  cmyk_float_t cmyk(0.1f, 0.2f, 0.3f, 0.4f);
  EXPECT_FLOAT_EQ(cmyk.c(), 0.1f);
  EXPECT_FLOAT_EQ(cmyk.m(), 0.2f);
  EXPECT_FLOAT_EQ(cmyk.y(), 0.3f);
  EXPECT_FLOAT_EQ(cmyk.k(), 0.4f);
}

TEST(CMYKTest, Uint8ParameterizedConstruction) {
  cmyk8_t cmyk(10, 20, 30, 40);
  EXPECT_EQ(cmyk.c(), 10);
  EXPECT_EQ(cmyk.m(), 20);
  EXPECT_EQ(cmyk.y(), 30);
  EXPECT_EQ(cmyk.k(), 40);
}

// ── Boundary values ───────────────────────────────────────────────────────────

TEST(CMYKTest, FloatBoundaryMin) {
  cmyk_float_t cmyk(0.0f, 0.0f, 0.0f, 0.0f);
  EXPECT_FLOAT_EQ(cmyk.c(), 0.0f);
  EXPECT_FLOAT_EQ(cmyk.k(), 0.0f);
}

TEST(CMYKTest, FloatBoundaryMax) {
  cmyk_float_t cmyk(1.0f, 1.0f, 1.0f, 1.0f);
  EXPECT_FLOAT_EQ(cmyk.c(), 1.0f);
  EXPECT_FLOAT_EQ(cmyk.k(), 1.0f);
}

TEST(CMYKTest, Uint8BoundaryMax) {
  cmyk8_t cmyk(100, 100, 100, 100);
  EXPECT_EQ(cmyk.c(), 100);
  EXPECT_EQ(cmyk.k(), 100);
}

TEST(CMYKTest, FloatOutOfRangeThrows) {
  EXPECT_THROW(cmyk_float_t(-0.1f, 0.0f, 0.0f, 0.0f), std::out_of_range);
  EXPECT_THROW(cmyk_float_t(0.0f, 1.1f, 0.0f, 0.0f), std::out_of_range);
  EXPECT_THROW(cmyk_float_t(0.0f, 0.0f, -0.1f, 0.0f), std::out_of_range);
  EXPECT_THROW(cmyk_float_t(0.0f, 0.0f, 0.0f, 1.1f), std::out_of_range);
}

TEST(CMYKTest, Uint8OutOfRangeThrows) {
  EXPECT_THROW(cmyk8_t(101, 0, 0, 0), std::out_of_range);
  EXPECT_THROW(cmyk8_t(0, 101, 0, 0), std::out_of_range);
  EXPECT_THROW(cmyk8_t(0, 0, 101, 0), std::out_of_range);
  EXPECT_THROW(cmyk8_t(0, 0, 0, 101), std::out_of_range);
}

// ── Member and indexed access ─────────────────────────────────────────────────

TEST(CMYKTest, MutableMemberAccess) {
  cmyk_float_t cmyk(0.1f, 0.2f, 0.3f, 0.4f);
  cmyk.c() = 0.5f;
  cmyk.m() = 0.6f;
  cmyk.y() = 0.7f;
  cmyk.k() = 0.8f;
  EXPECT_FLOAT_EQ(cmyk.c(), 0.5f);
  EXPECT_FLOAT_EQ(cmyk.m(), 0.6f);
  EXPECT_FLOAT_EQ(cmyk.y(), 0.7f);
  EXPECT_FLOAT_EQ(cmyk.k(), 0.8f);
}

TEST(CMYKTest, IndexAccess) {
  cmyk_float_t cmyk(0.1f, 0.2f, 0.3f, 0.4f);
  EXPECT_FLOAT_EQ(cmyk.get_index<0>(), 0.1f);
  EXPECT_FLOAT_EQ(cmyk.get_index<1>(), 0.2f);
  EXPECT_FLOAT_EQ(cmyk.get_index<2>(), 0.3f);
  EXPECT_FLOAT_EQ(cmyk.get_index<3>(), 0.4f);
}

TEST(CMYKTest, ConstCorrectness) {
  const cmyk_float_t cmyk(0.25f, 0.5f, 0.75f, 0.0f);
  EXPECT_FLOAT_EQ(cmyk.c(), 0.25f);
  EXPECT_FLOAT_EQ(cmyk.m(), 0.5f);
  EXPECT_FLOAT_EQ(cmyk.y(), 0.75f);
  EXPECT_FLOAT_EQ(cmyk.k(), 0.0f);
}

// ── Known color conversions ───────────────────────────────────────────────────

TEST(CMYKConversionTest, WhiteToFloat) {
  // White: RGB(1,1,1) → CMYK(0,0,0,0)
  auto cmyk = color_cast<cmyk_float_t>(rgbf_t{1.0f, 1.0f, 1.0f});
  EXPECT_NEAR(cmyk.c(), 0.0f, 1e-4f);
  EXPECT_NEAR(cmyk.m(), 0.0f, 1e-4f);
  EXPECT_NEAR(cmyk.y(), 0.0f, 1e-4f);
  EXPECT_NEAR(cmyk.k(), 0.0f, 1e-4f);
}

TEST(CMYKConversionTest, BlackToFloat) {
  // Black: RGB(0,0,0) → CMYK(0,0,0,1)
  auto cmyk = color_cast<cmyk_float_t>(rgbf_t{0.0f, 0.0f, 0.0f});
  EXPECT_NEAR(cmyk.c(), 0.0f, 1e-4f);
  EXPECT_NEAR(cmyk.m(), 0.0f, 1e-4f);
  EXPECT_NEAR(cmyk.y(), 0.0f, 1e-4f);
  EXPECT_NEAR(cmyk.k(), 1.0f, 1e-4f);
}

TEST(CMYKConversionTest, RedToFloat) {
  // Red: RGB(1,0,0) → CMYK(0,1,1,0)
  auto cmyk = color_cast<cmyk_float_t>(rgbf_t{1.0f, 0.0f, 0.0f});
  EXPECT_NEAR(cmyk.c(), 0.0f, 1e-4f);
  EXPECT_NEAR(cmyk.m(), 1.0f, 1e-4f);
  EXPECT_NEAR(cmyk.y(), 1.0f, 1e-4f);
  EXPECT_NEAR(cmyk.k(), 0.0f, 1e-4f);
}

TEST(CMYKConversionTest, GreenToFloat) {
  // Green: RGB(0,1,0) → CMYK(1,0,1,0)
  auto cmyk = color_cast<cmyk_float_t>(rgbf_t{0.0f, 1.0f, 0.0f});
  EXPECT_NEAR(cmyk.c(), 1.0f, 1e-4f);
  EXPECT_NEAR(cmyk.m(), 0.0f, 1e-4f);
  EXPECT_NEAR(cmyk.y(), 1.0f, 1e-4f);
  EXPECT_NEAR(cmyk.k(), 0.0f, 1e-4f);
}

TEST(CMYKConversionTest, BlueToFloat) {
  // Blue: RGB(0,0,1) → CMYK(1,1,0,0)
  auto cmyk = color_cast<cmyk_float_t>(rgbf_t{0.0f, 0.0f, 1.0f});
  EXPECT_NEAR(cmyk.c(), 1.0f, 1e-4f);
  EXPECT_NEAR(cmyk.m(), 1.0f, 1e-4f);
  EXPECT_NEAR(cmyk.y(), 0.0f, 1e-4f);
  EXPECT_NEAR(cmyk.k(), 0.0f, 1e-4f);
}

TEST(CMYKConversionTest, YellowToFloat) {
  // Yellow: RGB(1,1,0) → CMYK(0,0,1,0)
  auto cmyk = color_cast<cmyk_float_t>(rgbf_t{1.0f, 1.0f, 0.0f});
  EXPECT_NEAR(cmyk.c(), 0.0f, 1e-4f);
  EXPECT_NEAR(cmyk.m(), 0.0f, 1e-4f);
  EXPECT_NEAR(cmyk.y(), 1.0f, 1e-4f);
  EXPECT_NEAR(cmyk.k(), 0.0f, 1e-4f);
}

TEST(CMYKConversionTest, WhiteToUint8) {
  auto cmyk = color_cast<cmyk8_t>(rgb8_t{255, 255, 255});
  EXPECT_EQ(cmyk.c(), 0);
  EXPECT_EQ(cmyk.m(), 0);
  EXPECT_EQ(cmyk.y(), 0);
  EXPECT_EQ(cmyk.k(), 0);
}

TEST(CMYKConversionTest, BlackToUint8) {
  auto cmyk = color_cast<cmyk8_t>(rgb8_t{0, 0, 0});
  EXPECT_EQ(cmyk.c(), 0);
  EXPECT_EQ(cmyk.m(), 0);
  EXPECT_EQ(cmyk.y(), 0);
  EXPECT_EQ(cmyk.k(), 100);
}

// ── Round-trip ────────────────────────────────────────────────────────────────

TEST(CMYKConversionTest, FloatRoundTrip) {
  rgbf_t orig(0.6f, 0.2f, 0.8f);
  auto back = color_cast<rgbf_t>(color_cast<cmyk_float_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

TEST(CMYKConversionTest, Uint8RoundTrip) {
  // CMYK uint8 channels are [0,100], so quantization introduces ±1 error
  // when converting from 8-bit RGB (256 levels) and back.
  rgb8_t orig(180, 60, 90);
  auto back = color_cast<rgb8_t>(color_cast<cmyk8_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1);
  EXPECT_NEAR(back.g(), orig.g(), 1);
  EXPECT_NEAR(back.b(), orig.b(), 1);
}

// ── Type traits ───────────────────────────────────────────────────────────────

TEST(CMYKTest, TypeTraits) {
  using cmyk8_channels = typename traits::model_traits<cmyk::model::cmyk_u8>::channels_type;
  static_assert(std::tuple_size_v<cmyk8_channels> == 4);

  using c_ch = std::tuple_element_t<0, cmyk8_channels>;
  static_assert(std::is_same_v<c_ch::value_type, uint8_t>);
  static_assert(c_ch::min == 0);
  static_assert(c_ch::max == 100);

  using cmykf_channels = typename traits::model_traits<cmyk::model::cmyk_float>::channels_type;
  static_assert(std::tuple_size_v<cmykf_channels> == 4);

  using cf_ch = std::tuple_element_t<0, cmykf_channels>;
  static_assert(std::is_same_v<cf_ch::value_type, float>);
  static_assert(cf_ch::min == 0.0f);
  static_assert(cf_ch::max == 1.0f);
}

}  // namespace colorcpp::core::test
