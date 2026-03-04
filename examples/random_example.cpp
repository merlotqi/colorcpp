#include <colorcpp/colorcpp.hpp>
#include <iostream>
#include <random>

int main() {
  std::mt19937 rng{std::random_device{}()};
  color::operations::generation::rgb8_generator gen(rng);

  for (int i = 0; i < 20; ++i) {
    auto color = gen.next();
    std::cout << color << std::endl;
  }

  return 0;
}
