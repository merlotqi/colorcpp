/**
 * @file color_temperature_example.cpp
 * @brief Example: CCT (Kelvin) to sRGB and rough inverse estimate, with ANSI swatches.
 */

#include <colorcpp/colorcpp.hpp>
#include <iomanip>
#include <iostream>

using namespace colorcpp;
using namespace colorcpp::algorithms::color_temperature;
using namespace colorcpp::operations::conversion;
using namespace colorcpp::io::ansi;

static void section(const char* title) {
  std::cout << '\n' << bold() << "=== " << title << " ===" << reset() << '\n';
}

int main() {
  std::cout << std::fixed << std::setprecision(0);

  section("kelvin_to_rgba8 (blackbody-style sRGB)");
  for (float k : {2000.f, 2700.f, 4000.f, 5500.f, 6500.f, 10000.f}) {
    auto c = kelvin_to_rgba8(k);
    print_color(std::cout, c);
    std::cout << "  " << k << " K\n";
  }

  section("Gradient: warm to cool (2000 K → 12000 K)");
  print_gradient(std::cout, kelvin_to_rgba8(2000.f), kelvin_to_rgba8(12000.f), 24);

  section("rgb_to_kelvin_estimate (sRGB → scalar K, best on near-Planckian tones)");
  auto ref = kelvin_to_rgba8(5000.f);
  float est = rgb_to_kelvin_estimate(color_cast<rgbf_t>(ref));
  print_color(std::cout, ref, "5000 K reference");
  std::cout << "\n  estimated K from sRGB: " << est << '\n';

  return 0;
}
