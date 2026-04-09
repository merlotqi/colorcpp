/**
 * @file accessibility_example.cpp
 * @brief Example: WCAG 2.x contrast helpers vs terminal preview (io::ansi).
 */

#include <colorcpp/colorcpp.hpp>
#include <iomanip>
#include <iostream>

using namespace colorcpp::algorithms::accessibility;
using namespace colorcpp::constants;
using namespace colorcpp::io::ansi;
using namespace colorcpp::io::literals;

static void section(const char* title) {
  std::cout << '\n' << bold() << "=== " << title << " ===" << reset() << '\n';
}

int main() {
  std::cout << std::fixed << std::setprecision(2);

  section("contrast_ratio (linear sRGB luminance)");
  auto fg = 0x333333_rgb;
  auto bg = 0xEEEEEE_rgb;
  float r = contrast_ratio(fg, bg);
  std::cout << "  ratio=" << r << "  AA normal text: " << std::boolalpha
            << is_wcag_compliant(fg, bg, wcag_level::aa, text_size::normal) << '\n';
  print_contrast(std::cout, fg, bg);
  std::cout << '\n';

  section("contrast_text_color (pick black or white on background)");
  auto panel = steelblue;
  auto txt = contrast_text_color(panel);
  std::cout << "  background: ";
  print_color(std::cout, panel);
  std::cout << "\n  chosen text: ";
  print_color(std::cout, txt);
  std::cout << '\n';
  print_contrast(std::cout, txt, panel);
  std::cout << '\n';

  section("More WCAG previews (same helpers as ansi_example, different API surface)");
  print_contrast(std::cout, white, black);
  std::cout << '\n';
  print_contrast(std::cout, yellow, white);
  std::cout << '\n';

  return 0;
}
