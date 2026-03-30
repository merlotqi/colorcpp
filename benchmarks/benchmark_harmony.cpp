#include <benchmark/benchmark.h>

#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;
using algorithms::harmony::assess;
using algorithms::harmony::correct;
using algorithms::harmony::suggest;
using algorithms::harmony::harmony_scheme;
using operations::palette::palette_set;

// Assess complementary palette
static void BM_HarmonyAssess_Complementary(benchmark::State& state) {
  palette_set<rgba8_t> palette;
  palette.add(rgba8_t{255, 0, 0, 255});    // Red
  palette.add(rgba8_t{0, 255, 255, 255});  // Cyan
  for (auto _ : state) {
    benchmark::DoNotOptimize(assess(palette));
  }
}
BENCHMARK(BM_HarmonyAssess_Complementary);

// Assess triadic palette
static void BM_HarmonyAssess_Triadic(benchmark::State& state) {
  palette_set<rgba8_t> palette;
  palette.add(rgba8_t{255, 0, 0, 255});    // Red
  palette.add(rgba8_t{0, 255, 0, 255});    // Green
  palette.add(rgba8_t{0, 0, 255, 255});    // Blue
  for (auto _ : state) {
    benchmark::DoNotOptimize(assess(palette));
  }
}
BENCHMARK(BM_HarmonyAssess_Triadic);

static void BM_HarmonyAssess_Golden2(benchmark::State& state) {
  palette_set<rgba8_t> palette;
  palette.add(rgba8_t{255, 0, 0, 255});           // Red
  palette.add(rgba8_t{0, 128, 255, 255});         // Cyan-blue (~137.5°)
  for (auto _ : state) {
    benchmark::DoNotOptimize(assess(palette));
  }
}
BENCHMARK(BM_HarmonyAssess_Golden2);

static void BM_HarmonyAssess_Golden3(benchmark::State& state) {
  palette_set<rgba8_t> palette;
  palette.add(rgba8_t{255, 0, 0, 255});           // Red
  palette.add(rgba8_t{0, 128, 255, 255});         // ~137.5°
  palette.add(rgba8_t{128, 255, 0, 255});         // ~275°
  for (auto _ : state) {
    benchmark::DoNotOptimize(assess(palette));
  }
}
BENCHMARK(BM_HarmonyAssess_Golden3);

static void BM_HarmonyAssess_SplitAnalogous(benchmark::State& state) {
  palette_set<rgba8_t> palette;
  palette.add(rgba8_t{255, 0, 0, 255});           // Red
  palette.add(rgba8_t{255, 255, 0, 255});         // Yellow
  palette.add(rgba8_t{128, 255, 0, 255});         // Yellow-green
  for (auto _ : state) {
    benchmark::DoNotOptimize(assess(palette));
  }
}
BENCHMARK(BM_HarmonyAssess_SplitAnalogous);

static void BM_HarmonyAssess_AnalogousWide(benchmark::State& state) {
  palette_set<rgba8_t> palette;
  palette.add(rgba8_t{255, 0, 0, 255});           // Red
  palette.add(rgba8_t{255, 85, 0, 255});          // Red-orange
  palette.add(rgba8_t{255, 170, 0, 255});         // Orange
  palette.add(rgba8_t{255, 255, 0, 255});         // Yellow
  for (auto _ : state) {
    benchmark::DoNotOptimize(assess(palette));
  }
}
BENCHMARK(BM_HarmonyAssess_AnalogousWide);

static void BM_HarmonyAssess_Compound(benchmark::State& state) {
  palette_set<rgba8_t> palette;
  palette.add(rgba8_t{255, 0, 0, 255});           // Red
  palette.add(rgba8_t{255, 128, 0, 255});         // Orange
  palette.add(rgba8_t{0, 128, 255, 255});         // Cyan
  palette.add(rgba8_t{0, 0, 255, 255});           // Blue
  for (auto _ : state) {
    benchmark::DoNotOptimize(assess(palette));
  }
}
BENCHMARK(BM_HarmonyAssess_Compound);

static void BM_HarmonyAssess_DoubleComplementary(benchmark::State& state) {
  palette_set<rgba8_t> palette;
  palette.add(rgba8_t{255, 0, 0, 255});           // Red
  palette.add(rgba8_t{255, 255, 0, 255});         // Yellow
  palette.add(rgba8_t{0, 0, 255, 255});           // Blue
  palette.add(rgba8_t{255, 0, 255, 255});         // Magenta
  for (auto _ : state) {
    benchmark::DoNotOptimize(assess(palette));
  }
}
BENCHMARK(BM_HarmonyAssess_DoubleComplementary);

// Assess analogous palette
static void BM_HarmonyAssess_Analogous(benchmark::State& state) {
  palette_set<rgba8_t> palette;
  palette.add(rgba8_t{255, 0, 0, 255});    // Red
  palette.add(rgba8_t{255, 128, 0, 255});  // Orange
  palette.add(rgba8_t{255, 255, 0, 255});  // Yellow
  for (auto _ : state) {
    benchmark::DoNotOptimize(assess(palette));
  }
}
BENCHMARK(BM_HarmonyAssess_Analogous);

// Assess 5-color palette
static void BM_HarmonyAssess_5Colors(benchmark::State& state) {
  palette_set<rgba8_t> palette;
  palette.add(rgba8_t{255, 0, 0, 255});
  palette.add(rgba8_t{255, 128, 0, 255});
  palette.add(rgba8_t{255, 255, 0, 255});
  palette.add(rgba8_t{0, 255, 0, 255});
  palette.add(rgba8_t{0, 0, 255, 255});
  for (auto _ : state) {
    benchmark::DoNotOptimize(assess(palette));
  }
}
BENCHMARK(BM_HarmonyAssess_5Colors);

// Correct to complementary
static void BM_HarmonyCorrect_Complementary(benchmark::State& state) {
  palette_set<rgba8_t> palette;
  palette.add(rgba8_t{255, 0, 0, 255});
  palette.add(rgba8_t{0, 200, 200, 255});  // Slightly off cyan
  for (auto _ : state) {
    benchmark::DoNotOptimize(correct(palette, harmony_scheme::complementary));
  }
}
BENCHMARK(BM_HarmonyCorrect_Complementary);

// Correct to triadic
static void BM_HarmonyCorrect_Triadic(benchmark::State& state) {
  palette_set<rgba8_t> palette;
  palette.add(rgba8_t{255, 0, 0, 255});
  palette.add(rgba8_t{50, 255, 50, 255});
  palette.add(rgba8_t{50, 50, 255, 255});
  for (auto _ : state) {
    benchmark::DoNotOptimize(correct(palette, harmony_scheme::triadic));
  }
}
BENCHMARK(BM_HarmonyCorrect_Triadic);

// Suggest for bad harmony
static void BM_HarmonySuggest(benchmark::State& state) {
  palette_set<rgba8_t> palette;
  palette.add(rgba8_t{255, 0, 0, 255});
  palette.add(rgba8_t{200, 50, 0, 255});  // Too similar
  for (auto _ : state) {
    benchmark::DoNotOptimize(suggest(palette));
  }
}
BENCHMARK(BM_HarmonySuggest);
