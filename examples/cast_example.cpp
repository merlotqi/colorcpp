#include <colorcpp/colorcpp.hpp>
#include <iostream>

int main() {
  auto brown = color::constants::brown;

  using namespace  color::operations::conversion;
  auto brownf = color_cast<color::core::rgba_float_t>(brown);

  std::cout << brown << '\n';
  std::cout << brownf << '\n';

  return 0;
}
