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

// =====================================================================
// New short-link benchmarks (added after conversion chain optimization)
// =====================================================================

// Display P3 → sRGB (short link: 1 hop instead of 4)
static void BM_DisplayP3_to_sRGB(benchmark::State& state) {
  display_p3f_t c{0.8f, 0.5f, 0.3f};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<rgbf_t>(c));
  }
}
BENCHMARK(BM_DisplayP3_to_sRGB);

// sRGB → Display P3 (short link: 1 hop instead of 4)
static void BM_sRGB_to_DisplayP3(benchmark::State& state) {
  rgbf_t c{0.8f, 0.5f, 0.3f};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<display_p3f_t>(c));
  }
}
BENCHMARK(BM_sRGB_to_DisplayP3);

// Display P3 → sRGB roundtrip
static void BM_DisplayP3_sRGB_roundtrip(benchmark::State& state) {
  display_p3f_t c{0.6f, 0.4f, 0.7f};
  for (auto _ : state) {
    auto s = color_cast<rgbf_t>(c);
    benchmark::DoNotOptimize(color_cast<display_p3f_t>(s));
  }
}
BENCHMARK(BM_DisplayP3_sRGB_roundtrip);

// sRGB → CIELAB roundtrip (short link: 1 hop)
static void BM_RGBF_to_CIELAB_roundtrip(benchmark::State& state) {
  rgbf_t c{0.7f, 0.3f, 0.5f};
  for (auto _ : state) {
    auto lab = color_cast<cielab_t>(c);
    benchmark::DoNotOptimize(color_cast<rgbf_t>(lab));
  }
}
BENCHMARK(BM_RGBF_to_CIELAB_roundtrip);

// HSL → Linear RGB (short link: 1 hop instead of 2)
static void BM_HSL_to_LinearRGB(benchmark::State& state) {
  hsl_float_t c{120.0f, 0.8f, 0.6f};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<linear_rgbf_t>(c));
  }
}
BENCHMARK(BM_HSL_to_LinearRGB);

// Linear RGB → HSL (short link: 1 hop instead of 2)
static void BM_LinearRGB_to_HSL(benchmark::State& state) {
  linear_rgbf_t c{0.5f, 0.3f, 0.8f};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<hsl_float_t>(c));
  }
}
BENCHMARK(BM_LinearRGB_to_HSL);

// HSL → Linear RGB roundtrip
static void BM_HSL_to_LinearRGB_roundtrip(benchmark::State& state) {
  hsl_float_t c{200.0f, 0.7f, 0.5f};
  for (auto _ : state) {
    auto lin = color_cast<linear_rgbf_t>(c);
    benchmark::DoNotOptimize(color_cast<hsl_float_t>(lin));
  }
}
BENCHMARK(BM_HSL_to_LinearRGB_roundtrip);

// HSV → Linear RGB (short link: 1 hop instead of 2)
static void BM_HSV_to_LinearRGB(benchmark::State& state) {
  hsv_float_t c{60.0f, 0.9f, 0.7f};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<linear_rgbf_t>(c));
  }
}
BENCHMARK(BM_HSV_to_LinearRGB);

// Linear RGB → HSV (short link: 1 hop instead of 2)
static void BM_LinearRGB_to_HSV(benchmark::State& state) {
  linear_rgbf_t c{0.5f, 0.3f, 0.8f};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<hsv_float_t>(c));
  }
}
BENCHMARK(BM_LinearRGB_to_HSV);

// HSV → Linear RGB roundtrip
static void BM_HSV_to_LinearRGB_roundtrip(benchmark::State& state) {
  hsv_float_t c{240.0f, 0.6f, 0.4f};
  for (auto _ : state) {
    auto lin = color_cast<linear_rgbf_t>(c);
    benchmark::DoNotOptimize(color_cast<hsv_float_t>(lin));
  }
}
BENCHMARK(BM_HSV_to_LinearRGB_roundtrip);

// HWB → Linear RGB (short link: 1 hop instead of 2)
static void BM_HWB_to_LinearRGB(benchmark::State& state) {
  hwb_float_t c{300.0f, 0.2f, 0.1f};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<linear_rgbf_t>(c));
  }
}
BENCHMARK(BM_HWB_to_LinearRGB);

// Linear RGB → HWB (short link: 1 hop instead of 2)
static void BM_LinearRGB_to_HWB(benchmark::State& state) {
  linear_rgbf_t c{0.5f, 0.3f, 0.8f};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<hwb_float_t>(c));
  }
}
BENCHMARK(BM_LinearRGB_to_HWB);

// HWB → Linear RGB roundtrip
static void BM_HWB_to_LinearRGB_roundtrip(benchmark::State& state) {
  hwb_float_t c{180.0f, 0.15f, 0.05f};
  for (auto _ : state) {
    auto lin = color_cast<linear_rgbf_t>(c);
    benchmark::DoNotOptimize(color_cast<hwb_float_t>(lin));
  }
}
BENCHMARK(BM_HWB_to_LinearRGB_roundtrip);

// HSL → OkLab (multi-hop, benefits from new HSL → Linear RGB short link)
static void BM_HSL_to_OkLab(benchmark::State& state) {
  hsl_float_t c{150.0f, 0.6f, 0.4f};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<oklab_t>(c));
  }
}
BENCHMARK(BM_HSL_to_OkLab);

// HSL → CIELAB (multi-hop, benefits from new short links)
static void BM_HSL_to_CIELAB(benchmark::State& state) {
  hsl_float_t c{210.0f, 0.7f, 0.5f};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<cielab_t>(c));
  }
}
BENCHMARK(BM_HSL_to_CIELAB);

// Display P3 → CIELAB (multi-hop, benefits from Display P3 ↔ sRGB short link)
static void BM_DisplayP3_to_CIELAB(benchmark::State& state) {
  display_p3f_t c{0.5f, 0.7f, 0.3f};
  for (auto _ : state) {
    benchmark::DoNotOptimize(color_cast<cielab_t>(c));
  }
}
BENCHMARK(BM_DisplayP3_to_CIELAB);
