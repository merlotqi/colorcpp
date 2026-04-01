#include <benchmark/benchmark.h>

#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;
using namespace operations::conversion;

// RGB → OkLab
static void BM_RGBA8_to_OkLab(benchmark::State& state) {
  rgba8_t c{128, 64, 192, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<oklab_t>(c));
  }
}
BENCHMARK(BM_RGBA8_to_OkLab);

// RGB → CIELAB
static void BM_RGBF_to_CIELAB(benchmark::State& state) {
  rgbf_t c{0.3f, 0.6f, 0.9f};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<cielab_t>(c));
  }
}
BENCHMARK(BM_RGBF_to_CIELAB);

// RGB → HSV roundtrip
static void BM_RGBF_to_HSV_roundtrip(benchmark::State& state) {
  rgbf_t c{0.7f, 0.3f, 0.5f};
  for (auto _ : state) {
    auto h = color_cast<hsv_float_t>(c);
    benchmark::DoNotOptimize(color_cast<rgbf_t>(h));
  }
}
BENCHMARK(BM_RGBF_to_HSV_roundtrip);

// RGB → HSL roundtrip
static void BM_RGBF_to_HSL_roundtrip(benchmark::State& state) {
  rgbf_t c{0.7f, 0.3f, 0.5f};
  for (auto _ : state) {
    auto h = color_cast<hsla_float_t>(c);
    benchmark::DoNotOptimize(color_cast<rgbf_t>(h));
  }
}
BENCHMARK(BM_RGBF_to_HSL_roundtrip);

// RGB → XYZ
static void BM_RGBA8_to_XYZ(benchmark::State& state) {
  rgba8_t c{128, 64, 192, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<xyz_t>(c));
  }
}
BENCHMARK(BM_RGBA8_to_XYZ);

// RGB → CMYK roundtrip
static void BM_RGBF_to_CMYK_roundtrip(benchmark::State& state) {
  rgbf_t c{0.3f, 0.6f, 0.9f};
  for (auto _ : state) {
    auto k = color_cast<cmyk_float_t>(c);
    benchmark::DoNotOptimize(color_cast<rgbf_t>(k));
  }
}
BENCHMARK(BM_RGBF_to_CMYK_roundtrip);

// RGB → HWB roundtrip
static void BM_RGBF_to_HWB_roundtrip(benchmark::State& state) {
  rgbf_t c{0.7f, 0.3f, 0.5f};
  for (auto _ : state) {
    auto h = color_cast<hwb_float_t>(c);
    benchmark::DoNotOptimize(color_cast<rgbf_t>(h));
  }
}
BENCHMARK(BM_RGBF_to_HWB_roundtrip);

// RGB → Linear RGB roundtrip
static void BM_RGBF_to_LinearRGB_roundtrip(benchmark::State& state) {
  rgbf_t c{0.7f, 0.3f, 0.5f};
  for (auto _ : state) {
    auto l = color_cast<linear_rgbf_t>(c);
    benchmark::DoNotOptimize(color_cast<rgbf_t>(l));
  }
}
BENCHMARK(BM_RGBF_to_LinearRGB_roundtrip);

// HSV → HSL
static void BM_HSV_to_HSL(benchmark::State& state) {
  hsv_float_t c{120.0f, 0.8f, 0.6f};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<hsla_float_t>(c));
  }
}
BENCHMARK(BM_HSV_to_HSL);

// HSL → HSV
static void BM_HSL_to_HSV(benchmark::State& state) {
  hsla_float_t c{120.0f, 0.8f, 0.6f, 1.0f};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<hsv_float_t>(c));
  }
}
BENCHMARK(BM_HSL_to_HSV);

// OkLab → CIELAB
static void BM_OkLab_to_CIELAB(benchmark::State& state) {
  oklab_t c{0.65f, 0.12f, -0.08f};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<cielab_t>(c));
  }
}
BENCHMARK(BM_OkLab_to_CIELAB);

// CIELAB → OkLab
static void BM_CIELAB_to_OkLab(benchmark::State& state) {
  cielab_t c{65.0f, 12.0f, -8.0f};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<oklab_t>(c));
  }
}
BENCHMARK(BM_CIELAB_to_OkLab);

// RGBA8 → RGBF
static void BM_RGBA8_to_RGBF(benchmark::State& state) {
  rgba8_t c{128, 64, 192, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<rgbf_t>(c));
  }
}
BENCHMARK(BM_RGBA8_to_RGBF);

// RGBF → RGBA8
static void BM_RGBF_to_RGBA8(benchmark::State& state) {
  rgbf_t c{0.5f, 0.25f, 0.75f};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<rgba8_t>(c));
  }
}
BENCHMARK(BM_RGBF_to_RGBA8);
