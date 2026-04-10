#include <benchmark/benchmark.h>

#include <cstdint>
#include <vector>

#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;
using namespace colorcpp::algorithms;
using blend_mode = colorcpp::operations::blend::blend_mode;

namespace {

float to_unit_float(uint32_t value) {
  return static_cast<float>(value & 0x00FFFFFFu) / static_cast<float>(0x01000000u);
}

std::vector<rgbaf_t> make_rgba_data(std::size_t count, uint32_t seed) {
  std::vector<rgbaf_t> out;
  out.reserve(count);

  uint32_t state = seed;
  for (std::size_t i = 0; i < count; ++i) {
    state = state * 1664525u + 1013904223u;
    const float r = to_unit_float(state);
    state = state * 1664525u + 1013904223u;
    const float g = to_unit_float(state);
    state = state * 1664525u + 1013904223u;
    const float b = to_unit_float(state);
    state = state * 1664525u + 1013904223u;
    const float a = 0.2f + 0.8f * to_unit_float(state);
    out.emplace_back(r, g, b, a);
  }

  return out;
}

std::vector<oklab_t> make_oklab_data(std::size_t count, uint32_t seed) {
  std::vector<oklab_t> out;
  out.reserve(count);

  uint32_t state = seed;
  for (std::size_t i = 0; i < count; ++i) {
    state = state * 1664525u + 1013904223u;
    const float l = to_unit_float(state);
    state = state * 1664525u + 1013904223u;
    const float a = -0.4f + 0.8f * to_unit_float(state);
    state = state * 1664525u + 1013904223u;
    const float b = -0.4f + 0.8f * to_unit_float(state);
    out.emplace_back(l, a, b);
  }

  return out;
}

std::vector<linear_rgbaf_t> make_linear_rgba_data(std::size_t count, uint32_t seed) {
  std::vector<linear_rgbaf_t> out;
  out.reserve(count);

  uint32_t state = seed;
  for (std::size_t i = 0; i < count; ++i) {
    state = state * 1664525u + 1013904223u;
    const float r = to_unit_float(state);
    state = state * 1664525u + 1013904223u;
    const float g = to_unit_float(state);
    state = state * 1664525u + 1013904223u;
    const float b = to_unit_float(state);
    state = state * 1664525u + 1013904223u;
    const float a = 0.2f + 0.8f * to_unit_float(state);
    out.emplace_back(r, g, b, a);
  }

  return out;
}

template <typename InColor, typename OutColor, typename Fn>
void run_unary_batch(benchmark::State& state, const std::vector<InColor>& input, Fn&& fn) {
  std::vector<OutColor> out(input.size());

  for (auto _ : state) {
    for (std::size_t i = 0; i < input.size(); ++i) {
      out[i] = fn(input[i]);
    }
    benchmark::DoNotOptimize(out.data());
    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(input.size()));
}

void run_blend_batch(benchmark::State& state, blend_mode mode) {
  const auto count = static_cast<std::size_t>(state.range(0));
  const auto dst = make_rgba_data(count, 0x12345678u);
  const auto src = make_rgba_data(count, 0x9ABCDEF0u);
  std::vector<rgbaf_t> out(count);

  for (auto _ : state) {
    for (std::size_t i = 0; i < count; ++i) {
      out[i] = operations::blend::blend(dst[i], src[i], mode, 0.8f);
    }
    benchmark::ClobberMemory();
  }

  state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(count));
}

}  // namespace

static void BM_BlendMultiplyBatch(benchmark::State& state) { run_blend_batch(state, blend_mode::multiply); }
BENCHMARK(BM_BlendMultiplyBatch)->Arg(1024)->Arg(65536)->Arg(262144);

static void BM_BlendOverlayBatch(benchmark::State& state) { run_blend_batch(state, blend_mode::overlay); }
BENCHMARK(BM_BlendOverlayBatch)->Arg(1024)->Arg(65536)->Arg(262144);

static void BM_BlendHardLightBatch(benchmark::State& state) { run_blend_batch(state, blend_mode::hard_light); }
BENCHMARK(BM_BlendHardLightBatch)->Arg(1024)->Arg(65536)->Arg(262144);

