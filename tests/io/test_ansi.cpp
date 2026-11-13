/**
 * @file test_ansi.cpp
 * @brief Unit tests for ANSI terminal color utilities.
 */

#include <gtest/gtest.h>

#include <array>
#include <colorcpp/io/ansi.hpp>
#include <colorcpp/io/css/named_colors.hpp>
#include <colorcpp/io/literals.hpp>
#include <iomanip>
#include <sstream>

using namespace colorcpp;
using namespace colorcpp::io::ansi;
using namespace colorcpp::io::css;

// ===== ANSI Escape Sequence Tests =====

TEST(Ansi, ResetSequence) { EXPECT_STREQ(reset(), "\033[0m"); }

TEST(Ansi, BoldSequence) { EXPECT_STREQ(bold(), "\033[1m"); }

TEST(Ansi, FgSequence) {
  auto seq = fg(255, 0, 0);
  EXPECT_EQ(seq, "\033[38;2;255;0;0m");
}

TEST(Ansi, BgSequence) {
  auto seq = bg(0, 128, 255);
  EXPECT_EQ(seq, "\033[48;2;0;128;255m");
}

TEST(Ansi, FgFromRgba8) {
  core::rgba8_t c{10, 20, 30, 255};
  auto seq = fg(c);
  EXPECT_EQ(seq, "\033[38;2;10;20;30m");
}

TEST(Ansi, BgFromRgba8) {
  core::rgba8_t c{100, 150, 200, 255};
  auto seq = bg(c);
  EXPECT_EQ(seq, "\033[48;2;100;150;200m");
}

// ===== Print Functions Tests =====

TEST(Ansi, PrintColor) {
  std::ostringstream os;
  core::rgba8_t red{255, 0, 0, 255};
  print_color(os, red, "red");
  std::string output = os.str();

  // Should contain the ANSI background sequence for red
  EXPECT_NE(output.find("\033[48;2;255;0;0m"), std::string::npos);
  // Should contain hex value
  EXPECT_NE(output.find("#ff0000ff"), std::string::npos);
  // Should contain RGB values
  EXPECT_NE(output.find("RGB(255,0,0)"), std::string::npos);
  // Should contain label
  EXPECT_NE(output.find("red"), std::string::npos);
  // Should contain reset
  EXPECT_NE(output.find("\033[0m"), std::string::npos);
}

TEST(Ansi, PrintColorVerbose) {
  std::ostringstream os;
  core::rgba8_t coral{255, 127, 80, 255};
  print_color_verbose(os, coral, "coral");
  std::string output = os.str();

  // Should contain HSL values
  EXPECT_NE(output.find("HSL("), std::string::npos);
}

TEST(Ansi, PrintSwatch) {
  std::ostringstream os;
  core::rgba8_t blue{0, 0, 255, 255};
  print_swatch(os, blue, 4);
  std::string output = os.str();

  // Should contain ANSI background for blue
  EXPECT_NE(output.find("\033[48;2;0;0;255m"), std::string::npos);
  // Should contain reset
  EXPECT_NE(output.find("\033[0m"), std::string::npos);
}

TEST(Ansi, PrintPalette) {
  std::ostringstream os;
  core::rgba8_t colors[] = {
      core::rgba8_t{255, 0, 0, 255},
      core::rgba8_t{0, 255, 0, 255},
      core::rgba8_t{0, 0, 255, 255},
  };
  print_palette(os, colors, 3);
  std::string output = os.str();

  // Should contain all three ANSI background colors
  EXPECT_NE(output.find("\033[48;2;255;0;0m"), std::string::npos);
  EXPECT_NE(output.find("\033[48;2;0;255;0m"), std::string::npos);
  EXPECT_NE(output.find("\033[48;2;0;0;255m"), std::string::npos);
}

TEST(Ansi, PrintPaletteContainerOverload) {
  std::ostringstream os;
  std::array<core::rgba8_t, 3> colors = {
      core::rgba8_t{255, 0, 0, 255},
      core::rgba8_t{0, 255, 0, 255},
      core::rgba8_t{0, 0, 255, 255},
  };
  print_palette(os, colors);
  std::string output = os.str();

  EXPECT_NE(output.find("\033[48;2;255;0;0m"), std::string::npos);
  EXPECT_NE(output.find("\033[48;2;0;255;0m"), std::string::npos);
  EXPECT_NE(output.find("\033[48;2;0;0;255m"), std::string::npos);
}

