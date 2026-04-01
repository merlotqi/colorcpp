#include <colorcpp/colorcpp.hpp>
#include <iomanip>
#include <iostream>

using namespace colorcpp;
using namespace colorcpp::operations::interpolate;
using namespace colorcpp::operations::conversion;
using namespace colorcpp::io::literals;

void section(const char* s) { std::cout << "\n=== " << s << " ===\n"; }

int main() {
  auto red = 0xFF0000_rgb;  // rgba8
  auto blue = 0x0000FF_rgb;

  // lerp_hsl bug fix: 10° ↔ 350° should pass through 0°, not 180°
  section("lerp_hsl angle fix");
  auto a10 = color_cast<rgba8_t>(10'100'050_hsl);    // H=10
  auto a350 = color_cast<rgba8_t>(350'100'050_hsl);  // H=350
  auto mid_rgb = lerp(a10, a350, 0.5f);
  auto mid_hsl = lerp_hsl(a10, a350, 0.5f);
  std::cout << "RGB  lerp(10°,350°,0.5): " << color_cast<hsl_float_t>(mid_rgb) << "\n";
  std::cout << "HSL  lerp(10°,350°,0.5): " << color_cast<hsl_float_t>(mid_hsl) << "  (hue should be ~0)\n";

  // lerp_hsv (new)
  section("lerp_hsv");
  auto c1 = lerp_hsv(red, blue, 0.5f);
  std::cout << "hsv lerp(red,blue,0.5): " << std::hex << c1 << std::dec << "  hsv: " << color_cast<hsv_float_t>(c1)
            << "\n";

  // easing functions
  section("easing");
  std::cout << std::fixed << std::setprecision(3);
  for (float t : {0.0f, 0.25f, 0.5f, 0.75f, 1.0f}) {
    std::cout << "t=" << t << "  linear=" << easing::linear(t) << "  in_cubic=" << easing::in_cubic(t)
              << "  out_cubic=" << easing::out_cubic(t) << "  smoothstep=" << easing::smoothstep(t)
              << "  in_expo=" << easing::in_expo(t) << "\n";
  }
  std::cout << std::defaultfloat;

  // ease + ease_hsl
  section("ease / ease_hsl");
  auto c2 = ease(red, blue, 0.5f, easing::in_cubic);
  auto c3 = ease_hsl(red, blue, 0.5f, easing::smoothstep);
  std::cout << "ease(red,blue,0.5,in_cubic):       " << std::hex << c2 << "\n";
  std::cout << "ease_hsl(red,blue,0.5,smoothstep): " << c3 << "\n" << std::dec;

  // multi_lerp
  section("multi_lerp");
  auto m1 = multi_lerp<rgba8_t>({red, 0x00FF00_rgb, blue}, 0.0f);
  auto m2 = multi_lerp<rgba8_t>({red, 0x00FF00_rgb, blue}, 0.5f);
  auto m3 = multi_lerp<rgba8_t>({red, 0x00FF00_rgb, blue}, 1.0f);
  std::cout << "stops=[red,green,blue] t=0:   " << std::hex << m1 << "\n";
  std::cout << "stops=[red,green,blue] t=0.5: " << m2 << "\n";
  std::cout << "stops=[red,green,blue] t=1.0: " << m3 << "\n" << std::dec;

  // multi_lerp_hsl
  auto mh = multi_lerp_hsl<rgba8_t>({red, blue}, 0.5f);
  std::cout << "multi_lerp_hsl(red,blue,0.5): " << std::hex << mh << " hsl=" << color_cast<hsl_float_t>(mh) << "\n"
            << std::dec;

  // lerp_n / lerp_hsl_n
  section("lerp_n / lerp_hsl_n");
  std::cout << "lerp_n(red,blue,5):\n";
  for (auto& c : lerp_n(red, blue, 5))
    std::cout << "  " << std::hex << c << std::dec << "  hsl:" << color_cast<hsl_float_t>(c) << "\n";

  std::cout << "lerp_hsl_n(red,blue,5):\n";
  for (auto& c : lerp_hsl_n(red, blue, 5))
    std::cout << "  " << std::hex << c << std::dec << "  hsl:" << color_cast<hsl_float_t>(c) << "\n";
}
