
#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>
#include <colorcpp/literals/literals.hpp>

namespace colorcpp::literals::test {

using namespace colorcpp::literals;
using namespace colorcpp;

// ── _rgb ──────────────────────────────────────────────────────────────────────

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

TEST(RgbLiteralTest, RuntimeOverflowThrows) {
  EXPECT_THROW((void)(0x1000000_rgb), std::out_of_range);
}

// ── _rgba ─────────────────────────────────────────────────────────────────────

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

// ── _argb ─────────────────────────────────────────────────────────────────────

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

// ── _hex (string literal) ─────────────────────────────────────────────────────

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
  EXPECT_THROW((void)"#FF635"_hex, std::invalid_argument);   // 5 hex digits (not 3/4/6/8)
  EXPECT_THROW((void)"#FF63471"_hex, std::invalid_argument); // 7 hex digits (not 3/4/6/8)
}

// ── _hsl ──────────────────────────────────────────────────────────────────────

TEST(HslLiteralTest, GreenHsl) {
  // H=120, S=50%, L=75%  →  stored as (120, 0.50, 0.75)
  constexpr auto hsl = 120'050'075_hsl;
  static_assert(hsl.h() == 120.0f);
  EXPECT_FLOAT_EQ(hsl.s(), 0.50f);
  EXPECT_FLOAT_EQ(hsl.l(), 0.75f);
}

TEST(HslLiteralTest, RedHsl) {
  // H=0, S=100%, L=50%  → fully saturated red
  constexpr auto hsl = 0'100'050_hsl;
  static_assert(hsl.h() == 0.0f);
  EXPECT_FLOAT_EQ(hsl.s(), 1.0f);
  EXPECT_FLOAT_EQ(hsl.l(), 0.5f);
}

TEST(HslLiteralTest, BlueHsl) {
  constexpr auto hsl = 240'100'050_hsl;
  static_assert(hsl.h() == 240.0f);
  EXPECT_FLOAT_EQ(hsl.s(), 1.0f);
  EXPECT_FLOAT_EQ(hsl.l(), 0.5f);
}

TEST(HslLiteralTest, MaxHue) {
  constexpr auto hsl = 360'000'050_hsl;
  static_assert(hsl.h() == 360.0f);
  EXPECT_FLOAT_EQ(hsl.s(), 0.0f);
}

TEST(HslLiteralTest, AllZero) {
  constexpr auto hsl = 0'000'000_hsl;
  static_assert(hsl.h() == 0.0f);
  EXPECT_FLOAT_EQ(hsl.s(), 0.0f);
  EXPECT_FLOAT_EQ(hsl.l(), 0.0f);
}

// ── _hsla ─────────────────────────────────────────────────────────────────────

TEST(HslaLiteralTest, WithAlpha) {
  // H=120, S=50%, L=75%, A=100% → (120, 0.5, 0.75, 1.0)
  constexpr auto hsla = 120'050'075'100_hsla;
  static_assert(hsla.h() == 120.0f);
  EXPECT_FLOAT_EQ(hsla.s(), 0.5f);
  EXPECT_FLOAT_EQ(hsla.l(), 0.75f);
  EXPECT_FLOAT_EQ(hsla.a(), 1.0f);
}

TEST(HslaLiteralTest, SemiTransparent) {
  constexpr auto hsla = 0'100'050'050_hsla;
  EXPECT_FLOAT_EQ(hsla.a(), 0.5f);
}

TEST(HslaLiteralTest, FullyTransparent) {
  constexpr auto hsla = 180'060'040'000_hsla;
  EXPECT_FLOAT_EQ(hsla.a(), 0.0f);
}

// ── _hsv ──────────────────────────────────────────────────────────────────────

TEST(HsvLiteralTest, NavyBlue) {
  // H=210, S=80%, V=90%  → stored as (210, 0.80, 0.90)
  constexpr auto hsv = 210'080'090_hsv;
  static_assert(hsv.h() == 210.0f);
  EXPECT_FLOAT_EQ(hsv.s(), 0.80f);
  EXPECT_FLOAT_EQ(hsv.v(), 0.90f);
}

