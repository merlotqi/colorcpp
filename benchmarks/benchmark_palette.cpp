#include <benchmark/benchmark.h>

#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;
using operations::palette::easing_scale;
using operations::palette::easing_type;
using operations::palette::lerp_method;
using operations::palette::linear_scale;
using operations::palette::multi_scale;
using operations::palette::perceptual_scale;
using operations::palette::rainbow;
using operations::palette::spline_scale;
using operations::palette::visual_scale;
namespace palette_schemes = operations::palette::schemes;

// Complementary palette
static void BM_Palette_Complementary(benchmark::State& state) {
  rgba8_t base{255, 0, 0, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(palette_schemes::complementary(base));
  }
}
BENCHMARK(BM_Palette_Complementary);

// Analogous palette
static void BM_Palette_Analogous(benchmark::State& state) {
  rgba8_t base{255, 0, 0, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(palette_schemes::analogous(base));
  }
}
BENCHMARK(BM_Palette_Analogous);

// Triadic palette
static void BM_Palette_Triadic(benchmark::State& state) {
  rgba8_t base{255, 0, 0, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(palette_schemes::triadic(base));
  }
}
BENCHMARK(BM_Palette_Triadic);

// Split complementary palette
static void BM_Palette_SplitComplementary(benchmark::State& state) {
  rgba8_t base{255, 0, 0, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(palette_schemes::split_complementary(base));
  }
}
BENCHMARK(BM_Palette_SplitComplementary);

// Tetradic palette
static void BM_Palette_Tetradic(benchmark::State& state) {
  rgba8_t base{255, 0, 0, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(palette_schemes::tetradic(base));
  }
}
BENCHMARK(BM_Palette_Tetradic);

// Square palette
static void BM_Palette_Square(benchmark::State& state) {
  rgba8_t base{255, 0, 0, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(palette_schemes::square(base));
  }
}
BENCHMARK(BM_Palette_Square);

// Monochromatic palette (5 colors)
static void BM_Palette_Monochromatic_5(benchmark::State& state) {
  rgba8_t base{128, 64, 192, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(palette_schemes::monochromatic(base, 5));
  }
}
BENCHMARK(BM_Palette_Monochromatic_5);

// Monochromatic palette (10 colors)
static void BM_Palette_Monochromatic_10(benchmark::State& state) {
  rgba8_t base{128, 64, 192, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(palette_schemes::monochromatic(base, 10));
  }
}
BENCHMARK(BM_Palette_Monochromatic_10);

// Linear scale (8 colors)
static void BM_Palette_LinearScale_8(benchmark::State& state) {
  rgba8_t a{255, 0, 0, 255};
  rgba8_t b{0, 0, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(linear_scale(a, b, 8));
  }
}
BENCHMARK(BM_Palette_LinearScale_8);

// Visual scale (8 colors)
static void BM_Palette_VisualScale_8(benchmark::State& state) {
  rgba8_t a{255, 0, 0, 255};
  rgba8_t b{0, 0, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(visual_scale(a, b, 8));
  }
}
BENCHMARK(BM_Palette_VisualScale_8);

// Perceptual scale (8 colors)
static void BM_Palette_PerceptualScale_8(benchmark::State& state) {
  rgba8_t a{255, 0, 0, 255};
  rgba8_t b{0, 0, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(perceptual_scale(a, b, 8));
  }
}
BENCHMARK(BM_Palette_PerceptualScale_8);

// Rainbow (7 colors)
static void BM_Palette_Rainbow_7(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(rainbow<rgba8_t>(7));
  }
}
BENCHMARK(BM_Palette_Rainbow_7);

// Rainbow (12 colors)
static void BM_Palette_Rainbow_12(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(rainbow<rgba8_t>(12));
  }
}
BENCHMARK(BM_Palette_Rainbow_12);

// ============================================================================
// Advanced scales benchmarks
// ============================================================================

