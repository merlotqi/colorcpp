#include <gtest/gtest.h>

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

TEST(ContrastGenerator, ConvenienceFunction) {
  core::rgbf_t bg{1.0f, 1.0f, 1.0f};
  auto c = random_contrast_color<core::rgbf_t>(bg, 4.5f, 42u);
  float ratio = algorithms::accessibility::contrast_ratio(c, bg);
  EXPECT_GE(ratio, 4.0f);
}
