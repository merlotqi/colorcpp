#include <benchmark/benchmark.h>

#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;

static void BM_BlendNormal(benchmark::State& state) {
  rgba8_t dst{200, 100, 50, 255};
  rgba8_t src{50, 100, 200, 200};
  for (auto _ : state) {
    benchmark::DoNotOptimize(colorcpp::operations::blend::blend(
        dst, src, colorcpp::operations::blend::blend_mode::normal));
  }
}
BENCHMARK(BM_BlendNormal);

static void BM_BlendMultiply(benchmark::State& state) {
  rgba8_t dst{200, 100, 50, 255};
  rgba8_t src{50, 100, 200, 200};
  for (auto _ : state) {
    benchmark::DoNotOptimize(colorcpp::operations::blend::blend(
        dst, src, colorcpp::operations::blend::blend_mode::multiply));
  }
}
BENCHMARK(BM_BlendMultiply);

static void BM_BlendOverlay(benchmark::State& state) {
  rgba8_t dst{200, 100, 50, 255};
  rgba8_t src{50, 100, 200, 200};
  for (auto _ : state) {
    benchmark::DoNotOptimize(colorcpp::operations::blend::blend(
        dst, src, colorcpp::operations::blend::blend_mode::overlay));
  }
}
BENCHMARK(BM_BlendOverlay);
