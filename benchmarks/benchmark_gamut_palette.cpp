#include <benchmark/benchmark.h>

#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;
using colorcpp::operations::gamut::is_in_srgb_gamut;
using colorcpp::operations::palette::perceptual_scale;

static void BM_IsInSrgbGamut_OkLab(benchmark::State& state) {
  oklab_t c{0.65f, 0.12f, -0.08f};
  for (auto _ : state) {
    benchmark::DoNotOptimize(is_in_srgb_gamut(c));
  }
}
BENCHMARK(BM_IsInSrgbGamut_OkLab);

static void BM_IsInSrgbGamut_RGBA8(benchmark::State& state) {
  rgba8_t c{128, 64, 192, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(is_in_srgb_gamut(c));
  }
}
BENCHMARK(BM_IsInSrgbGamut_RGBA8);

static void BM_PerceptualScale_16(benchmark::State& state) {
  rgba8_t a{255, 0, 0, 255};
  rgba8_t b{0, 0, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(perceptual_scale(a, b, 16));
  }
}
BENCHMARK(BM_PerceptualScale_16);
