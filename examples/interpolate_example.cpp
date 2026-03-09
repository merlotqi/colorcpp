#include <colorcpp/colorcpp.hpp>
#include <iostream>

int main() {
  auto red = colorcpp::constants::red;
  auto blue = colorcpp::constants::blue;

  using namespace colorcpp::operations::interpolate;

  auto color = colorcpp::operations::interpolate::lerp(red, blue, 0.5);

  std::cout << red << '\n';
  std::cout << blue << '\n';
  std::cout << color << '\n';

  return 0;
}
