
#include <gtest/gtest.h>

#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::core::test {

using namespace colorcpp::operations::conversion;

// OKLab Construction

TEST(OKLabTest, DefaultConstruction) {
  oklab_t lab;
  EXPECT_FLOAT_EQ(lab.l(), 0.0f);
  EXPECT_FLOAT_EQ(lab.a(), 0.0f);
  EXPECT_FLOAT_EQ(lab.b(), 0.0f);
}

TEST(OKLabTest, ParameterizedConstruction) {
  oklab_t lab(0.5f, 0.1f, -0.2f);
  EXPECT_FLOAT_EQ(lab.l(), 0.5f);
  EXPECT_FLOAT_EQ(lab.a(), 0.1f);
  EXPECT_FLOAT_EQ(lab.b(), -0.2f);
}

TEST(OKLabTest, BoundaryValues) {
  oklab_t black(0.0f, 0.0f, 0.0f);
  EXPECT_FLOAT_EQ(black.l(), 0.0f);

  oklab_t white(1.0f, 0.0f, 0.0f);
  EXPECT_FLOAT_EQ(white.l(), 1.0f);

  // a and b range: [-0.5, 0.5]
  oklab_t extremes(0.5f, -0.5f, 0.5f);
  EXPECT_FLOAT_EQ(extremes.a(), -0.5f);
  EXPECT_FLOAT_EQ(extremes.b(), 0.5f);
}

TEST(OKLabTest, OutOfRangeThrows) {
  EXPECT_THROW(oklab_t(-0.1f, 0.0f, 0.0f), std::out_of_range);  // L < 0
  EXPECT_THROW(oklab_t(1.1f, 0.0f, 0.0f), std::out_of_range);   // L > 1
  EXPECT_THROW(oklab_t(0.5f, -0.6f, 0.0f), std::out_of_range);  // a < -0.5
  EXPECT_THROW(oklab_t(0.5f, 0.6f, 0.0f), std::out_of_range);   // a > 0.5
  EXPECT_THROW(oklab_t(0.5f, 0.0f, -0.6f), std::out_of_range);  // b < -0.5
  EXPECT_THROW(oklab_t(0.5f, 0.0f, 0.6f), std::out_of_range);   // b > 0.5
}

// OKLCh Construction

TEST(OKLChTest, DefaultConstruction) {
  oklch_t lch;
  EXPECT_FLOAT_EQ(lch.l(), 0.0f);
  EXPECT_FLOAT_EQ(lch.c(), 0.0f);
  EXPECT_FLOAT_EQ(lch.h(), 0.0f);
}

TEST(OKLChTest, ParameterizedConstruction) {
  oklch_t lch(0.7f, 0.15f, 200.0f);
  EXPECT_FLOAT_EQ(lch.l(), 0.7f);
  EXPECT_FLOAT_EQ(lch.c(), 0.15f);
  EXPECT_FLOAT_EQ(lch.h(), 200.0f);
}

TEST(OKLChTest, OutOfRangeThrows) {
  EXPECT_THROW(oklch_t(-0.1f, 0.1f, 180.0f), std::out_of_range);  // L < 0
  EXPECT_THROW(oklch_t(1.1f, 0.1f, 180.0f), std::out_of_range);   // L > 1
  EXPECT_THROW(oklch_t(0.5f, -0.1f, 180.0f), std::out_of_range);  // C < 0
  EXPECT_THROW(oklch_t(0.5f, 0.5f, 180.0f), std::out_of_range);   // C > 0.4
  EXPECT_THROW(oklch_t(0.5f, 0.1f, -1.0f), std::out_of_range);    // H < 0
  EXPECT_THROW(oklch_t(0.5f, 0.1f, 361.0f), std::out_of_range);   // H > 360
}

// Member and indexed access

TEST(OKLabTest, MutableMemberAccess) {
  oklab_t lab(0.5f, 0.1f, 0.2f);
  lab.l() = 0.8f;
  lab.a() = -0.1f;
  lab.b() = 0.3f;
  EXPECT_FLOAT_EQ(lab.l(), 0.8f);
  EXPECT_FLOAT_EQ(lab.a(), -0.1f);
  EXPECT_FLOAT_EQ(lab.b(), 0.3f);
}

TEST(OKLabTest, IndexAccess) {
  oklab_t lab(0.6f, 0.1f, -0.2f);
  EXPECT_FLOAT_EQ(lab.get_index<0>(), 0.6f);
  EXPECT_FLOAT_EQ(lab.get_index<1>(), 0.1f);
  EXPECT_FLOAT_EQ(lab.get_index<2>(), -0.2f);
}

