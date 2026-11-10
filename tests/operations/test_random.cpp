#include <gtest/gtest.h>

#include <cmath>
#include <colorcpp/colorcpp.hpp>
// Not pulled in by colorcpp.hpp / random.hpp; include explicitly.
#include <colorcpp/operations/random/poisson_disk_sampling.hpp>
#include <set>

using namespace colorcpp;
using namespace colorcpp::operations;
using namespace colorcpp::operations::random;

TEST(RandomBasic, Rgb8Reproducible) {
  rgb8_generator g(12345u);
  auto a = g.next();
  rgb8_generator g2(12345u);
  auto b = g2.next();
  EXPECT_EQ(a.r(), b.r());
  EXPECT_EQ(a.g(), b.g());
  EXPECT_EQ(a.b(), b.b());
}

TEST(RandomBasic, RandomColorMatchesGenerator) {
  auto c = random_color<core::rgbf_t>(99u);
  rgbf_generator g(99u);
  auto d = g.next();
  EXPECT_FLOAT_EQ(c.r(), d.r());
  EXPECT_FLOAT_EQ(c.g(), d.g());
  EXPECT_FLOAT_EQ(c.b(), d.b());
}

TEST(HslGenerator, InvalidOptionsThrow) {
  hsl_generator::options bad{};
  bad.h_min = 200.f;
  bad.h_max = 100.f;
  EXPECT_THROW(hsl_generator(0u, bad), std::invalid_argument);
}

TEST(HslGenerator, SetOptionsValidates) {
  hsl_generator g(1u);
  hsl_generator::options bad{};
  bad.s_min = 0.8f;
  bad.s_max = 0.2f;
  EXPECT_THROW(g.set_options(bad), std::invalid_argument);
}

TEST(HslaGenerator, AlphaVaries) {
  hsla_generator g(42u);
  std::set<float> alphas;
  for (int i = 0; i < 40; ++i) alphas.insert(g.next().a());
  EXPECT_GT(alphas.size(), 1u);
}

TEST(HslaGenerator, AlphaRangeRespected) {
  hsla_generator::options o{};
  o.a_min = 0.4f;
  o.a_max = 0.6f;
  hsla_generator g(7u, o);
  for (int i = 0; i < 20; ++i) {
    float a = g.next().a();
    EXPECT_GE(a, 0.4f);
    EXPECT_LE(a, 0.6f);
  }
}

TEST(HslaGenerator, InvalidAlphaRangeThrows) {
  hsla_generator::options o{};
  o.a_min = 1.f;
  o.a_max = 0.f;
  EXPECT_THROW(hsla_generator(0u, o), std::invalid_argument);
}

TEST(GoldenAngle, FirstSampleIsInitialHue) {
  golden_gen g1(1000u);
  golden_gen g2(1000u);
  auto c1 = g1.next();
  auto c2 = g2.next();
  EXPECT_FLOAT_EQ(c1.h(), c2.h());
  EXPECT_FLOAT_EQ(c1.s(), c2.s());
  EXPECT_FLOAT_EQ(c1.l(), c2.l());
}

TEST(GoldenAngle, SecondStepAdvancesHue) {
  golden_gen g(2000u);
  float h0 = g.next().h();
  float h1 = g.next().h();
  float expected = std::fmod(h0 + 137.5077640500378f, 360.0f);
  EXPECT_NEAR(h1, expected, 1e-4f);
}

// --- Issue #97: golden_angle built core::hsl_float_t{current_hue, ...} and
//     cast it to the target: for OkLCH/CIELCH (hue at channel index 2) the
//     HSL hue is not the model's hue, so consecutive hues were ~118 deg apart
//     instead of 137.5 and the step hue never landed on the hue channel.
//     The step hue must be written to the channel tagged traits::hue_tag. ---

TEST(GoldenAngle, OklchHueIsWrittenToHueChannel) {
  golden_angle_generator<core::oklch_t> g(42u);
  auto c = g.next();
  // Initial hue is the first draw, spec bit pattern (issue #96), and must be
  // the model's own hue channel — not the OkLCH hue of a converted HSL color.
  const float expected =
      static_cast<float>(static_cast<double>(0.0f) +
                         (static_cast<double>(360.0f) - static_cast<double>(0.0f)) * (1608637542.0 / 4294967296.0));
  EXPECT_EQ(c.h(), expected);
}

TEST(GoldenAngle, OklchStepAdvancesHueByGoldenAngle) {
  golden_angle_generator<core::oklch_t> g(2000u);
  float h0 = g.next().h();
  float h1 = g.next().h();
  float expected = std::fmod(h0 + 137.5077640500378f, 360.0f);
  EXPECT_NEAR(h1, expected, 1e-4f) << "oklch hue step must be the golden angle on the hue channel";
}

