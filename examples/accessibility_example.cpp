/**
 * @file accessibility_example.cpp
 * @brief Example: practical WCAG 2.x and APCA accessibility checks.
 *
 * Expands the basic contrast demo into a small accessibility audit workflow:
 * relative luminance, WCAG compliance tables, automatic text-color selection,
 * and a compact APCA polarity reminder.
 */

#include <colorcpp/colorcpp.hpp>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace colorcpp;
using namespace colorcpp::algorithms::accessibility;
using namespace colorcpp::constants;
using namespace colorcpp::io::ansi;
using namespace colorcpp::io::literals;

static void section(const char* title) { std::cout << '\n' << bold() << "=== " << title << " ===" << reset() << '\n'; }

static const char* pass_fail(bool ok) { return ok ? "PASS" : "FAIL"; }

struct named_color {
  const char* label;
  rgba8_t color;
};

struct contrast_case {
  const char* label;
  rgba8_t fg;
  rgba8_t bg;
};

static void print_luminance_sample(const named_color& sample) {
  std::cout << "  " << std::left << std::setw(14) << sample.label;
  print_swatch(std::cout, sample.color, 3);
  std::cout << "  Y=" << std::fixed << std::setprecision(3) << relative_luminance(sample.color) << '\n';
}

static void print_wcag_case(const contrast_case& sample) {
  float ratio = contrast_ratio(sample.fg, sample.bg);

  std::cout << "  " << std::left << std::setw(18) << sample.label;
  print_swatch(std::cout, sample.fg, 2);
  std::cout << " on ";
  print_swatch(std::cout, sample.bg, 2);
  std::cout << "  ratio=" << std::fixed << std::setprecision(2) << std::setw(5) << ratio
            << "  AA-N=" << pass_fail(is_wcag_compliant(sample.fg, sample.bg, wcag_level::aa, text_size::normal))
            << "  AA-L=" << pass_fail(is_wcag_compliant(sample.fg, sample.bg, wcag_level::aa, text_size::large))
            << "  AAA-N=" << pass_fail(is_wcag_compliant(sample.fg, sample.bg, wcag_level::aaa, text_size::normal))
            << "  AAA-L=" << pass_fail(is_wcag_compliant(sample.fg, sample.bg, wcag_level::aaa, text_size::large))
            << '\n';
}

static void print_auto_text_case(const named_color& sample) {
  auto text = contrast_text_color(sample.color);
  float ratio = contrast_ratio(text, sample.color);
  float lc = apca_contrast(text, sample.color);
  bool chooses_black = contrast_ratio(sample.color, black) >= contrast_ratio(sample.color, white);

  std::cout << "  " << std::left << std::setw(14) << sample.label;
  print_swatch(std::cout, sample.color, 3);
  std::cout << " -> ";
  print_swatch(std::cout, text, 2);
  std::cout << "  chose=" << (chooses_black ? "black" : "white") << "  WCAG=" << std::fixed << std::setprecision(2)
            << std::setw(5) << ratio << ":1" << "  APCA=" << std::showpos << std::setw(7) << lc << std::noshowpos
            << "  body=" << pass_fail(apca_meets_min_abs_lc(text, sample.color, 75.0f))
            << "  large=" << pass_fail(apca_meets_min_abs_lc(text, sample.color, 45.0f)) << '\n';
}

int main() {
  std::cout << std::fixed;

  // ========================================================================
  section("1. Relative Luminance Quick Reference");

  std::cout << "  WCAG contrast comes from linear-light luminance, not from how vivid a color looks.\n\n";

  std::vector<named_color> luminance_samples = {
      {"Black", black},         {"White", white},           {"Red", red}, {"Lime", lime}, {"Blue", blue},
      {"SteelBlue", steelblue}, {"Mid gray", 0x808080_rgb},
  };

  for (const auto& sample : luminance_samples) {
    print_luminance_sample(sample);
  }

  // ========================================================================
  section("2. WCAG Contrast Checks");

  std::cout << "  AA normal >= 4.5:1, AA large >= 3.0:1, AAA normal >= 7.0:1, AAA large >= 4.5:1\n\n";

  std::vector<contrast_case> wcag_cases = {
      {"Body text", 0x333333_rgb, 0xFFFFFF_rgb},       {"Large only", 0xFFFFFF_rgb, 0x808080_rgb},
      {"Yellow on white", 0xFFFF00_rgb, 0xFFFFFF_rgb}, {"Link blue", 0x0000EE_rgb, 0xFFFFFF_rgb},
      {"Dark mode body", 0xE5E7EB_rgb, 0x111827_rgb},
  };

  for (const auto& sample : wcag_cases) {
    print_wcag_case(sample);
  }

  std::cout << "\n  The same pair also works with the ANSI preview helper:\n";
  print_contrast(std::cout, 0x333333_rgb, 0xFFFFFF_rgb);
  std::cout << '\n';
  print_contrast(std::cout, 0xFFFFFF_rgb, 0x808080_rgb);
  std::cout << '\n';

  // ========================================================================
  section("3. Automatic Text Color Selection");

  std::cout << "  contrast_text_color() only chooses black or white, which is handy for cards, chips, and badges.\n\n";

  std::vector<named_color> surfaces = {
      {"Paper", 0xFFFFFF_rgb},   {"Warm panel", 0xFFE4C7_rgb},  {"Mint chip", 0xD1FAE5_rgb},
      {"Steel blue", steelblue}, {"Rose accent", 0xD946EF_rgb}, {"Night panel", 0x111827_rgb},
  };

  for (const auto& surface : surfaces) {
    print_auto_text_case(surface);
  }

  // ========================================================================
  section("4. APCA Polarity Reminder");

  auto text = 0x1F2937_rgb;
  auto panel = 0xF9FAFB_rgb;

  std::cout << "  WCAG ratio is symmetric, APCA is signed and order-dependent.\n\n";
  std::cout << "  contrast_ratio(text, panel) = " << std::setprecision(2) << contrast_ratio(text, panel) << ":1\n";
  std::cout << "  apca_contrast(text, panel)  = " << std::showpos << apca_contrast(text, panel) << std::noshowpos
            << '\n';
  std::cout << "  apca_contrast(panel, text)  = " << std::showpos << apca_contrast(panel, text) << std::noshowpos
            << '\n';

  std::cout << "\n";
  return 0;
}
