#include <benchmark/benchmark.h>

#include <colorcpp/colorcpp.hpp>
#include <sstream>
#include <string>

using namespace colorcpp;

// Parse RGB text
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

// Parse RGBA text
static void BM_ParseRGBAText(benchmark::State& state) {
  const std::string input = "rgba(128, 64, 192, 0.5)";
  for (auto _ : state) {
    std::istringstream iss(input);
    rgba8_t c;
    iss >> c;
    benchmark::DoNotOptimize(c);
  }
}
BENCHMARK(BM_ParseRGBAText);

// Parse HSL text
static void BM_ParseHSLText(benchmark::State& state) {
  const std::string input = "hsl(120, 50%, 75%)";
  for (auto _ : state) {
    std::istringstream iss(input);
    rgba8_t c;
    iss >> c;
    benchmark::DoNotOptimize(c);
  }
}
BENCHMARK(BM_ParseHSLText);

// Parse hex text
static void BM_ParseHexText(benchmark::State& state) {
  const std::string input = "#8040c0";
  for (auto _ : state) {
    std::istringstream iss(input);
    rgba8_t c;
    iss >> c;
    benchmark::DoNotOptimize(c);
  }
}
BENCHMARK(BM_ParseHexText);

// Parse hex with alpha
static void BM_ParseHexAlpha(benchmark::State& state) {
  const std::string input = "#8040c0ff";
  for (auto _ : state) {
    std::istringstream iss(input);
    rgba8_t c;
    iss >> c;
    benchmark::DoNotOptimize(c);
  }
}
BENCHMARK(BM_ParseHexAlpha);

// Format RGB text
static void BM_FormatRGBText(benchmark::State& state) {
  rgba8_t c{128, 64, 192, 255};
  for (auto _ : state) {
    std::ostringstream oss;
    oss << c;
    benchmark::DoNotOptimize(oss.str());
  }
}
BENCHMARK(BM_FormatRGBText);

// Format RGB hex
static void BM_FormatRGBHex(benchmark::State& state) {
  rgba8_t c{128, 64, 192, 255};
  for (auto _ : state) {
    std::ostringstream oss;
    oss << std::hex << c;
    benchmark::DoNotOptimize(oss.str());
  }
}
BENCHMARK(BM_FormatRGBHex);

// Format HSL text
static void BM_FormatHSLText(benchmark::State& state) {
  rgba8_t c{128, 64, 192, 255};
  for (auto _ : state) {
    std::ostringstream oss;
    oss << std::hex << c;
    benchmark::DoNotOptimize(oss.str());
  }
}
BENCHMARK(BM_FormatHSLText);
