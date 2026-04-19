#include <colorcpp/colorcpp.hpp>
#include <iostream>

using namespace colorcpp;
using namespace colorcpp::algorithms::palette;
using namespace colorcpp::operations::palette;
using namespace colorcpp::operations::conversion;
using namespace colorcpp::io::ansi;

static void section(const char* title) { std::cout << '\n' << bold() << "=== " << title << " ===" << reset() << '\n'; }

template <typename Color>
static void dump_palette_row(const char* title, const palette_set<Color>& p) {
  std::cout << title << " (" << p.size() << "): ";
  for (const auto& c : p) print_swatch(std::cout, c, 4);
  std::cout << "\n";
  for (const auto& c : p) std::cout << "  " << std::hex << c << std::dec << "\n";
}

int main() {
  auto base = constants::tomato;

  section("Base");
  print_color(std::cout, base, "tomato");
  std::cout << "\n  HSL: " << color_cast<hsla_float_t>(base) << "\n";

  section("Harmony palettes");
  dump_palette_row("complementary (2)", schemes::complementary(base));
  dump_palette_row("analogous (3)", schemes::analogous(base));
  dump_palette_row("triadic (3)", schemes::triadic(base));
  dump_palette_row("split_comp (3)", schemes::split_complementary(base));
  dump_palette_row("tetradic (4)", schemes::tetradic(base));
  dump_palette_row("square (4)", schemes::square(base));

  section("Monochromatic scale");
  std::cout << "monochromatic (5):\n";
  for (const auto& c : schemes::monochromatic(base, 5)) {
    print_swatch(std::cout, c, 4);
    std::cout << "  " << std::hex << c << std::dec << "  hsl: " << color_cast<hsla_float_t>(c) << "\n";
  }

  section("Linear vs visual scale (tomato → complement)");
  auto comp = schemes::complementary(base);
  std::cout << "linear_scale (5):\n  ";
  for (const auto& c : linear_scale(comp[0], comp[1], 5)) print_swatch(std::cout, c, 3);
  std::cout << "\nvisual_scale (5):\n  ";
  for (const auto& c : visual_scale(comp[0], comp[1], 5)) print_swatch(std::cout, c, 3);
  std::cout << "\n";

  section("palette_set API");
  auto tri = schemes::triadic(base);
  std::cout << "  size=" << tri.size() << "  empty=" << tri.empty() << "  at(0)=";
  print_color(std::cout, tri.at(0));
  std::cout << "\n";

  try {
    tri.at(99);
  } catch (const std::out_of_range& e) {
    std::cout << "  at(99) threw: " << e.what() << "\n";
  }

  std::cout << "  [5] (wraps to [5%3=2])=" << std::hex << tri[5] << std::dec << "\n";

  return 0;
}
