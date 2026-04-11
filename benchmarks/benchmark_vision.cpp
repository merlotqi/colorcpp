#include <benchmark/benchmark.h>

#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;
using algorithms::vision::simulate_achromatopsia;
using algorithms::vision::simulate_deuteranopia;
using algorithms::vision::simulate_protanopia;
using algorithms::vision::simulate_tritanopia;

// Protanopia simulation
static void BM_Vision_Protanopia(benchmark::State& state) {
  rgba8_t c{255, 0, 0, 255};  // Red
  for (auto _ : state) {
    benchmark::DoNotOptimize(simulate_protanopia(c));
  }
}
BENCHMARK(BM_Vision_Protanopia);

// Deuteranopia simulation
static void BM_Vision_Deuteranopia(benchmark::State& state) {
  rgba8_t c{0, 255, 0, 255};  // Green
  for (auto _ : state) {
    benchmark::DoNotOptimize(simulate_deuteranopia(c));
  }
}
BENCHMARK(BM_Vision_Deuteranopia);

// Tritanopia simulation
static void BM_Vision_Tritanopia(benchmark::State& state) {
  rgba8_t c{0, 0, 255, 255};  // Blue
  for (auto _ : state) {
    benchmark::DoNotOptimize(simulate_tritanopia(c));
  }
}
BENCHMARK(BM_Vision_Tritanopia);

// Achromatopsia simulation
static void BM_Vision_Achromatopsia(benchmark::State& state) {
  rgba8_t c{128, 64, 192, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(simulate_achromatopsia(c));
  }
}
BENCHMARK(BM_Vision_Achromatopsia);

// Protanopia on palette (5 colors)
static void BM_Vision_Protanopia_Palette(benchmark::State& state) {
  std::vector<rgba8_t> palette;
  palette.push_back(rgba8_t{255, 0, 0, 255});
  palette.push_back(rgba8_t{0, 255, 0, 255});
  palette.push_back(rgba8_t{0, 0, 255, 255});
  palette.push_back(rgba8_t{255, 255, 0, 255});
  palette.push_back(rgba8_t{128, 64, 192, 255});
  for (auto _ : state) {
    for (const auto& c : palette) {
      benchmark::DoNotOptimize(simulate_protanopia(c));
    }
  }
}
BENCHMARK(BM_Vision_Protanopia_Palette);
