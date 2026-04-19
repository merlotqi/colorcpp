#include <gtest/gtest.h>

#include <cmath>
#include <colorcpp/colorcpp.hpp>
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
