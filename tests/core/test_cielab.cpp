
#include <gtest/gtest.h>

#include <colorcpp/core/cielab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::core::test {

using namespace colorcpp::operations::conversion;

// ── CIELAB Construction ───────────────────────────────────────────────────────

TEST(CIELabTest, DefaultConstruction) {
  cielab_t lab;
  EXPECT_FLOAT_EQ(lab.l(), 0.0f);
  EXPECT_FLOAT_EQ(lab.a(), 0.0f);
  EXPECT_FLOAT_EQ(lab.b(), 0.0f);
}

TEST(CIELabTest, ParameterizedConstruction) {
  cielab_t lab(50.0f, 20.0f, -30.0f);
  EXPECT_FLOAT_EQ(lab.l(), 50.0f);
  EXPECT_FLOAT_EQ(lab.a(), 20.0f);
  EXPECT_FLOAT_EQ(lab.b(), -30.0f);
}

TEST(CIELabTest, BoundaryValues) {
  cielab_t black(0.0f, 0.0f, 0.0f);
  EXPECT_FLOAT_EQ(black.l(), 0.0f);

  cielab_t white(100.0f, 0.0f, 0.0f);
  EXPECT_FLOAT_EQ(white.l(), 100.0f);

  // a and b range: [-128, 128]
  cielab_t extremes(50.0f, -128.0f, 128.0f);
  EXPECT_FLOAT_EQ(extremes.a(), -128.0f);
  EXPECT_FLOAT_EQ(extremes.b(), 128.0f);
}

TEST(CIELabTest, OutOfRangeThrows) {
  EXPECT_THROW(cielab_t(-1.0f, 0.0f, 0.0f), std::out_of_range);   // L < 0
  EXPECT_THROW(cielab_t(101.0f, 0.0f, 0.0f), std::out_of_range);  // L > 100
  EXPECT_THROW(cielab_t(50.0f, -129.0f, 0.0f), std::out_of_range);
  EXPECT_THROW(cielab_t(50.0f, 129.0f, 0.0f), std::out_of_range);
  EXPECT_THROW(cielab_t(50.0f, 0.0f, -129.0f), std::out_of_range);
  EXPECT_THROW(cielab_t(50.0f, 0.0f, 129.0f), std::out_of_range);
}

// ── CIELCH Construction ───────────────────────────────────────────────────────

TEST(CIELChTest, DefaultConstruction) {
  cielch_t lch;
  EXPECT_FLOAT_EQ(lch.l(), 0.0f);
  EXPECT_FLOAT_EQ(lch.c(), 0.0f);
  EXPECT_FLOAT_EQ(lch.h(), 0.0f);
}

TEST(CIELChTest, ParameterizedConstruction) {
  cielch_t lch(70.0f, 50.0f, 150.0f);
  EXPECT_FLOAT_EQ(lch.l(), 70.0f);
  EXPECT_FLOAT_EQ(lch.c(), 50.0f);
  EXPECT_FLOAT_EQ(lch.h(), 150.0f);
}

TEST(CIELChTest, OutOfRangeThrows) {
  EXPECT_THROW(cielch_t(-1.0f, 50.0f, 180.0f), std::out_of_range);  // L < 0
  EXPECT_THROW(cielch_t(101.0f, 50.0f, 180.0f), std::out_of_range); // L > 100
  EXPECT_THROW(cielch_t(50.0f, -1.0f, 180.0f), std::out_of_range);  // C < 0
  EXPECT_THROW(cielch_t(50.0f, 201.0f, 180.0f), std::out_of_range); // C > 200
  EXPECT_THROW(cielch_t(50.0f, 50.0f, -1.0f), std::out_of_range);   // H < 0
  EXPECT_THROW(cielch_t(50.0f, 50.0f, 361.0f), std::out_of_range);  // H > 360
}

// ── Member and indexed access ─────────────────────────────────────────────────

TEST(CIELabTest, MutableMemberAccess) {
  cielab_t lab(50.0f, 20.0f, -10.0f);
  lab.l() = 75.0f;
  lab.a() = -30.0f;
  lab.b() = 40.0f;
  EXPECT_FLOAT_EQ(lab.l(), 75.0f);
  EXPECT_FLOAT_EQ(lab.a(), -30.0f);
  EXPECT_FLOAT_EQ(lab.b(), 40.0f);
}

TEST(CIELabTest, IndexAccess) {
  cielab_t lab(60.0f, 15.0f, -25.0f);
  EXPECT_FLOAT_EQ(lab.get_index<0>(), 60.0f);
  EXPECT_FLOAT_EQ(lab.get_index<1>(), 15.0f);
  EXPECT_FLOAT_EQ(lab.get_index<2>(), -25.0f);
}

TEST(CIELabTest, ConstCorrectness) {
  const cielab_t lab(50.0f, 10.0f, -20.0f);
  EXPECT_FLOAT_EQ(lab.l(), 50.0f);
  EXPECT_FLOAT_EQ(lab.a(), 10.0f);
  EXPECT_FLOAT_EQ(lab.b(), -20.0f);
}

// ── Known color conversions: sRGB → CIELAB ───────────────────────────────────

TEST(CIELabConversionTest, BlackIsOrigin) {
  auto lab = color_cast<cielab_t>(rgbf_t{0.0f, 0.0f, 0.0f});
  EXPECT_NEAR(lab.l(), 0.0f, 1e-3f);
  EXPECT_NEAR(lab.a(), 0.0f, 1e-3f);
  EXPECT_NEAR(lab.b(), 0.0f, 1e-3f);
}

