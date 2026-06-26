/**
 * @file rec2020_example.cpp
 * @brief Demonstrates Rec.2020 (BT.2020) wide-gamut color space usage.
 *
 * Build: cmake -DCOLORCPP_BUILD_EXAMPLES=ON && make rec2020_example
 */

#include <colorcpp/colorcpp.hpp>
#include <iostream>

using namespace colorcpp;

int main() {
  std::cout << "=== Rec.2020 (BT.2020) Wide-Gamut Color Space ===\n\n";

  // 1. Construct Rec.2020 colors
  std::cout << "1. Construction\n";
  core::rec2020_rgbf_t red(0.8f, 0.1f, 0.1f);
  core::rec2020_rgbf_t green(0.1f, 0.8f, 0.1f);
  core::rec2020_rgbf_t blue(0.1f, 0.1f, 0.8f);
  std::cout << "   Red:   " << red << "\n";
  std::cout << "   Green: " << green << "\n";
  std::cout << "   Blue:  " << blue << "\n\n";

  // 2. Gamut hierarchy: sRGB ⊂ Display P3 ⊂ Adobe RGB ⊂ Rec.2020 ⊂ ProPhoto
  std::cout << "2. Gamut hierarchy comparison\n";
  core::rgbf_t test_color(0.0f, 1.0f, 0.0f);  // sRGB pure green

  auto dp3 = conversion::color_cast<core::display_p3f_t>(test_color);
  auto argb = conversion::color_cast<core::adobe_rgbf_t>(test_color);
  auto r2020 = conversion::color_cast<core::rec2020_rgbf_t>(test_color);
  auto pprgb = conversion::color_cast<core::prophoto_rgbf_t>(test_color);

  std::cout << "   sRGB green:\n";
  std::cout << "     -> Display P3:  " << dp3 << "\n";
  std::cout << "     -> Adobe RGB:   " << argb << "\n";
  std::cout << "     -> Rec.2020:    " << r2020 << "\n";
  std::cout << "     -> ProPhoto:    " << pprgb << "\n";
  std::cout << "   (Notice how the same sRGB green maps to different values\n";
  std::cout << "    in each space due to different primaries)\n\n";

  // 3. Round-trip fidelity
  std::cout << "3. Rec.2020 round-trip fidelity\n";
  core::rec2020_rgbf_t orig(0.5f, 0.3f, 0.7f);
  auto via_srgb = conversion::color_cast<core::rec2020_rgbf_t>(conversion::color_cast<core::rgbf_t>(orig));
  std::cout << "   Original:  " << orig << "\n";
  std::cout << "   Via sRGB:   " << via_srgb << "\n\n";

  // 4. Transfer function
  std::cout << "4. BT.709-style piecewise transfer function\n";
  core::rec2020_rgbf_t mid(0.5f, 0.5f, 0.5f);
  auto linear = conversion::color_cast<core::linear_rec2020_rgbf_t>(mid);
  std::cout << "   Rec.2020 (gamma): " << mid << "\n";
  std::cout << "   Linear Rec.2020:   " << linear << "\n";
  std::cout << "   (alpha=1.0993, beta=0.0181 -- slightly different from sRGB)\n\n";

  // 5. CSS parsing
  std::cout << "5. CSS color(rec2020 ...) parsing\n";
  using namespace colorcpp::io::css;
  auto css = parse_css_color<core::rec2020_rgbaf_t>("color(rec2020 0.64 0.33 0.21)");
  if (css) {
    std::cout << "   Parsed: " << *css << "\n";
    std::cout << "   CSS string (sRGB fallback): " << to_css_color_string(conversion::color_cast<core::rgbaf_t>(*css))
              << "\n";
  }
  std::cout << "\n";

  // 6. Rec.2020 is widely used in UHD/HDR
  std::cout << "6. Rec.2020 fact\n";
  std::cout << "   Rec.2020 covers ~75% of CIE 1931 visible gamut.\n";
  std::cout << "   It is the standard color space for 4K/8K UHDTV.\n\n";

  std::cout << "Done.\n";
  return 0;
}
