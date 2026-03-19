
#include <gtest/gtest.h>

#include <colorcpp/core/hsv.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::core::test {

using namespace colorcpp::operations::conversion;

// ── Construction ──────────────────────────────────────────────────────────────

TEST(HSVTest, DefaultConstruction) {
  hsv_float_t hsv;
  EXPECT_FLOAT_EQ(hsv.h(), 0.0f);
  EXPECT_FLOAT_EQ(hsv.s(), 0.0f);
  EXPECT_FLOAT_EQ(hsv.v(), 0.0f);
}

TEST(HSVTest, ParameterizedConstruction) {
  hsv_float_t hsv(120.0f, 0.8f, 0.6f);
  EXPECT_FLOAT_EQ(hsv.h(), 120.0f);
  EXPECT_FLOAT_EQ(hsv.s(), 0.8f);
  EXPECT_FLOAT_EQ(hsv.v(), 0.6f);
}

TEST(HSVATest, ParameterizedWithAlpha) {
  hsva_float_t hsva(240.0f, 1.0f, 0.5f, 0.75f);
  EXPECT_FLOAT_EQ(hsva.h(), 240.0f);
  EXPECT_FLOAT_EQ(hsva.s(), 1.0f);
  EXPECT_FLOAT_EQ(hsva.v(), 0.5f);
  EXPECT_FLOAT_EQ(hsva.a(), 0.75f);
}

// ── Boundary values ───────────────────────────────────────────────────────────

TEST(HSVTest, BoundaryValuesMin) {
  hsv_float_t hsv(0.0f, 0.0f, 0.0f);
  EXPECT_FLOAT_EQ(hsv.h(), 0.0f);
  EXPECT_FLOAT_EQ(hsv.s(), 0.0f);
  EXPECT_FLOAT_EQ(hsv.v(), 0.0f);
}

TEST(HSVTest, BoundaryValuesMax) {
  hsv_float_t hsv(360.0f, 1.0f, 1.0f);
  EXPECT_FLOAT_EQ(hsv.h(), 360.0f);
  EXPECT_FLOAT_EQ(hsv.s(), 1.0f);
  EXPECT_FLOAT_EQ(hsv.v(), 1.0f);
}

TEST(HSVTest, OutOfRangeThrows) {
  EXPECT_THROW(hsv_float_t(-1.0f, 0.5f, 0.5f), std::out_of_range);
  EXPECT_THROW(hsv_float_t(361.0f, 0.5f, 0.5f), std::out_of_range);
  EXPECT_THROW(hsv_float_t(180.0f, -0.1f, 0.5f), std::out_of_range);
  EXPECT_THROW(hsv_float_t(180.0f, 1.1f, 0.5f), std::out_of_range);
  EXPECT_THROW(hsv_float_t(180.0f, 0.5f, -0.1f), std::out_of_range);
  EXPECT_THROW(hsv_float_t(180.0f, 0.5f, 1.1f), std::out_of_range);
}

// ── Member and indexed access ─────────────────────────────────────────────────

TEST(HSVTest, MutableMemberAccess) {
  hsv_float_t hsv(180.0f, 0.5f, 0.5f);
  hsv.h() = 90.0f;
  hsv.s() = 0.25f;
  hsv.v() = 0.75f;
  EXPECT_FLOAT_EQ(hsv.h(), 90.0f);
  EXPECT_FLOAT_EQ(hsv.s(), 0.25f);
  EXPECT_FLOAT_EQ(hsv.v(), 0.75f);
}

TEST(HSVTest, IndexAccess) {
  hsv_float_t hsv(120.0f, 0.6f, 0.4f);
  EXPECT_FLOAT_EQ(hsv.get_index<0>(), 120.0f);
  EXPECT_FLOAT_EQ(hsv.get_index<1>(), 0.6f);
  EXPECT_FLOAT_EQ(hsv.get_index<2>(), 0.4f);
}

TEST(HSVTest, ConstCorrectness) {
  const hsv_float_t hsv(30.0f, 0.7f, 0.3f);
  EXPECT_FLOAT_EQ(hsv.h(), 30.0f);
  EXPECT_FLOAT_EQ(hsv.s(), 0.7f);
  EXPECT_FLOAT_EQ(hsv.v(), 0.3f);
}

// ── Known color conversions ───────────────────────────────────────────────────

TEST(HSVConversionTest, BlackRgbToHsv) {
  auto hsv = color_cast<hsv_float_t>(rgbf_t{0.0f, 0.0f, 0.0f});
  EXPECT_NEAR(hsv.s(), 0.0f, 1e-4f);
  EXPECT_NEAR(hsv.v(), 0.0f, 1e-4f);
}

TEST(HSVConversionTest, WhiteRgbToHsv) {
  auto hsv = color_cast<hsv_float_t>(rgbf_t{1.0f, 1.0f, 1.0f});
  EXPECT_NEAR(hsv.s(), 0.0f, 1e-4f);
  EXPECT_NEAR(hsv.v(), 1.0f, 1e-4f);
}

TEST(HSVConversionTest, RedRgbToHsv) {
  auto hsv = color_cast<hsv_float_t>(rgbf_t{1.0f, 0.0f, 0.0f});
  // Red: h=0°, s=1, v=1
  EXPECT_NEAR(hsv.s(), 1.0f, 1e-4f);
  EXPECT_NEAR(hsv.v(), 1.0f, 1e-4f);
}

