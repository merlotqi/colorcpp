#include <gtest/gtest.h>

#include <algorithm>
#include <colorcpp/colorcpp.hpp>
#include <set>

using namespace colorcpp;
using namespace colorcpp::operations;
using namespace colorcpp::operations::random;

// ============================================================
// Luminance Generator Tests
// ============================================================

TEST(LuminanceGenerator, Reproducible) {
  luminance_rgbf_generator g1(12345u);
  luminance_rgbf_generator g2(12345u);
  auto a = g1.next();
  auto b = g2.next();
  EXPECT_FLOAT_EQ(a.r(), b.r());
  EXPECT_FLOAT_EQ(a.g(), b.g());
  EXPECT_FLOAT_EQ(a.b(), b.b());
}

TEST(LuminanceGenerator, LightnessInRange) {
  luminance_generator<core::rgbf_t>::options o;
  o.l_min = 0.3f;
  o.l_max = 0.7f;
  o.gamut_map = true;
  luminance_generator<core::rgbf_t> gen(42u, o);

  for (int i = 0; i < 50; ++i) {
    auto c = gen.next();
    auto oklab = conversion::color_cast<core::oklab_t>(c);
    float L = oklab.l();
    // After gamut clipping, lightness may shift slightly, allow small tolerance
    EXPECT_GE(L, 0.28f) << "L too low at iteration " << i;
    EXPECT_LE(L, 0.72f) << "L too high at iteration " << i;
  }
}

TEST(LuminanceGenerator, FullRangeProducesDiverse) {
  luminance_rgbf_generator g(99u);
  std::set<int> buckets;
  for (int i = 0; i < 50; ++i) {
    auto c = g.next();
    auto oklab = conversion::color_cast<core::oklab_t>(c);
    buckets.insert(static_cast<int>(oklab.l() * 10));
  }
  // Should hit multiple lightness buckets
  EXPECT_GT(buckets.size(), 3u);
}

TEST(LuminanceGenerator, NarrowRange) {
  luminance_generator<core::rgbf_t>::options o;
  o.l_min = 0.5f;
  o.l_max = 0.5f;
  o.gamut_map = true;
  luminance_generator<core::rgbf_t> gen(42u, o);

  for (int i = 0; i < 20; ++i) {
    auto c = gen.next();
    auto oklab = conversion::color_cast<core::oklab_t>(c);
    // Gamut clipping can shift L slightly due to sRGB round-trip
    EXPECT_NEAR(oklab.l(), 0.5f, 0.15f) << "L deviated too much at iteration " << i;
  }
}

TEST(LuminanceGenerator, GenerateN) {
  luminance_rgbf_generator g(42u);
  auto colors = g.generate_n(10);
  EXPECT_EQ(colors.size(), 10u);
}

TEST(LuminanceGenerator, InvalidOptionsThrow) {
  luminance_generator<core::rgbf_t>::options bad;
  bad.l_min = 0.8f;
  bad.l_max = 0.2f;
  EXPECT_THROW(luminance_generator<core::rgbf_t>(0u, bad), std::invalid_argument);
}

TEST(LuminanceGenerator, NegativeChromaThrows) {
  luminance_generator<core::rgbf_t>::options bad;
  bad.chroma_max = -0.1f;
  EXPECT_THROW(luminance_generator<core::rgbf_t>(0u, bad), std::invalid_argument);
}

TEST(LuminanceGenerator, ColorDiversity) {
  luminance_rgbf_generator g(42u);
  auto a = g.next();
  auto b = g.next();
  // Very unlikely to be identical
  bool differ = (a.r() != b.r()) || (a.g() != b.g()) || (a.b() != b.b());
  EXPECT_TRUE(differ);
}

TEST(LuminanceGenerator, U8LightnessIsFloatingPoint) {
  luminance_generator<core::rgb8_t>::options o;
  o.l_min = 0.4f;
  o.l_max = 0.6f;
  o.gamut_map = true;
  luminance_generator<core::rgb8_t> gen(42u, o);

  // Before the fix, T=uint8_t truncated the float bounds to integers,
  // so sampled L collapsed to {0, 1} (black or white only).
  // Tolerance ±0.1 covers gamut-map + 8-bit round-trip shift (see NarrowRange).
  for (int i = 0; i < 30; ++i) {
    auto c = gen.next();
    auto oklab = conversion::color_cast<core::oklab_t>(c);
    EXPECT_GE(oklab.l(), 0.3f) << "L too low at iteration " << i;
    EXPECT_LE(oklab.l(), 0.7f) << "L too high at iteration " << i;
  }
}

TEST(LuminanceGenerator, ConvenienceFunction) {
  auto c = random_luminance_color<core::rgbf_t>(0.4f, 0.6f, 42u);
  auto oklab = conversion::color_cast<core::oklab_t>(c);
  EXPECT_GE(oklab.l(), 0.35f);
  EXPECT_LE(oklab.l(), 0.65f);
}

// ============================================================
// Contrast Generator Tests
// ============================================================

