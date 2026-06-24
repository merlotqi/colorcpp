/**
 * @file chromatic_adaptation_example.cpp
 * @brief Demonstrates chromatic adaptation (white point conversion).
 *
 * Build: cmake -DCOLORCPP_BUILD_EXAMPLES=ON && make chromatic_adaptation_example
 */

#include <iostream>
#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;

int main() {
  std::cout << "=== Chromatic Adaptation (White Point Conversion) ===\n\n";

  using namespace algorithms::chromatic_adaptation;

  // 1. White point constants
  std::cout << "1. Standard white points (CIE XYZ, Y=1)\n";
  std::cout << "   D65 (noon daylight):     " << WHITEPOINT_D65 << "\n";
  std::cout << "   D50 (horizon daylight):  " << WHITEPOINT_D50 << "\n";
  std::cout << "   A   (tungsten):          " << WHITEPOINT_A << "\n";
  std::cout << "   D55 (mid-day):           " << WHITEPOINT_D55 << "\n\n";

  // 2. Bradford D65 → D50 adaptation
  std::cout << "2. Bradford adaptation: D65 → D50\n";
  core::xyz_t color_d65(0.5f, 0.4f, 0.3f);
  auto color_d50 = bradford_adapt(color_d65, WHITEPOINT_D65, WHITEPOINT_D50);
  std::cout << "   Color under D65: " << color_d65 << "\n";
  std::cout << "   → under D50:     " << color_d50 << "\n\n";

  // 3. Round-trip verification
  std::cout << "3. Round-trip: D65 → D50 → D65\n";
  auto back_to_d65 = bradford_adapt(color_d50, WHITEPOINT_D50, WHITEPOINT_D65);
  std::cout << "   Original:  " << color_d65 << "\n";
  std::cout << "   After D65→D50→D65: " << back_to_d65 << "\n";
  std::cout << "   (Should match original within floating-point precision)\n\n";

  // 4. Tungsten (A) simulation — warm color shift
  std::cout << "4. Daylight → Tungsten (D65 → A) — warm shift\n";
  core::rgbf_t white(1.0f, 1.0f, 1.0f);
  auto warm_white = bradford_adapt(white, WHITEPOINT_D65, WHITEPOINT_A);
  std::cout << "   White under D65:    " << white << "\n";
  std::cout << "   → under Tungsten A: " << warm_white << "\n";
  std::cout << "   (Expect warm/yellowish shift)\n\n";

  // 5. Von Kries comparison
  std::cout << "5. Bradford vs Von Kries (D65 → A)\n";
  core::xyz_t test(0.3f, 0.5f, 0.2f);
  auto brad = bradford_adapt(test, WHITEPOINT_D65, WHITEPOINT_A);
  auto vk = von_kries_adapt(test, WHITEPOINT_D65, WHITEPOINT_A);
  std::cout << "   Original:        " << test << "\n";
  std::cout << "   Bradford(D65→A): " << brad << "\n";
  std::cout << "   Von Kries(D65→A): " << vk << "\n";
  std::cout << "   (Bradford is more perceptually accurate)\n\n";

  // 6. ANSI swatches
  std::cout << "6. Visual comparison\n";
  auto white_d65_rgb = conversion::color_cast<core::rgb8_t>(core::rgbf_t{1.0f, 1.0f, 1.0f});
  std::cout << "   D65 white  ";
  colorcpp::io::ansi::print_swatch(std::cout, white_d65_rgb);
  std::cout << "\n";

  auto tungsten_rgb = conversion::color_cast<core::rgb8_t>(
      bradford_adapt(core::rgbf_t{1.0f, 1.0f, 1.0f}, WHITEPOINT_D65, WHITEPOINT_A));
  std::cout << "   Tungsten A ";
  colorcpp::io::ansi::print_swatch(std::cout, tungsten_rgb);
  std::cout << "\n\n";

  std::cout << "Done.\n";
  return 0;
}