TEST(GoldenAngle, OklchKeepsLightnessAndChromaAcrossSteps) {
  // "Sequential hues separated by the golden angle": consecutive samples
  // differ only in hue, on every target model.
  golden_angle_generator<core::oklch_t> g(2000u);
  auto a = g.next();
  auto b = g.next();
  EXPECT_FLOAT_EQ(a.l(), b.l());
  EXPECT_FLOAT_EQ(a.c(), b.c());
}

// --- Issue #97 (harmony): next_harmony read the hue from index 0 and
//     construct_from_hue replaced index 0 — HSL/HSV layout. For OkLCH that
//     overwrote L and left the hue untouched (and hue_max() = max_at<0>() was
//     L's max, so the harmony offsets were garbage too). ---

TEST(HarmonyPoisson, OklchComplementaryRotatesHueChannel) {
  harmony_generator<core::oklch_t> gen(400u);
  const core::oklch_t base{0.7f, 0.15f, 40.0f};
  auto c = gen.next_harmony(base, harmony_mode::complementary);
  EXPECT_NEAR(c.h(), 220.0f, 1e-3f) << "complementary = base hue + 180 on the model's hue channel";
  EXPECT_FLOAT_EQ(c.l(), 0.7f) << "lightness must be untouched";
  EXPECT_FLOAT_EQ(c.c(), 0.15f) << "chroma must be untouched";
}

// --- Issue #97 (sampling): basic_hsl_generator's H/S/L options must reach
//     the model's hue/chroma/value channels wherever they sit (OkLCH is
//     L, C, H — hue at index 2), not positional indices 0/1/2. ---

TEST(HslGenerator, OklchOptionsRouteByChannelRole) {
  basic_hsl_generator<core::oklch_t> g(42u);
  basic_hsl_generator<core::oklch_t>::options o;
  o.h_min = 100.0f;
  o.h_max = 120.0f;  // hue channel (index 2)
  o.s_min = 0.1f;
  o.s_max = 0.2f;  // chroma channel (index 1)
  o.l_min = 0.4f;
  o.l_max = 0.5f;  // value channel — L (index 0)
  g.set_options(o);
  for (int i = 0; i < 20; ++i) {
    const auto c = g.next();
    EXPECT_GE(c.h(), 100.0f) << "hue below hue range at iteration " << i;
    EXPECT_LE(c.h(), 120.0f) << "hue above hue range at iteration " << i;
    EXPECT_GE(c.c(), 0.1f) << "chroma below saturation range at iteration " << i;
    EXPECT_LE(c.c(), 0.2f) << "chroma above saturation range at iteration " << i;
    EXPECT_GE(c.l(), 0.4f) << "L below lightness range at iteration " << i;
    EXPECT_LE(c.l(), 0.5f) << "L above lightness range at iteration " << i;
  }
}

TEST(HslGenerator, OklchDefaultRangesMatchChannelRoles) {
  // Default option bounds come from each role's channel max (issue #97), not
  // from positional max_at<0/1/2> — which gave OkLCH (h, s, l) = (2, 2, 360):
  // hue_max = max_at<0>() was L's max and l_max was the hue ring.
  const basic_hsl_generator<core::oklch_t>::options o{};
  EXPECT_FLOAT_EQ(o.h_max, 360.0f) << "hue default bound = hue channel max";
  EXPECT_FLOAT_EQ(o.s_max, 2.0f) << "saturation default bound = chroma channel max (okc nominal)";
  EXPECT_FLOAT_EQ(o.l_max, 2.0f) << "lightness default bound = value channel max (okl nominal)";
}

TEST(HarmonyPoisson, AppendsWhenFeasible) {
  std::mt19937 rng(300u);
  harmony_gen hgen(rng);
  std::vector<float> samples;
  auto c = hgen.next_poisson(samples, 5.f, 200);
  (void)c;
  EXPECT_EQ(samples.size(), 1u);
}

TEST(HarmonyPoisson, FallbackDoesNotAppendViolatingHue) {
  harmony_gen hgen(400u);
  std::vector<float> samples;
  samples.push_back(0.f);
  samples.push_back(1.f);
  samples.push_back(2.f);
  std::size_t before = samples.size();
  (void)hgen.next_poisson(samples, 180.f, 2u);
  EXPECT_EQ(samples.size(), before);
}

// ============================================================
// Poisson Disk Sampling Tests
// ============================================================

