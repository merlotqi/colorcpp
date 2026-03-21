#include <benchmark/benchmark.h>

#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;
using namespace colorcpp::operations::conversion;

static void BM_RGBA8_to_OkLab(benchmark::State& state) {
  rgba8_t c{128, 64, 192, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<oklab_t>(c));
  }
}
BENCHMARK(BM_RGBA8_to_OkLab);

static void BM_RGBF_to_CIELAB(benchmark::State& state) {
  rgbf_t c{0.3f, 0.6f, 0.9f};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<cielab_t>(c));
  }
}
BENCHMARK(BM_RGBF_to_CIELAB);

static void BM_RGBF_to_HSV_roundtrip(benchmark::State& state) {
  rgbf_t c{0.7f, 0.3f, 0.5f};
  for (auto _ : state) {
    auto h = color_cast<hsv_float_t>(c);
    benchmark::DoNotOptimize(color_cast<rgbf_t>(h));
  }
}
BENCHMARK(BM_RGBF_to_HSV_roundtrip);

static void BM_RGBA8_to_XYZ(benchmark::State& state) {
  rgba8_t c{128, 64, 192, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<xyz_t>(c));
  }
}
BENCHMARK(BM_RGBA8_to_XYZ);
