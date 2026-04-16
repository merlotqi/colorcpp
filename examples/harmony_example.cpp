/**
 * @file harmony_example.cpp
 * @brief Example: Color harmony analysis and correction.
 *
 * Demonstrates harmony assessment, scheme detection, and suggestions
 * for improving color palette harmony.
 */

#include <colorcpp/colorcpp.hpp>
#include <iomanip>
#include <iostream>

using namespace colorcpp;
using namespace colorcpp::algorithms::harmony;
using namespace colorcpp::operations::palette;
using namespace colorcpp::io::ansi;
using namespace colorcpp::io::literals;

static void section(const char* title) { std::cout << '\n' << bold() << "=== " << title << " ===" << reset() << '\n'; }

static void print_palette(const palette_set<rgba8_t>& palette, const char* label) {
  std::cout << "  " << label << ": ";
  for (const auto& c : palette) {
    print_swatch(std::cout, c, 2);
    std::cout << " ";
  }
  std::cout << '\n';
}

int main() {
  std::cout << std::fixed << std::setprecision(2);

  // ========================================================================
  section("1. Complementary Harmony");

  {
    palette_set<rgba8_t> palette = {
        0xFF0000_rgb,  // Red
        0x00FFFF_rgb,  // Cyan (complementary)
    };
    print_palette(palette, "Complementary");
    auto result = assess(palette);
    std::cout << "  Score: " << result.score << "  Scheme: " << to_string(result.scheme)
              << "  Deviation: " << result.deviation << "°\n";
  }

  // ========================================================================
  section("2. Triadic Harmony");

  {
    palette_set<rgba8_t> palette = {
        0xFF0000_rgb,  // Red (0°)
        0x00FF00_rgb,  // Green (120°)
        0x0000FF_rgb,  // Blue (240°)
    };
    print_palette(palette, "Triadic");
    auto result = assess(palette);
    std::cout << "  Score: " << result.score << "  Scheme: " << to_string(result.scheme)
              << "  Deviation: " << result.deviation << "°\n";
  }

  // ========================================================================
  section("3. Analogous Harmony");

  {
    palette_set<rgba8_t> palette = {
        0xFF0000_rgb,  // Red (0°)
        0xFF8000_rgb,  // Orange (30°)
        0xFFFF00_rgb,  // Yellow (60°)
    };
    print_palette(palette, "Analogous");
    auto result = assess(palette);
    std::cout << "  Score: " << result.score << "  Scheme: " << to_string(result.scheme)
              << "  Deviation: " << result.deviation << "°\n";
  }

  // ========================================================================
  section("4. Split Complementary");

  {
    palette_set<rgba8_t> palette = {
        0xFF0000_rgb,  // Red (0°)
        0x00FF80_rgb,  // Yellow-green (150°)
        0x0080FF_rgb,  // Blue (210°)
    };
    print_palette(palette, "Split Complementary");
    auto result = assess(palette);
    std::cout << "  Score: " << result.score << "  Scheme: " << to_string(result.scheme)
              << "  Deviation: " << result.deviation << "°\n";
  }

  // ========================================================================
  section("5. Square Harmony");

  {
    palette_set<rgba8_t> palette = {
        0xFF0000_rgb,  // Red (0°)
        0xFFFF00_rgb,  // Yellow (90°)
        0x00FFFF_rgb,  // Cyan (180°)
        0x0000FF_rgb,  // Blue (270°)
    };
    print_palette(palette, "Square");
    auto result = assess(palette);
    std::cout << "  Score: " << result.score << "  Scheme: " << to_string(result.scheme)
              << "  Deviation: " << result.deviation << "°\n";
  }

  // ========================================================================
  section("6. Dissonant Palette (Low Score)");

  {
    palette_set<rgba8_t> palette = {
        0xFF0000_rgb,  // Red (0°)
        0xFF1A00_rgb,  // Slightly different red (≈5°)
        0xFF3300_rgb,  // Another slightly different red (≈10°)
    };
    print_palette(palette, "Dissonant");
    auto result = assess(palette);
    std::cout << "  Score: " << result.score << "  Scheme: " << to_string(result.scheme)
              << "  Deviation: " << result.deviation << "°\n";
  }

  // ========================================================================
  section("7. Harmony Suggestions");

  {
    palette_set<rgba8_t> palette = {
        0xFF0000_rgb,  // Red (0°)
        0x00AA00_rgb,  // Dark green (~120°, off from ideal)
        0x000080_rgb,  // Dark blue (~240°, off from ideal)
    };
    print_palette(palette, "Before");
    auto result = assess(palette);
    std::cout << "  Score: " << result.score << "  Scheme: " << to_string(result.scheme)
              << "  Deviation: " << result.deviation << "°\n";

    auto suggestions = suggest(palette);
    std::cout << "\n  Suggestions:\n";
    for (const auto& s : suggestions) {
      std::cout << "    Color " << s.color_index << ": " << s.action << '\n';
    }
  }

  // ========================================================================
  section("8. Golden Angle Harmony");

  {
    palette_set<rgba8_t> palette = {
        0xFF0000_rgb,  // Red (0°)
        0x8000FF_rgb,  // Purple (~137.5°)
    };
    print_palette(palette, "Golden Angle");
    auto result = assess(palette);
    std::cout << "  Score: " << result.score << "  Scheme: " << to_string(result.scheme)
              << "  Deviation: " << result.deviation << "°\n";
  }

  // ========================================================================
  section("9. Scheme String Conversion");

  std::cout << "  to_string(harmony_scheme::complementary):  " << to_string(harmony_scheme::complementary) << '\n';
  std::cout << "  to_string(harmony_scheme::triadic):        " << to_string(harmony_scheme::triadic) << '\n';
  std::cout << "  to_string(harmony_scheme::analogous):      " << to_string(harmony_scheme::analogous) << '\n';
  std::cout << "  to_string(harmony_scheme::split_complementary): " << to_string(harmony_scheme::split_complementary)
            << '\n';
  std::cout << "  to_string(harmony_scheme::square):         " << to_string(harmony_scheme::square) << '\n';
  std::cout << "  to_string(harmony_scheme::golden):         " << to_string(harmony_scheme::golden) << '\n';
  std::cout << "  to_string(harmony_scheme::tetradic):       " << to_string(harmony_scheme::tetradic) << '\n';

  std::cout << "\n  from_string(\"triadic\"):        " << to_string(from_string("triadic")) << '\n';
  std::cout << "  from_string(\"complementary\"):  " << to_string(from_string("complementary")) << '\n';
  std::cout << "  from_string(\"square\"):          " << to_string(from_string("square")) << '\n';

  std::cout << "\n";
  return 0;
}
