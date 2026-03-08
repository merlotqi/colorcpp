#include <colorcpp/colorcpp.hpp>
#include <iostream>

int main() {
  auto red = colorcpp::constants::red;
  std::cout << red << std::endl;


  colorcpp::core::rgb8_t color;
  std::cin >> color;

  std::cout << color << '\n';

  std::cout << std::hex << std::uppercase << color << '\n';
  return 0;
}
