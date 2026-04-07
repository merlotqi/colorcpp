/**
 * @file flow_example.cpp
 * @brief Demonstrating the colorflow module features.
 */

#include <colorcpp/operations/flow.hpp>
#include <iostream>

using namespace colorcpp::operations::flow;

int main() {
  // Fluent Pipeline
  std::cout << "=== Fluent Pipeline ===" << std::endl;

  auto coral = make(0xff6347ff);
  std::cout << "Original: " << coral.to_hex() << std::endl;
  std::cout << "Lighten:  " << coral.lighten(30).to_hex() << std::endl;
  std::cout << "Darken:   " << coral.darken(30).to_hex() << std::endl;
  std::cout << "Rotate:   " << coral.rotate_hue(180).to_hex() << std::endl;

  // Palette
  std::cout << "\n=== Palette ===" << std::endl;
  auto palette = coral.to_palette(5);
  for (size_t i = 0; i < palette.size(); ++i) {
    std::cout << "  [" << i << "] " << make(palette[i]).to_hex() << std::endl;
  }

  // Theme System
  std::cout << "\n=== Theme System ===" << std::endl;
  auto theme = flow_theme::from_seed(0x6750A7ff);
  auto light = theme.light_mode();
  auto dark = theme.dark_mode();
  std::cout << "Light primary:   " << make(light.primary).to_hex() << std::endl;
  std::cout << "Dark  primary:   " << make(dark.primary).to_hex() << std::endl;

  // Color Schemes
  std::cout << "\n=== Color Schemes ===" << std::endl;
  auto base = make_rgb(200, 80, 40).color();
  auto comp = generate_scheme(base, scheme_type::complementary);
  for (size_t i = 0; i < comp.size(); ++i) {
    std::cout << "  [" << i << "] " << make(comp[i]).to_hex() << std::endl;
  }

  // Export
  std::cout << "\n=== CSS Export ===" << std::endl;
  std::cout << export_::to_css_variables(light) << std::endl;

  return 0;
}
