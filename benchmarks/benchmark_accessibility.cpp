#include <benchmark/benchmark.h>

#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;
using algorithms::accessibility::apca_contrast;
using algorithms::accessibility::contrast_ratio;

// WCAG contrast ratio - high contrast
static void BM_WCAGContrast_High(benchmark::State& state) {
  rgba8_t fg{0, 0, 0, 255};        // Black
  rgba8_t bg{255, 255, 255, 255};  // White
  for (auto _ : state) {
    benchmark::DoNotOptimize(contrast_ratio(fg, bg));
  }
}
BENCHMARK(BM_WCAGContrast_High);

// WCAG contrast ratio - medium contrast
static void BM_WCAGContrast_Medium(benchmark::State& state) {
  rgba8_t fg{100, 100, 100, 255};
  rgba8_t bg{255, 255, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(contrast_ratio(fg, bg));
  }
}
BENCHMARK(BM_WCAGContrast_Medium);

// WCAG contrast ratio - low contrast
static void BM_WCAGContrast_Low(benchmark::State& state) {
  rgba8_t fg{200, 200, 200, 255};
  rgba8_t bg{255, 255, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(contrast_ratio(fg, bg));
  }
}
BENCHMARK(BM_WCAGContrast_Low);

// APCA contrast - high contrast
static void BM_APCAContrast_High(benchmark::State& state) {
  rgba8_t fg{0, 0, 0, 255};
  rgba8_t bg{255, 255, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(apca_contrast(fg, bg));
  }
}
BENCHMARK(BM_APCAContrast_High);

// APCA contrast - medium contrast
static void BM_APCAContrast_Medium(benchmark::State& state) {
  rgba8_t fg{100, 100, 100, 255};
  rgba8_t bg{255, 255, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(apca_contrast(fg, bg));
  }
}
BENCHMARK(BM_APCAContrast_Medium);

// APCA contrast - colored foreground
static void BM_APCAContrast_Colored(benchmark::State& state) {
  rgba8_t fg{0, 100, 200, 255};
  rgba8_t bg{255, 255, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(apca_contrast(fg, bg));
  }
}
BENCHMARK(BM_APCAContrast_Colored);