TEST(HSVConversionTest, GreenRgbToHsv) {
  auto hsv = color_cast<hsv_float_t>(rgbf_t{0.0f, 1.0f, 0.0f});
  EXPECT_NEAR(hsv.h(), 120.0f, 1e-3f);
  EXPECT_NEAR(hsv.s(), 1.0f, 1e-4f);
  EXPECT_NEAR(hsv.v(), 1.0f, 1e-4f);
}

TEST(HSVConversionTest, BlueRgbToHsv) {
  auto hsv = color_cast<hsv_float_t>(rgbf_t{0.0f, 0.0f, 1.0f});
  EXPECT_NEAR(hsv.h(), 240.0f, 1e-3f);
  EXPECT_NEAR(hsv.s(), 1.0f, 1e-4f);
  EXPECT_NEAR(hsv.v(), 1.0f, 1e-4f);
}

TEST(HSVConversionTest, YellowRgbToHsv) {
  auto hsv = color_cast<hsv_float_t>(rgbf_t{1.0f, 1.0f, 0.0f});
  EXPECT_NEAR(hsv.h(), 60.0f, 1e-3f);
  EXPECT_NEAR(hsv.s(), 1.0f, 1e-4f);
  EXPECT_NEAR(hsv.v(), 1.0f, 1e-4f);
}

TEST(HSVConversionTest, CyanRgbToHsv) {
  auto hsv = color_cast<hsv_float_t>(rgbf_t{0.0f, 1.0f, 1.0f});
  EXPECT_NEAR(hsv.h(), 180.0f, 1e-3f);
  EXPECT_NEAR(hsv.s(), 1.0f, 1e-4f);
  EXPECT_NEAR(hsv.v(), 1.0f, 1e-4f);
}

TEST(HSVConversionTest, MagentaRgbToHsv) {
  auto hsv = color_cast<hsv_float_t>(rgbf_t{1.0f, 0.0f, 1.0f});
  EXPECT_NEAR(hsv.h(), 300.0f, 1e-3f);
  EXPECT_NEAR(hsv.s(), 1.0f, 1e-4f);
  EXPECT_NEAR(hsv.v(), 1.0f, 1e-4f);
}

// ── HSV vs HSL: same hue, different value semantics ──────────────────────────

TEST(HSVConversionTest, DarkColorValueVsLightness) {
  // A dark red: RGB (0.5, 0, 0)
  // HSL: h=0, s=1, l=0.25   (lightness = midpoint)
  // HSV: h=0, s=1, v=0.5    (value = max channel)
  rgbf_t dark_red(0.5f, 0.0f, 0.0f);
  auto hsv = color_cast<hsv_float_t>(dark_red);
  EXPECT_NEAR(hsv.s(), 1.0f, 1e-4f);
  EXPECT_NEAR(hsv.v(), 0.5f, 1e-4f);
}

// ── Round-trip ────────────────────────────────────────────────────────────────

TEST(HSVConversionTest, FloatRoundTrip) {
  rgbf_t orig(0.6f, 0.2f, 0.8f);
  auto back = color_cast<rgbf_t>(color_cast<hsv_float_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

TEST(HSVConversionTest, Uint8RoundTrip) {
  rgb8_t orig(180, 60, 90);
  auto back = color_cast<rgb8_t>(color_cast<hsv_float_t>(orig));
  EXPECT_EQ(back.r(), orig.r());
  EXPECT_EQ(back.g(), orig.g());
  EXPECT_EQ(back.b(), orig.b());
}

TEST(HSVAConversionTest, AlphaPreservedRoundTrip) {
  rgbaf_t orig(0.5f, 0.25f, 0.75f, 0.6f);
  auto back = color_cast<rgbaf_t>(color_cast<hsva_float_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
  EXPECT_NEAR(back.a(), orig.a(), 1e-4f);
}

// ── Achromatic invariant ──────────────────────────────────────────────────────

TEST(HSVConversionTest, GrayHasSaturationZero) {
  for (uint8_t v : {0u, 64u, 128u, 192u, 255u}) {
    rgb8_t gray(v, v, v);
    auto hsv = color_cast<hsv_float_t>(gray);
    EXPECT_NEAR(hsv.s(), 0.0f, 1e-4f) << "v=" << (int)v;
  }
}

TEST(HSVConversionTest, GrayValueMatchesChannel) {
  // v = max channel = v/255 for gray
  rgb8_t gray128(128, 128, 128);
  auto hsv = color_cast<hsv_float_t>(gray128);
  EXPECT_NEAR(hsv.v(), 128.0f / 255.0f, 1e-3f);
}

// ── Type traits ───────────────────────────────────────────────────────────────

TEST(HSVTest, TypeTraits) {
  using hsv_channels = typename traits::model_traits<hsv::model::hsv>::channels_type;
  static_assert(std::tuple_size_v<hsv_channels> == 3);

  using h_ch = std::tuple_element_t<0, hsv_channels>;
  static_assert(std::is_same_v<h_ch::value_type, float>);
  static_assert(h_ch::min == 0.0f);
  static_assert(h_ch::max == 360.0f);

  using hsva_channels = typename traits::model_traits<hsv::model::hsva>::channels_type;
  static_assert(std::tuple_size_v<hsva_channels> == 4);

  static_assert(!traits::has_channel_tag_v<hsv::model::hsv, hsv::channel::a_tag>);
  static_assert(traits::has_channel_tag_v<hsv::model::hsva, hsv::channel::a_tag>);
}

}  // namespace colorcpp::core::test
