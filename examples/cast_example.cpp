#include <colorcpp/colorcpp.hpp>
#include <iostream>

int main() {
  auto brown = color::constants::brown;

  auto brownf = color::to_rgba_float(brown);

  std::cout << brown << '\n';
  std::cout << brownf << '\n';

  return 0;
}
