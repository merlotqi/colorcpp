#include <colorcpp/colorcpp.hpp>
#include <iostream>

int main() {
  auto red = color::constants::red;
  auto blue = color::constants::blue;

  using namespace color::operations::interpolate;

  auto color = color::operations::interpolate::lerp(red, blue, 0.5);

  std::cout << red << '\n';
  std::cout << blue << '\n';
  std::cout << color << '\n';

  return 0;
}
