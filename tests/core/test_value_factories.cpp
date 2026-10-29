/**
 * @file test_value_factories.cpp
 * @brief clamped()/make_checked() — opt-in boundary policy for value-type colors.
 */

#include <gtest/gtest.h>

#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/rgb.hpp>
#include <limits>
#include <stdexcept>

namespace colorcpp::core::test {

// Colors never validate at construction; the factories are the opt-in boundary.

TEST(ValueFactoryTest, ClampedWrapsChannels) {
  const rgbf_t c = clamped<rgbf_t>(2.0f, -1.0f, 0.5f);
  EXPECT_FLOAT_EQ(c.r(), 1.0f);
  EXPECT_FLOAT_EQ(c.g(), 0.0f);
  EXPECT_FLOAT_EQ(c.b(), 0.5f);
}

TEST(ValueFactoryTest, ClampedRoundsForIntegralChannels) {
  const rgb8_t c = clamped<rgb8_t>(300, -5, 200.7f);
  EXPECT_EQ(c.r(), 255);
  EXPECT_EQ(c.g(), 0);
  EXPECT_EQ(c.b(), 201);
}

TEST(ValueFactoryTest, ClampedOnFamilyTypes) {
  const hsl_float_t h = clamped<hsl_float_t>(400.0f, 0.5f, 1.5f);
  EXPECT_FLOAT_EQ(h.h(), 360.0f);
  EXPECT_FLOAT_EQ(h.s(), 0.5f);
  EXPECT_FLOAT_EQ(h.l(), 1.0f);
}

TEST(ValueFactoryTest, ClampedNaNBecomesZero) {
  // clamp passes NaN through; casting NaN to uint8_t is UB. clamped() must
  // define the mapping (NaN → 0) instead.
  const rgb8_t c = clamped<rgb8_t>(std::numeric_limits<float>::quiet_NaN(), 128.0f, 200.7f);
  EXPECT_EQ(c.r(), 0);
  EXPECT_EQ(c.g(), 128);
  EXPECT_EQ(c.b(), 201);
}

TEST(ValueFactoryTest, MakeCheckedAcceptsInNominalRange) {
  EXPECT_NO_THROW(make_checked<rgbf_t>(0.5f, 0.5f, 0.5f));
  EXPECT_NO_THROW(make_checked<rgb8_t>(0, 128, 255));
  const rgbf_t c = make_checked<rgbf_t>(0.25f, 0.5f, 0.75f);
  EXPECT_FLOAT_EQ(c.b(), 0.75f);
}

TEST(ValueFactoryTest, MakeCheckedThrowsOutsideNominalRange) {
  EXPECT_THROW(make_checked<rgbf_t>(1.5f, 0.5f, 0.5f), std::out_of_range);
  EXPECT_THROW(make_checked<rgbf_t>(0.5f, -0.1f, 0.5f), std::out_of_range);
  EXPECT_THROW(make_checked<rgb8_t>(300, 0, 0), std::out_of_range);  // caught before narrowing
}

}  // namespace colorcpp::core::test
