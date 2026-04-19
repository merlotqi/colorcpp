/**
 * @file apca_example.cpp
 * @brief Example: Advanced Perceptual Contrast Algorithm (APCA) vs WCAG 2.1.
 *
 * Demonstrates the modern APCA contrast metric, comparison with legacy WCAG
 * contrast ratio, real-world UI text scenarios, and contrast-based text color selection.
 */

#include <colorcpp/colorcpp.hpp>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace colorcpp;
using namespace colorcpp::algorithms::accessibility;
using namespace colorcpp::io::ansi;
using namespace colorcpp::io::literals;

static void section(const char* title) { std::cout << '\n' << bold() << "=== " << title << " ===" << reset() << '\n'; }

static void print_contrast_pair(rgba8_t fg, rgba8_t bg, const char* label) {
  float wcag = contrast_ratio(fg, bg);
  float apca = apca_contrast(fg, bg);

  std::cout << "  " << std::left << std::setw(18) << label << "  ";
  print_swatch(std::cout, fg, 2);
  std::cout << " on ";
  print_swatch(std::cout, bg, 2);

  std::cout << "  WCAG: " << std::fixed << std::setprecision(2) << std::setw(5) << wcag << ":1";
  std::cout << "  APCA: " << std::setw(6) << std::showpos << apca << std::noshowpos;

  if (std::abs(apca) >= 90) {
    std::cout << "  ✅ Excellent";
  } else if (std::abs(apca) >= 75) {
    std::cout << "  ✅ Good";
  } else if (std::abs(apca) >= 60) {
    std::cout << "  ⚠️  Minimum body text";
  } else if (std::abs(apca) >= 45) {
    std::cout << "  ⚠️  Large text only";
  } else {
    std::cout << "  ❌ Insufficient";
  }

  std::cout << '\n';
}

int main() {
  std::cout << std::fixed << std::setprecision(2);

  // ========================================================================
  section("1. APCA Basics");

  std::cout << "  Advanced Perceptual Contrast Algorithm 1.0.4\n";
  std::cout << "  WCAG 3 candidate perceptual contrast metric for modern displays\n\n";
  std::cout << "  Scale reference:\n";
  std::cout << "    Lc >= 90   Excellent contrast for any text size\n";
  std::cout << "    Lc >= 75   Preferred minimum for body text\n";
  std::cout << "    Lc >= 60   Absolute minimum for body text\n";
  std::cout << "    Lc >= 45   Minimum for large / bold text\n";
  std::cout << "    Lc < 45    Insufficient contrast for text\n\n";

  // ========================================================================
  section("2. Classic Pairs");

  print_contrast_pair(0x000000_rgb, 0xFFFFFF_rgb, "Black on White");
  print_contrast_pair(0xFFFFFF_rgb, 0x000000_rgb, "White on Black");
  print_contrast_pair(0x111111_rgb, 0xFFFFFF_rgb, "Dark gray on White");
  print_contrast_pair(0x333333_rgb, 0xFFFFFF_rgb, "#333 on White");
  print_contrast_pair(0x666666_rgb, 0xFFFFFF_rgb, "#666 on White");
  print_contrast_pair(0x888888_rgb, 0xFFFFFF_rgb, "#888 on White");

  // ========================================================================
  section("3. WCAG vs APCA Behaviour Differences");

  std::cout << "\n  Cases where WCAG and APCA give different results:\n\n";

  // Yellow on white - WCAG passes, APCA correctly fails
  print_contrast_pair(0xFFFF00_rgb, 0xFFFFFF_rgb, "Yellow on White");

  // Red on black - WCAG overestimates, APCA correctly flags as low
  print_contrast_pair(0xFF0000_rgb, 0x000000_rgb, "Red on Black");

  // Blue on white - WCAG underestimates, APCA correctly shows good contrast
  print_contrast_pair(0x0000EE_rgb, 0xFFFFFF_rgb, "Blue on White");

  // Purple on white
  print_contrast_pair(0x800080_rgb, 0xFFFFFF_rgb, "Purple on White");

  // ========================================================================
  section("4. Common UI Backgrounds");

  auto white = 0xFFFFFF_rgb;
  auto light_gray = 0xF5F5F5_rgb;
  auto dark_gray = 0x212121_rgb;
  auto black = 0x000000_rgb;

  std::cout << "\n  Light mode background variations:\n\n";
  print_contrast_pair(0x000000_rgb, white, "#000 on #FFF");
  print_contrast_pair(0x000000_rgb, light_gray, "#000 on #F5F5F5");

  std::cout << "\n  Dark mode background variations:\n\n";
  print_contrast_pair(0xFFFFFF_rgb, black, "#FFF on #000");
  print_contrast_pair(0xFFFFFF_rgb, dark_gray, "#FFF on #212121");

  // ========================================================================
  section("5. Automatic Text Color Selection");

  std::vector<rgba8_t> test_backgrounds = {0xFFFFFF_rgb, 0xFFE4E4_rgb, 0xE4FFE4_rgb, 0xE4E4FF_rgb,
                                           0xFFFFE4_rgb, 0xE4FFFF_rgb, 0xFFE4FF_rgb, 0x333333_rgb};

  std::cout << "\n  contrast_text_color() automatically selects black or white:\n\n";

  for (auto bg : test_backgrounds) {
    auto text = contrast_text_color(bg);
    float contrast = apca_contrast(text, bg);

    std::cout << "  Background: ";
    print_swatch(std::cout, bg, 4);
    std::cout << " → Text: ";
    print_swatch(std::cout, text, 4);
    std::cout << "  APCA Lc: " << std::setw(6) << std::showpos << contrast;
    std::cout << std::noshowpos << '\n';
  }

  // ========================================================================
  section("6. APCA Compliance Checks");

  rgba8_t fg = 0x333333_rgb;
  rgba8_t bg = 0xFFFFFF_rgb;

  std::cout << '\n';
  std::cout << "  #333 on #FFF: " << std::boolalpha << '\n';
  std::cout << "    WCAG AA normal:      " << is_wcag_compliant(fg, bg, wcag_level::aa, text_size::normal) << '\n';
  std::cout << "    WCAG AAA normal:     " << is_wcag_compliant(fg, bg, wcag_level::aaa, text_size::normal) << '\n';
  std::cout << "    APCA body text:      " << apca_meets_min_abs_lc(fg, bg, 75.0f) << '\n';
  std::cout << "    APCA large text:     " << apca_meets_min_abs_lc(fg, bg, 45.0f) << '\n';
  std::cout << "    APCA placeholder:    " << apca_meets_min_abs_lc(fg, bg, 30.0f) << '\n';

  // ========================================================================
  section("7. Threshold Comparison Table");

  std::cout << "\n  Contrast levels for #000 on #FFF:\n\n";
  std::cout << "    WCAG 2.1 ratio:  " << contrast_ratio(0x000000_rgb, 0xFFFFFF_rgb) << ":1\n";
  std::cout << "    APCA Lc value:    " << apca_contrast(0x000000_rgb, 0xFFFFFF_rgb) << '\n';

  std::cout << "\n";
  return 0;
}
