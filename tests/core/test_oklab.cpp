
#include <gtest/gtest.h>

#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/prophoto_rgb.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::core::test {

using namespace colorcpp::operations::conversion;

// OKLab Construction

TEST(OKLabTest, DefaultConstruction) {
  oklab_t lab;
  EXPECT_FLOAT_EQ(lab.l(), 0.0f);
  EXPECT_FLOAT_EQ(lab.a(), 0.0f);
  EXPECT_FLOAT_EQ(lab.b(), 0.0f);
}

TEST(OKLabTest, ParameterizedConstruction) {
  oklab_t lab(0.5f, 0.1f, -0.2f);
  EXPECT_FLOAT_EQ(lab.l(), 0.5f);
  EXPECT_FLOAT_EQ(lab.a(), 0.1f);
  EXPECT_FLOAT_EQ(lab.b(), -0.2f);
}

TEST(OKLabTest, BoundaryValues) {
  oklab_t black(0.0f, 0.0f, 0.0f);
  EXPECT_FLOAT_EQ(black.l(), 0.0f);

  oklab_t white(1.0f, 0.0f, 0.0f);
  EXPECT_FLOAT_EQ(white.l(), 1.0f);

  // a and b range: [-0.5, 0.5]
  oklab_t extremes(0.5f, -0.5f, 0.5f);
  EXPECT_FLOAT_EQ(extremes.a(), -0.5f);
  EXPECT_FLOAT_EQ(extremes.b(), 0.5f);
}

TEST(OKLabTest, NominalChannelRanges) {
  using namespace oklab::channel;
  static_assert(okl::min == 0.0f && okl::max == 2.0f);
  static_assert(oka::min == -2.0f && oka::max == 2.0f);
  static_assert(okb::min == -2.0f && okb::max == 2.0f);
  SUCCEED();
}

// OKLCh Construction

TEST(OKLChTest, DefaultConstruction) {
  oklch_t lch;
  EXPECT_FLOAT_EQ(lch.l(), 0.0f);
  EXPECT_FLOAT_EQ(lch.c(), 0.0f);
  EXPECT_FLOAT_EQ(lch.h(), 0.0f);
}

TEST(OKLChTest, ParameterizedConstruction) {
  oklch_t lch(0.7f, 0.15f, 200.0f);
  EXPECT_FLOAT_EQ(lch.l(), 0.7f);
  EXPECT_FLOAT_EQ(lch.c(), 0.15f);
  EXPECT_FLOAT_EQ(lch.h(), 200.0f);
}

TEST(OKLChTest, NominalChannelRanges) {
  using namespace oklab::channel;
  static_assert(okl::min == 0.0f && okl::max == 2.0f);
  static_assert(okc::min == 0.0f && okc::max == 2.0f);
  static_assert(okh::min == 0.0f && okh::max == 360.0f);
  SUCCEED();
}

// Range relaxation: the nominal range no longer clamps natural-unit conversion math.

TEST(OKLabRangeRelaxTest, WideChromaSurvivesOklabToOklch) {
  const oklab_t lab{0.7f, 0.45f, 0.0f};  // a beyond the old +-0.5 nominal
  const oklch_t lch = color_cast<oklch_t>(lab);
  EXPECT_FLOAT_EQ(lch.l(), 0.7f);
  EXPECT_FLOAT_EQ(lch.c(), 0.45f);  // was clamped to 0.4
  EXPECT_NEAR(std::abs(lch.h()), 0.0f, 1e-3f);
}

TEST(OKLabRangeRelaxTest, WideChromaSurvivesOklchToOklab) {
  const oklch_t lch{0.7f, 0.6f, 10.0f};  // C beyond the old 0.4 nominal
  const oklab_t lab = color_cast<oklab_t>(lch);
  EXPECT_FLOAT_EQ(lab.l(), 0.7f);
  EXPECT_NEAR(lab.a(), 0.6f * std::cos(10.0f * 3.14159265f / 180.0f), 1e-5f);
}

// --- Issue #67: OkLab storage ranges clipped wide-gamut colors. ProPhoto
//     blue has raw a ≈ -1.346 (beyond the old ±1 bound), and above-white
//     values exist at all (ProPhoto white L ≈ 1.000003, XYZ (1,1,1)
//     L ≈ 1.0032), so ProPhoto → OkLab → ProPhoto corrupted them. The
//     nominal ranges must cover every supported gamut. ---

