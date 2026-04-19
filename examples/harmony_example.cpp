/**
 * @file harmony_example.cpp
 * @brief Example usage of the color harmony analysis module.
 *
 * This example demonstrates all features of the harmony module:
 * - Assessing palette harmony
 * - Weighted multi-channel assessment
 * - Auto detecting harmony schemes
 * - Correcting palettes to target schemes
 * - Using different correction modes
 * - Generating improvement suggestions
 */

#include <colorcpp/colorcpp.hpp>
#include <iostream>

using namespace colorcpp;
using namespace colorcpp::core;
using namespace colorcpp::algorithms;
using namespace colorcpp::operations;
using namespace colorcpp::operations::conversion;
using namespace colorcpp::io::ansi;

static void section(const char* title) { std::cout << '\n' << bold() << "=== " << title << " ===" << reset() << '\n'; }

int main() {
  std::cout << "============================================" << std::endl;
  std::cout << "ColorCPP Harmony Module Example" << std::endl;
  std::cout << "============================================" << std::endl;
  std::cout << std::endl;

  section("Example Palette");

  palette_set<rgbaf_t> palette;
  palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});  // Red (0°)
  palette.add(rgbaf_t{1.0f, 0.5f, 0.0f, 1.0f});  // Orange (30°)
  palette.add(rgbaf_t{0.0f, 1.0f, 1.0f, 1.0f});  // Cyan (180°)
  palette.add(rgbaf_t{0.0f, 0.5f, 1.0f, 1.0f});  // Cyan-blue (210°)

  std::cout << "Palette size: " << palette.size() << " colors" << std::endl;
  std::cout << std::endl;

  section("Harmony Assessment");

  auto result = harmony::assess(palette);

  std::cout << "Detected scheme: " << harmony::to_string(result.scheme) << std::endl;
  std::cout << "Harmony score: " << result.score << " / 100" << std::endl;
  std::cout << "Average deviation: " << result.deviation << "°" << std::endl;
  std::cout << std::endl;

  section("Weighted Assessment");

  // Use balanced weights (equal importance for all channels)
  auto weighted_result = harmony::assess_weighted(palette, harmony::harmony_weights::balanced());

  std::cout << "Total weighted score: " << weighted_result.score << " / 100" << std::endl;
  std::cout << "  Hue angle score:    " << weighted_result.angle_score << std::endl;
  std::cout << "  Saturation score:   " << weighted_result.saturation_score << std::endl;
  std::cout << "  Lightness score:    " << weighted_result.lightness_score << std::endl;
  std::cout << std::endl;

  section("Palette Correction");

  // Correct to square scheme
  auto corrected = harmony::correct(palette, harmony::harmony_scheme::square);

  std::cout << "Detected scheme: " << harmony::to_string(corrected.detected_scheme) << std::endl;
  std::cout << "Target scheme:   " << harmony::to_string(corrected.target_scheme) << std::endl;
  std::cout << "Status:          " << (corrected.applied() ? "Corrected" : "Unchanged") << std::endl;

  if (corrected.applied()) {
    auto new_score = harmony::assess(corrected.palette);
    std::cout << "New harmony score after correction: " << new_score.score << " / 100" << std::endl;
  }
  std::cout << std::endl;

  section("Correction Modes");

  // Minimal correction (only adjust the color with largest deviation)
  auto minimal = harmony::correct_minimal(palette, harmony::harmony_scheme::square);
  std::cout << "Minimal correction applied: " << std::boolalpha << minimal.applied() << std::endl;

  // Hue only correction (preserve saturation and lightness)
  auto hue_only = harmony::correct_hue_only(palette, harmony::harmony_scheme::square);
  std::cout << "Hue only correction applied: " << std::boolalpha << hue_only.applied() << std::endl;

  // Correction with tolerance threshold (only correct if deviation > 5°)
  auto tolerance = harmony::correct(palette, harmony::harmony_scheme::square, 5.0f);
  std::cout << "Tolerance correction status: ";
  switch (tolerance.status) {
    case harmony::correction_status::corrected:
      std::cout << "Corrected";
      break;
    case harmony::correction_status::unchanged:
      std::cout << "Unchanged (within tolerance)";
      break;
    case harmony::correction_status::tolerance_exceeded:
      std::cout << "Corrected (tolerance exceeded)";
      break;
    default:
      std::cout << "Other";
  }
  std::cout << std::endl << std::endl;

  section("Improvement Suggestions");

  // Create a palette with poor harmony
  palette_set<rgbaf_t> bad_palette;
  bad_palette.add(rgbaf_t{1.0f, 0.0f, 0.0f, 1.0f});  // Red (0°)
  bad_palette.add(rgbaf_t{0.9f, 0.2f, 0.0f, 1.0f});  // Slightly different red (10°)

  auto suggestions = harmony::suggest(bad_palette);

  if (!suggestions.empty()) {
    std::cout << "Found " << suggestions.size() << " improvement suggestions:" << std::endl;
    for (const auto& s : suggestions) {
      std::cout << "  Color " << s.color_index << ": " << s.action << std::endl;
    }
  } else {
    std::cout << "Palette is already harmonious, no suggestions needed" << std::endl;
  }
  std::cout << std::endl;

  section("Supported Harmony Schemes");

  std::vector<harmony::harmony_scheme> schemes = {harmony::harmony_scheme::complementary,
                                                  harmony::harmony_scheme::golden,
                                                  harmony::harmony_scheme::analogous,
                                                  harmony::harmony_scheme::triadic,
                                                  harmony::harmony_scheme::split_complementary,
                                                  harmony::harmony_scheme::split_analogous,
                                                  harmony::harmony_scheme::tetradic,
                                                  harmony::harmony_scheme::square,
                                                  harmony::harmony_scheme::analogous_wide,
                                                  harmony::harmony_scheme::compound,
                                                  harmony::harmony_scheme::double_complementary};

  for (const auto& scheme : schemes) {
    auto rule = harmony::rule_for(scheme);
    std::cout << "  " << harmony::to_string(scheme) << " (" << rule.color_count << " colors)" << std::endl;
  }
  std::cout << std::endl;

  section("Golden Angle Palette");

  palette_set<rgbaf_t> golden_palette;
  golden_palette.add(color_cast<rgbaf_t>(hsla_float_t{0.0f, 1.0f, 0.5f, 1.0f}));
  golden_palette.add(color_cast<rgbaf_t>(hsla_float_t{137.5f, 1.0f, 0.5f, 1.0f}));
  golden_palette.add(color_cast<rgbaf_t>(hsla_float_t{275.0f, 1.0f, 0.5f, 1.0f}));

  auto golden_result = harmony::assess(golden_palette);
  std::cout << "Golden angle 3-color palette" << std::endl;
  std::cout << "Detected scheme: " << harmony::to_string(golden_result.scheme) << std::endl;
  std::cout << "Harmony score: " << golden_result.score << " / 100" << std::endl;
  std::cout << std::endl;

  std::cout << "============================================" << std::endl;
  std::cout << "Example completed successfully" << std::endl;
  std::cout << "============================================" << std::endl;

  return 0;
}
