#include <color.hpp>
#include <iostream>
#include "color/core/rgb.hpp"

using namespace color::core;

int main() {
  Color8 dynamic_red(colors::cyan{});

  std::cout << (int)dynamic_red.r() << '\n';
  std::cout << (int)dynamic_red.g() << '\n';
  std::cout << (int)dynamic_red.b() << '\n';
  int p = 0;
}
