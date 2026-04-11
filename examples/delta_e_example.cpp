/**
 * @file delta_e_example.cpp
 * @brief Example: Color difference metrics and comparison algorithms.
 */

#include <colorcpp/colorcpp.hpp>
#include <iomanip>
#include <iostream>

using namespace colorcpp::core;
using namespace colorcpp::algorithms::delta_e;
using namespace colorcpp::operations::conversion;
using namespace colorcpp::io::literals;
using namespace colorcpp::io::ansi;

static void section(const char* title) { std::cout << '\n' << bold() << "=== " << title << " ===" << reset() << '\n'; }

static const char* band_label(float diff) {
  if (diff < 1.0f) return "[imperceptible]";
  if (diff < 2.0f) return "[close]";
  if (diff < 5.0f) return "[noticeable]";
  if (diff < 10.0f) return "[significant]";
  return "[very different]";
}

int main() {
  std::cout << std::fixed << std::setprecision(4);

  auto red = 0xFF0000_rgb;
  auto dark_red = 0xCC0000_rgb;
  auto green = 0x00FF00_rgb;
  auto blue = 0x0000FF_rgb;
  auto tomato = 0xFF6347_rgb;
  auto coral = 0xFF7F50_rgb;

  section("Delta E metrics (tomato vs coral)");
  print_color(std::cout, tomato, "tomato");
  std::cout << "\n";
  print_color(std::cout, coral, "coral");
  std::cout << "\n\n";

  auto de76 = delta_e_76(tomato, coral);
  auto de94 = delta_e_94(tomato, coral);
  auto de2000 = delta_e_2000(tomato, coral);
  auto deok = delta_e_ok(tomato, coral);

  std::cout << "  ΔE 76:      " << de76 << "\n";
  std::cout << "  ΔE 94:      " << de94 << "\n";
  std::cout << "  ΔE 2000:    " << de2000 << "\n";
  std::cout << "  ΔE OK:      " << deok << "\n";

  section("ΔE2000 interpretation (rule-of-thumb)");
  std::cout << "  < 1.0  Imperceptible\n";
  std::cout << "  1–2    Noticeable only on close inspection\n";
  std::cout << "  2–5    Clearly different\n";
  std::cout << "  5–10   Large difference\n";
  std::cout << "  > 50   Very different hues\n";

  section("Pairs with ΔE2000 + swatches");
  std::vector<std::pair<rgba8_t, rgba8_t>> pairs = {
      {red, dark_red}, {tomato, coral}, {red, green}, {red, blue}, {green, blue},
  };

  for (auto& pair : pairs) {
    float diff = delta_e_2000(pair.first, pair.second);
    std::cout << "  ";
    print_swatch(std::cout, pair.first, 2);
    std::cout << ' ';
    print_swatch(std::cout, pair.second, 2);
    std::cout << "  ΔE2000=" << std::setw(7) << diff << "  " << band_label(diff) << '\n';
  }

  section("Oklab ΔE and JND");
  std::cout << "  Typical OKLab JND scale: " << oklab_jnd_typical << " (see library docs; not comparable to ΔE2000)\n";
  if (deok < oklab_jnd_typical) {
    std::cout << "  Tomato vs coral: below typical OKLab JND (often treated as same for that threshold).\n";
  } else {
    std::cout << "  Tomato vs coral: above typical OKLab JND (visually distinct on that scale).\n";
  }

  section("Rainbow: adjacent OKLab ΔE");
  std::vector<rgba8_t> palette = {0xFF0000_rgb, 0xFF7F00_rgb, 0xFFFF00_rgb, 0x00FF00_rgb,
                                  0x0000FF_rgb, 0x4B0082_rgb, 0x9400D3_rgb};
  std::cout << "  ";
  print_palette(std::cout, palette.data(), palette.size(), 3);
  for (size_t i = 0; i + 1 < palette.size(); ++i) {
    float d = delta_e_ok(palette[i], palette[i + 1]);
    std::cout << "  Step " << i << "→" << (i + 1) << ": ΔE_OK=" << d << '\n';
  }

  std::cout << '\n';
  return 0;
}