TEST(ContrastGenerator, Reproducible) {
  contrast_rgbf_generator g1(12345u);
  contrast_rgbf_generator g2(12345u);
  core::rgbf_t bg{1.0f, 1.0f, 1.0f};
  auto a = g1.next(bg);
  auto b = g2.next(bg);
  EXPECT_FLOAT_EQ(a.r(), b.r());
  EXPECT_FLOAT_EQ(a.g(), b.g());
  EXPECT_FLOAT_EQ(a.b(), b.b());
}

TEST(ContrastGenerator, DefaultReferenceWhite) {
  contrast_rgbf_generator g(42u);
  auto c = g.next();
  float ratio = algorithms::accessibility::contrast_ratio(c, core::rgbf_t{1.0f, 1.0f, 1.0f});
  EXPECT_GE(ratio, 4.5f - 0.1f);
}

TEST(ContrastGenerator, CustomReference) {
  contrast_rgbf_generator g(42u);
  core::rgbf_t bg{0.2f, 0.2f, 0.2f};
  auto c = g.next(bg);
  float ratio = algorithms::accessibility::contrast_ratio(c, bg);
  EXPECT_GE(ratio, 4.5f - 0.1f);
}

TEST(ContrastGenerator, AAARatio) {
  contrast_generator<core::rgbf_t>::options o;
  o.min_ratio = 7.0f;
  contrast_generator<core::rgbf_t> gen(42u, o);

  // Dark background: L≈0.034, max achievable ratio ≈10.2:1, so 7:1 is feasible
  core::rgbf_t bg{0.2f, 0.2f, 0.2f};
  for (int i = 0; i < 20; ++i) {
    auto c = gen.next(bg);
    float ratio = algorithms::accessibility::contrast_ratio(c, bg);
    EXPECT_GE(ratio, 6.8f) << "Ratio too low at iteration " << i;
  }
}

TEST(ContrastGenerator, GenerateN) {
  contrast_rgbf_generator g(42u);
  auto colors = g.generate_n(10);
  EXPECT_EQ(colors.size(), 10u);
}

TEST(ContrastGenerator, GenerateNWithReference) {
  contrast_rgbf_generator g(42u);
  core::rgbf_t bg{0.8f, 0.8f, 0.8f};
  auto colors = g.generate_n(10, bg);
  EXPECT_EQ(colors.size(), 10u);
  for (const auto& c : colors) {
    float ratio = algorithms::accessibility::contrast_ratio(c, bg);
    EXPECT_GE(ratio, 4.0f) << "Contrast too low";
  }
}

TEST(ContrastGenerator, TightConstraintDoesNotThrow) {
  contrast_generator<core::rgbf_t>::options o;
  o.min_ratio = 20.0f;
  o.max_attempts = 10;
  contrast_generator<core::rgbf_t> gen(42u, o);
  // Should return a color even if constraint not fully met
  auto c = gen.next();
  (void)c;
}

TEST(ContrastGenerator, TryNextReturnsValueWhenFeasible) {
  contrast_rgbf_generator g(42u);
  auto result = g.try_next(core::rgbf_t{1.0f, 1.0f, 1.0f});
  ASSERT_TRUE(result.has_value());
  float ratio = algorithms::accessibility::contrast_ratio(*result, core::rgbf_t{1.0f, 1.0f, 1.0f});
  EXPECT_GE(ratio, 4.4f);
}

TEST(ContrastGenerator, TryNextReturnsNulloptOnExhaustion) {
  // L pinned to 0.5: max achievable contrast vs white is ~4.5:1, so 10:1 is unreachable.
  contrast_generator<core::rgbf_t>::options o;
  o.l_min = 0.5f;
  o.l_max = 0.5f;
  o.min_ratio = 10.0f;
  o.max_attempts = 50;
  contrast_generator<core::rgbf_t> gen(42u, o);
  auto result = gen.try_next(core::rgbf_t{1.0f, 1.0f, 1.0f});
  EXPECT_FALSE(result.has_value());
}

TEST(ContrastGenerator, InvalidOptionsThrow) {
  contrast_generator<core::rgbf_t>::options bad;
  bad.min_ratio = 0.5f;
  EXPECT_THROW(contrast_generator<core::rgbf_t>(0u, bad), std::invalid_argument);
}

TEST(ContrastGenerator, OptionsRoundtrip) {
  contrast_generator<core::rgbf_t> g(42u);
  contrast_generator<core::rgbf_t>::options o;
  o.min_ratio = 7.0f;
  o.l_min = 0.1f;
  o.l_max = 0.9f;
  o.chroma_max = 0.3f;
  g.set_options(o);
  auto got = g.get_options();
  EXPECT_FLOAT_EQ(got.min_ratio, 7.0f);
  EXPECT_FLOAT_EQ(got.l_min, 0.1f);
  EXPECT_FLOAT_EQ(got.l_max, 0.9f);
  EXPECT_FLOAT_EQ(got.chroma_max, 0.3f);
}