TEST(HsvLiteralTest, FullRed) {
  constexpr auto hsv = 0'100'100_hsv;
  static_assert(hsv.h() == 0.0f);
  EXPECT_FLOAT_EQ(hsv.s(), 1.0f);
  EXPECT_FLOAT_EQ(hsv.v(), 1.0f);
}

TEST(HsvLiteralTest, Black) {
  constexpr auto hsv = 0'000'000_hsv;
  EXPECT_FLOAT_EQ(hsv.s(), 0.0f);
  EXPECT_FLOAT_EQ(hsv.v(), 0.0f);
}

TEST(HsvLiteralTest, MaxHue) {
  constexpr auto hsv = 360'050'050_hsv;
  static_assert(hsv.h() == 360.0f);
}

// ── _hsva ─────────────────────────────────────────────────────────────────────

TEST(HsvaLiteralTest, WithAlpha75) {
  // H=210, S=80%, V=90%, A=75%  → (210, 0.80, 0.90, 0.75)
  constexpr auto hsva = 210'080'090'075_hsva;
  static_assert(hsva.h() == 210.0f);
  EXPECT_FLOAT_EQ(hsva.s(), 0.80f);
  EXPECT_FLOAT_EQ(hsva.v(), 0.90f);
  EXPECT_FLOAT_EQ(hsva.a(), 0.75f);
}

TEST(HsvaLiteralTest, FullyOpaque) {
  constexpr auto hsva = 0'100'100'100_hsva;
  EXPECT_FLOAT_EQ(hsva.a(), 1.0f);
}

TEST(HsvaLiteralTest, FullyTransparent) {
  constexpr auto hsva = 0'100'100'000_hsva;
  EXPECT_FLOAT_EQ(hsva.a(), 0.0f);
}

// ── _cmyk ─────────────────────────────────────────────────────────────────────

TEST(CmykLiteralTest, PrintInk) {
  // C=50, M=30, Y=0, K=20
  constexpr auto cmyk = 050'030'000'020_cmyk;
  static_assert(cmyk.c() == 50);
  static_assert(cmyk.m() == 30);
  static_assert(cmyk.y() == 0);
  static_assert(cmyk.k() == 20);
}

TEST(CmykLiteralTest, AllZero) {
  constexpr auto cmyk = 0'0'0'0_cmyk;
  static_assert(cmyk.c() == 0 && cmyk.m() == 0 && cmyk.y() == 0 && cmyk.k() == 0);
}

TEST(CmykLiteralTest, Black) {
  // CMYK black: C=0, M=0, Y=0, K=100
  constexpr auto cmyk = 000'000'000'100_cmyk;
  static_assert(cmyk.k() == 100);
  static_assert(cmyk.c() == 0 && cmyk.m() == 0 && cmyk.y() == 0);
}

TEST(CmykLiteralTest, CyanPrimary) {
  // Cyan: C=100, M=0, Y=0, K=0
  constexpr auto cmyk = 100'000'000'000_cmyk;
  static_assert(cmyk.c() == 100);
  static_assert(cmyk.m() == 0 && cmyk.y() == 0 && cmyk.k() == 0);
}

TEST(CmykLiteralTest, MagentaPrimary) {
  constexpr auto cmyk = 000'100'000'000_cmyk;
  static_assert(cmyk.m() == 100);
}

TEST(CmykLiteralTest, YellowPrimary) {
  constexpr auto cmyk = 000'000'100'000_cmyk;
  static_assert(cmyk.y() == 100);
}

// ── Constexpr evaluation ──────────────────────────────────────────────────────

TEST(LiteralConstexprTest, AllLiteralsAreConstexpr) {
  // Verify the compiler evaluates these at compile time
  static_assert((0xFF0000_rgb).r() == 255);
  static_assert((0xFF000080_rgba).a() == 128);
  static_assert((0x80FF0000_argb).a() == 128);
  static_assert((120'050'075_hsl).h() == 120.0f);
  static_assert((0'100'050'100_hsla).a() == 1.0f);
  static_assert((0'100'100_hsv).v() == 1.0f);
  static_assert((210'080'090'075_hsva).a() == 0.75f);
  static_assert((000'000'000'100_cmyk).k() == 100);
}

// ── _rgb vs _hex consistency ──────────────────────────────────────────────────

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

}  // namespace colorcpp::literals::test
