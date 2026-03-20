
#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

namespace colorcpp::operations::test {

using namespace core;
using namespace conversion;

// Identity cast

TEST(ConversionTest, IdentityCast) {
  rgbf_t c(0.4f, 0.6f, 0.8f);
  auto copy = color_cast<rgbf_t>(c);
  EXPECT_FLOAT_EQ(copy.r(), c.r());
  EXPECT_FLOAT_EQ(copy.g(), c.g());
  EXPECT_FLOAT_EQ(copy.b(), c.b());
}

// sRGB ↔ Linear sRGB

TEST(ConversionTest, LinearizeBlackAndWhite) {
  auto lin_black = color_cast<linear_rgbf_t>(rgbf_t{0.0f, 0.0f, 0.0f});
  EXPECT_NEAR(lin_black.r(), 0.0f, 1e-5f);
  EXPECT_NEAR(lin_black.g(), 0.0f, 1e-5f);
  EXPECT_NEAR(lin_black.b(), 0.0f, 1e-5f);

  auto lin_white = color_cast<linear_rgbf_t>(rgbf_t{1.0f, 1.0f, 1.0f});
  EXPECT_NEAR(lin_white.r(), 1.0f, 1e-4f);
  EXPECT_NEAR(lin_white.g(), 1.0f, 1e-4f);
  EXPECT_NEAR(lin_white.b(), 1.0f, 1e-4f);
}

TEST(ConversionTest, LinearizeMidGrayIsNonLinear) {
  // sRGB 0.5 linearizes to ≈0.214, not 0.5
  auto lin = color_cast<linear_rgbf_t>(rgbf_t{0.5f, 0.5f, 0.5f});
  EXPECT_NEAR(lin.r(), 0.2140f, 1e-3f);
}

TEST(ConversionTest, SrgbLinearRoundTrip) {
  rgbf_t orig(0.3f, 0.6f, 0.9f);
  auto back = color_cast<rgbf_t>(color_cast<linear_rgbf_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

// sRGB ↔ HSL

TEST(ConversionTest, SrgbHslRoundTrip) {
  rgbf_t orig(0.7f, 0.3f, 0.5f);
  auto back = color_cast<rgbf_t>(color_cast<hsl_float_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

TEST(ConversionTest, SrgbHsvRoundTrip) {
  rgbf_t orig(0.2f, 0.8f, 0.4f);
  auto back = color_cast<rgbf_t>(color_cast<hsv_float_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

// sRGB ↔ OKLab

TEST(ConversionTest, BlackToOklab) {
  auto lab = color_cast<oklab_t>(rgbf_t{0.0f, 0.0f, 0.0f});
  EXPECT_NEAR(lab.l(), 0.0f, 1e-4f);
  EXPECT_NEAR(lab.a(), 0.0f, 1e-4f);
  EXPECT_NEAR(lab.b(), 0.0f, 1e-4f);
}

TEST(ConversionTest, WhiteToOklab) {
  auto lab = color_cast<oklab_t>(rgbf_t{1.0f, 1.0f, 1.0f});
  EXPECT_NEAR(lab.l(), 1.0f, 1e-4f);
  EXPECT_NEAR(lab.a(), 0.0f, 1e-4f);
  EXPECT_NEAR(lab.b(), 0.0f, 1e-4f);
}

TEST(ConversionTest, SrgbOklabRoundTrip) {
  rgbf_t orig(0.5f, 0.2f, 0.8f);
  auto back = color_cast<rgbf_t>(color_cast<oklab_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-3f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-3f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-3f);
}

// sRGB ↔ CIELAB

TEST(ConversionTest, BlackToCielab) {
  auto lab = color_cast<cielab_t>(rgbf_t{0.0f, 0.0f, 0.0f});
  EXPECT_NEAR(lab.l(), 0.0f, 1e-3f);
}

TEST(ConversionTest, WhiteToCielab) {
  auto lab = color_cast<cielab_t>(rgbf_t{1.0f, 1.0f, 1.0f});
  EXPECT_NEAR(lab.l(), 100.0f, 1e-2f);
  EXPECT_NEAR(lab.a(), 0.0f, 1e-2f);
  EXPECT_NEAR(lab.b(), 0.0f, 1e-2f);
}

TEST(ConversionTest, SrgbCielabRoundTrip) {
  rgbf_t orig(0.6f, 0.4f, 0.2f);
  auto back = color_cast<rgbf_t>(color_cast<cielab_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-3f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-3f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-3f);
}

// OKLab ↔ OKLCh

TEST(ConversionTest, OklabOklchRoundTrip) {
  rgbf_t orig(0.8f, 0.3f, 0.1f);
  auto oklch = color_cast<oklch_t>(orig);
  auto back = color_cast<rgbf_t>(oklch);
  EXPECT_NEAR(back.r(), orig.r(), 1e-3f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-3f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-3f);
}

// Alpha handling

TEST(ConversionTest, AlphaPreservedThroughHslRoundTrip) {
  rgbaf_t orig(0.5f, 0.25f, 0.75f, 0.5f);
  auto back = color_cast<rgbaf_t>(color_cast<hsla_float_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.a(), orig.a(), 1e-4f);
}

// uint8 ↔ float precision

TEST(ConversionTest, Rgb8ToRgbfKnownValues) {
  auto f = color_cast<rgbf_t>(rgb8_t{0, 128, 255});
  EXPECT_NEAR(f.r(), 0.0f, 1e-4f);
  EXPECT_NEAR(f.g(), 128.0f / 255.0f, 1e-4f);
  EXPECT_NEAR(f.b(), 1.0f, 1e-4f);
}

TEST(ConversionTest, Rgb8RoundTrip) {
  rgb8_t orig(100, 150, 200);
  auto back = color_cast<rgb8_t>(color_cast<rgbf_t>(orig));
  EXPECT_EQ(back.r(), orig.r());
  EXPECT_EQ(back.g(), orig.g());
  EXPECT_EQ(back.b(), orig.b());
}

// Multi-hop conversions

TEST(ConversionTest, HslToOklabMultiHop) {
  hsl_float_t hsl(180.0f, 0.5f, 0.5f);
  auto lab = color_cast<oklab_t>(hsl);
  EXPECT_GE(lab.l(), 0.0f);
  EXPECT_LE(lab.l(), 1.0f);
}

TEST(ConversionTest, CielabToOklabViaXyz) {
  cielab_t lab(50.0f, 20.0f, -30.0f);
  auto oklab = color_cast<oklab_t>(lab);
  auto back = color_cast<cielab_t>(oklab);
  EXPECT_NEAR(back.l(), lab.l(), 0.5f);
}

}  // namespace colorcpp::operations::test
