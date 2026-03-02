#include <color.hpp>
#include <iostream>

using namespace color::core;

int main() {
  Color8 black;

  Color8 red(255, 0, 0);
  Color8 green(0, 255, 0);
  Color8 blue(0, 0, 255);

  ColorF light_red(1.0, 0.05, 0.5);

  Color8 dynamic_red(rgb8<8, 9, 89>{});

  std::cout << dynamic_red.r() << '\n';
  std::cout << dynamic_red.g() << '\n';
  std::cout << dynamic_red.b() << '\n';
  int p = 0;
}