TEST(Ansi, PrintGradient) {
  std::ostringstream os;
  core::rgba8_t black{0, 0, 0, 255};
  core::rgba8_t white{255, 255, 255, 255};
  print_gradient(os, black, white, 4);
  std::string output = os.str();

  // Should contain start and end colors
  EXPECT_NE(output.find("\033[48;2;0;0;0m"), std::string::npos);
  EXPECT_NE(output.find("\033[48;2;255;255;255m"), std::string::npos);
}

TEST(Ansi, PrintGradientZeroStepsProducesOnlyNewline) {
  std::ostringstream os;
  core::rgba8_t black{0, 0, 0, 255};
  core::rgba8_t white{255, 255, 255, 255};

  print_gradient(os, black, white, 0);

  EXPECT_EQ(os.str(), "\n");
}

TEST(Ansi, PrintGradientOneStepUsesStartColor) {
  std::ostringstream os;
  core::rgba8_t black{0, 0, 0, 255};
  core::rgba8_t white{255, 255, 255, 255};

  print_gradient(os, black, white, 1);
  std::string output = os.str();

  EXPECT_NE(output.find("\033[48;2;0;0;0m"), std::string::npos);
  EXPECT_EQ(output.find("\033[48;2;255;255;255m"), std::string::npos);
}

// ===== Issue #116: 256/16-color quantization and level control. =====
// The xterm 6×6×6 cube uses levels {0,95,135,175,215,255} at indices
// 16-231 and a 24-step gray ramp 8+10k at 232-255. System colors 0-15
// are skipped: their RGB values depend on the terminal theme.

TEST(AnsiLevel, QuantizeAnsi256Anchors) {
  EXPECT_EQ(quantize_ansi256(255, 255, 255), 231);  // cube white corner
  EXPECT_EQ(quantize_ansi256(0, 0, 0), 16);         // cube black corner
  EXPECT_EQ(quantize_ansi256(255, 0, 0), 196);      // pure red
  EXPECT_EQ(quantize_ansi256(0, 255, 0), 46);       // pure green
  EXPECT_EQ(quantize_ansi256(0, 0, 255), 21);       // pure blue
  // Gray ramp is exact for 8 + 10k: 128 = 8 + 10*12 -> index 244
  EXPECT_EQ(quantize_ansi256(128, 128, 128), 244);
  // System indices 0-15 decode through the xterm default palette.
  EXPECT_EQ(ansi256_rgb(9), (std::array<uint8_t, 3>{255, 0, 0}));
  EXPECT_EQ(ansi256_rgb(15), (std::array<uint8_t, 3>{255, 255, 255}));
  // Cube level 95 (second level) and its decode — pins the exact palette.
  EXPECT_EQ(quantize_ansi256(95, 95, 95), 59);
  EXPECT_EQ(ansi256_rgb(59), (std::array<uint8_t, 3>{95, 95, 95}));
  // Ties keep the lower index: cube 0 vs gray 8 both give d=48 at (4,4,4),
  // and the gray ramp's internal tie at v=13 (d=75 for 8 and 18) keeps 232.
  EXPECT_EQ(quantize_ansi256(4, 4, 4), 16);
  EXPECT_EQ(quantize_ansi256(13, 13, 13), 232);
}

