#include <benchmark/benchmark.h>

#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;
using operations::interpolate::lerp;
using operations::interpolate::lerp_catmull_rom;
using operations::interpolate::lerp_hsl;
using operations::interpolate::lerp_oklab;
using operations::interpolate::multi_lerp;
using operations::interpolate::multi_lerp_oklab;

// Single interpolation - RGB
static void BM_LerpRGB(benchmark::State& state) {
  rgba8_t a{255, 0, 0, 255};
  rgba8_t b{0, 0, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(lerp(a, b, 0.37f));
  }
}
BENCHMARK(BM_LerpRGB);

// Single interpolation - HSL
static void BM_LerpHSL(benchmark::State& state) {
  rgba8_t a{255, 0, 0, 255};
  rgba8_t b{0, 0, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(lerp_hsl(a, b, 0.37f));
  }
}
BENCHMARK(BM_LerpHSL);

// Single interpolation - OkLab
static void BM_LerpOkLab(benchmark::State& state) {
  rgba8_t a{255, 0, 0, 255};
  rgba8_t b{0, 0, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(lerp_oklab(a, b, 0.37f));
  }
}
BENCHMARK(BM_LerpOkLab);

// Multi-stop interpolation - RGB (3 stops)
static void BM_MultiLerpRGB_3stops(benchmark::State& state) {
  rgba8_t r{255, 0, 0, 255};
  rgba8_t g{0, 255, 0, 255};
  rgba8_t b{0, 0, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(multi_lerp({r, g, b}, 0.37f));
  }
}
BENCHMARK(BM_MultiLerpRGB_3stops);

// Multi-stop interpolation - OkLab (3 stops)
static void BM_MultiLerpOkLab_3stops(benchmark::State& state) {
  rgba8_t r{255, 0, 0, 255};
  rgba8_t g{0, 255, 0, 255};
  rgba8_t b{0, 0, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(multi_lerp_oklab({r, g, b}, 0.37f));
  }
}
BENCHMARK(BM_MultiLerpOkLab_3stops);

// Multi-stop interpolation - RGB (5 stops)
static void BM_MultiLerpRGB_5stops(benchmark::State& state) {
  rgba8_t c1{255, 0, 0, 255};
  rgba8_t c2{255, 128, 0, 255};
  rgba8_t c3{255, 255, 0, 255};
  rgba8_t c4{0, 255, 0, 255};
  rgba8_t c5{0, 0, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(multi_lerp({c1, c2, c3, c4, c5}, 0.37f));
  }
}
BENCHMARK(BM_MultiLerpRGB_5stops);

// Multi-stop interpolation - OkLab (5 stops)
static void BM_MultiLerpOkLab_5stops(benchmark::State& state) {
  rgba8_t c1{255, 0, 0, 255};
  rgba8_t c2{255, 128, 0, 255};
  rgba8_t c3{255, 255, 0, 255};
  rgba8_t c4{0, 255, 0, 255};
  rgba8_t c5{0, 0, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(multi_lerp_oklab({c1, c2, c3, c4, c5}, 0.37f));
  }
}
BENCHMARK(BM_MultiLerpOkLab_5stops);

// Catmull-Rom interpolation - uniform
static void BM_CatmullRom_Uniform(benchmark::State& state) {
  rgba8_t c0{128, 0, 0, 255};
  rgba8_t c1{255, 0, 0, 255};
  rgba8_t c2{255, 128, 0, 255};
  rgba8_t c3{255, 255, 0, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(lerp_catmull_rom(c0, c1, c2, c3, 0.5f));
  }
}
BENCHMARK(BM_CatmullRom_Uniform);

// Catmull-Rom interpolation - centripetal
static void BM_CatmullRom_Centripetal(benchmark::State& state) {
  rgba8_t c0{128, 0, 0, 255};
  rgba8_t c1{255, 0, 0, 255};
  rgba8_t c2{255, 128, 0, 255};
  rgba8_t c3{255, 255, 0, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(lerp_catmull_rom(c0, c1, c2, c3, 0.5f, operations::interpolate::cr_mode::centripetal));
  }
}
BENCHMARK(BM_CatmullRom_Centripetal);
