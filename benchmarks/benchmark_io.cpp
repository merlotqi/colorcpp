#include <benchmark/benchmark.h>

#include <colorcpp/colorcpp.hpp>

#include <sstream>
#include <string>

using namespace colorcpp;

static void BM_ParseRGBText(benchmark::State& state) {
  const std::string input = "rgb(128, 64, 192)";
  for (auto _ : state) {
    std::istringstream iss(input);
    rgba8_t c;
    iss >> c;
    benchmark::DoNotOptimize(c);
  }
}
BENCHMARK(BM_ParseRGBText);

static void BM_FormatRGBText(benchmark::State& state) {
  rgba8_t c{128, 64, 192, 255};
  for (auto _ : state) {
    std::ostringstream oss;
    oss << c;
    benchmark::DoNotOptimize(oss.str());
  }
}
BENCHMARK(BM_FormatRGBText);

static void BM_FormatRGBHex(benchmark::State& state) {
  rgba8_t c{128, 64, 192, 255};
  for (auto _ : state) {
    std::ostringstream oss;
    oss << std::hex << c;
    benchmark::DoNotOptimize(oss.str());
  }
}
BENCHMARK(BM_FormatRGBHex);
