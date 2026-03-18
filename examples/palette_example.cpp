#include <colorcpp/colorcpp.hpp>
#include <iostream>

using namespace colorcpp;
using namespace colorcpp::operations::palette;
using namespace colorcpp::operations::conversion;

template <typename Color>
static void print_palette(const char* title, const palette_set<Color>& p) {
  std::cout << title << " (" << p.size() << "):\n";
  for (const auto& c : p) std::cout << "  " << std::hex << c << "\n";
  std::cout << std::dec;
}

int main() {
  // constants use _rgb literal which produces rgba8_t (alpha=255)
  auto base = constants::tomato;  // rgba8_t: #FF6347FF (~9° hue)

  std::cout << "Base color: " << std::hex << base << std::dec << "\n";
  std::cout << "  HSL: " << color_cast<hsla_float_t>(base) << "\n\n";

  // --- harmony generators ---
  print_palette("complementary (2)", generate::complementary(base));
  print_palette("analogous (3)", generate::analogous(base));
  print_palette("triadic (3)", generate::triadic(base));
  print_palette("split_comp (3)", generate::split_complementary(base));
  print_palette("tetradic (4)", generate::tetradic(base));
  print_palette("square (4)", generate::square(base));

  // --- monochromatic scale ---
  std::cout << "\nmonochromatic (5):\n";
  for (const auto& c : generate::monochromatic(base, 5))
    std::cout << "  " << std::hex << c << std::dec << "  hsl: " << color_cast<hsla_float_t>(c) << "\n";

  // --- linear vs visual scale ---
  auto comp = generate::complementary(base);
  std::cout << "\nlinear_scale (5) tomato -> complement:\n";
  for (const auto& c : linear_scale(comp[0], comp[1], 5)) std::cout << "  " << std::hex << c << "\n";
  std::cout << std::dec;

  std::cout << "\nvisual_scale (5) tomato -> complement:\n";
  for (const auto& c : visual_scale(comp[0], comp[1], 5)) std::cout << "  " << std::hex << c << "\n";
  std::cout << std::dec;

  // --- palette_set API ---
  std::cout << "\npalette_set API:\n";
  auto tri = generate::triadic(base);
  std::cout << "  size=" << tri.size() << "  empty=" << tri.empty() << "  at(0)=" << std::hex << tri.at(0) << std::dec
            << "\n";

  try {
    tri.at(99);
  } catch (const std::out_of_range& e) {
    std::cout << "  at(99) threw: " << e.what() << "\n";
  }

  // operator[] wraps on out-of-bounds
  std::cout << "  [5] (wraps to [5%3=2])=" << std::hex << tri[5] << std::dec << "\n";
}
