#include <benchmark/benchmark.h>

#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;
using operations::compare::perceptual_equal;
using operations::compare::epsilon_equal;

// Exact equality
static void BM_Compare_Exact(benchmark::State& state) {
  rgba8_t a{128, 64, 192, 255};
  rgba8_t b{128, 64, 192, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(a == b);
  }
}
BENCHMARK(BM_Compare_Exact);

// Inequality
static void BM_Compare_Inequality(benchmark::State& state) {
  rgba8_t a{128, 64, 192, 255};
  rgba8_t b{128, 64, 191, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(a == b);
  }
}
BENCHMARK(BM_Compare_Inequality);

// Perceptual equality - same color
static void BM_Compare_Perceptual_Same(benchmark::State& state) {
  rgba8_t a{128, 64, 192, 255};
  rgba8_t b{128, 64, 192, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(perceptual_equal(a, b));
  }
}
BENCHMARK(BM_Compare_Perceptual_Same);

// Perceptual equality - similar color
static void BM_Compare_Perceptual_Similar(benchmark::State& state) {
  rgba8_t a{128, 64, 192, 255};
  rgba8_t b{130, 66, 190, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(perceptual_equal(a, b));
  }
}
BENCHMARK(BM_Compare_Perceptual_Similar);

// Perceptual equality - different color
static void BM_Compare_Perceptual_Different(benchmark::State& state) {
  rgba8_t a{128, 64, 192, 255};
  rgba8_t b{200, 100, 50, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(perceptual_equal(a, b));
  }
}
BENCHMARK(BM_Compare_Perceptual_Different);

// Epsilon equality - same color
static void BM_Compare_Epsilon_Same(benchmark::State& state) {
  rgba8_t a{128, 64, 192, 255};
  rgba8_t b{128, 64, 192, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(epsilon_equal(a, b));
  }
}
BENCHMARK(BM_Compare_Epsilon_Same);

// Epsilon equality - within threshold
static void BM_Compare_Epsilon_Within(benchmark::State& state) {
  rgba8_t a{128, 64, 192, 255};
  rgba8_t b{129, 65, 193, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(epsilon_equal(a, b));
  }
}
BENCHMARK(BM_Compare_Epsilon_Within);

// Epsilon equality - outside threshold
static void BM_Compare_Epsilon_Outside(benchmark::State& state) {
  rgba8_t a{128, 64, 192, 255};
  rgba8_t b{200, 100, 50, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(epsilon_equal(a, b));
  }
}
BENCHMARK(BM_Compare_Epsilon_Outside);