TEST(ContrastGenerator, U8MeetsContrastAndHasChroma) {
  contrast_rgb8_generator g(42u);
  const core::rgbf_t white{1.0f, 1.0f, 1.0f};

  // Before the fix, uint8_t bounds collapsed candidates to black/white only:
  // contrast passed (black vs white = 21:1) but chroma was always 0.
  float max_chroma = 0.0f;
  for (int i = 0; i < 30; ++i) {
    auto c = g.next(white);
    float ratio = algorithms::accessibility::contrast_ratio(c, white);
    EXPECT_GE(ratio, 4.4f) << "Ratio too low at iteration " << i;
    auto oklch = conversion::color_cast<core::oklch_t>(c);
    max_chroma = std::max(max_chroma, oklch.c());
  }
  EXPECT_GT(max_chroma, 0.05f) << "All samples were achromatic (degenerate black/white path)";
}

TEST(ContrastGenerator, ConvenienceFunction) {
  core::rgbf_t bg{1.0f, 1.0f, 1.0f};
  auto c = random_contrast_color<core::rgbf_t>(bg, 4.5f, 42u);
  float ratio = algorithms::accessibility::contrast_ratio(c, bg);
  EXPECT_GE(ratio, 4.0f);
}

// --- Issue #61: color_cast<Color>(oklab) already clamps to the target gamut,
//     so gamut_clip_preserve_lightness received an in-gamut color and returned
//     it unchanged — the sampled OkLab lightness was silently lost (up to 0.103
//     off with l_min = l_max = 0.7), and contrast_generator's gamut_map option
//     changed ~nothing. Gamut mapping must happen in OkLab before the cast. ---

TEST(LuminanceGenerator, PinnedLightnessSurvivesGamutMappingRgbf) {
  luminance_generator<core::rgbf_t>::options o;
  o.l_min = 0.7f;
  o.l_max = 0.7f;
  o.chroma_max = 0.4f;
  o.gamut_map = true;
  luminance_generator<core::rgbf_t> gen(42u, o);

  for (int i = 0; i < 100; ++i) {
    auto c = gen.next();
    auto oklab = conversion::color_cast<core::oklab_t>(c);
    EXPECT_NEAR(oklab.l(), 0.7f, 0.01f) << "L deviated at iteration " << i;
  }
}

TEST(LuminanceGenerator, PinnedLightnessSurvivesGamutMappingRgb8) {
  luminance_generator<core::rgb8_t>::options o;
  o.l_min = 0.7f;
  o.l_max = 0.7f;
  o.chroma_max = 0.4f;
  o.gamut_map = true;
  luminance_generator<core::rgb8_t> gen(42u, o);

  for (int i = 0; i < 100; ++i) {
    auto c = gen.next();
    auto oklab = conversion::color_cast<core::oklab_t>(c);
    // 8-bit quantization on top of the map: a little more slack than rgbf.
    EXPECT_NEAR(oklab.l(), 0.7f, 0.02f) << "L deviated at iteration " << i;
  }
}

TEST(ContrastGenerator, GamutMapKeepsLightnessAndContrastRgb8) {
  contrast_generator<core::rgb8_t>::options o;
  o.min_ratio = 2.0f;
  o.l_min = 0.7f;
  o.l_max = 0.7f;
  o.gamut_map = true;
  contrast_generator<core::rgb8_t> gen(42u, o);
  const core::rgbf_t white{1.0f, 1.0f, 1.0f};

  for (int i = 0; i < 50; ++i) {
    auto c = gen.next(white);
    auto oklab = conversion::color_cast<core::oklab_t>(c);
    EXPECT_NEAR(oklab.l(), 0.7f, 0.02f) << "L deviated at iteration " << i;
    float ratio = algorithms::accessibility::contrast_ratio(c, white);
    EXPECT_GE(ratio, 2.0f) << "Contrast too low at iteration " << i;
  }
}

TEST(ContrastGenerator, TryGenerateNSucceedsWhenFeasible) {
  contrast_rgbf_generator g(42u);
  const core::rgbf_t white{1.0f, 1.0f, 1.0f};
  auto result = g.try_generate_n(10, white);
  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->size(), 10u);
  for (const auto& c : *result) {
    float ratio = algorithms::accessibility::contrast_ratio(c, white);
    EXPECT_GE(ratio, 4.4f);
  }
}

TEST(ContrastGenerator, TryGenerateNReportsImpossibleConstraint) {
  // L pinned at 0.5: contrast 10:1 vs white is unreachable, so the batch
  // must report failure instead of silently returning failing colors.
  contrast_generator<core::rgbf_t>::options o;
  o.l_min = 0.5f;
  o.l_max = 0.5f;
  o.min_ratio = 10.0f;
  o.max_attempts = 50;
  contrast_generator<core::rgbf_t> gen(42u, o);
  auto result = gen.try_generate_n(5, core::rgbf_t{1.0f, 1.0f, 1.0f});
  EXPECT_FALSE(result.has_value());
}
