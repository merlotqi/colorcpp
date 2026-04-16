/**
 * @file display_p3_example.cpp
 * @brief Example: Display P3 color space usage.
 *
 * Demonstrates Display P3 color creation, conversion to/from sRGB,
 * gamut comparison, and practical use cases for wide-gamut displays.
 */

#include <colorcpp/colorcpp.hpp>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace colorcpp;
using namespace colorcpp::operations::conversion;
using namespace colorcpp::io::ansi;
using namespace colorcpp::io::literals;

static void section(const char* title) { std::cout << '\n' << bold() << "=== " << title << " ===" << reset() << '\n'; }

int main() {
  std::cout << std::fixed << std::setprecision(4);

  // ========================================================================
  section("1. Display P3 Basics");

  std::cout << "  Display P3 uses DCI-P3 primaries with D65 white point.\n";
  std::cout << "  Wider gamut than sRGB, common on modern Apple displays.\n";
  std::cout << "  Uses the same transfer function (gamma) as sRGB.\n\n";

  auto red_p3 = display_p3f_t{1.0f, 0.0f, 0.0f};
  auto green_p3 = display_p3f_t{0.0f, 1.0f, 0.0f};
  auto blue_p3 = display_p3f_t{0.0f, 0.0f, 1.0f};

  std::cout << "  P3 Red:   " << red_p3 << '\n';
  std::cout << "  P3 Green: " << green_p3 << '\n';
  std::cout << "  P3 Blue:  " << blue_p3 << '\n';

  // ========================================================================
  section("2. Display P3 ↔ sRGB Conversion");

  // Vivid orange (out of sRGB gamut)
  auto vivid_orange_p3 = display_p3f_t{1.0f, 0.5f, 0.0f};
  auto vivid_orange_srgb = color_cast<rgbf_t>(vivid_orange_p3);
  auto back_to_p3 = color_cast<display_p3f_t>(vivid_orange_srgb);

  std::cout << "  Vivid orange in P3:  " << vivid_orange_p3 << '\n';
  std::cout << "  Converted to sRGB:   " << vivid_orange_srgb << '\n';
  std::cout << "  Back to P3:          " << back_to_p3 << '\n';

  // Vivid green (out of sRGB gamut)
  auto vivid_green_p3 = display_p3f_t{0.0f, 1.0f, 0.2f};
  auto vivid_green_srgb = color_cast<rgbf_t>(vivid_green_p3);

  std::cout << "\n  Vivid green in P3:   " << vivid_green_p3 << '\n';
  std::cout << "  Converted to sRGB:   " << vivid_green_srgb << '\n';

  // ========================================================================
  section("3. Gamut Comparison");

  std::cout << "\n  P3 primary → sRGB comparison:\n\n";

  struct primary_test {
    const char* name;
    display_p3f_t p3;
  };

  primary_test tests[] = {
      {"Pure Red", display_p3f_t{1.0f, 0.0f, 0.0f}},     {"Pure Green", display_p3f_t{0.0f, 1.0f, 0.0f}},
      {"Pure Blue", display_p3f_t{0.0f, 0.0f, 1.0f}},    {"Cyan", display_p3f_t{0.0f, 1.0f, 1.0f}},
      {"Yellow", display_p3f_t{1.0f, 1.0f, 0.0f}},       {"Magenta", display_p3f_t{1.0f, 0.0f, 1.0f}},
      {"Vivid Orange", display_p3f_t{1.0f, 0.5f, 0.0f}}, {"Vivid Magenta", display_p3f_t{1.0f, 0.0f, 0.5f}},
  };

  for (const auto& t : tests) {
    auto srgb = color_cast<rgbf_t>(t.p3);
    bool in_gamut = (srgb.r() >= 0.0f && srgb.r() <= 1.0f && srgb.g() >= 0.0f && srgb.g() <= 1.0f && srgb.b() >= 0.0f &&
                     srgb.b() <= 1.0f);

    std::cout << "  " << std::left << std::setw(16) << t.name;
    print_swatch(std::cout, t.p3, 2);
    std::cout << " → ";
    print_swatch(std::cout, srgb, 2);
    std::cout << "  " << (in_gamut ? "✅ in sRGB" : "⚠️  clamped") << '\n';
  }

  // ========================================================================
  section("4. CSS Color Level 4 - display-p3");

  std::cout << "\n  CSS Color Level 4 defines display-p3() function:\n";
  std::cout << "    color(display-p3 1 0.5 0)   // vivid orange\n";
  std::cout << "    color(display-p3 0 1 0.2)   // vivid green\n\n";

  // Simulate parsing CSS color(display-p3 r g b)
  auto css_vivid_green = display_p3f_t{0.0f, 1.0f, 0.2f};
  auto css_srgb = color_cast<rgbf_t>(css_vivid_green);

  std::cout << "  CSS: color(display-p3 0 1 0.2)\n";
  std::cout << "  P3 value: " << css_vivid_green << '\n';
  std::cout << "  sRGB fallback: " << css_srgb << '\n';

  // ========================================================================
  section("5. Practical: Apple P3 Color to Web sRGB");

  std::cout << "\n  Converting wide-gamut display colors to web-safe sRGB:\n\n";

  std::vector<display_p3f_t> p3_colors = {
      display_p3f_t{0.98f, 0.35f, 0.13f},  // Apple-style red-orange
      display_p3f_t{0.20f, 0.78f, 0.35f},  // Apple-style green
      display_p3f_t{0.35f, 0.35f, 0.95f},  // Apple-style blue
      display_p3f_t{0.95f, 0.90f, 0.30f},  // Apple-style yellow
  };

  for (size_t i = 0; i < p3_colors.size(); ++i) {
    auto p3 = p3_colors[i];
    auto srgb = color_cast<rgbf_t>(p3);
    auto cielab = color_cast<core::cielab_t>(p3);

    std::cout << "  Color " << (i + 1) << ":\n";
    std::cout << "    P3:     ";
    print_swatch(std::cout, p3, 3);
    std::cout << "\n    sRGB:   ";
    print_swatch(std::cout, srgb, 3);
    std::cout << "\n    CIELAB: L=" << std::setw(6) << cielab.l() << " a=" << std::setw(6) << cielab.a()
              << " b=" << std::setw(6) << cielab.b() << "\n\n";
  }

  // ========================================================================
  section("6. Alpha Support (Display P3A)");

  auto p3a = display_p3af_t{0.8f, 0.3f, 0.6f, 0.75f};
  auto srgba = color_cast<rgbaf_t>(p3a);

  std::cout << "  Display P3A with alpha: " << p3a << '\n';
  std::cout << "  Converted to sRGBA:     " << srgba << '\n';

  std::cout << "\n";
  return 0;
}
