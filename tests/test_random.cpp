#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>
#include <random>

TEST(random, basic_random) {
  using namespace color::operations::generation;
  
  std::mt19937 rng{std::random_device{}()};
  rgb8_generator rgb8_gen(rng);

  rgb8_gen.next();
}
