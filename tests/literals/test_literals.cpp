#include <colorcpp/colorcpp.hpp>
#include <colorcpp/literals/literals.hpp>
#include <iostream>

using namespace colorcpp::literals;
using namespace colorcpp;

int main() {
  // RGB (numeric + template + hex string)
  std::cout << "=== RGB ===\n";
  std::cout << std::hex;
  std::cout << "0xFF6347_rgb:       " << 0xFF6347_rgb << "\n";     // rgba8
  std::cout << "0xFF634780_rgba:    " << 0xFF634780_rgba << "\n";  // rgba8 alpha=128
  std::cout << "0x80FF6347_argb:    " << 0x80FF6347_argb << "\n";  // same as above, ARGB order
  std::cout << "\"#FF6347\"_hex:      " << "#FF6347"_hex << "\n";  // string form
  std::cout << "\"#F63\"_hex:         " << "#F63"_hex << "\n";     // shorthand #RGB
  std::cout << std::dec;

  // HSL (template only — avoids octal ambiguity)
  std::cout << "\n=== HSL ===\n";
  std::cout << "120'050'075_hsl:       " << 120'050'075_hsl << "\n";       // H=120 S=50 L=75
  std::cout << "120'050'075'100_hsla:  " << 120'050'075'100_hsla << "\n";  // +alpha=100
  std::cout << "0'100'050_hsl:         " << 0'100'050_hsl << "\n";         // H=0, S=100, L=50 (no octal trap)
  std::cout << "360'000'050_hsl:       " << 360'000'050_hsl << "\n";       // H=360 edge

  // HSV
  std::cout << "\n=== HSV ===\n";
  std::cout << "210'080'090_hsv:       " << 210'080'090_hsv << "\n";
  std::cout << "210'080'090'075_hsva:  " << 210'080'090'075_hsva << "\n";

  // CMYK
  std::cout << "\n=== CMYK ===\n";
  std::cout << "50'030'000'020_cmyk:   " << 50'030'000'020_cmyk << "\n";  // C=50 M=30 Y=0 K=20
  std::cout << "0'0'0'0_cmyk:          " << 0'0'0'0_cmyk << "\n";         // all zero
}