TEST(PoissonDisk, DefaultDistanceIsOkLabScale) {
  // Before the fix, the default was 10 (CIE ΔE scale), which is unreachable
  // in OkLab (box diagonal is 3), so generate() could only ever return 1 color.
  poisson_disk_generator<core::oklab_t> gen(42u);
  EXPECT_FLOAT_EQ(gen.min_distance(), 0.1f);
}

TEST(PoissonDisk, RepeatedCallsAdvanceEngine) {
  // generate() used to build its sampler from a copy of the engine and never
  // write the state back, so every call replayed the identical palette.
  poisson_disk_generator<core::oklab_t> gen(42u);
  auto first = gen.generate(8);
  auto second = gen.generate(8);
  ASSERT_EQ(first.size(), 8u);
  ASSERT_EQ(second.size(), 8u);
  bool any_diff = false;
  for (std::size_t i = 0; i < first.size(); ++i) {
    if (first[i] != second[i]) any_diff = true;
  }
  EXPECT_TRUE(any_diff) << "consecutive generate() calls returned identical palettes";
}

TEST(PoissonDisk, NonPositiveMinDistanceClampsAndTerminates) {
  // min_distance <= 0 makes every candidate valid, so generate_max() never
  // saw a failure and looped forever. The constructor must clamp to a
  // positive default instead.
  poisson_disk_generator<core::oklab_t> gen(42u, 0.f);
  EXPECT_GT(gen.min_distance(), 0.f);
  gen.set_min_distance(-1.f);
  EXPECT_GT(gen.min_distance(), 0.f);
}

TEST(PoissonDisk, DefaultGeneratesRequestedCount) {
  poisson_disk_generator<core::oklab_t> gen(42u);
  auto colors = gen.generate(8);
  ASSERT_EQ(colors.size(), 8u);
  for (std::size_t i = 0; i < colors.size(); ++i) {
    for (std::size_t j = i + 1; j < colors.size(); ++j) {
      float d = algorithms::delta_e::delta_e_ok(colors[i], colors[j]);
      EXPECT_GE(d, 0.1f - 1e-4f) << "Pair (" << i << ", " << j << ") closer than min_distance";
    }
  }
}

// ============================================================
// Distance guarantee in the received color (Issue #69)
//
// Candidates were distance-checked as raw OkLab and only converted to the
// output type at the very end: for rgb8 the cast clamps out-of-sRGB
// candidates and quantizes, so raw candidates that were far apart collapsed
// onto identical received values (exact duplicates, pairs closer than
// min_distance). The guarantee must hold in the color the caller receives.
// ============================================================

TEST(PoissonDisk, Rgb8PaletteKeepsMinDistanceAfterQuantization) {
  poisson_disk_generator<core::rgb8_t> gen(42u, 0.1f);
  auto palette = gen.generate(30);
  // A usable palette must still come back; the pairwise loop below is
  // meaningless if almost everything was rejected.
  ASSERT_GE(palette.size(), 20u);
  for (std::size_t i = 0; i < palette.size(); ++i) {
    for (std::size_t j = i + 1; j < palette.size(); ++j) {
      // Measure in the color the caller actually receives (the rgb8 value
      // mapped to OkLab), not the raw candidate that was drawn.
      const auto a = conversion::color_cast<core::oklab_t>(palette[i]);
      const auto b = conversion::color_cast<core::oklab_t>(palette[j]);
      const float d = algorithms::delta_e::delta_e_ok(a, b);
      EXPECT_GE(d, 0.1f - 1e-4f) << "Received pair (" << i << ", " << j << ") closer than min_distance"
                                 << " (pre-fix: raw candidates were checked, then clamped to rgb8)";
    }
  }
}

TEST(PoissonDisk, Rgb8GenerateMaxKeepsMinDistanceAndNoDuplicates) {
  poisson_disk_generator<core::rgb8_t> gen(42u, 0.1f);
  auto colors = gen.generate_max();
  ASSERT_GE(colors.size(), 20u);
  for (std::size_t i = 0; i < colors.size(); ++i) {
    for (std::size_t j = i + 1; j < colors.size(); ++j) {
      const auto a = conversion::color_cast<core::oklab_t>(colors[i]);
      const auto b = conversion::color_cast<core::oklab_t>(colors[j]);
      const float d = algorithms::delta_e::delta_e_ok(a, b);
      // Also subsumes exact duplicates (ΔE = 0 after rgb8 clamping).
      EXPECT_GE(d, 0.1f - 1e-4f) << "Received pair (" << i << ", " << j << ") closer than min_distance"
                                 << " (pre-fix: generate_max reported thousands of 'distinct' colors that"
                                 << " collapsed after rgb8 quantization)";
    }
  }
}

