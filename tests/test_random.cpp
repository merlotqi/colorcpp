#include <gtest/gtest.h>

#include <color/operations/generation/random.hpp>
#include <random>

TEST(random, basic_random) {
  using namespace color::operations::generation;

  std::mt19937 rng(64);
  auto gen = basic_rgb_generator<uint8_t, 255>(rng);
  gen.next();
}