TEST(AnsiLevel, Ansi256RoundTripWithinCubeStep) {
  // Cube-only quantization stays within half the largest cube gap (95/2 = 47.5
  // -> 48 per channel), but plain Euclidean nearest lets the gray ramp win for
  // slightly-off-gray inputs, trading one channel for overall closeness:
  // brute-forced full-grid worst case is (45,48,126) -> gray 238 (68,68,68),
  // 58 on blue (4293 < 4315, so gray beats the cube by a hair). Step 3 hits
  // that point; the bound is the measured full-grid maximum.
  for (int r = 0; r <= 255; r += 3) {
    for (int g = 0; g <= 255; g += 3) {
      for (int b = 0; b <= 255; b += 3) {
        const auto q = quantize_ansi256(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b));
        const auto rgb = ansi256_rgb(q);
        EXPECT_LE(std::abs(rgb[0] - r), 58) << "r=" << r << " g=" << g << " b=" << b;
        EXPECT_LE(std::abs(rgb[1] - g), 58) << "r=" << r << " g=" << g << " b=" << b;
        EXPECT_LE(std::abs(rgb[2] - b), 58) << "r=" << r << " g=" << g << " b=" << b;
      }
    }
  }
  // Gray ramp covers 8..238 within its 10-step spacing (<=5, or the exact
  // cube corners); below 8 the crossover with cube 0 caps error at 4, and
  // above 238 the cube corner 255 takes over (~246 crossover, max 8 at v=246).
  for (int v = 0; v <= 238; ++v) {
    const auto q = quantize_ansi256(static_cast<uint8_t>(v), static_cast<uint8_t>(v), static_cast<uint8_t>(v));
    const auto rgb = ansi256_rgb(q);
    EXPECT_LE(std::abs(rgb[0] - v), 5) << "gray v=" << v;
  }
  for (int v = 239; v <= 255; ++v) {
    const auto q = quantize_ansi256(static_cast<uint8_t>(v), static_cast<uint8_t>(v), static_cast<uint8_t>(v));
    const auto rgb = ansi256_rgb(q);
    EXPECT_LE(std::abs(rgb[0] - v), 8) << "gray v=" << v;
  }
}

TEST(AnsiLevel, QuantizeAnsi16Anchors) {
  EXPECT_EQ(quantize_ansi16(0, 0, 0), 0);         // black
  EXPECT_EQ(quantize_ansi16(255, 255, 255), 15);  // white
  EXPECT_EQ(quantize_ansi16(255, 0, 0), 9);       // bright red
  EXPECT_EQ(quantize_ansi16(128, 0, 0), 1);       // maroon (exact)
  EXPECT_EQ(quantize_ansi16(0, 128, 0), 2);       // green (exact)
  EXPECT_EQ(quantize_ansi16(192, 192, 192), 7);   // silver (exact)
  EXPECT_EQ(quantize_ansi16(128, 128, 128), 8);   // gray (exact)
  // Tie black(0) vs maroon(128) at d=4096: lower index wins.
  EXPECT_EQ(quantize_ansi16(64, 0, 0), 0);
}

TEST(AnsiLevel, EmissionPerLevel) {
  // Default (and explicit truecolor) keeps the 24-bit sequence.
  EXPECT_EQ(fg(255, 0, 0), "\033[38;2;255;0;0m");
  EXPECT_EQ(fg(255, 0, 0, level::truecolor), "\033[38;2;255;0;0m");
  EXPECT_EQ(bg(0, 128, 255, level::truecolor), "\033[48;2;0;128;255m");

  // 256-color: 38;5;N / 48;5;N.
  EXPECT_EQ(fg(255, 0, 0, level::ansi256), "\033[38;5;196m");
  EXPECT_EQ(bg(255, 0, 0, level::ansi256), "\033[48;5;196m");

  // 16-color: 30-37/90-97 fg, 40-47/100-107 bg.
  EXPECT_EQ(fg(255, 0, 0, level::ansi16), "\033[91m");  // index 9 -> bright
  EXPECT_EQ(bg(255, 0, 0, level::ansi16), "\033[101m");
  EXPECT_EQ(fg(128, 0, 0, level::ansi16), "\033[31m");  // index 1 -> normal
  EXPECT_EQ(bg(128, 0, 0, level::ansi16), "\033[41m");

  // rgba8_t overloads forward the level.
  core::rgba8_t red{255, 0, 0, 255};
  EXPECT_EQ(fg(red, level::ansi256), "\033[38;5;196m");
  EXPECT_EQ(bg(red, level::ansi16), "\033[101m");
}

TEST(AnsiLevel, WritersThreadLevel) {
  core::rgba8_t red{255, 0, 0, 255};

  std::ostringstream sw;
  print_swatch(sw, red, 4, level::ansi256);
  EXPECT_NE(sw.str().find("\033[48;5;196m"), std::string::npos);
  EXPECT_EQ(sw.str().find("\033[48;2;"), std::string::npos);

  std::ostringstream pal;
  core::rgba8_t colors[] = {red};
  print_palette(pal, colors, 1, 4, level::ansi16);
  EXPECT_NE(pal.str().find("\033[101m"), std::string::npos);
  EXPECT_EQ(pal.str().find("48;2"), std::string::npos);
  EXPECT_EQ(pal.str().find("48;5"), std::string::npos);

  std::ostringstream grad;
  print_gradient(grad, red, red, 2, level::ansi256);
  EXPECT_NE(grad.str().find("\033[48;5;196m"), std::string::npos);

  std::ostringstream info;
  print_color(info, red, "r", level::ansi16);
  EXPECT_NE(info.str().find("\033[101m"), std::string::npos);

  std::ostringstream con;
  print_contrast(con, red, red, level::ansi16);
  EXPECT_NE(con.str().find("\033[101m"), std::string::npos);  // bg preview
  EXPECT_EQ(con.str().find("48;2"), std::string::npos);
}