// ============================================================
// sRGB-reachable candidates (Issue #94)
//
// Candidates were drawn from the whole OkLab working box (L in [0, 1],
// a/b in ±0.5) and accepted on OkLab distance only: 29/30 of them fall
// outside the sRGB gamut, so the moment the palette is converted to any RGB
// target the clipped colors collapse together. Every color the generator
// returns must be sRGB-reachable, checked with the unclamped gamut test
// (oklab_to_linrgb + linrgb_in_gamut — a color_cast would clamp and report
// everything in-gamut).
// ============================================================

namespace {

bool srgb_reachable(const core::oklab_t& c) {
  float r, g, b;
  algorithms::gamut::details::oklab_to_linrgb(c.l(), c.a(), c.b(), r, g, b);
  return algorithms::gamut::details::linrgb_in_gamut(r, g, b);
}

}  // namespace

TEST(PoissonDisk, GeneratedColorsAreSrgbReachable) {
  poisson_disk_generator<core::oklab_t> gen(42u, 0.1f);
  auto colors = gen.generate(30);
  ASSERT_EQ(colors.size(), 30u);
  for (std::size_t i = 0; i < colors.size(); ++i) {
    EXPECT_TRUE(srgb_reachable(colors[i])) << "color " << i << " outside sRGB gamut";
  }
}

TEST(PoissonDisk, GenerateMaxColorsAreSrgbReachable) {
  poisson_disk_generator<core::oklab_t> gen(42u, 0.1f);
  auto colors = gen.generate_max();
  ASSERT_GE(colors.size(), 20u);
  for (std::size_t i = 0; i < colors.size(); ++i) {
    EXPECT_TRUE(srgb_reachable(colors[i])) << "color " << i << " outside sRGB gamut";
  }
}

// ============================================================
// Sampling domains (Issue #67)
//
// Channel ranges are *nominal* — deliberately wide so conversions never
// clip — and must not double as random-sampling boxes: after the Lab/Luv/XYZ
// ranges widened to ±400/[-1,2], most of that box fell outside every display
// gamut. Generators sample the perceptual working range instead.
// ============================================================

TEST(RandomSamplingDomain, OklabStaysInPerceptualBox) {
  // Nominal a/b are widening to ±2; sampling stays at the CSS ±0.5 box so
  // random OkLab colors remain plausibly displayable.
  oklab_generator g(42u);
  for (int i = 0; i < 200; ++i) {
    const auto c = g.next();
    EXPECT_GE(c.l(), 0.0f);
    EXPECT_LE(c.l(), 1.0f);
    EXPECT_GE(c.a(), -0.5f) << "sample " << i;
    EXPECT_LE(c.a(), 0.5f) << "sample " << i;
    EXPECT_GE(c.b(), -0.5f) << "sample " << i;
    EXPECT_LE(c.b(), 0.5f) << "sample " << i;
  }
}

TEST(RandomSamplingDomain, OklchStaysInPerceptualBox) {
  oklch_generator g(42u);
  for (int i = 0; i < 200; ++i) {
    const auto c = g.next();
    EXPECT_GE(c.l(), 0.0f);
    EXPECT_LE(c.l(), 1.0f);
    EXPECT_GE(c.c(), 0.0f);
    EXPECT_LE(c.c(), 0.4f) << "sample " << i;
    EXPECT_GE(c.h(), 0.0f);
    EXPECT_LE(c.h(), 360.0f);
  }
}

TEST(RandomSamplingDomain, CielabStaysInPerceptualBox) {
  // Nominal a*/b* are ±400 since #56; sampling restores the ±128 box so
  // random Lab colors are not mostly outside sRGB.
  cielab_generator g(42u);
  for (int i = 0; i < 200; ++i) {
    const auto c = g.next();
    EXPECT_GE(c.l(), 0.0f);
    EXPECT_LE(c.l(), 100.0f);
    EXPECT_GE(c.a(), -128.0f) << "sample " << i;
    EXPECT_LE(c.a(), 128.0f) << "sample " << i;
    EXPECT_GE(c.b(), -128.0f) << "sample " << i;
    EXPECT_LE(c.b(), 128.0f) << "sample " << i;
  }
}

TEST(RandomSamplingDomain, CielchStaysInPerceptualBox) {
  cielch_generator g(42u);
  for (int i = 0; i < 200; ++i) {
    const auto c = g.next();
    EXPECT_GE(c.l(), 0.0f);
    EXPECT_LE(c.l(), 100.0f);
    EXPECT_GE(c.c(), 0.0f);
    EXPECT_LE(c.c(), 200.0f) << "sample " << i;
    EXPECT_GE(c.h(), 0.0f);
    EXPECT_LE(c.h(), 360.0f);
  }
}