static void BM_EasingScale_Linear(benchmark::State& state) {
  rgba8_t a{255, 0, 0, 255};
  rgba8_t b{0, 0, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(easing_scale(a, b, 16, easing_type::linear));
  }
}
BENCHMARK(BM_EasingScale_Linear);

static void BM_EasingScale_EaseInOut(benchmark::State& state) {
  rgba8_t a{255, 0, 0, 255};
  rgba8_t b{0, 0, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(easing_scale(a, b, 16, easing_type::ease_in_out));
  }
}
BENCHMARK(BM_EasingScale_EaseInOut);

static void BM_EasingScale_Bounce(benchmark::State& state) {
  rgba8_t a{255, 0, 0, 255};
  rgba8_t b{0, 0, 255, 255};
  for (auto _ : state) {
    benchmark::DoNotOptimize(easing_scale(a, b, 16, easing_type::bounce));
  }
}
BENCHMARK(BM_EasingScale_Bounce);

static void BM_SplineScale_4Points(benchmark::State& state) {
  std::vector<rgba8_t> control;
  control.push_back(rgba8_t{255, 0, 0, 255});
  control.push_back(rgba8_t{255, 255, 0, 255});
  control.push_back(rgba8_t{0, 255, 0, 255});
  control.push_back(rgba8_t{0, 0, 255, 255});
  for (auto _ : state) {
    benchmark::DoNotOptimize(spline_scale(control, 16));
  }
}
BENCHMARK(BM_SplineScale_4Points);

static void BM_SplineScale_6Points(benchmark::State& state) {
  std::vector<rgba8_t> control;
  control.push_back(rgba8_t{255, 0, 0, 255});
  control.push_back(rgba8_t{255, 128, 0, 255});
  control.push_back(rgba8_t{255, 255, 0, 255});
  control.push_back(rgba8_t{0, 255, 0, 255});
  control.push_back(rgba8_t{0, 0, 255, 255});
  control.push_back(rgba8_t{128, 0, 255, 255});
  for (auto _ : state) {
    benchmark::DoNotOptimize(spline_scale(control, 24));
  }
}
BENCHMARK(BM_SplineScale_6Points);

static void BM_MultiScale_3Colors_OkLab(benchmark::State& state) {
  std::vector<rgba8_t> colors;
  colors.push_back(rgba8_t{255, 0, 0, 255});
  colors.push_back(rgba8_t{0, 255, 0, 255});
  colors.push_back(rgba8_t{0, 0, 255, 255});
  for (auto _ : state) {
    benchmark::DoNotOptimize(multi_scale(colors, 16, lerp_method::oklab));
  }
}
BENCHMARK(BM_MultiScale_3Colors_OkLab);

static void BM_MultiScale_3Colors_HSL(benchmark::State& state) {
  std::vector<rgba8_t> colors;
  colors.push_back(rgba8_t{255, 0, 0, 255});
  colors.push_back(rgba8_t{0, 255, 0, 255});
  colors.push_back(rgba8_t{0, 0, 255, 255});
  for (auto _ : state) {
    benchmark::DoNotOptimize(multi_scale(colors, 16, lerp_method::hsl));
  }
}
BENCHMARK(BM_MultiScale_3Colors_HSL);

static void BM_MultiScale_5Colors(benchmark::State& state) {
  std::vector<rgba8_t> colors;
  colors.push_back(rgba8_t{255, 0, 0, 255});
  colors.push_back(rgba8_t{255, 255, 0, 255});
  colors.push_back(rgba8_t{0, 255, 0, 255});
  colors.push_back(rgba8_t{0, 255, 255, 255});
  colors.push_back(rgba8_t{0, 0, 255, 255});
  for (auto _ : state) {
    benchmark::DoNotOptimize(multi_scale(colors, 20, lerp_method::oklab));
  }
}
BENCHMARK(BM_MultiScale_5Colors);
