/**
 * @file vision_example.cpp
 * @brief Example: Color vision deficiency simulation.
 */

#include <colorcpp/colorcpp.hpp>
#include <iostream>
#include <iomanip>

using namespace colorcpp;
using namespace colorcpp::algorithms::vision;
using namespace colorcpp::algorithms::delta_e;
using namespace colorcpp::operations::conversion;
using namespace colorcpp::io::ansi;
using namespace colorcpp::io::literals;

static void section(const char* title) {
  std::cout << '\n' << bold() << "=== " << title << " ===" << reset() << '\n';
}

int main() {
    auto red = 0xFF0000_rgb;
    auto green = 0x00FF00_rgb;
    auto blue = 0x0000FF_rgb;
    auto yellow = 0xFFFF00_rgb;
    auto cyan = 0x00FFFF_rgb;
    auto magenta = 0xFF00FF_rgb;
    auto orange = 0xFFA500_rgb;
    auto purple = 0x800080_rgb;

    std::vector<rgba8_t> test_colors = { red, green, blue, yellow, cyan, magenta, orange, purple };

    // ========================================================================
    section("1. Color Vision Deficiencies");

    std::cout << "Brettel / Viénot-style full dichromacy (simulate_*) and Machado 2009 matrices (severity).\n\n";
    std::cout << "  Protanopia / Deuteranopia / Tritanopia: LMS-plane models\n";
    std::cout << "  Achromatopsia: luminance-only\n";

    // ========================================================================
    section("2. Simulation Demo");

    std::cout << "Original colors:\n";
    for (auto& c : test_colors) {
        print_color(std::cout, c);
        std::cout << "  ";
    }
    std::cout << "\n\n";

    // Protanopia
    std::cout << "Protanopia (red blind):\n";
    for (auto& c : test_colors) {
        auto sim = simulate_protanopia(c);
        print_color(std::cout, sim);
        std::cout << "  ";
    }
    std::cout << "\n\n";

    // Deuteranopia
    std::cout << "Deuteranopia (green blind):\n";
    for (auto& c : test_colors) {
        auto sim = simulate_deuteranopia(c);
        print_color(std::cout, sim);
        std::cout << "  ";
    }
    std::cout << "\n\n";

    // Tritanopia
    std::cout << "Tritanopia (blue blind):\n";
    for (auto& c : test_colors) {
        auto sim = simulate_tritanopia(c);
        print_color(std::cout, sim);
        std::cout << "  ";
    }
    std::cout << "\n\n";

    // Achromatopsia
    std::cout << "Achromatopsia (total color blindness):\n";
    for (auto& c : test_colors) {
        auto sim = simulate_achromatopsia(c);
        print_color(std::cout, sim);
        std::cout << "  ";
    }
    std::cout << "\n\n";

    // ========================================================================
    section("3. Machado protan severity (0 = normal, 1 = dichromacy)");

    for (float severity : {0.0f, 0.25f, 0.5f, 0.75f, 1.0f}) {
        std::cout << "  Severity " << std::setw(4) << severity << ":  ";
        print_color(std::cout, simulate_protan_machado(orange, severity));
        std::cout << "\n";
    }

    // ========================================================================
    section("4. Red-Green Confusion Test");

    std::cout << "Red and Green comparison:\n\n";
    std::cout << "  Normal vision:      ";
    print_color(std::cout, red);
    std::cout << " vs ";
    print_color(std::cout, green);
    std::cout << "  ΔE = " << delta_e_2000(red, green) << "\n";

    auto red_pro = simulate_protanopia(red);
    auto green_pro = simulate_protanopia(green);

    std::cout << "  Protanopia:         ";
    print_color(std::cout, red_pro);
    std::cout << " vs ";
    print_color(std::cout, green_pro);
    std::cout << "  ΔE = " << delta_e_2000(red_pro, green_pro) << "\n";

    if (delta_e_2000(red_pro, green_pro) < 1.0f) {
        std::cout << "\n  Under this metric, protan-simulated red/green are nearly identical.\n";
    }

    std::cout << "\n";
    return 0;
}
