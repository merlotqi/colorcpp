#include <gtest/gtest.h>

#include <array>
#include <colorcpp/colorcpp.hpp>
#include <limits>
#include <string>

namespace colorcpp::operations::test {

using namespace core;
using namespace blend;

namespace {

float apply_scalar_mode(float dst, float src, blend_mode mode) {
  using algorithms = blend::details::algorithms;

  switch (mode) {
    case blend_mode::normal:
      return algorithms::normal(dst, src);
    case blend_mode::multiply:
      return algorithms::multiply(dst, src);
    case blend_mode::screen:
      return algorithms::screen(dst, src);
    case blend_mode::overlay:
      return algorithms::overlay(dst, src);
    case blend_mode::darken:
      return algorithms::darken(dst, src);
    case blend_mode::lighten:
      return algorithms::lighten(dst, src);
    case blend_mode::addition:
      return algorithms::addition(dst, src);
    case blend_mode::subtraction:
      return algorithms::subtraction(dst, src);
    case blend_mode::difference:
      return algorithms::difference(dst, src);
    case blend_mode::exclusion:
      return algorithms::exclusion(dst, src);
    case blend_mode::hard_light:
      return algorithms::hard_light(dst, src);
    default:
      return algorithms::normal(dst, src);
  }
}

rgbaf_t blend_scalar_reference(const rgbaf_t& dst, const rgbaf_t& src, blend_mode mode, float opacity) {
  const float da = dst.a();
  const float sa = src.a() * std::clamp(opacity, 0.0f, 1.0f);
  const float out_a = sa + da * (1.0f - sa);

  if (out_a <= 0.0f) return rgbaf_t{0.0f, 0.0f, 0.0f, 0.0f};

  auto channel = [&](float d, float s) {
    const float mixed = apply_scalar_mode(d, s, mode);
    const float composed = s * sa * (1.0f - da) + d * da * (1.0f - sa) + da * sa * mixed;
    return std::clamp(composed / out_a, 0.0f, 1.0f);
  };

  return rgbaf_t{channel(dst.r(), src.r()), channel(dst.g(), src.g()), channel(dst.b(), src.b()), out_a};
}

constexpr std::array<blend_mode, 11> kSimdModes = {
    blend_mode::normal,     blend_mode::multiply,  blend_mode::screen,     blend_mode::overlay,
    blend_mode::darken,     blend_mode::lighten,   blend_mode::addition,   blend_mode::subtraction,
    blend_mode::difference, blend_mode::exclusion, blend_mode::hard_light,
};

}  // namespace

TEST(BlendSimdTest, FastPathPolicyMatchesCurrentScope) {
  for (blend_mode mode : kSimdModes) {
    EXPECT_TRUE(blend::details::has_simd_fast_path(mode)) << static_cast<int>(mode);
  }

  EXPECT_FALSE(blend::details::has_simd_fast_path(blend_mode::soft_light));
  EXPECT_FALSE(blend::details::has_simd_fast_path(blend_mode::color_dodge));
  EXPECT_FALSE(blend::details::has_simd_fast_path(blend_mode::color_burn));
  EXPECT_FALSE(blend::details::has_simd_fast_path(blend_mode::divide));
  EXPECT_FALSE(blend::details::has_simd_fast_path(blend_mode::hue));
}

TEST(BlendSimdTest, CoveredModesMatchScalarReference) {
  const rgbaf_t dst{0.23f, 0.71f, 0.44f, 0.85f};
  const rgbaf_t src{0.82f, 0.19f, 0.63f, 0.57f};
  constexpr float opacity = 0.61f;

  for (blend_mode mode : kSimdModes) {
    SCOPED_TRACE(std::to_string(static_cast<int>(mode)));

    const auto actual = blend::blend(dst, src, mode, opacity);
    const auto expected = blend_scalar_reference(dst, src, mode, opacity);

    EXPECT_NEAR(actual.r(), expected.r(), 1e-5f);
    EXPECT_NEAR(actual.g(), expected.g(), 1e-5f);
    EXPECT_NEAR(actual.b(), expected.b(), 1e-5f);
    EXPECT_NEAR(actual.a(), expected.a(), 1e-6f);
  }
}

#if defined(COLORCPP_ENABLE_SIMD) && (defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2))

// NaN opacity (issue #91): the SSE2 path must agree with the scalar path -
// previously clamp01_sse2's maxps folded NaN channels to 0 while scalar kept
// NaN, so the same call gave different colors depending on dispatch.

TEST(BlendSimdTest, NanOpacityMatchesZeroOpacityOnSse2Path) {
  const float nan = std::numeric_limits<float>::quiet_NaN();
  const rgbaf_t dst{0.23f, 0.71f, 0.44f, 0.85f};
  const rgbaf_t src{0.82f, 0.19f, 0.63f, 0.57f};

  for (blend_mode mode : kSimdModes) {
    SCOPED_TRACE(std::to_string(static_cast<int>(mode)));
    const auto expected = blend::details::blend_rgbaf(dst, src, mode, 0.0f);
    const auto actual = blend::details::blend_rgbaf_sse2(dst, src, mode, nan);
    EXPECT_FALSE(std::isnan(actual.r()));
    EXPECT_FALSE(std::isnan(actual.g()));
    EXPECT_FALSE(std::isnan(actual.b()));
    EXPECT_FALSE(std::isnan(actual.a()));
    EXPECT_NEAR(actual.r(), expected.r(), 1e-6f);
    EXPECT_NEAR(actual.g(), expected.g(), 1e-6f);
    EXPECT_NEAR(actual.b(), expected.b(), 1e-6f);
    EXPECT_NEAR(actual.a(), expected.a(), 1e-6f);
  }
}

#endif

}  // namespace colorcpp::operations::test