// Known color conversions: sRGB → OKLab

TEST(OKLabConversionTest, BlackIsOrigin) {
  auto lab = color_cast<oklab_t>(rgbf_t{0.0f, 0.0f, 0.0f});
  EXPECT_NEAR(lab.l(), 0.0f, 1e-4f);
  EXPECT_NEAR(lab.a(), 0.0f, 1e-4f);
  EXPECT_NEAR(lab.b(), 0.0f, 1e-4f);
}

TEST(OKLabConversionTest, WhiteIsLOne) {
  auto lab = color_cast<oklab_t>(rgbf_t{1.0f, 1.0f, 1.0f});
  EXPECT_NEAR(lab.l(), 1.0f, 1e-4f);
  EXPECT_NEAR(lab.a(), 0.0f, 1e-4f);
  EXPECT_NEAR(lab.b(), 0.0f, 1e-4f);
}

TEST(OKLabConversionTest, AchromaticHasZeroChroma) {
  // All grays should have a≈0, b≈0
  for (uint8_t v : {64u, 128u, 192u}) {
    rgb8_t gray(v, v, v);
    auto lab = color_cast<oklab_t>(gray);
    EXPECT_NEAR(lab.a(), 0.0f, 1e-3f) << "gray=" << (int)v;
    EXPECT_NEAR(lab.b(), 0.0f, 1e-3f) << "gray=" << (int)v;
  }
}

TEST(OKLabConversionTest, LighterColorHasHigherL) {
  auto dark = color_cast<oklab_t>(rgbf_t{0.2f, 0.2f, 0.2f});
  auto light = color_cast<oklab_t>(rgbf_t{0.8f, 0.8f, 0.8f});
  EXPECT_LT(dark.l(), light.l());
}

// Round-trip

TEST(OKLabConversionTest, FloatRoundTrip) {
  rgbf_t orig(0.5f, 0.2f, 0.8f);
  auto back = color_cast<rgbf_t>(color_cast<oklab_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

TEST(OKLabConversionTest, PrimaryColorsRoundTrip) {
  for (auto orig : {rgbf_t{1.0f, 0.0f, 0.0f}, rgbf_t{0.0f, 1.0f, 0.0f}, rgbf_t{0.0f, 0.0f, 1.0f}}) {
    auto back = color_cast<rgbf_t>(color_cast<oklab_t>(orig));
    EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
    EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
    EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
  }
}

// OKLab ↔ OKLCh

TEST(OKLChConversionTest, AchromaticHasZeroChroma) {
  auto lch = color_cast<oklch_t>(rgbf_t{0.5f, 0.5f, 0.5f});
  EXPECT_NEAR(lch.c(), 0.0f, 1e-3f);
}

TEST(OKLChConversionTest, LightnessMatchesOklab) {
  rgbf_t color(0.4f, 0.6f, 0.2f);
  auto lab = color_cast<oklab_t>(color);
  auto lch = color_cast<oklch_t>(color);
  EXPECT_NEAR(lch.l(), lab.l(), 1e-4f);
}

TEST(OKLChConversionTest, ChromaFromAB) {
  // For oklab (L, a, b), chroma C = sqrt(a²+b²)
  oklab_t lab(0.5f, 0.1f, 0.0f);
  auto lch = color_cast<oklch_t>(lab);
  EXPECT_NEAR(lch.c(), 0.1f, 1e-4f);
  EXPECT_NEAR(lch.h(), 0.0f, 1e-3f);  // a>0, b=0 → hue=0°
}

TEST(OKLChConversionTest, RoundTripOklabToLch) {
  rgbf_t orig(0.6f, 0.3f, 0.9f);
  auto back = color_cast<rgbf_t>(color_cast<oklch_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-3f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-3f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-3f);
}

// Type traits

TEST(OKLabTest, TypeTraits) {
  using oklab_channels = typename traits::model_traits<oklab::model::oklab>::channels_type;
  static_assert(std::tuple_size_v<oklab_channels> == 3);

  using l_ch = std::tuple_element_t<0, oklab_channels>;
  static_assert(std::is_same_v<l_ch::value_type, float>);
  static_assert(l_ch::min == 0.0f);
  static_assert(l_ch::max == 1.0f);

  using a_ch = std::tuple_element_t<1, oklab_channels>;
  static_assert(a_ch::min == -0.5f);
  static_assert(a_ch::max == 0.5f);

  using oklch_channels = typename traits::model_traits<oklab::model::oklch>::channels_type;
  static_assert(std::tuple_size_v<oklch_channels> == 3);
}

}  // namespace colorcpp::core::test
