/**
 * @file test_serialization.cpp
 * @brief Unit tests for serialization: traits, options.
 *
 * Note: JSON/MessagePack adapter tests require a concrete adapter specialization.
 * Here we test the traits and the non-adapter-dependent parts.
 */

#include <gtest/gtest.h>

#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/io/serialization.hpp>

using namespace colorcpp;
using namespace colorcpp::io::serialization;

// ===== Traits Tests =====

TEST(SerializationTraits, FormatEnum) {
  EXPECT_EQ(static_cast<int>(serialization_format::compact), 0);
  EXPECT_EQ(static_cast<int>(serialization_format::named), 1);
}

TEST(SerializationTraits, ColorSpaceNames) {
  EXPECT_EQ(color_space_name(color_space_id::rgb8), "rgb8");
  EXPECT_EQ(color_space_name(color_space_id::rgba8), "rgba8");
  EXPECT_EQ(color_space_name(color_space_id::rgbf), "rgbf");
  EXPECT_EQ(color_space_name(color_space_id::oklab), "oklab");
  EXPECT_EQ(color_space_name(color_space_id::oklch), "oklch");
  EXPECT_EQ(color_space_name(color_space_id::cielab), "cielab");
  EXPECT_EQ(color_space_name(color_space_id::display_p3), "display_p3");
  EXPECT_EQ(color_space_name(color_space_id::xyz), "xyz");
}

TEST(SerializationTraits, DefaultOptions) {
  serialization_options opts;
  EXPECT_EQ(opts.format, serialization_format::compact);
  EXPECT_FALSE(opts.include_color_space);
}

// ===== Details Tests =====

TEST(SerializationDetails, ToDoubleUint8) {
  using namespace colorcpp::io::serialization::details;
  EXPECT_DOUBLE_EQ(to_double(static_cast<uint8_t>(0)), 0.0);
  EXPECT_DOUBLE_EQ(to_double(static_cast<uint8_t>(255)), 1.0);
  EXPECT_DOUBLE_EQ(to_double(static_cast<uint8_t>(128)), 128.0 / 255.0);
}

TEST(SerializationDetails, FromDoubleUint8) {
  using namespace colorcpp::io::serialization::details;
  EXPECT_EQ(from_double<uint8_t>(0.0), 0);
  EXPECT_EQ(from_double<uint8_t>(1.0), 255);
  EXPECT_EQ(from_double<uint8_t>(0.5), 128);
}

TEST(SerializationDetails, FromDoubleFloat) {
  using namespace colorcpp::io::serialization::details;
  EXPECT_FLOAT_EQ(from_double<float>(0.5f), 0.5f);
  EXPECT_FLOAT_EQ(from_double<float>(1.0f), 1.0f);
}

TEST(SerializationDetails, DefaultChannelNames) {
  using namespace colorcpp::io::serialization::details;
  EXPECT_EQ(default_channel_name(0), "ch0");
  EXPECT_EQ(default_channel_name(1), "ch1");
  EXPECT_EQ(default_channel_name(2), "ch2");
}
