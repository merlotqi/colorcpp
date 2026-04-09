#include <benchmark/benchmark.h>

#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;
using namespace colorcpp::algorithms;

static void BM_DeltaE76(benchmark::State& state) {
  rgba8_t a{200, 100, 50, 255};
  rgba8_t b{50, 100, 200, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(delta_e::delta_e_76(a, b));
  }
}
BENCHMARK(BM_DeltaE76);

static void BM_DeltaE94(benchmark::State& state) {
  rgba8_t a{200, 100, 50, 255};
  rgba8_t b{50, 100, 200, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(delta_e::delta_e_94(a, b));
  }
}
BENCHMARK(BM_DeltaE94);

static void BM_DeltaE2000(benchmark::State& state) {
  rgba8_t a{200, 100, 50, 255};
  rgba8_t b{50, 100, 200, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(delta_e::delta_e_2000(a, b));
  }
}
BENCHMARK(BM_DeltaE2000);

static void BM_DeltaEOk(benchmark::State& state) {
  oklab_t a{0.62f, 0.11f, -0.09f};
  oklab_t b{0.41f, -0.08f, 0.15f};
  for (auto _ : state) {
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    benchmark::DoNotOptimize(delta_e::delta_e_ok(a, b));
  }
}
BENCHMARK(BM_DeltaEOk);
