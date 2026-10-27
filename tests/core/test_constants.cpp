/**
 * @file test_constants.cpp
 * @brief Named color constants keep their documented values, rgba8_t type, and
 *        core layering (constants.hpp must be usable without the io layer).
 */

#include <gtest/gtest.h>

#include <colorcpp/core/constants.hpp>
#include <type_traits>

namespace colorcpp::constants::test {

static_assert(std::is_same_v<std::decay_t<decltype(black)>, core::rgba8_t>);
static_assert(std::is_same_v<std::decay_t<decltype(tomato)>, core::rgba8_t>);

TEST(ConstantsTest, BasicColorsMatchW3CHex) {
  EXPECT_EQ(black.r(), 0);
  EXPECT_EQ(black.g(), 0);
  EXPECT_EQ(black.b(), 0);
  EXPECT_EQ(black.a(), 255);  // all constants are full opacity

  EXPECT_EQ(white.r(), 255);
  EXPECT_EQ(white.g(), 255);
  EXPECT_EQ(white.b(), 255);

  EXPECT_EQ(red.r(), 255);
  EXPECT_EQ(red.g(), 0);
  EXPECT_EQ(red.b(), 0);

  EXPECT_EQ(lime.r(), 0);
  EXPECT_EQ(lime.g(), 255);
  EXPECT_EQ(lime.b(), 0);

  EXPECT_EQ(blue.r(), 0);
  EXPECT_EQ(blue.g(), 0);
  EXPECT_EQ(blue.b(), 255);
}

TEST(ConstantsTest, ChannelBytesMapFromHexWithoutShiftErrors) {
  // Each byte of the 0xRRGGBB pattern must land in its own channel.
  EXPECT_EQ(tomato.r(), 255);  // 0xFF6347
  EXPECT_EQ(tomato.g(), 99);
  EXPECT_EQ(tomato.b(), 71);
  EXPECT_EQ(tomato.a(), 255);

  EXPECT_EQ(steelblue.r(), 70);  // 0x4682B4
  EXPECT_EQ(steelblue.g(), 130);
  EXPECT_EQ(steelblue.b(), 180);

  EXPECT_EQ(goldenrod.r(), 218);  // 0xDAA520
  EXPECT_EQ(goldenrod.g(), 165);
  EXPECT_EQ(goldenrod.b(), 32);
}

TEST(ConstantsTest, AliasesShareTheSameValue) {
  EXPECT_EQ(aqua.r(), cyan.r());
  EXPECT_EQ(aqua.g(), cyan.g());
  EXPECT_EQ(aqua.b(), cyan.b());

  EXPECT_EQ(fuchsia.r(), magenta.r());
  EXPECT_EQ(fuchsia.g(), magenta.g());
  EXPECT_EQ(fuchsia.b(), magenta.b());

  EXPECT_EQ(grey.r(), gray.r());
  EXPECT_EQ(grey.g(), gray.g());
  EXPECT_EQ(grey.b(), gray.b());

  EXPECT_EQ(dimgrey.r(), dimgray.r());
  EXPECT_EQ(dimgrey.g(), dimgray.g());
  EXPECT_EQ(dimgrey.b(), dimgray.b());
}

TEST(ConstantsTest, ConstantsAreUsableInConstantExpressions) {
  static_assert(black.r() == 0 && black.a() == 255);
  static_assert(tomato.r() == 255 && tomato.g() == 99 && tomato.b() == 71);
  SUCCEED();
}

}  // namespace colorcpp::constants::test
