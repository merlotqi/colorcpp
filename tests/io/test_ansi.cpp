/**
 * @file test_ansi.cpp
 * @brief Unit tests for ANSI terminal color utilities.
 */

#include <gtest/gtest.h>

#include <colorcpp/io/ansi.hpp>
#include <colorcpp/io/css/named_colors.hpp>
#include <colorcpp/io/literals.hpp>
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
