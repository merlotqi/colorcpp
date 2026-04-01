/**
 * @file ansi_example.cpp
 * @brief Example: ANSI terminal color utilities for debugging and visualization.
 */

#include <colorcpp/colorcpp.hpp>
#include <iostream>

using namespace colorcpp;
using namespace colorcpp::io::ansi;
using namespace colorcpp::io::css;
using namespace colorcpp::io::literals;

static void section(const char* title) { std::cout << "\n── " << title << " ──\n"; }

int main() {
  using namespace named_literal;

  // === ANSI escape sequences ===
  section("ANSI Escape Sequences");

  std::cout << "Foreground: " << fg(255, 0, 0) << "Red text" << reset() << "\n";
  std::cout << "Background: " << bg(0, 128, 255) << "Blue background" << reset() << "\n";
  std::cout << "Bold: " << bold() << "Bold text" << reset() << "\n";
  std::cout << "Combined: " << bg(255, 255, 0) << fg(0, 0, 0) << bold() << " Yellow on black " << reset() << "\n";

  // === Color swatch ===
  section("Color Swatch");

  core::rgba8_t coral = "coral"_color;
  print_swatch(std::cout, coral);
  std::cout << " coral\n";

  // === Print color info ===
  section("Print Color");

  print_color(std::cout, coral, "coral");
  std::cout << "\n";

  core::rgba8_t steelblue = "steelblue"_color;
  print_color(std::cout, steelblue, "steelblue");
  std::cout << "\n";

  // === Verbose color info ===
  section("Verbose Color Info");

  print_color_verbose(std::cout, coral, "coral");
  std::cout << "\n";
  print_color_verbose(std::cout, steelblue, "steelblue");
  std::cout << "\n";
  print_color_verbose(std::cout, "gold"_color, "gold");
  std::cout << "\n";

  // === Palette ===
  section("Palette");

  core::rgba8_t palette[] = {
      "red"_color, "orange"_color, "yellow"_color, "green"_color, "blue"_color, "indigo"_color, "violet"_color,
  };
  std::cout << "Rainbow: ";
  print_palette(std::cout, palette, 7);

  // === Gradient ===
  section("Gradient");

  std::cout << "Black → White:  ";
  print_gradient(std::cout, "black"_color, "white"_color);

  std::cout << "Red → Blue:     ";
  print_gradient(std::cout, "red"_color, "blue"_color);

  std::cout << "Coral → Gold:   ";
  print_gradient(std::cout, "coral"_color, "gold"_color);

  // === Contrast check ===
  section("Contrast Check (WCAG)");

  print_contrast(std::cout, "white"_color, "black"_color);
  std::cout << "\n";
  print_contrast(std::cout, "black"_color, "white"_color);
  std::cout << "\n";
  print_contrast(std::cout, "white"_color, "navy"_color);
  std::cout << "\n";
  print_contrast(std::cout, "white"_color, "gray"_color);
  std::cout << "\n";
  print_contrast(std::cout, "yellow"_color, "white"_color);
  std::cout << "\n";

  // === Named colors integration ===
  section("Named Colors");

  std::string_view names[] = {"coral", "tomato", "orangered", "gold", "orange"};
  for (auto name : names) {
    auto c = get_named_color(name);
    if (c) {
      print_color(std::cout, *c, name);
      std::cout << "\n";
    }
  }

  return 0;
}
