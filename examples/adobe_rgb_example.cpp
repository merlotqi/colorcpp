/**
 * @file adobe_rgb_example.cpp
 * @brief Demonstrates Adobe RGB (A98-RGB) wide-gamut color space usage.
 *
 * Build: cmake -DCOLORCPP_BUILD_EXAMPLES=ON && make adobe_rgb_example
 */

#include <iostream>
#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;
using namespace colorcpp::io::ansi;

int main() {
  std::cout << "=== Adobe RGB (A98-RGB) Wide-Gamut Color Space ===\n\n";

  // 1. Construct Adobe RGB colors
  std::cout << "1. Construction\n";
  core::adobe_rgbf_t pure_red(1.0f, 0.0f, 0.0f);
  core::adobe_rgbf_t pure_green(0.0f, 1.0f, 0.0f);
  core::adobe_rgbf_t pure_blue(0.0f, 0.0f, 1.0f);
  std::cout << "   Red:   " << pure_red << "\n";
  std::cout << "   Green: " << pure_green << "\n";
  std::cout << "   Blue:  " << pure_blue << "\n\n";

  // 2. Convert sRGB → Adobe RGB → sRGB (show gamut differences)
  std::cout << "2. sRGB ↔ Adobe RGB round-trip\n";
  core::rgbf_t srgb_green(0.0f, 1.0f, 0.0f);
  auto adobe_green = conversion::color_cast<core::adobe_rgbf_t>(srgb_green);
  auto back_to_srgb = conversion::color_cast<core::rgbf_t>(adobe_green);
  std::cout << "   sRGB green:        " << srgb_green << "\n";
  std::cout << "   → Adobe RGB:       " << adobe_green << "\n";
  std::cout << "   → back to sRGB:    " << back_to_srgb << "\n\n";

  // 3. Gamma encoding: show linear values
  std::cout << "3. Gamma encoding (Adobe RGB → Linear Adobe RGB)\n";
  auto linear_adobe = conversion::color_cast<core::linear_adobe_rgbf_t>(adobe_green);
  std::cout << "   Adobe RGB (gamma): " << adobe_green << "\n";
  std::cout << "   Linear Adobe RGB:   " << linear_adobe << "\n";
  std::cout << "   (Pure gamma 2.2 — mid-tones are darker in linear)\n\n";

  // 4. Cross-space chain: Adobe RGB → Display P3 → Rec.2020 → OKLab
  std::cout << "4. Cross-space chain: Adobe RGB → Display P3 → Rec.2020 → OKLab\n";
  core::adobe_rgbf_t test_color(0.7f, 0.2f, 0.4f);
  auto dp3 = conversion::color_cast<core::display_p3f_t>(test_color);
  auto r2020 = conversion::color_cast<core::rec2020_rgbf_t>(dp3);
  auto oklab = conversion::color_cast<core::oklab_t>(r2020);
  std::cout << "   Input (Adobe RGB): " << test_color << "\n";
  std::cout << "   → Display P3:      " << dp3 << "\n";
  std::cout << "   → Rec.2020:        " << r2020 << "\n";
  std::cout << "   → OKLab:           " << oklab << "\n\n";

  // 5. CSS parsing
  std::cout << "5. CSS color(a98-rgb ...) parsing\n";
  auto css = colorcpp::io::css::parse_css_color<core::adobe_rgbaf_t>("color(a98-rgb 0.64 0.33 0.21)");
  if (css) {
    std::cout << "   Parsed: " << *css << "\n";
    std::cout << "   CSS string: " << colorcpp::io::css::to_css_color_string(conversion::color_cast<core::rgbaf_t>(*css)) << "\n";
  }
  std::cout << "\n";

  // 6. ANSI swatch
  std::cout << "6. Color swatches\n";
  std::cout << "   Adobe Red  ";
  print_swatch(std::cout, conversion::color_cast<core::rgb8_t>(core::adobe_rgbf_t{0.64f, 0.1f, 0.1f}));
  std::cout << "\n   Adobe Green";
  print_swatch(std::cout, conversion::color_cast<core::rgb8_t>(core::adobe_rgbf_t{0.1f, 0.64f, 0.1f}));
  std::cout << "\n   Adobe Blue ";
  print_swatch(std::cout, conversion::color_cast<core::rgb8_t>(core::adobe_rgbf_t{0.1f, 0.1f, 0.64f}));
  std::cout << "\n";

  std::cout << "Done.\n";
  return 0;
}
