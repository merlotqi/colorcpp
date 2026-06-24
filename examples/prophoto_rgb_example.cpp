/**
 * @file prophoto_rgb_example.cpp
 * @brief Demonstrates ProPhoto RGB (ROMM RGB) wide-gamut color space usage.
 *
 * Build: cmake -DCOLORCPP_BUILD_EXAMPLES=ON && make prophoto_rgb_example
 */

#include <colorcpp/colorcpp.hpp>
#include <iostream>

using namespace colorcpp;

int main() {
  std::cout << "=== ProPhoto RGB (ROMM RGB) Wide-Gamut Color Space ===\n\n";

  // 1. Construct ProPhoto RGB colors
  std::cout << "1. Construction\n";
  core::prophoto_rgbf_t red(0.8f, 0.1f, 0.1f);
  core::prophoto_rgbf_t green(0.1f, 0.8f, 0.1f);
  core::prophoto_rgbf_t blue(0.1f, 0.1f, 0.8f);
  std::cout << "   Red:   " << red << "\n";
  std::cout << "   Green: " << green << "\n";
  std::cout << "   Blue:  " << blue << "\n\n";

  // 2. sRGB → ProPhoto → sRGB (gamut expansion)
  std::cout << "2. sRGB ↔ ProPhoto RGB round-trip\n";
  core::rgbf_t srgb_red(1.0f, 0.0f, 0.0f);
  auto pprgb = conversion::color_cast<core::prophoto_rgbf_t>(srgb_red);
  auto back = conversion::color_cast<core::rgbf_t>(pprgb);
  std::cout << "   sRGB red:          " << srgb_red << "\n";
  std::cout << "   → ProPhoto RGB:    " << pprgb << "\n";
  std::cout << "   → back to sRGB:    " << back << "\n\n";

  // 3. D50 white point / chromatic adaptation
  std::cout << "3. Chromatic adaptation (D50 ↔ D65)\n";
  core::xyz_t white_d65 = algorithms::chromatic_adaptation::WHITEPOINT_D65;
  core::xyz_t white_d50 = algorithms::chromatic_adaptation::WHITEPOINT_D50;
  std::cout << "   D65 white: " << white_d65 << "\n";
  std::cout << "   D50 white: " << white_d50 << "\n";
  auto d65_to_d50 = algorithms::chromatic_adaptation::bradford_adapt(white_d65, white_d65, white_d50);
  std::cout << "   Bradford(D65→D50): " << d65_to_d50 << "\n\n";

  // 4. Gamut check
  std::cout << "4. Gamut membership\n";
  core::rgbf_t in_gamut(0.5f, 0.5f, 0.5f);
  std::cout << "   Gray 0.5 in ProPhoto? " << std::boolalpha << algorithms::gamut::is_in_prophoto_gamut(in_gamut)
            << "\n";
  // ProPhoto is huge — most sRGB colors are well inside it
  std::cout << "   sRGB red in ProPhoto? " << algorithms::gamut::is_in_prophoto_gamut(srgb_red) << "\n\n";

  // 5. CSS parsing
  std::cout << "5. CSS color(prophoto-rgb ...) parsing\n";
  auto css = colorcpp::io::css::parse_css_color<core::prophoto_rgbaf_t>("color(prophoto-rgb 0.5 0.3 0.7 / 0.9)");
  if (css) {
    std::cout << "   Parsed: " << *css << "\n";
    std::cout << "   CSS string: "
              << colorcpp::io::css::to_css_color_string(conversion::color_cast<core::rgbaf_t>(*css)) << "\n";
  }
  std::cout << "\n";

  // 6. Gamma encoding
  std::cout << "6. Gamma encoding (Gamma 1.8 with linear segment)\n";
  auto linear_pp = conversion::color_cast<core::linear_prophoto_rgbf_t>(core::prophoto_rgbf_t{0.5f, 0.5f, 0.5f});
  std::cout << "   ProPhoto (gamma):  " << core::prophoto_rgbf_t{0.5f, 0.5f, 0.5f} << "\n";
  std::cout << "   Linear ProPhoto:   " << linear_pp << "\n";
  std::cout << "   (Gamma 1.8 is gentler than sRGB's ~2.2)\n\n";

  std::cout << "Done.\n";
  return 0;
}
