#include <colorcpp/colorcpp.hpp>
#include <iostream>

using namespace colorcpp;

int main() {
  auto brown = colorcpp::constants::brown;

  auto brownf = colorcpp::conversion::color_cast<colorcpp::core::rgbaf_t>(brown);

  std::cout << brown << '\n';
  std::cout << brownf << '\n';

  return 0;
}
