#include <colorcpp/colorcpp.hpp>
#include <iostream>

int main() {
  auto base = colorcpp::constants::skyblue;

  auto pairs = colorcpp::operations::palette::generate::complementary(base);

  auto shades = colorcpp::operations::palette::visual_scale(pairs[0], pairs[1], 10);
  for (const auto& c : shades.colors()) {
    std::cout << c << '\n';
  }
}
