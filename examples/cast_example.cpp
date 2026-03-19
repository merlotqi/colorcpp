#include <colorcpp/colorcpp.hpp>
#include <iostream>

using namespace colorcpp;
using namespace colorcpp::operations::conversion;
using namespace colorcpp::literals;

int main() {
  auto red_rgba = constants::thistle;
  std::cout << "red_rgba:   " << red_rgba << "\n";
  auto red_rgb3 = color_cast<rgb8_t>(red_rgba);  // 3-channel, drop alpha
  std::cout << "rgba → rgb:   " << red_rgb3 << "\n";

  // Bug 1 fix: 3-channel HSL targets (was compile error before)
  auto hsl3 = color_cast<hsl_float_t>(red_rgba);    // rgb → hsl (3-ch)
  auto hsla4 = color_cast<hsla_float_t>(red_rgba);  // rgb → hsla (4-ch)
  std::cout << "rgb → hsl_float_t:   " << hsl3 << "\n";
  std::cout << "rgb → hsla_float_t:  " << hsla4 << "\n";

  // HSV 3/4-channel
  auto hsv3 = color_cast<hsv_float_t>(red_rgba);
  auto hsva4 = color_cast<hsva_float_t>(red_rgba);
  std::cout << "rgb → hsv_float_t:   " << hsv3 << "\n";
  std::cout << "rgb → hsva_float_t:  " << hsva4 << "\n";

  // CMYK (new paths)
  auto cmyk = color_cast<cmyk8_t>(red_rgba);
  auto cmykf = color_cast<cmyk_float_t>(red_rgba);
  std::cout << "\nrgb → cmyk8_t:       " << cmyk << "\n";
  std::cout << "rgb → cmyk_float_t:  " << cmykf << "\n";

  // CMYK → RGB roundtrip
  auto back_rgb = color_cast<rgba8_t>(cmyk);
  auto back_rgb3 = color_cast<rgb8_t>(cmyk);
  std::cout << "cmyk8 → rgba8_t:     " << std::hex << back_rgb << "\n";
  std::cout << "cmyk8 → rgb8_t:      " << back_rgb3 << "\n" << std::dec;

  // Two-hop (HSL ↔ CMYK via RGBaf — was static_assert "unsupported" before)
  auto cyan_hsl = 180'100'050_hsl;
  auto cyan_cmyk = color_cast<cmyk8_t>(cyan_hsl);
  std::cout << "\nhsl(180,100,50) → cmyk8: " << cyan_cmyk << "\n";
  auto back_hsl = color_cast<hsl_float_t>(cyan_cmyk);
  std::cout << "cmyk8 → hsl_float_t:     " << back_hsl << "\n";

  // Same-category precision cast: cmyk8_t → cmyk_float_t
  auto cmyk_f2 = color_cast<cmyk_float_t>(cmyk);
  std::cout << "\ncmyk8 → cmyk_float_t:   " << cmyk_f2 << "\n";
}
