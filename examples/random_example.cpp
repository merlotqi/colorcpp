#include <colorcpp/colorcpp.hpp>
#include <iostream>
#include <random>

static void gen_rgb() {
  std::cout << "generate rgb8 color \n";
  std::mt19937 rng{std::random_device{}()};

  colorcpp::operations::generation::rgb8_generator gen(rng);

  for (int i = 0; i < 20; ++i) {
    auto color = gen.next();
    std::cout << color << std::endl;
  }
}

static void gen_rgba() {
  std::cout << "generate rgb8 color \n";
  std::mt19937 rng{std::random_device{}()};

  colorcpp::operations::generation::rgba8_generator gen(rng);

  for (int i = 0; i < 20; ++i) {
    auto color = gen.next();
    std::cout << color << std::endl;
  }
}

int main() {
  gen_rgb();
  gen_rgba();

  return 0;
}