TEST(CIELabConversionTest, WhiteIsLOneHundred) {
  auto lab = color_cast<cielab_t>(rgbf_t{1.0f, 1.0f, 1.0f});
  EXPECT_NEAR(lab.l(), 100.0f, 1e-2f);
  EXPECT_NEAR(lab.a(), 0.0f, 1e-2f);
  EXPECT_NEAR(lab.b(), 0.0f, 1e-2f);
}

TEST(CIELabConversionTest, AchromaticHasZeroChroma) {
  // All grays: a*≈0, b*≈0
  for (uint8_t v : {64u, 128u, 192u}) {
    rgb8_t gray(v, v, v);
    auto lab = color_cast<cielab_t>(gray);
    EXPECT_NEAR(lab.a(), 0.0f, 1e-2f) << "gray=" << (int)v;
    EXPECT_NEAR(lab.b(), 0.0f, 1e-2f) << "gray=" << (int)v;
  }
}

TEST(CIELabConversionTest, LighterColorHasHigherL) {
  auto dark = color_cast<cielab_t>(rgbf_t{0.2f, 0.2f, 0.2f});
  auto light = color_cast<cielab_t>(rgbf_t{0.8f, 0.8f, 0.8f});
  EXPECT_LT(dark.l(), light.l());
}

TEST(CIELabConversionTest, RedHasPositiveA) {
  // Red in CIELAB: a* > 0 (red-green axis, positive = red)
  auto lab = color_cast<cielab_t>(rgbf_t{1.0f, 0.0f, 0.0f});
  EXPECT_GT(lab.a(), 0.0f);
}

TEST(CIELabConversionTest, GreenHasNegativeA) {
  // Green in CIELAB: a* < 0 (red-green axis, negative = green)
  auto lab = color_cast<cielab_t>(rgbf_t{0.0f, 1.0f, 0.0f});
  EXPECT_LT(lab.a(), 0.0f);
}

TEST(CIELabConversionTest, YellowHasPositiveB) {
  // Yellow in CIELAB: b* > 0 (blue-yellow axis, positive = yellow)
  auto lab = color_cast<cielab_t>(rgbf_t{1.0f, 1.0f, 0.0f});
  EXPECT_GT(lab.b(), 0.0f);
}

TEST(CIELabConversionTest, BlueHasNegativeB) {
  // Blue in CIELAB: b* < 0 (blue-yellow axis, negative = blue)
  auto lab = color_cast<cielab_t>(rgbf_t{0.0f, 0.0f, 1.0f});
  EXPECT_LT(lab.b(), 0.0f);
}

// ── Round-trip ────────────────────────────────────────────────────────────────

TEST(CIELabConversionTest, FloatRoundTrip) {
  rgbf_t orig(0.5f, 0.3f, 0.7f);
  auto back = color_cast<rgbf_t>(color_cast<cielab_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

TEST(CIELabConversionTest, PrimaryColorsRoundTrip) {
  for (auto orig : {rgbf_t{1.0f, 0.0f, 0.0f},
                    rgbf_t{0.0f, 1.0f, 0.0f},
                    rgbf_t{0.0f, 0.0f, 1.0f}}) {
    auto back = color_cast<rgbf_t>(color_cast<cielab_t>(orig));
    EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
    EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
    EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
  }
}

// ── CIELAB ↔ CIELCH ───────────────────────────────────────────────────────────

TEST(CIELChConversionTest, AchromaticHasZeroChroma) {
  auto lch = color_cast<cielch_t>(rgbf_t{0.5f, 0.5f, 0.5f});
  EXPECT_NEAR(lch.c(), 0.0f, 1e-2f);
}

TEST(CIELChConversionTest, LightnessMatchesCielab) {
  rgbf_t color(0.4f, 0.6f, 0.2f);
  auto lab = color_cast<cielab_t>(color);
  auto lch = color_cast<cielch_t>(color);
  EXPECT_NEAR(lch.l(), lab.l(), 1e-4f);
}

TEST(CIELChConversionTest, ChromaFromAB) {
  // For cielab (L, a, b), chroma C = sqrt(a²+b²)
  cielab_t lab(50.0f, 30.0f, 0.0f);
  auto lch = color_cast<cielch_t>(lab);
  EXPECT_NEAR(lch.c(), 30.0f, 1e-3f);
  EXPECT_NEAR(lch.h(), 0.0f, 1e-3f);  // a>0, b=0 → hue=0°
}

TEST(CIELChConversionTest, RoundTripLabToLch) {
  rgbf_t orig(0.6f, 0.3f, 0.9f);
  auto back = color_cast<rgbf_t>(color_cast<cielch_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-3f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-3f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-3f);
}

// ── Type traits ───────────────────────────────────────────────────────────────

TEST(CIELabTest, TypeTraits) {
  using cielab_channels = typename traits::model_traits<lab::model::cielab>::channels_type;
  static_assert(std::tuple_size_v<cielab_channels> == 3);

  using l_ch = std::tuple_element_t<0, cielab_channels>;
  static_assert(std::is_same_v<l_ch::value_type, float>);
  static_assert(l_ch::min == 0.0f);
  static_assert(l_ch::max == 100.0f);

  using a_ch = std::tuple_element_t<1, cielab_channels>;
  static_assert(a_ch::min == -128.0f);
  static_assert(a_ch::max == 128.0f);

  using cielch_channels = typename traits::model_traits<lab::model::cielch>::channels_type;
  static_assert(std::tuple_size_v<cielch_channels> == 3);

  using c_ch = std::tuple_element_t<1, cielch_channels>;
  static_assert(c_ch::min == 0.0f);
  static_assert(c_ch::max == 200.0f);
}

}  // namespace colorcpp::core::test
