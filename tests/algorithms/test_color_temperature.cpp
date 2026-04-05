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
