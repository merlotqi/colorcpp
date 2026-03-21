#include <benchmark/benchmark.h>

#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;
using colorcpp::operations::interpolate::lerp;
using colorcpp::operations::interpolate::lerp_hsl;
using colorcpp::operations::interpolate::lerp_oklab;
using colorcpp::operations::interpolate::multi_lerp;
using colorcpp::operations::interpolate::multi_lerp_oklab;

static void BM_LerpRGB(benchmark::State& state) {
  rgba8_t a{255, 0, 0, 255};
  rgba8_t b{0, 0, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(lerp(a, b, 0.37f));
  }
}
BENCHMARK(BM_LerpRGB);

static void BM_LerpHSL(benchmark::State& state) {
  rgba8_t a{255, 0, 0, 255};
  rgba8_t b{0, 0, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(lerp_hsl(a, b, 0.37f));
  }
}
BENCHMARK(BM_LerpHSL);

static void BM_LerpOkLab(benchmark::State& state) {
  rgba8_t a{255, 0, 0, 255};
  rgba8_t b{0, 0, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(lerp_oklab(a, b, 0.37f));
  }
}
BENCHMARK(BM_LerpOkLab);

static void BM_MultiLerpRGB_3stops(benchmark::State& state) {
  rgba8_t r{255, 0, 0, 255};
  rgba8_t g{0, 255, 0, 255};
  rgba8_t b{0, 0, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(multi_lerp({r, g, b}, 0.37f));
  }
}
BENCHMARK(BM_MultiLerpRGB_3stops);

static void BM_MultiLerpOkLab_3stops(benchmark::State& state) {
  rgba8_t r{255, 0, 0, 255};
  rgba8_t g{0, 255, 0, 255};
  rgba8_t b{0, 0, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(multi_lerp_oklab({r, g, b}, 0.37f));
  }
}
BENCHMARK(BM_MultiLerpOkLab_3stops);