TEST(OklabWideGamutTest, ProPhotoBlueOklabValuesUnclipped) {
  const auto lab = color_cast<oklab_t>(prophoto_rgbf_t(0.0f, 0.0f, 1.0f));
  EXPECT_NEAR(lab.l(), 0.2167f, 5e-3f);
  EXPECT_NEAR(lab.a(), -1.3457f, 5e-3f) << "old a/b bound ±1 clipped ProPhoto blue";
  EXPECT_NEAR(lab.b(), -0.4273f, 5e-3f);
  const auto lch = color_cast<oklch_t>(lab);
  EXPECT_NEAR(lch.c(), 1.4119f, 5e-3f) << "old C bound 1 clipped ProPhoto blue";
}

TEST(OklabWideGamutTest, ProPhotoBlueSurvivesOklabRoundTrip) {
  const prophoto_rgbf_t blue(0.0f, 0.0f, 1.0f);
  const auto back = color_cast<prophoto_rgbf_t>(color_cast<oklab_t>(blue));
  EXPECT_NEAR(back.r(), 0.0f, 1e-3f);
  EXPECT_NEAR(back.g(), 0.0f, 1e-3f);
  EXPECT_NEAR(back.b(), 1.0f, 1e-3f) << "pre-fix: (0.247, 0, 0.942)";
}

TEST(OklabWideGamutTest, ProPhotoBlueSurvivesOklchRoundTrip) {
  const prophoto_rgbf_t blue(0.0f, 0.0f, 1.0f);
  const auto back = color_cast<prophoto_rgbf_t>(color_cast<oklch_t>(blue));
  EXPECT_NEAR(back.r(), 0.0f, 1e-3f);
  EXPECT_NEAR(back.g(), 0.0f, 1e-3f);
  EXPECT_NEAR(back.b(), 1.0f, 1e-3f);
}

TEST(OklabWideGamutTest, ProPhotoWhiteLightnessAboveOne) {
  // ProPhoto white adapts to ≈ D65 white: raw OkLab L = 1.0000031. The old
  // L ≤ 1 bound stored exactly 1.0 and lost the above-one value.
  const auto lab = color_cast<oklab_t>(prophoto_rgbf_t(1.0f, 1.0f, 1.0f));
  EXPECT_GT(lab.l(), 1.0f);
  EXPECT_NEAR(lab.l(), 1.000003f, 1e-5f);
}

TEST(OklabWideGamutTest, SuperWhiteXyzLightnessAboveOne) {
  // XYZ (1, 1, 1) sits above the D65 white point on every axis → L ≈ 1.0032.
  const auto lab = color_cast<oklab_t>(xyz_t(1.0f, 1.0f, 1.0f));
  EXPECT_GT(lab.l(), 1.0f);
  EXPECT_NEAR(lab.l(), 1.0032f, 5e-4f);
}

// Member and indexed access

TEST(OKLabTest, MutableMemberAccess) {
  oklab_t lab(0.5f, 0.1f, 0.2f);
  lab.l() = 0.8f;
  lab.a() = -0.1f;
  lab.b() = 0.3f;
  EXPECT_FLOAT_EQ(lab.l(), 0.8f);
  EXPECT_FLOAT_EQ(lab.a(), -0.1f);
  EXPECT_FLOAT_EQ(lab.b(), 0.3f);
}

TEST(OKLabTest, IndexAccess) {
  oklab_t lab(0.6f, 0.1f, -0.2f);
  EXPECT_FLOAT_EQ(lab.get_index<0>(), 0.6f);
  EXPECT_FLOAT_EQ(lab.get_index<1>(), 0.1f);
  EXPECT_FLOAT_EQ(lab.get_index<2>(), -0.2f);
}

// Known color conversions: sRGB → OKLab

TEST(OKLabConversionTest, BlackIsOrigin) {
  auto lab = color_cast<oklab_t>(rgbf_t{0.0f, 0.0f, 0.0f});
  EXPECT_NEAR(lab.l(), 0.0f, 1e-4f);
  EXPECT_NEAR(lab.a(), 0.0f, 1e-4f);
  EXPECT_NEAR(lab.b(), 0.0f, 1e-4f);
}

TEST(OKLabConversionTest, WhiteIsLOne) {
  auto lab = color_cast<oklab_t>(rgbf_t{1.0f, 1.0f, 1.0f});
  EXPECT_NEAR(lab.l(), 1.0f, 1e-4f);
  EXPECT_NEAR(lab.a(), 0.0f, 1e-4f);
  EXPECT_NEAR(lab.b(), 0.0f, 1e-4f);
}

