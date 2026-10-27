
#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

namespace colorcpp::io::test {

using namespace colorcpp::io::literals;
using namespace colorcpp;

// _rgb

TEST(RgbLiteralTest, TomatoColor) {
  constexpr auto c = 0xFF6347_rgb;
  static_assert(c.r() == 255);
  static_assert(c.g() == 99);
  static_assert(c.b() == 71);
  static_assert(c.a() == 255);
}

TEST(RgbLiteralTest, Black) {
  constexpr auto c = 0x000000_rgb;
  static_assert(c.r() == 0 && c.g() == 0 && c.b() == 0);
  static_assert(c.a() == 255);
}

TEST(RgbLiteralTest, White) {
  constexpr auto c = 0xFFFFFF_rgb;
  static_assert(c.r() == 255 && c.g() == 255 && c.b() == 255);
}

TEST(RgbLiteralTest, PureRed) {
  constexpr auto c = 0xFF0000_rgb;
  static_assert(c.r() == 255 && c.g() == 0 && c.b() == 0);
}

TEST(RgbLiteralTest, PureGreen) {
  constexpr auto c = 0x00FF00_rgb;
  static_assert(c.r() == 0 && c.g() == 255 && c.b() == 0);
}

TEST(RgbLiteralTest, PureBlue) {
  constexpr auto c = 0x0000FF_rgb;
  static_assert(c.r() == 0 && c.g() == 0 && c.b() == 255);
}

// Wrong-digit rejection (formerly RuntimeOverflowThrows) is now a compile-time
// guarantee, pinned by the negative-compilation tests in tests/compile_fail/.

// _rgba

TEST(RgbaLiteralTest, WithAlpha128) {
  constexpr auto c = 0xFF634780_rgba;
  static_assert(c.r() == 255 && c.g() == 99 && c.b() == 71);
  static_assert(c.a() == 128);
}

TEST(RgbaLiteralTest, FullyOpaque) {
  constexpr auto c = 0xFF6347FF_rgba;
  static_assert(c.a() == 255);
}

TEST(RgbaLiteralTest, FullyTransparent) {
  constexpr auto c = 0xFF634700_rgba;
  static_assert(c.a() == 0);
}

TEST(RgbaLiteralTest, White) {
  constexpr auto c = 0xFFFFFFFF_rgba;
  static_assert(c.r() == 255 && c.g() == 255 && c.b() == 255 && c.a() == 255);
}

// _argb

TEST(ArgbLiteralTest, SameAlphaAsRgba) {
  // 0xFF634780_rgba and 0x80FF6347_argb should produce the same color
  constexpr auto by_rgba = 0xFF634780_rgba;
  constexpr auto by_argb = 0x80FF6347_argb;
  static_assert(by_rgba.r() == by_argb.r());
  static_assert(by_rgba.g() == by_argb.g());
  static_assert(by_rgba.b() == by_argb.b());
  static_assert(by_rgba.a() == by_argb.a());
}

TEST(ArgbLiteralTest, AlphaInHighByte) {
  constexpr auto c = 0x40AABBCC_argb;
  static_assert(c.r() == 0xAA);
  static_assert(c.g() == 0xBB);
  static_assert(c.b() == 0xCC);
  static_assert(c.a() == 0x40);
}

TEST(ArgbLiteralTest, FullyTransparentBlack) {
  constexpr auto c = 0x00000000_argb;
  static_assert(c.r() == 0 && c.g() == 0 && c.b() == 0 && c.a() == 0);
}

// _hex (string literal)

TEST(HexLiteralTest, FullForm_RRGGBB) {
  auto c = "#FF6347"_hex;
  EXPECT_EQ(c.r(), 255);
  EXPECT_EQ(c.g(), 99);
  EXPECT_EQ(c.b(), 71);
  EXPECT_EQ(c.a(), 255);
}

TEST(HexLiteralTest, FullForm_RRGGBBAA) {
  auto c = "#FF634780"_hex;
  EXPECT_EQ(c.r(), 255);
  EXPECT_EQ(c.g(), 99);
  EXPECT_EQ(c.b(), 71);
  EXPECT_EQ(c.a(), 128);
}

TEST(HexLiteralTest, ShortForm_RGB) {
  // #F63 → #FF6633
  auto c = "#F63"_hex;
  EXPECT_EQ(c.r(), 0xFF);
  EXPECT_EQ(c.g(), 0x66);
  EXPECT_EQ(c.b(), 0x33);
  EXPECT_EQ(c.a(), 255);
}

TEST(HexLiteralTest, ShortForm_RGBA) {
  // #F63A → #FF6633AA
  auto c = "#F63A"_hex;
  EXPECT_EQ(c.r(), 0xFF);
  EXPECT_EQ(c.g(), 0x66);
  EXPECT_EQ(c.b(), 0x33);
  EXPECT_EQ(c.a(), 0xAA);
}

TEST(HexLiteralTest, Black) {
  auto c = "#000000"_hex;
  EXPECT_EQ(c.r(), 0) << "black r";
  EXPECT_EQ(c.g(), 0) << "black g";
  EXPECT_EQ(c.b(), 0) << "black b";
}

TEST(HexLiteralTest, White) {
  auto c = "#FFF"_hex;
  EXPECT_EQ(c.r(), 255);
  EXPECT_EQ(c.g(), 255);
  EXPECT_EQ(c.b(), 255);
}

TEST(HexLiteralTest, InvalidLengthThrows) {
  EXPECT_THROW((void)"#FF635"_hex, std::invalid_argument);    // 5 hex digits (not 3/4/6/8)
  EXPECT_THROW((void)"#FF63471"_hex, std::invalid_argument);  // 7 hex digits (not 3/4/6/8)
}

TEST(HexLiteralTest, InvalidHexDigitsThrow) {
  EXPECT_THROW((void)"#GG0000"_hex, std::invalid_argument);
  EXPECT_THROW((void)"#12X"_hex, std::invalid_argument);
  EXPECT_THROW((void)"#F63Z"_hex, std::invalid_argument);
}



// Constexpr evaluation

TEST(LiteralConstexprTest, AllLiteralsAreConstexpr) {
  // Verify the compiler evaluates these at compile time
  static_assert((0xFF0000_rgb).r() == 255);
  static_assert((0xFF000080_rgba).a() == 128);
  static_assert((0x80FF0000_argb).a() == 128);
}

// _rgb vs _hex consistency

TEST(LiteralConsistencyTest, RgbAndHexProduceSameColor) {
  constexpr auto by_rgb = 0xFF6347_rgb;
  auto by_hex = "#FF6347"_hex;
  EXPECT_EQ(by_rgb.r(), by_hex.r());
  EXPECT_EQ(by_rgb.g(), by_hex.g());
  EXPECT_EQ(by_rgb.b(), by_hex.b());
  EXPECT_EQ(by_rgb.a(), by_hex.a());
}

TEST(LiteralConsistencyTest, RgbaAndArgbSameColor) {
  constexpr auto by_rgba = 0xABCDEF40_rgba;
  constexpr auto by_argb = 0x40ABCDEF_argb;
  static_assert(by_rgba.r() == by_argb.r());
  static_assert(by_rgba.g() == by_argb.g());
  static_assert(by_rgba.b() == by_argb.b());
  static_assert(by_rgba.a() == by_argb.a());
}

}  // namespace colorcpp::io::test
