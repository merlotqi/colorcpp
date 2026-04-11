#include <colorcpp/colorcpp.hpp>
#include <iomanip>
#include <iostream>

using namespace colorcpp;
using namespace colorcpp::operations::interpolate;
using namespace colorcpp::operations::conversion;
using namespace colorcpp::io::literals;
using namespace colorcpp::io::ansi;

static void section(const char* s) { std::cout << '\n' << bold() << "=== " << s << " ===" << reset() << '\n'; }

int main() {
  auto red = 0xFF0000_rgb;
  auto blue = 0x0000FF_rgb;

  section("lerp_hsl angle fix (10° ↔ 350°)");
  auto a10 = color_cast<rgba8_t>(10'100'050_hsl);
  auto a350 = color_cast<rgba8_t>(350'100'050_hsl);
  auto mid_rgb = lerp(a10, a350, 0.5f);
  auto mid_hsl = lerp_hsl(a10, a350, 0.5f);
  std::cout << "  H=10: ";
  print_swatch(std::cout, a10, 3);
  std::cout << "  H=350: ";
  print_swatch(std::cout, a350, 3);
  std::cout << "\n  RGB lerp midpoint: ";
  print_swatch(std::cout, mid_rgb, 3);
  std::cout << " " << color_cast<hsl_float_t>(mid_rgb) << "\n";
  std::cout << "  HSL lerp midpoint: ";
  print_swatch(std::cout, mid_hsl, 3);
  std::cout << " " << color_cast<hsl_float_t>(mid_hsl) << "  (hue ~0°)\n";
  std::cout << "  Channel blend a10→a350 (linear RGB between endpoints):\n  ";
  print_gradient(std::cout, a10, a350, 12);
  std::cout << "  Same endpoints via lerp_hsl (short hue arc):\n  ";
  for (int i = 0; i <= 11; ++i) {
    float t = static_cast<float>(i) / 11.0f;
    print_swatch(std::cout, lerp_hsl(a10, a350, t), 2);
  }
  std::cout << '\n';

  section("lerp_hsv");
  auto c1 = lerp_hsv(red, blue, 0.5f);
  std::cout << "  ";
  print_swatch(std::cout, c1, 4);
  std::cout << "  " << std::hex << c1 << std::dec << "  hsv: " << color_cast<hsv_float_t>(c1) << "\n";

  section("easing");
  std::cout << std::fixed << std::setprecision(3);
  for (float t : {0.0f, 0.25f, 0.5f, 0.75f, 1.0f}) {
    std::cout << "t=" << t << "  linear=" << easing::linear(t) << "  in_cubic=" << easing::in_cubic(t)
              << "  out_cubic=" << easing::out_cubic(t) << "  smoothstep=" << easing::smoothstep(t)
              << "  in_expo=" << easing::in_expo(t) << "\n";
  }
  std::cout << std::defaultfloat;

  section("ease / ease_hsl");
  auto c2 = ease(red, blue, 0.5f, easing::in_cubic);
  auto c3 = ease_hsl(red, blue, 0.5f, easing::smoothstep);
  std::cout << "  ease RGB:  ";
  print_swatch(std::cout, c2, 3);
  std::cout << "  " << std::hex << c2 << "\n";
  std::cout << "  ease_hsl:  ";
  print_swatch(std::cout, c3, 3);
  std::cout << "  " << c3 << std::dec << '\n';

  section("multi_lerp");
  auto m1 = multi_lerp<rgba8_t>({red, 0x00FF00_rgb, blue}, 0.0f);
  auto m2 = multi_lerp<rgba8_t>({red, 0x00FF00_rgb, blue}, 0.5f);
  auto m3 = multi_lerp<rgba8_t>({red, 0x00FF00_rgb, blue}, 1.0f);
  std::cout << "  t=0 / 0.5 / 1.0:  ";
  print_swatch(std::cout, m1, 3);
  print_swatch(std::cout, m2, 3);
  print_swatch(std::cout, m3, 3);
  std::cout << '\n';
  std::cout << "  " << std::hex << m1 << "  " << m2 << "  " << m3 << std::dec << '\n';

  auto mh = multi_lerp_hsl<rgba8_t>({red, blue}, 0.5f);
  std::cout << "  multi_lerp_hsl(red,blue,0.5): ";
  print_swatch(std::cout, mh, 3);
  std::cout << "  " << std::hex << mh << std::dec << "  hsl=" << color_cast<hsl_float_t>(mh) << "\n";

  section("lerp_n / lerp_hsl_n");
  std::cout << "  lerp_n(red,blue,5):\n  ";
  for (auto& c : lerp_n(red, blue, 5)) print_swatch(std::cout, c, 2);
  std::cout << '\n';
  for (auto& c : lerp_n(red, blue, 5))
    std::cout << "    " << std::hex << c << std::dec << "  hsl:" << color_cast<hsl_float_t>(c) << "\n";

  std::cout << "  lerp_hsl_n(red,blue,5):\n  ";
  for (auto& c : lerp_hsl_n(red, blue, 5)) print_swatch(std::cout, c, 2);
  std::cout << '\n';
  for (auto& c : lerp_hsl_n(red, blue, 5))
    std::cout << "    " << std::hex << c << std::dec << "  hsl:" << color_cast<hsl_float_t>(c) << "\n";

  return 0;
}
