/**
 * @file test_color_temperature.cpp
 * @brief Unit tests for Kelvin color temperature to RGB conversion.
 */

#include <gtest/gtest.h>

#include <colorcpp/algorithms/color_temperature.hpp>

using namespace colorcpp::algorithms::color_temperature;

// ===== kelvin_to_rgba8 Tests =====

TEST(ColorTemperature, D65_WhitePoint) {
  // 6500K is approximately D65, should be close to white
  auto c = kelvin_to_rgba8(6500.0f);
  EXPECT_GT(c.r(), 240);
  EXPECT_GT(c.g(), 240);
  EXPECT_GT(c.b(), 240);
}

TEST(ColorTemperature, WarmLight_2700K) {
  // Warm white (tungsten bulb) should be reddish/yellowish
  auto c = kelvin_to_rgba8(2700.0f);
  EXPECT_GT(c.r(), c.b());  // More red than blue
  EXPECT_GT(c.g(), c.b());  // More green than blue
}

TEST(ColorTemperature, CoolBlue_10000K) {
  // Very cool light should be bluish
  auto c = kelvin_to_rgba8(10000.0f);
  EXPECT_GT(c.b(), c.r());  // More blue than red
}

TEST(ColorTemperature, Candle_1850K) {
  // Candle light is very warm (deep orange/red)
  auto c = kelvin_to_rgba8(1850.0f);
  EXPECT_GT(c.r(), c.b());
  EXPECT_GT(c.r(), 100);
}

TEST(ColorTemperature, OvercastSky_7500K) {
  // Slightly cool
  auto c = kelvin_to_rgba8(7500.0f);
  EXPECT_GT(c.r(), 200);
  EXPECT_GT(c.g(), 200);
  EXPECT_GT(c.b(), 200);
}

TEST(ColorTemperature, Boundary_1000K) {
  // Minimum valid temperature
  auto c = kelvin_to_rgba8(1000.0f);
  EXPECT_GT(c.r(), c.b());
  EXPECT_GT(c.r(), c.g());
}

TEST(ColorTemperature, Boundary_40000K) {
  // Maximum valid temperature, very blue
  auto c = kelvin_to_rgba8(40000.0f);
  EXPECT_GT(c.b(), c.r());
}

TEST(ColorTemperature, AlphaDefault) {
  auto c = kelvin_to_rgba8(6500.0f);
  EXPECT_EQ(c.a(), 255);
}

TEST(ColorTemperature, AlphaCustom) {
  auto c = kelvin_to_rgba8(6500.0f, 128);
  EXPECT_EQ(c.a(), 128);
}

// ===== kelvin_to_rgb Tests =====

TEST(ColorTemperature, RgbInRange) {
  for (float k = 1000.0f; k <= 40000.0f; k += 1000.0f) {
    auto c = kelvin_to_rgb(k);
    EXPECT_GE(c.r(), 0.0f) << "R out of range at " << k << "K";
    EXPECT_LE(c.r(), 1.0f) << "R out of range at " << k << "K";
    EXPECT_GE(c.g(), 0.0f) << "G out of range at " << k << "K";
    EXPECT_LE(c.g(), 1.0f) << "G out of range at " << k << "K";
    EXPECT_GE(c.b(), 0.0f) << "B out of range at " << k << "K";
    EXPECT_LE(c.b(), 1.0f) << "B out of range at " << k << "K";
  }
}

// ===== kelvin_to_linear_rgb Tests =====

TEST(ColorTemperature, LinearRgbInRange) {
  for (float k = 1000.0f; k <= 40000.0f; k += 1000.0f) {
    auto c = kelvin_to_linear_rgb(k);
    EXPECT_GE(c.r(), 0.0f) << "R out of range at " << k << "K";
    EXPECT_LE(c.r(), 1.0f) << "R out of range at " << k << "K";
    EXPECT_GE(c.g(), 0.0f) << "G out of range at " << k << "K";
    EXPECT_LE(c.g(), 1.0f) << "G out of range at " << k << "K";
    EXPECT_GE(c.b(), 0.0f) << "B out of range at " << k << "K";
    EXPECT_LE(c.b(), 1.0f) << "B out of range at " << k << "K";
  }
}

// ===== Temperature trend tests =====

TEST(ColorTemperature, WarmToCoolTrend) {
  // As temperature increases, blue should increase and red should decrease
  auto warm = kelvin_to_rgba8(2000.0f);
  auto cool = kelvin_to_rgba8(10000.0f);
  EXPECT_GT(warm.r(), cool.r());
  EXPECT_GT(cool.b(), warm.b());
}

TEST(ColorTemperature, MonotonicRed) {
  // Red channel should decrease as temperature increases
  auto t1 = kelvin_to_rgba8(1000.0f);
  auto t2 = kelvin_to_rgba8(3000.0f);
  auto t3 = kelvin_to_rgba8(6500.0f);
  auto t4 = kelvin_to_rgba8(20000.0f);
  EXPECT_GE(t1.r(), t2.r());
  EXPECT_GE(t2.r(), t3.r());
  EXPECT_GE(t3.r(), t4.r());
}

