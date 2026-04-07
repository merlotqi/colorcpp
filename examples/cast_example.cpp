#include <colorcpp/colorcpp.hpp>
#include <iostream>

using namespace colorcpp;
using namespace colorcpp::operations::conversion;
using namespace colorcpp::io::literals;
using namespace colorcpp::io::ansi;

static void section(const char* title) {
  std::cout << '\n' << bold() << "=== " << title << " ===" << reset() << '\n';
}

int main() {
  section("RGBA and channel casts");
  auto red_rgba = constants::thistle;
  print_color(std::cout, red_rgba, "thistle");
  std::cout << "\n";
  auto red_rgb3 = color_cast<rgb8_t>(red_rgba);
  std::cout << "rgba → rgb8: " << red_rgb3 << "\n";

  section("HSL / HSV (3- and 4-channel)");
  auto hsl3 = color_cast<hsl_float_t>(red_rgba);
  auto hsla4 = color_cast<hsla_float_t>(red_rgba);
  std::cout << "rgb → hsl_float_t:   " << hsl3 << "\n";
  std::cout << "rgb → hsla_float_t:  " << hsla4 << "\n";

  auto hsv3 = color_cast<hsv_float_t>(red_rgba);
  auto hsva4 = color_cast<hsva_float_t>(red_rgba);
  std::cout << "rgb → hsv_float_t:   " << hsv3 << "\n";
  std::cout << "rgb → hsva_float_t:  " << hsva4 << "\n";

  section("CMYK");
  auto cmyk = color_cast<cmyk8_t>(red_rgba);
  auto cmykf = color_cast<cmyk_float_t>(red_rgba);
  std::cout << "rgb → cmyk8_t:       " << cmyk << "\n";
  std::cout << "rgb → cmyk_float_t:  " << cmykf << "\n";

  auto back_rgb = color_cast<rgba8_t>(cmyk);
  auto back_rgb3 = color_cast<rgb8_t>(cmyk);
  std::cout << "cmyk8 → rgba8_t:     " << std::hex << back_rgb << "\n";
  std::cout << "cmyk8 → rgb8_t:      " << back_rgb3 << "\n" << std::dec;
  print_color(std::cout, back_rgb, "CMYK roundtrip");
  std::cout << "\n";

  section("HSL ↔ CMYK (via RGB)");
  auto cyan_hsl = 180'100'050_hsl;
  auto cyan_cmyk = color_cast<cmyk8_t>(cyan_hsl);
  std::cout << "hsl(180,100,50) → cmyk8: " << cyan_cmyk << "\n";
  auto back_hsl = color_cast<hsl_float_t>(cyan_cmyk);
  std::cout << "cmyk8 → hsl_float_t:     " << back_hsl << "\n";
  print_color(std::cout, color_cast<rgba8_t>(cyan_hsl), "cyan (HSL)");
  std::cout << "  ";
  print_color(std::cout, color_cast<rgba8_t>(cyan_cmyk), "via CMYK");
  std::cout << "\n";

  section("Precision cast (same family)");
  auto cmyk_f2 = color_cast<cmyk_float_t>(cmyk);
  std::cout << "cmyk8 → cmyk_float_t:   " << cmyk_f2 << "\n";

  return 0;
}
