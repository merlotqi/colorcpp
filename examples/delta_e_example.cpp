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

  section("CMC l:c (Colour Measurement Committee)");
  std::cout << "  CMC metric: textile industry standard (DIN 6176-2:1986)\n";
  std::cout << "  Parameters: l (lightness), c (chroma)\n";
  std::cout << "    l=2, c=1: Acceptability threshold\n";
  std::cout << "    l=1, c=1: Perceptibility threshold\n\n";

  // CMC with different parameter sets
  auto cmc_acceptability = delta_e_cmc(tomato, coral, 2.0f, 1.0f);
  auto cmc_perceptibility = delta_e_cmc(tomato, coral, 1.0f, 1.0f);

  std::cout << "  Tomato vs Coral (CMC):\n";
  std::cout << "    l=2, c=1 (acceptability): " << cmc_acceptability << "\n";
  std::cout << "    l=1, c=1 (perceptibility): " << cmc_perceptibility << "\n";

  // Compare with ΔE2000
  auto cmc_red_darkred = delta_e_cmc(red, dark_red, 2.0f, 1.0f);
  auto cmc_red_green = delta_e_cmc(red, green, 2.0f, 1.0f);
  auto cmc_red_blue = delta_e_cmc(red, blue, 2.0f, 1.0f);

  std::cout << "\n  CMC (l=2, c=1) vs ΔE2000 comparison:\n";
  std::pair<rgba8_t, rgba8_t> pairs2[] = {{red, dark_red}, {red, green}, {red, blue}};
  float cmc_vals[] = {cmc_red_darkred, cmc_red_green, cmc_red_blue};
  const char* names[] = {"red/dark_red", "red/green", "red/blue"};

  for (size_t i = 0; i < 3; ++i) {
    float d2000 = delta_e_2000(pairs2[i].first, pairs2[i].second);
    std::cout << "  " << names[i] << ": CMC=" << std::setw(7) << cmc_vals[i] << "  ΔE2000=" << std::setw(7) << d2000
              << "\n";
  }

  section("CMC is_visually_same_cmc()");
  bool same_acceptable = is_visually_same_cmc(tomato, coral, 1.0f, 2.0f, 1.0f);
  bool same_perceptible = is_visually_same_cmc(tomato, coral, 0.5f, 1.0f, 1.0f);
  std::cout << "  Tomato vs Coral within CMC acceptability threshold (1.0): " << (same_acceptable ? "YES" : "NO")
            << "\n";
  std::cout << "  Tomato vs Coral within CMC perceptibility threshold (0.5): " << (same_perceptible ? "YES" : "NO")
            << "\n";

  section("DIN99 (DIN 6176)");
  std::cout << "  DIN99: German standard with improved perceptual uniformity\n";
  std::cout << "  Applies non-linear transforms to CIELAB for better uniformity\n\n";

  // DIN99 differences
  auto din99_red_darkred = delta_e_din99(red, dark_red);
  auto din99_tomato_coral = delta_e_din99(tomato, coral);
  auto din99_red_green = delta_e_din99(red, green);
  auto din99_red_blue = delta_e_din99(red, blue);
  auto din99_green_blue = delta_e_din99(green, blue);

  std::cout << "  DIN99 differences:\n";
  std::cout << "    red/dark_red:   " << din99_red_darkred << "\n";
  std::cout << "    tomato/coral:   " << din99_tomato_coral << "\n";
  std::cout << "    red/green:      " << din99_red_green << "\n";
  std::cout << "    red/blue:       " << din99_red_blue << "\n";
  std::cout << "    green/blue:     " << din99_green_blue << "\n";

  section("DIN99 is_visually_same_din99()");
  bool din99_same = is_visually_same_din99(tomato, coral, 1.0f, 1.0f, 1.0f);
  std::cout << "  Tomato vs Coral within DIN99 threshold (1.0): " << (din99_same ? "YES" : "NO") << "\n";
  din99_same = is_visually_same_din99(red, dark_red, 1.0f, 1.0f, 1.0f);
  std::cout << "  Red vs Dark Red within DIN99 threshold (1.0): " << (din99_same ? "YES" : "NO") << "\n";

  section("All metrics comparison: red vs dark_red");
  float d76 = delta_e_76(red, dark_red);
  float d94 = delta_e_94(red, dark_red);
  float d2000 = delta_e_2000(red, dark_red);
  float dok = delta_e_ok(red, dark_red);
  float dcmc = delta_e_cmc(red, dark_red, 2.0f, 1.0f);
  float ddin = delta_e_din99(red, dark_red);

  std::cout << "  ΔE 76:        " << std::setw(7) << d76 << "  " << band_label(d76) << "\n";
  std::cout << "  ΔE 94:        " << std::setw(7) << d94 << "  " << band_label(d94) << "\n";
  std::cout << "  ΔE 2000:      " << std::setw(7) << d2000 << "  " << band_label(d2000) << "\n";
  std::cout << "  ΔE CMC(2,1):  " << std::setw(7) << dcmc << "  " << band_label(dcmc) << "\n";
  std::cout << "  ΔE DIN99:     " << std::setw(7) << ddin << "  " << band_label(ddin) << "\n";
  std::cout << "  ΔE OK:        " << std::setw(7) << dok << "  " << band_label(dok * 50.0f) << "\n";

  std::cout << '\n';
  return 0;
}