static void BM_DeltaEOkBatch(benchmark::State& state) {
  const auto count = static_cast<std::size_t>(state.range(0));
  const auto a = make_oklab_data(count, 0x24681357u);
  const auto b = make_oklab_data(count, 0x13572468u);

  for (auto _ : state) {
    float sum = 0.0f;
    for (std::size_t i = 0; i < count; ++i) {
      sum += delta_e::delta_e_ok(a[i], b[i]);
    }
    benchmark::DoNotOptimize(sum);
  }

  state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(count));
}
BENCHMARK(BM_DeltaEOkBatch)->Arg(1024)->Arg(65536)->Arg(262144);

static void BM_VisionProtanopiaBatch(benchmark::State& state) {
  const auto count = static_cast<std::size_t>(state.range(0));
  const auto input = make_rgba_data(count, 0xA1B2C3D4u);
  run_unary_batch<rgbaf_t, rgbaf_t>(state, input, [](const rgbaf_t& c) { return vision::simulate_protanopia(c); });
}
BENCHMARK(BM_VisionProtanopiaBatch)->Arg(1024)->Arg(65536)->Arg(262144);

static void BM_VisionAchromatopsiaBatch(benchmark::State& state) {
  const auto count = static_cast<std::size_t>(state.range(0));
  const auto input = make_rgba_data(count, 0xCAFEBABEu);
  run_unary_batch<rgbaf_t, rgbaf_t>(state, input,
                                    [](const rgbaf_t& c) { return vision::simulate_achromatopsia(c); });
}
BENCHMARK(BM_VisionAchromatopsiaBatch)->Arg(1024)->Arg(65536)->Arg(262144);

static void BM_VisionMachadoBatch(benchmark::State& state) {
  const auto count = static_cast<std::size_t>(state.range(0));
  const auto input = make_rgba_data(count, 0x55AA10F0u);
  run_unary_batch<rgbaf_t, rgbaf_t>(state, input,
                                    [](const rgbaf_t& c) { return vision::simulate_protan_machado(c, 0.6f); });
}
BENCHMARK(BM_VisionMachadoBatch)->Arg(1024)->Arg(65536)->Arg(262144);

static void BM_ConversionSrgbToLinearBatch(benchmark::State& state) {
  const auto count = static_cast<std::size_t>(state.range(0));
  const auto input = make_rgba_data(count, 0x10293847u);
  run_unary_batch<rgbaf_t, linear_rgbaf_t>(
      state, input, [](const rgbaf_t& c) { return operations::conversion::color_cast<linear_rgbaf_t>(c); });
}
BENCHMARK(BM_ConversionSrgbToLinearBatch)->Arg(1024)->Arg(65536)->Arg(262144);

static void BM_ConversionLinearToXyzBatch(benchmark::State& state) {
  const auto count = static_cast<std::size_t>(state.range(0));
  const auto input = make_linear_rgba_data(count, 0x99887766u);
  run_unary_batch<linear_rgbaf_t, xyz_t>(
      state, input, [](const linear_rgbaf_t& c) { return operations::conversion::color_cast<xyz_t>(c); });
}
BENCHMARK(BM_ConversionLinearToXyzBatch)->Arg(1024)->Arg(65536)->Arg(262144);

static void BM_ConversionLinearToOklabBatch(benchmark::State& state) {
  const auto count = static_cast<std::size_t>(state.range(0));
  const auto input = make_linear_rgba_data(count, 0x31415926u);
  run_unary_batch<linear_rgbaf_t, oklab_t>(
      state, input, [](const linear_rgbaf_t& c) { return operations::conversion::color_cast<oklab_t>(c); });
}
BENCHMARK(BM_ConversionLinearToOklabBatch)->Arg(1024)->Arg(65536)->Arg(262144);

static void BM_ConversionSrgbToOklabBatch(benchmark::State& state) {
  const auto count = static_cast<std::size_t>(state.range(0));
  const auto input = make_rgba_data(count, 0x27182818u);
  run_unary_batch<rgbaf_t, oklab_t>(
      state, input, [](const rgbaf_t& c) { return operations::conversion::color_cast<oklab_t>(c); });
}
BENCHMARK(BM_ConversionSrgbToOklabBatch)->Arg(1024)->Arg(65536)->Arg(262144);
