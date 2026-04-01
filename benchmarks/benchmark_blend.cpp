#include <benchmark/benchmark.h>

#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;
using blend_mode = operations::blend::blend_mode;

static void BM_BlendNormal(benchmark::State& state) {
  rgba8_t dst{200, 100, 50, 255};
  rgba8_t src{50, 100, 200, 200};
  for (auto _ : state) {
    benchmark::DoNotOptimize(operations::blend::blend(dst, src, blend_mode::normal));
  }
}
BENCHMARK(BM_BlendNormal);

static void BM_BlendMultiply(benchmark::State& state) {
  rgba8_t dst{200, 100, 50, 255};
  rgba8_t src{50, 100, 200, 200};
  for (auto _ : state) {
    benchmark::DoNotOptimize(operations::blend::blend(dst, src, blend_mode::multiply));
  }
}
BENCHMARK(BM_BlendMultiply);

static void BM_BlendScreen(benchmark::State& state) {
  rgba8_t dst{200, 100, 50, 255};
  rgba8_t src{50, 100, 200, 200};
  for (auto _ : state) {
    benchmark::DoNotOptimize(operations::blend::blend(dst, src, blend_mode::screen));
  }
}
BENCHMARK(BM_BlendScreen);

static void BM_BlendOverlay(benchmark::State& state) {
  rgba8_t dst{200, 100, 50, 255};
  rgba8_t src{50, 100, 200, 200};
  for (auto _ : state) {
    benchmark::DoNotOptimize(operations::blend::blend(dst, src, blend_mode::overlay));
  }
}
BENCHMARK(BM_BlendOverlay);

static void BM_BlendDarken(benchmark::State& state) {
  rgba8_t dst{200, 100, 50, 255};
  rgba8_t src{50, 100, 200, 200};
  for (auto _ : state) {
    benchmark::DoNotOptimize(operations::blend::blend(dst, src, blend_mode::darken));
  }
}
BENCHMARK(BM_BlendDarken);

static void BM_BlendLighten(benchmark::State& state) {
  rgba8_t dst{200, 100, 50, 255};
  rgba8_t src{50, 100, 200, 200};
  for (auto _ : state) {
    benchmark::DoNotOptimize(operations::blend::blend(dst, src, blend_mode::lighten));
  }
}
BENCHMARK(BM_BlendLighten);

static void BM_BlendAddition(benchmark::State& state) {
  rgba8_t dst{200, 100, 50, 255};
  rgba8_t src{50, 100, 200, 200};
  for (auto _ : state) {
    benchmark::DoNotOptimize(operations::blend::blend(dst, src, blend_mode::addition));
  }
}
BENCHMARK(BM_BlendAddition);

static void BM_BlendSubtraction(benchmark::State& state) {
  rgba8_t dst{200, 100, 50, 255};
  rgba8_t src{50, 100, 200, 200};
  for (auto _ : state) {
    benchmark::DoNotOptimize(operations::blend::blend(dst, src, blend_mode::subtraction));
  }
}
BENCHMARK(BM_BlendSubtraction);

static void BM_BlendDifference(benchmark::State& state) {
  rgba8_t dst{200, 100, 50, 255};
  rgba8_t src{50, 100, 200, 200};
  for (auto _ : state) {
    benchmark::DoNotOptimize(operations::blend::blend(dst, src, blend_mode::difference));
  }
}
BENCHMARK(BM_BlendDifference);

static void BM_BlendExclusion(benchmark::State& state) {
  rgba8_t dst{200, 100, 50, 255};
  rgba8_t src{50, 100, 200, 200};
  for (auto _ : state) {
    benchmark::DoNotOptimize(operations::blend::blend(dst, src, blend_mode::exclusion));
  }
}
BENCHMARK(BM_BlendExclusion);

static void BM_BlendHardLight(benchmark::State& state) {
  rgba8_t dst{200, 100, 50, 255};
  rgba8_t src{50, 100, 200, 200};
  for (auto _ : state) {
    benchmark::DoNotOptimize(operations::blend::blend(dst, src, blend_mode::hard_light));
  }
}
BENCHMARK(BM_BlendHardLight);

static void BM_BlendSoftLight(benchmark::State& state) {
  rgba8_t dst{200, 100, 50, 255};
  rgba8_t src{50, 100, 200, 200};
  for (auto _ : state) {
    benchmark::DoNotOptimize(operations::blend::blend(dst, src, blend_mode::soft_light));
  }
}
BENCHMARK(BM_BlendSoftLight);

static void BM_BlendColorDodge(benchmark::State& state) {
  rgba8_t dst{200, 100, 50, 255};
  rgba8_t src{50, 100, 200, 200};
  for (auto _ : state) {
    benchmark::DoNotOptimize(operations::blend::blend(dst, src, blend_mode::color_dodge));
  }
}
BENCHMARK(BM_BlendColorDodge);

static void BM_BlendColorBurn(benchmark::State& state) {
  rgba8_t dst{200, 100, 50, 255};
  rgba8_t src{50, 100, 200, 200};
  for (auto _ : state) {
    benchmark::DoNotOptimize(operations::blend::blend(dst, src, blend_mode::color_burn));
  }
}
BENCHMARK(BM_BlendColorBurn);

static void BM_BlendDivide(benchmark::State& state) {
  rgba8_t dst{200, 100, 50, 255};
  rgba8_t src{50, 100, 200, 200};
  for (auto _ : state) {
    benchmark::DoNotOptimize(operations::blend::blend(dst, src, blend_mode::divide));
  }
}
BENCHMARK(BM_BlendDivide);

static void BM_BlendHue(benchmark::State& state) {
  rgba8_t dst{200, 100, 50, 255};
  rgba8_t src{50, 100, 200, 200};
  for (auto _ : state) {
    benchmark::DoNotOptimize(operations::blend::blend(dst, src, blend_mode::hue));
  }
}
BENCHMARK(BM_BlendHue);

static void BM_BlendSaturation(benchmark::State& state) {
  rgba8_t dst{200, 100, 50, 255};
  rgba8_t src{50, 100, 200, 200};
  for (auto _ : state) {
    benchmark::DoNotOptimize(operations::blend::blend(dst, src, blend_mode::saturation));
  }
}
BENCHMARK(BM_BlendSaturation);

static void BM_BlendColorBlend(benchmark::State& state) {
  rgba8_t dst{200, 100, 50, 255};
  rgba8_t src{50, 100, 200, 200};
  for (auto _ : state) {
    benchmark::DoNotOptimize(operations::blend::blend(dst, src, blend_mode::color_blend));
  }
}
BENCHMARK(BM_BlendColorBlend);

static void BM_BlendLuminosity(benchmark::State& state) {
  rgba8_t dst{200, 100, 50, 255};
  rgba8_t src{50, 100, 200, 200};
  for (auto _ : state) {
    benchmark::DoNotOptimize(operations::blend::blend(dst, src, blend_mode::luminosity));
  }
}
BENCHMARK(BM_BlendLuminosity);