TEST(ColorTemperature, MonotonicBlue) {
  // Blue channel should increase as temperature increases
  auto t1 = kelvin_to_rgba8(1000.0f);
  auto t2 = kelvin_to_rgba8(3000.0f);
  auto t3 = kelvin_to_rgba8(6500.0f);
  auto t4 = kelvin_to_rgba8(20000.0f);
  EXPECT_LE(t1.b(), t2.b());
  EXPECT_LE(t2.b(), t3.b());
  EXPECT_LE(t3.b(), t4.b());
}

TEST(ColorTemperature, RgbToKelvinRoundTripRgb) {
  for (float k : {2700.0f, 4500.0f, 6500.0f, 9000.0f}) {
    auto rgb = kelvin_to_rgb(k);
    float k_est = rgb_to_kelvin_estimate(rgb);
    auto rgb2 = kelvin_to_rgb(k_est);
    EXPECT_NEAR(rgb.r(), rgb2.r(), 0.06f) << "K=" << k;
    EXPECT_NEAR(rgb.g(), rgb2.g(), 0.06f) << "K=" << k;
    EXPECT_NEAR(rgb.b(), rgb2.b(), 0.06f) << "K=" << k;
  }
}

TEST(ColorTemperature, LinearRgbToKelvinConsistent) {
  auto lin = kelvin_to_linear_rgb(5500.0f);
  float k_est = linear_rgb_to_kelvin_estimate(lin);
  EXPECT_GT(k_est, 4000.0f);
  EXPECT_LT(k_est, 7000.0f);
}

// ===== Issue #100: Helland's channel formulas already output sRGB-encoded
//     values; kelvin_to_linear_rgb only divided by 255 and kelvin_to_rgb
//     then applied linear_to_srgb again (double gamma). Outputs washed out
//     vs the reference (1000K: 255,141,0 instead of 255,68,0) and
//     rgb_to_kelvin_estimate was biased for reference-correct sRGB colors. =====

TEST(ColorTemperature, KelvinToRgba8MatchesHellandReference) {
  // Reference values from Tanner Helland's algorithm (already sRGB-encoded).
  auto c1000 = kelvin_to_rgba8(1000.0f);
  EXPECT_EQ(c1000.r(), 255) << "issue #100: double gamma washed out R";
  EXPECT_EQ(c1000.g(), 68) << "issue #100: 1000K G must be the Helland 68, not 141";
  EXPECT_EQ(c1000.b(), 0);
  auto c3000 = kelvin_to_rgba8(3000.0f);
  EXPECT_EQ(c3000.r(), 255);
  EXPECT_EQ(c3000.g(), 177) << "issue #100: 3000K G must be the Helland 177, not 217";
  EXPECT_EQ(c3000.b(), 110) << "issue #100: 3000K B must be the Helland 110, not 175";
}

TEST(ColorTemperature, HellandReferenceVectors) {
  // Full reference vector across the 1000-40000 K range. Values come from an
  // independent double-precision evaluation of Helland's published formulas
  // (Python), not from this implementation: they pin the post-#100 single
  // gamma decode and Helland's blue = 255 for temp >= 6600 K.
  struct Sample {
    float k;
    uint8_t r, g, b;
  };
  const Sample refs[] = {
      {1000.0f, 255, 68, 0},    {1500.0f, 255, 108, 0},   {2000.0f, 255, 137, 14},
      {3000.0f, 255, 177, 110}, {4000.0f, 255, 206, 166}, {5500.0f, 255, 237, 222},
      {6500.0f, 255, 254, 250}, {8000.0f, 221, 230, 255}, {10000.0f, 202, 218, 255},
      {20000.0f, 171, 198, 255}, {40000.0f, 152, 186, 255},
  };
  for (const auto& s : refs) {
    const auto c = kelvin_to_rgba8(s.k);
    EXPECT_EQ(c.r(), s.r) << "K=" << s.k;
    EXPECT_EQ(c.g(), s.g) << "K=" << s.k;
    EXPECT_EQ(c.b(), s.b) << "K=" << s.k;
  }
}

TEST(ColorTemperature, RgbToKelvinEstimateMatchesHellandCurve) {
  // An external reference-correct sRGB color must map back to its
  // temperature; the double-gamma build returned ~2111K for 3000K.
  const colorcpp::core::rgbf_t ref3000{1.0f, 177.0f / 255.0f, 110.0f / 255.0f};
  const float k = rgb_to_kelvin_estimate(ref3000);
  EXPECT_NEAR(k, 3000.0f, 30.0f) << "issue #100: reverse estimator biased by double gamma";
}

TEST(ColorTemperature, KelvinToLinearRgbIsDecodedToLinear) {
  // Helland 1000K G = 68/255 encoded; linear must be srgb_to_linear(68/255)
  // ~ 0.0577, not the raw 0.2664.
  const auto lin = kelvin_to_linear_rgb(1000.0f);
  EXPECT_NEAR(lin.g(), 0.0577f, 0.002f) << "issue #100: output must be linearized, not encoded";
  EXPECT_NEAR(lin.r(), 1.0f, 1e-6f);
  EXPECT_NEAR(lin.b(), 0.0f, 1e-6f);
}
