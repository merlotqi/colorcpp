/**
 * @file compare_example.cpp
 * @brief Example: Color comparison strategies (exact, epsilon, relative, perceptual).
 *
 * Demonstrates different color comparison policies available in colorcpp:
 * - exact: Exact channel-by-channel equality
 * - epsilon: Comparison within fixed epsilon tolerance
 * - relative: Comparison within relative tolerance
 * - perceptual: Comparison using ΔE2000 perceptual distance
 */

#include <colorcpp/colorcpp.hpp>
#include <iomanip>
#include <iostream>

#include "colorcpp/operations/conversion.hpp"

using namespace colorcpp;
using namespace colorcpp::operations;
using namespace colorcpp::io::literals;
using namespace colorcpp::io::ansi;

static void section(const char* title) { std::cout << '\n' << bold() << "=== " << title << " ===" << reset() << '\n'; }

int main() {
  std::cout << std::fixed << std::setprecision(4);

  // ========================================================================
  section("1. Exact Comparison");

  auto c1 = 0xFF8000_rgb;
  auto c2 = 0xFF8000_rgb;
  auto c3 = 0xFF8001_rgb;

  std::cout << "  exact_equal(#FF8000, #FF8000): " << std::boolalpha << compare::exact_equal(c1, c2) << '\n';
  std::cout << "  exact_equal(#FF8000, #FF8001): " << std::boolalpha << compare::exact_equal(c1, c3) << '\n';

  // ========================================================================
  section("2. Epsilon Comparison");

  auto a = 0xFF8000_rgb;
  auto b = 0xFE8001_rgb;  // slightly different

  // Convert to float for epsilon comparison
  auto af = conversion::color_cast<core::rgbaf_t>(a);
  auto bf = conversion::color_cast<core::rgbaf_t>(b);

  std::cout << "  Colors: #FF8000 vs #FE8001\n\n";

  // With different epsilon values
  std::cout << "  epsilon_equal(eps=0.0):  " << compare::epsilon_equal(af, bf, 0.0f) << '\n';
  std::cout << "  epsilon_equal(eps=0.01): " << compare::epsilon_equal(af, bf, 0.01f) << '\n';
  std::cout << "  epsilon_equal(eps=0.02): " << compare::epsilon_equal(af, bf, 0.02f) << '\n';

  // ========================================================================
  section("3. Relative Comparison");

  // Dark colors: small absolute difference, large relative difference
  auto dark1 = 0x010101_rgb;
  auto dark2 = 0x020202_rgb;

  // Bright colors: same absolute difference, small relative difference
  auto bright1 = 0xC8C8C8_rgb;
  auto bright2 = 0xC9C9C9_rgb;

  auto dark1f = conversion::color_cast<core::rgbaf_t>(dark1);
  auto dark2f = conversion::color_cast<core::rgbaf_t>(dark2);
  auto bright1f = conversion::color_cast<core::rgbaf_t>(bright1);
  auto bright2f = conversion::color_cast<core::rgbaf_t>(bright2);

  std::cout << "  Dark pair:  #010101 vs #020202\n";
  std::cout << "    relative_equal(rel=0.1): " << compare::relative_equal(dark1f, dark2f, 0.1f) << '\n';
  std::cout << "    relative_equal(rel=0.5): " << compare::relative_equal(dark1f, dark2f, 0.5f) << '\n';
  std::cout << "    relative_equal(rel=1.0): " << compare::relative_equal(dark1f, dark2f, 1.0f) << '\n';

  std::cout << "\n  Bright pair: #C8C8C8 vs #C9C9C9\n";
  std::cout << "    relative_equal(rel=0.1): " << compare::relative_equal(bright1f, bright2f, 0.1f) << '\n';
  std::cout << "    relative_equal(rel=0.5): " << compare::relative_equal(bright1f, bright2f, 0.5f) << '\n';
  std::cout << "    relative_equal(rel=1.0): " << compare::relative_equal(bright1f, bright2f, 1.0f) << '\n';

  // ========================================================================
  section("4. Perceptual Comparison (ΔE2000)");

  auto red = 0xFF0000_rgb;
  auto orange = 0xFF8000_rgb;
  auto red_close = 0xFE0102_rgb;

  std::cout << "  perceptual_equal(#FF0000, #FF8000, threshold=10): " << compare::perceptual_equal(red, orange, 10.0f)
            << '\n';
  std::cout << "  perceptual_equal(#FF0000, #FF8000, threshold=30): " << compare::perceptual_equal(red, orange, 30.0f)
            << '\n';
  std::cout << "  perceptual_equal(#FF0000, #FE0102, threshold=1):  " << compare::perceptual_equal(red, red_close, 1.0f)
            << '\n';
  std::cout << "  perceptual_equal(#FF0000, #FE0102, threshold=3):  " << compare::perceptual_equal(red, red_close, 3.0f)
            << '\n';

  // ========================================================================
  section("5. Generic compare::equal with Policy");

  auto x = 0x336699_rgb;
  auto y = 0x33669A_rgb;

  std::cout << "  Colors: #336699 vs #33669A\n\n";

  // Default (exact)
  std::cout << "  compare::equal():              " << compare::equal(x, y) << '\n';

  // Epsilon with custom value
  auto xf = conversion::color_cast<core::rgbaf_t>(x);
  auto yf = conversion::color_cast<core::rgbaf_t>(y);
  std::cout << "  compare::equal<epsilon>(0.01): " << compare::equal<compare::epsilon_policy>(xf, yf, 0.01f) << '\n';

  // Perceptual
  std::cout << "  compare::equal<perceptual>(5): " << compare::equal<compare::perceptual_policy>(x, y, 5.0f) << '\n';

  // ========================================================================
  section("6. Practical Use Case: Color Palette Deduplication");

  std::vector<rgba8_t> palette = {
      0xFF0000_rgb, 0xFF0001_rgb,  // nearly identical reds
      0x00FF00_rgb,                // green
      0x0000FF_rgb, 0x0000FE_rgb,  // nearly identical blues
      0xFFFF00_rgb                 // yellow
  };

  std::cout << "  Original palette size: " << palette.size() << '\n';

  // Deduplicate using exact comparison
  std::vector<rgba8_t> deduped_exact;
  for (const auto& c : palette) {
    bool found = false;
    for (const auto& d : deduped_exact) {
      if (compare::exact_equal(c, d)) {
        found = true;
        break;
      }
    }
    if (!found) deduped_exact.push_back(c);
  }
  std::cout << "  After exact dedup:     " << deduped_exact.size() << '\n';

  // Deduplicate using perceptual comparison (threshold=1.0)
  std::vector<rgba8_t> deduped_perceptual;
  for (const auto& c : palette) {
    bool found = false;
    for (const auto& d : deduped_perceptual) {
      if (compare::perceptual_equal(c, d, 1.0f)) {
        found = true;
        break;
      }
    }
    if (!found) deduped_perceptual.push_back(c);
  }
  std::cout << "  After perceptual(1.0): " << deduped_perceptual.size() << '\n';

  std::cout << "\n";
  return 0;
}
