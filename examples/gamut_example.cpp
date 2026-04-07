/**
 * @file gamut_example.cpp
 * @brief Example: Gamut mapping, boundary checking and clipping strategies.
 */

#include <colorcpp/colorcpp.hpp>
#include <iostream>
#include <iomanip>

using namespace colorcpp;
using namespace colorcpp::algorithms::gamut;
using namespace colorcpp::operations::conversion;
using namespace colorcpp::io::ansi;

static void section(const char* title) {
  std::cout << '\n' << bold() << "=== " << title << " ===" << reset() << '\n';
}

int main() {
    std::cout << std::fixed << std::setprecision(3);

    // ========================================================================
    section("1. Gamut Checking");

    // Create an out-of-gamut color in Oklab (high chroma)
    oklch_t out_of_gamut { 0.6f, 0.4f, 180.0f }; // Very saturated cyan
    rgba8_t srgb_color = color_cast<rgba8_t>(out_of_gamut);

    std::cout << "Oklch color: " << out_of_gamut << "\n";
    std::cout << "  In sRGB gamut:     " << std::boolalpha << is_in_srgb_gamut(out_of_gamut) << "\n";
    std::cout << "  Gamut distance:    " << gamut_distance(out_of_gamut) << " ΔE_OK\n";

    // ========================================================================
    section("2. Clipping Strategies Comparison");

    auto clipped_simple = gamut_clip(out_of_gamut, clip_method::clip);
    auto clipped_chroma = gamut_clip(out_of_gamut, clip_method::chroma_reduce);
    auto clipped_css    = gamut_clip(out_of_gamut, clip_method::css_color4);

    std::cout << "Original Oklch:     " << out_of_gamut << "\n";
    std::cout << "Simple clip:        " << clipped_simple << "\n";
    std::cout << "Chroma reduce:      " << clipped_chroma << "\n";
    std::cout << "CSS Color 4:        " << clipped_css << "\n\n";

    std::cout << "Original as sRGB:   " << std::hex << srgb_color << "\n";
    std::cout << "Clipped simple:     " << color_cast<rgba8_t>(clipped_simple) << "\n";
    std::cout << "Clipped chroma:     " << color_cast<rgba8_t>(clipped_chroma) << "\n";
    std::cout << "Clipped css4:       " << color_cast<rgba8_t>(clipped_css) << std::dec << "\n";

    // ========================================================================
    section("3. Preservation Modes");

    std::cout << "Preservation strategies for out-of-gamut color:\n\n";

    auto preserve_lightness = gamut_clip_preserve_lightness(out_of_gamut);
    auto preserve_hue       = gamut_clip_preserve_hue(out_of_gamut);
    auto preserve_saturation= gamut_clip_preserve_saturation(out_of_gamut);
    auto preserve_perceptual= gamut_clip_perceptual(out_of_gamut);

    std::cout << "Original:            " << out_of_gamut << "\n";
    std::cout << "Preserve lightness:  " << preserve_lightness << "\n";
    std::cout << "Preserve hue:        " << preserve_hue << "\n";
    std::cout << "Preserve saturation: " << preserve_saturation << "\n";
    std::cout << "Perceptual optimal:  " << preserve_perceptual << "\n";

    // ========================================================================
    section("4. Detailed Mapping Result");

    auto result = gamut_clip_with_info(out_of_gamut);

    std::cout << "Mapping result info:\n";
    std::cout << "  Was in gamut:      " << result.was_in_gamut << "\n";
    std::cout << "  Original chroma:   " << result.original_chroma << "\n";
    std::cout << "  Mapped chroma:     " << result.mapped_chroma << "\n";
    std::cout << "  Chroma reduction:  " << result.chroma_reduction_percent() << "%\n";
    std::cout << "  Delta E:           " << result.delta_e << "\n";

    // ========================================================================
    section("5. Advanced Usage");

    if (is_in_srgb_gamut(out_of_gamut)) {
        std::cout << "Color is in sRGB gamut (no clip needed for standard displays).\n";
    } else {
        std::cout << "Out of sRGB gamut; mapping loses about " << result.chroma_reduction_percent()
                  << "% chroma for standard displays.\n";
    }

    // ========================================================================
    section("6. Visual Demo");

    std::cout << "\nOriginal out-of-gamut color:\n";
    print_color(std::cout, srgb_color, "High chroma cyan");

    std::cout << "\n\nClipping method comparison:\n";
    print_color(std::cout, color_cast<rgba8_t>(clipped_simple), "Simple clip");
    std::cout << "  ";
    print_color(std::cout, color_cast<rgba8_t>(clipped_chroma), "Chroma reduce");
    std::cout << "  ";
    print_color(std::cout, color_cast<rgba8_t>(clipped_css), "CSS Color 4");

    std::cout << "\n\n";

    return 0;
}