TEST(RandomSamplingDomain, LuvStaysInPerceptualBox) {
  basic_random_generator<core::luv_t> g(42u);
  for (int i = 0; i < 200; ++i) {
    const auto c = g.next();
    EXPECT_GE(c.l(), 0.0f);
    EXPECT_LE(c.l(), 100.0f);
    EXPECT_GE(c.u(), -128.0f) << "sample " << i;
    EXPECT_LE(c.u(), 128.0f) << "sample " << i;
    EXPECT_GE(c.v(), -128.0f) << "sample " << i;
    EXPECT_LE(c.v(), 128.0f) << "sample " << i;
  }
}

TEST(RandomSamplingDomain, XyzStaysNonNegative) {
  // Nominal XYZ is [-1, 2] (#56) to preserve chromatic-adaptation
  // intermediates; sampling stays in the physical [0, 1] box.
  basic_random_generator<core::xyz_t> g(42u);
  for (int i = 0; i < 200; ++i) {
    const auto c = g.next();
    EXPECT_GE(c.x(), 0.0f) << "sample " << i;
    EXPECT_LE(c.x(), 1.0f) << "sample " << i;
    EXPECT_GE(c.y(), 0.0f) << "sample " << i;
    EXPECT_LE(c.y(), 1.0f) << "sample " << i;
    EXPECT_GE(c.z(), 0.0f) << "sample " << i;
    EXPECT_LE(c.z(), 1.0f) << "sample " << i;
  }
}

// ============================================================
// Portable engine→range mapping (Issue #96)
//
// std::mt19937's raw sequence is specified by the standard, but
// std::uniform_*_distribution leaves the engine→value mapping
// implementation-defined: libstdc++, libc++ and MSVC disagree, so
// "same seed → same palette" only held within one standard-library build.
// The mapping is now part of the module's spec — integers: u % span over
// the engine range with the biasing tail rejected; reals: u / range as a
// double in [0, 1), then min + (max - min) * x in double — and is pinned
// here against the standard's raw mt19937 draws for seed 42:
//   u1 = 1608637542, u2 = 3421126067, u3 = 4083286876
// ============================================================

TEST(RandomPortability, UnitMappingIsExactFixedPointScaling) {
  // Double-precision pin of the spec: x = (u - min) / range exactly. The
  // float-level pins cannot see a wrong denominator (u/2^32 vs u/(2^32 - 1)
  // differ at the 33rd bit), so the unit mapping itself is pinned here.
  std::mt19937 rng(42u);
  EXPECT_EQ(details::portable_unit(rng), 1608637542.0 / 4294967296.0);
}

TEST(RandomPortability, IntDrawsFollowSpecifiedMapping) {
  rgb8_generator g(42u);
  auto c = g.next();
  // span 256 divides the 2^32 engine range exactly, so no rejection:
  // channel = u % 256.
  EXPECT_EQ(c.r(), static_cast<uint8_t>(1608637542u % 256u));
  EXPECT_EQ(c.g(), static_cast<uint8_t>(3421126067u % 256u));
  EXPECT_EQ(c.b(), static_cast<uint8_t>(4083286876u % 256u));
}

TEST(RandomPortability, RealDrawsFollowSpecifiedMapping) {
  luminance_generator<core::oklab_t>::options o;
  o.l_min = 0.4f;
  o.l_max = 0.6f;
  o.chroma_max = 0.0f;
  o.gamut_map = false;
  luminance_generator<core::oklab_t> gen(42u, o);
  auto c = gen.next();
  // L is the first draw, spec-mandated bit pattern: min + (max - min) * x
  // with x = u1 / 2^32, the whole expression in double, converted to float
  // once (std distributions compute in the result type and disagree with
  // this across standard libraries — issue #96).
  const float expected =
      static_cast<float>(static_cast<double>(0.4f) +
                         (static_cast<double>(0.6f) - static_cast<double>(0.4f)) * (1608637542.0 / 4294967296.0));
  EXPECT_EQ(c.l(), expected);
}

TEST(RandomPortability, GoldenAngleHueFollowsSpecifiedMapping) {
  golden_gen g(42u);
  auto c = g.next();
  // Initial hue is the first draw: min + (max - min) * (u1 / 2^32) in
  // double, converted to float once (spec bit pattern, issue #96).
  const float expected =
      static_cast<float>(static_cast<double>(0.0f) +
                         (static_cast<double>(360.0f) - static_cast<double>(0.0f)) * (1608637542.0 / 4294967296.0));
  EXPECT_EQ(c.h(), expected);
}