TEST(OKLabConversionTest, AchromaticHasZeroChroma) {
  // All grays should have a≈0, b≈0
  for (const int v : {64, 128, 192}) {
    rgb8_t gray(static_cast<uint8_t>(v), static_cast<uint8_t>(v), static_cast<uint8_t>(v));
    auto lab = color_cast<oklab_t>(gray);
    EXPECT_NEAR(lab.a(), 0.0f, 1e-3f) << "gray=" << (int)v;
    EXPECT_NEAR(lab.b(), 0.0f, 1e-3f) << "gray=" << (int)v;
  }
}

TEST(OKLabConversionTest, LighterColorHasHigherL) {
  auto dark = color_cast<oklab_t>(rgbf_t{0.2f, 0.2f, 0.2f});
  auto light = color_cast<oklab_t>(rgbf_t{0.8f, 0.8f, 0.8f});
  EXPECT_LT(dark.l(), light.l());
}

// Round-trip

TEST(OKLabConversionTest, FloatRoundTrip) {
  rgbf_t orig(0.5f, 0.2f, 0.8f);
  auto back = color_cast<rgbf_t>(color_cast<oklab_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

TEST(OKLabConversionTest, PrimaryColorsRoundTrip) {
  for (auto orig : {rgbf_t{1.0f, 0.0f, 0.0f}, rgbf_t{0.0f, 1.0f, 0.0f}, rgbf_t{0.0f, 0.0f, 1.0f}}) {
    auto back = color_cast<rgbf_t>(color_cast<oklab_t>(orig));
    EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
    EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
    EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
  }
}

// OKLab ↔ OKLCh

TEST(OKLChConversionTest, AchromaticHasZeroChroma) {
  auto lch = color_cast<oklch_t>(rgbf_t{0.5f, 0.5f, 0.5f});
  EXPECT_NEAR(lch.c(), 0.0f, 1e-3f);
}

TEST(OKLChConversionTest, LightnessMatchesOklab) {
  rgbf_t color(0.4f, 0.6f, 0.2f);
  auto lab = color_cast<oklab_t>(color);
  auto lch = color_cast<oklch_t>(color);
  EXPECT_NEAR(lch.l(), lab.l(), 1e-4f);
}

TEST(OKLChConversionTest, ChromaFromAB) {
  // For oklab (L, a, b), chroma C = sqrt(a²+b²)
  oklab_t lab(0.5f, 0.1f, 0.0f);
  auto lch = color_cast<oklch_t>(lab);
  EXPECT_NEAR(lch.c(), 0.1f, 1e-4f);
  EXPECT_NEAR(lch.h(), 0.0f, 1e-3f);  // a>0, b=0 → hue=0°
}

TEST(OKLChConversionTest, RoundTripOklabToLch) {
  rgbf_t orig(0.6f, 0.3f, 0.9f);
  auto back = color_cast<rgbf_t>(color_cast<oklch_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-3f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-3f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-3f);
}

// Type traits

TEST(OKLabTest, TypeTraits) {
  using oklab_channels = typename traits::model_traits<oklab::model::oklab>::channels_type;
  static_assert(std::tuple_size_v<oklab_channels> == 3);

  using l_ch = std::tuple_element_t<0, oklab_channels>;
  static_assert(std::is_same_v<l_ch::value_type, float>);
  static_assert(l_ch::min == 0.0f);
  static_assert(l_ch::max == 2.0f);

  using a_ch = std::tuple_element_t<1, oklab_channels>;
  static_assert(a_ch::min == -2.0f);
  static_assert(a_ch::max == 2.0f);

  using oklch_channels = typename traits::model_traits<oklab::model::oklch>::channels_type;
  static_assert(std::tuple_size_v<oklch_channels> == 3);
}

TEST(OKLabTest, CopyConstructors) {
  oklab_t original(0.5f, 0.1f, -0.2f);
  oklab_t copy(original);

  EXPECT_FLOAT_EQ(copy.l(), 0.5f);
  EXPECT_FLOAT_EQ(copy.a(), 0.1f);
  EXPECT_FLOAT_EQ(copy.b(), -0.2f);
}

TEST(OKLabTest, CopyAssignment) {
  oklab_t original(0.5f, 0.1f, -0.2f);
  oklab_t copy(0.0f, 0.0f, 0.0f);
  copy = original;

  EXPECT_FLOAT_EQ(copy.l(), 0.5f);
  EXPECT_FLOAT_EQ(copy.a(), 0.1f);
  EXPECT_FLOAT_EQ(copy.b(), -0.2f);
}

TEST(OKLabTest, MoveSemantics) {
  oklab_t original(0.5f, 0.1f, -0.2f);
  oklab_t moved(std::move(original));

  EXPECT_FLOAT_EQ(moved.l(), 0.5f);
  EXPECT_FLOAT_EQ(moved.a(), 0.1f);
  EXPECT_FLOAT_EQ(moved.b(), -0.2f);
}

TEST(OKLabTest, MoveAssignment) {
  oklab_t original(0.5f, 0.1f, -0.2f);
  oklab_t moved(0.0f, 0.0f, 0.0f);
  moved = std::move(original);

  EXPECT_FLOAT_EQ(moved.l(), 0.5f);
  EXPECT_FLOAT_EQ(moved.a(), 0.1f);
  EXPECT_FLOAT_EQ(moved.b(), -0.2f);
}

TEST(OKLabTest, EqualityComparison) {
  oklab_t color1(0.5f, 0.1f, -0.2f);
  oklab_t color2(0.5f, 0.1f, -0.2f);
  oklab_t color3(0.6f, 0.1f, -0.2f);

  EXPECT_TRUE(color1 == color2);
  EXPECT_FALSE(color1 == color3);
  EXPECT_TRUE(color1 != color3);
}

TEST(OKLabTest, ConstexprBoundaryValues) {
  constexpr oklab_t min_color(0.0f, -0.5f, -0.5f);
  static_assert(min_color.l() == 0.0f);
  static_assert(min_color.a() == -0.5f);
  static_assert(min_color.b() == -0.5f);

  constexpr oklab_t max_color(1.0f, 0.5f, 0.5f);
  static_assert(max_color.l() == 1.0f);
  static_assert(max_color.a() == 0.5f);
  static_assert(max_color.b() == 0.5f);
}

TEST(OKLChTest, CopyConstructors) {
  oklch_t original(0.7f, 0.15f, 200.0f);
  oklch_t copy(original);

  EXPECT_FLOAT_EQ(copy.l(), 0.7f);
  EXPECT_FLOAT_EQ(copy.c(), 0.15f);
  EXPECT_FLOAT_EQ(copy.h(), 200.0f);
}

TEST(OKLChTest, CopyAssignment) {
  oklch_t original(0.7f, 0.15f, 200.0f);
  oklch_t copy(0.0f, 0.0f, 0.0f);
  copy = original;

  EXPECT_FLOAT_EQ(copy.l(), 0.7f);
  EXPECT_FLOAT_EQ(copy.c(), 0.15f);
  EXPECT_FLOAT_EQ(copy.h(), 200.0f);
}

TEST(OKLChTest, MoveSemantics) {
  oklch_t original(0.7f, 0.15f, 200.0f);
  oklch_t moved(std::move(original));

  EXPECT_FLOAT_EQ(moved.l(), 0.7f);
  EXPECT_FLOAT_EQ(moved.c(), 0.15f);
  EXPECT_FLOAT_EQ(moved.h(), 200.0f);
}

TEST(OKLChTest, MoveAssignment) {
  oklch_t original(0.7f, 0.15f, 200.0f);
  oklch_t moved(0.0f, 0.0f, 0.0f);
  moved = std::move(original);

  EXPECT_FLOAT_EQ(moved.l(), 0.7f);
  EXPECT_FLOAT_EQ(moved.c(), 0.15f);
  EXPECT_FLOAT_EQ(moved.h(), 200.0f);
}

TEST(OKLChTest, EqualityComparison) {
  oklch_t color1(0.7f, 0.15f, 200.0f);
  oklch_t color2(0.7f, 0.15f, 200.0f);
  oklch_t color3(0.8f, 0.15f, 200.0f);

  EXPECT_TRUE(color1 == color2);
  EXPECT_FALSE(color1 == color3);
  EXPECT_TRUE(color1 != color3);
}

TEST(OKLChTest, ConstexprBoundaryValues) {
  constexpr oklch_t min_color(0.0f, 0.0f, 0.0f);
  static_assert(min_color.l() == 0.0f);
  static_assert(min_color.c() == 0.0f);
  static_assert(min_color.h() == 0.0f);

  constexpr oklch_t max_color(1.0f, 0.4f, 360.0f);
  static_assert(max_color.l() == 1.0f);
  static_assert(max_color.c() == 0.4f);
  static_assert(max_color.h() == 360.0f);
}

TEST(OKLabTest, HueAngleWrapping) {
  // Test hue angle normalization for 0° and 360° equivalence
  oklch_t color1(0.5f, 0.1f, 0.0f);
  oklch_t color2(0.5f, 0.1f, 360.0f);

  EXPECT_NEAR(color1.h(), 0.0f, 1e-6f);
  EXPECT_NEAR(color2.h(), 360.0f, 1e-6f);
}

}  // namespace colorcpp::core::test