// ===== Contrast Tests =====

TEST(Ansi, RelativeLuminanceBlack) {
  float lum = relative_luminance(0, 0, 0);
  EXPECT_NEAR(lum, 0.0f, 1e-5f);
}

TEST(Ansi, RelativeLuminanceWhite) {
  float lum = relative_luminance(255, 255, 255);
  EXPECT_NEAR(lum, 1.0f, 1e-5f);
}

TEST(Ansi, ContrastRatioBlackOnWhite) {
  core::rgba8_t black{0, 0, 0, 255};
  core::rgba8_t white{255, 255, 255, 255};
  float ratio = contrast_ratio(black, white);
  EXPECT_NEAR(ratio, 21.0f, 0.1f);
}

TEST(Ansi, ContrastRatioSameColor) {
  core::rgba8_t gray{128, 128, 128, 255};
  float ratio = contrast_ratio(gray, gray);
  EXPECT_NEAR(ratio, 1.0f, 1e-5f);
}

TEST(Ansi, WcagLevelAAA) {
  EXPECT_EQ(wcag_level(7.0f), "AAA");
  EXPECT_EQ(wcag_level(10.0f), "AAA");
}

TEST(Ansi, WcagLevelAA) {
  EXPECT_EQ(wcag_level(4.5f), "AA");
  EXPECT_EQ(wcag_level(6.9f), "AA");
}

TEST(Ansi, WcagLevelAALarge) {
  EXPECT_EQ(wcag_level(3.0f), "AA Large");
  EXPECT_EQ(wcag_level(4.4f), "AA Large");
}

TEST(Ansi, WcagLevelFail) {
  EXPECT_EQ(wcag_level(2.9f), "Fail");
  EXPECT_EQ(wcag_level(1.0f), "Fail");
}

TEST(Ansi, PrintContrast) {
  std::ostringstream os;
  core::rgba8_t white{255, 255, 255, 255};
  core::rgba8_t black{0, 0, 0, 255};
  print_contrast(os, white, black);
  std::string output = os.str();

  // Should contain contrast ratio
  EXPECT_NE(output.find("21.0:1"), std::string::npos);
  // Should contain AAA level
  EXPECT_NE(output.find("AAA"), std::string::npos);
}

TEST(Ansi, PrintColorRestoresFormattingState) {
  std::ostringstream os;
  os << std::scientific << std::setprecision(3) << std::setfill('_');

  core::rgba8_t red{255, 0, 0, 255};
  print_color(os, red, "red");
  os << "|" << 1.25;

  EXPECT_NE(os.str().find("1.250e+00"), std::string::npos);
}

TEST(Ansi, PrintColorVerboseRestoresFormattingState) {
  std::ostringstream os;
  os << std::scientific << std::setprecision(3) << std::setfill('_');

  core::rgba8_t red{255, 0, 0, 255};
  print_color_verbose(os, red, "red");
  os << "|" << 1.25;

  EXPECT_NE(os.str().find("1.250e+00"), std::string::npos);
}

TEST(Ansi, PrintContrastRestoresFormattingState) {
  std::ostringstream os;
  os << std::scientific << std::setprecision(3) << std::setfill('_');

  core::rgba8_t white{255, 255, 255, 255};
  core::rgba8_t black{0, 0, 0, 255};
  print_contrast(os, white, black);
  os << "|" << 1.25;

  EXPECT_NE(os.str().find("1.250e+00"), std::string::npos);
}

// ===== Named Color Integration =====

TEST(Ansi, PrintNamedColor) {
  using namespace named_literal;
  std::ostringstream os;
  auto coral = "coral"_color;
  print_color(os, coral, "coral");
  std::string output = os.str();

  EXPECT_NE(output.find("#ff7f50ff"), std::string::npos);
  EXPECT_NE(output.find("coral"), std::string::npos);
}
