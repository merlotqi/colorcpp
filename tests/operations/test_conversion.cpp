
#include <gtest/gtest.h>

#include <colorcpp/operations/conversion.hpp>
#include <type_traits>

namespace colorcpp::operations::test {

using namespace core;
using namespace conversion;

// Identity cast

TEST(ConversionTest, IdentityCast) {
  rgbf_t c(0.4f, 0.6f, 0.8f);
  auto copy = color_cast<rgbf_t>(c);
  EXPECT_FLOAT_EQ(copy.r(), c.r());
  EXPECT_FLOAT_EQ(copy.g(), c.g());
  EXPECT_FLOAT_EQ(copy.b(), c.b());
}

// sRGB ↔ Linear sRGB

TEST(ConversionTest, LinearizeBlackAndWhite) {
  auto lin_black = color_cast<linear_rgbf_t>(rgbf_t{0.0f, 0.0f, 0.0f});
  EXPECT_NEAR(lin_black.r(), 0.0f, 1e-5f);
  EXPECT_NEAR(lin_black.g(), 0.0f, 1e-5f);
  EXPECT_NEAR(lin_black.b(), 0.0f, 1e-5f);

  auto lin_white = color_cast<linear_rgbf_t>(rgbf_t{1.0f, 1.0f, 1.0f});
  EXPECT_NEAR(lin_white.r(), 1.0f, 1e-4f);
  EXPECT_NEAR(lin_white.g(), 1.0f, 1e-4f);
  EXPECT_NEAR(lin_white.b(), 1.0f, 1e-4f);
}

TEST(ConversionTest, LinearizeMidGrayIsNonLinear) {
  // sRGB 0.5 linearizes to ≈0.214, not 0.5
  auto lin = color_cast<linear_rgbf_t>(rgbf_t{0.5f, 0.5f, 0.5f});
  EXPECT_NEAR(lin.r(), 0.2140f, 1e-3f);
}

TEST(ConversionTest, SrgbLinearRoundTrip) {
  rgbf_t orig(0.3f, 0.6f, 0.9f);
  auto back = color_cast<rgbf_t>(color_cast<linear_rgbf_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

// sRGB ↔ HSL

TEST(ConversionTest, SrgbHslRoundTrip) {
  rgbf_t orig(0.7f, 0.3f, 0.5f);
  auto back = color_cast<rgbf_t>(color_cast<hsl_float_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

TEST(ConversionTest, SrgbHsvRoundTrip) {
  rgbf_t orig(0.2f, 0.8f, 0.4f);
  auto back = color_cast<rgbf_t>(color_cast<hsv_float_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

// sRGB ↔ OKLab

TEST(ConversionTest, BlackToOklab) {
  auto lab = color_cast<oklab_t>(rgbf_t{0.0f, 0.0f, 0.0f});
  EXPECT_NEAR(lab.l(), 0.0f, 1e-4f);
  EXPECT_NEAR(lab.a(), 0.0f, 1e-4f);
  EXPECT_NEAR(lab.b(), 0.0f, 1e-4f);
}

TEST(ConversionTest, WhiteToOklab) {
  auto lab = color_cast<oklab_t>(rgbf_t{1.0f, 1.0f, 1.0f});
  EXPECT_NEAR(lab.l(), 1.0f, 1e-4f);
  EXPECT_NEAR(lab.a(), 0.0f, 1e-4f);
  EXPECT_NEAR(lab.b(), 0.0f, 1e-4f);
}

TEST(ConversionTest, SrgbOklabRoundTrip) {
  rgbf_t orig(0.5f, 0.2f, 0.8f);
  auto back = color_cast<rgbf_t>(color_cast<oklab_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-3f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-3f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-3f);
}

// sRGB ↔ CIELAB

TEST(ConversionTest, BlackToCielab) {
  auto lab = color_cast<cielab_t>(rgbf_t{0.0f, 0.0f, 0.0f});
  EXPECT_NEAR(lab.l(), 0.0f, 1e-3f);
}

TEST(ConversionTest, WhiteToCielab) {
  auto lab = color_cast<cielab_t>(rgbf_t{1.0f, 1.0f, 1.0f});
  EXPECT_NEAR(lab.l(), 100.0f, 1e-2f);
  EXPECT_NEAR(lab.a(), 0.0f, 1e-2f);
  EXPECT_NEAR(lab.b(), 0.0f, 1e-2f);
}

TEST(ConversionTest, SrgbCielabRoundTrip) {
  rgbf_t orig(0.6f, 0.4f, 0.2f);
  auto back = color_cast<rgbf_t>(color_cast<cielab_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-3f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-3f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-3f);
}

// OKLab ↔ OKLCh

TEST(ConversionTest, OklabOklchRoundTrip) {
  rgbf_t orig(0.8f, 0.3f, 0.1f);
  auto oklch = color_cast<oklch_t>(orig);
  auto back = color_cast<rgbf_t>(oklch);
  EXPECT_NEAR(back.r(), orig.r(), 1e-3f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-3f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-3f);
}

TEST(ConversionTest, RouteSelectionPrefersShorterPathToOklch) {
  auto direct = details::oklab_to_oklch<oklch_t>(details::rgbf_to_oklab_reg(rgbf_t{0.25f, 0.5f, 0.75f}));
  auto casted = color_cast<oklch_t>(rgbf_t{0.25f, 0.5f, 0.75f});
  EXPECT_NEAR(casted.l(), direct.l(), 1e-6f);
  EXPECT_NEAR(casted.c(), direct.c(), 1e-6f);
  EXPECT_NEAR(casted.h(), direct.h(), 1e-4f);
}

TEST(ConversionGraphTest, GraphRoutingUsesCanonicalChainToOklab) {
  static_assert(graph::minimal_conversion_cost<display_p3f_t, oklab_t>() == 3);
  static_assert(std::is_same_v<graph::next_hop_t<display_p3f_t, oklab_t>, linear_display_p3f_t>);
  static_assert(graph::minimal_conversion_cost<rgbf_t, cielab_t>() == 2);
  static_assert(std::is_same_v<graph::next_hop_t<rgbf_t, cielab_t>, linear_rgbf_t>);

  const display_p3f_t sample{0.90f, 0.35f, 0.15f};
  const auto expected = details::xyz_to_oklab<oklab_t>(
      details::linear_display_p3_to_xyz<xyz_t>(details::display_p3_to_linear_display_p3<linear_display_p3f_t>(sample)));
  const auto casted = color_cast<oklab_t>(sample);

  EXPECT_NEAR(casted.l(), expected.l(), 1e-6f);
  EXPECT_NEAR(casted.a(), expected.a(), 1e-6f);
  EXPECT_NEAR(casted.b(), expected.b(), 1e-6f);
}

TEST(ConversionGraphTest, WideGamutPairsRouteViaCanonicalChain) {
  // Shortcut-edge registrations are gone: every wide-gamut pair routes through
  // the canonical chain (Linear -> XYZ -> Linear), the single numeric source of
  // truth. The old display_p3af <-> rgbaf cost-3 shortcut was inconsistent with
  // its non-alpha twin (cost 5) and was the only shortcut edge ever picked.
  static_assert(std::is_same_v<graph::next_hop_t<rgbaf_t, display_p3af_t>, linear_rgbaf_t>);
  static_assert(graph::minimal_conversion_cost<rgbaf_t, display_p3af_t>() == 4);
  static_assert(std::is_same_v<graph::next_hop_t<rgbf_t, display_p3f_t>, linear_rgbf_t>);

  // The cast result matches the canonical chain bit-for-bit (the chain's own
  // helpers skip alpha, so alpha is compared against the source instead).
  const rgbaf_t sample{0.90f, 0.35f, 0.15f, 0.5f};
  const auto expected =
      details::linear_display_p3_to_display_p3<display_p3af_t>(details::xyz_to_linear_display_p3<linear_display_p3af_t>(
          details::linear_rgb_to_xyz<xyz_t>(details::srgb_to_linear_rgb<linear_rgbaf_t>(sample))));
  const auto casted = color_cast<display_p3af_t>(sample);
  EXPECT_NEAR(casted.r(), expected.r(), 1e-6f);
  EXPECT_NEAR(casted.g(), expected.g(), 1e-6f);
  EXPECT_NEAR(casted.b(), expected.b(), 1e-6f);
  EXPECT_NEAR(casted.a(), sample.a(), 1e-6f);
}

TEST(ConversionDebugContractTest, PublicDebugHelpersReflectGraphRouting) {
  static_assert(can_convert<hsl_float_t, oklab_t>());
  static_assert(verify_path<hsl_float_t, oklab_t>());

  using info = conversion_path_info<hsl_float_t, oklab_t>;
  static_assert(info::is_identity == false);
  static_assert(info::has_direct_conversion == false);
  static_assert(info::has_graph_path);
  static_assert(info::minimal_graph_cost == 2);
  static_assert(info::is_possible);

  const hsl_float_t sample{180.0f, 0.5f, 0.5f};
  const auto expected = color_cast<oklab_t>(color_cast<rgbf_t>(sample));
  const auto casted = color_cast<oklab_t>(sample);
  EXPECT_NEAR(casted.l(), expected.l(), 1e-6f);
  EXPECT_NEAR(casted.a(), expected.a(), 1e-6f);
  EXPECT_NEAR(casted.b(), expected.b(), 1e-6f);
}

// Alpha handling

TEST(ConversionTest, AlphaPreservedThroughHslRoundTrip) {
  rgbaf_t orig(0.5f, 0.25f, 0.75f, 0.5f);
  auto back = color_cast<rgbaf_t>(color_cast<hsla_float_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.a(), orig.a(), 1e-4f);
}

// Alpha must ride outside the route: minimal-cost paths cross 3-channel
// transit nodes (XYZ / Oklab / CIELAB) that cannot store alpha. The source
// alpha has to be restored on the result regardless of the chosen path.

TEST(ConversionAlphaTest, PreservedThroughXyzHubRoute) {
  const rgbaf_t src{0.2f, 0.4f, 0.6f, 0.37f};
  const auto dst = color_cast<rec2020_rgbaf_t>(src);
  EXPECT_NEAR(dst.a(), 0.37f, 1e-6f);
}

TEST(ConversionAlphaTest, PreservedThroughXyzHubRouteReverse) {
  const rec2020_rgbaf_t src{0.2f, 0.4f, 0.6f, 0.42f};
  const auto dst = color_cast<rgbaf_t>(src);
  EXPECT_NEAR(dst.a(), 0.42f, 1e-6f);
}

TEST(ConversionAlphaTest, PreservedThroughXyzHubToAdobeAndProphoto) {
  const rgbaf_t src{0.2f, 0.4f, 0.6f, 0.63f};
  EXPECT_NEAR(color_cast<adobe_rgbaf_t>(src).a(), 0.63f, 1e-6f);
  EXPECT_NEAR(color_cast<prophoto_rgbaf_t>(src).a(), 0.63f, 1e-6f);
}

TEST(ConversionAlphaTest, PreservedBetweenLinearWideGamutVariants) {
  const linear_rgbaf_t src{0.2f, 0.4f, 0.6f, 0.29f};
  const auto dst = color_cast<linear_rec2020_rgbaf_t>(src);
  EXPECT_NEAR(dst.a(), 0.29f, 1e-6f);
}

// Regression guards: alpha correctness must not depend on which concrete
// route the graph picks (direct edge or multi-hop canonical chain).

TEST(ConversionAlphaTest, PreservedToDisplayP3) {
  const rgbaf_t src{0.2f, 0.4f, 0.6f, 0.31f};
  EXPECT_NEAR(color_cast<display_p3af_t>(src).a(), 0.31f, 1e-6f);
  EXPECT_NEAR(color_cast<rgbaf_t>(display_p3af_t{0.2f, 0.4f, 0.6f, 0.32f}).a(), 0.32f, 1e-6f);
}

TEST(ConversionAlphaTest, PreservedWithQuantizedSource) {
  const rgba8_t src{51, 51, 51, 128};
  EXPECT_NEAR(color_cast<rec2020_rgbaf_t>(src).a(), 128.0f / 255.0f, 1e-4f);
}

TEST(ConversionAlphaTest, ExpandedFillsOpaqueAlpha) {
  EXPECT_NEAR(color_cast<rec2020_rgbaf_t>(rgbf_t{0.2f, 0.4f, 0.6f}).a(), 1.0f, 1e-6f);
}

TEST(ConversionAlphaTest, CmykIsNotAnAlphaModel) {
  // K is not alpha: it must not leak into alpha, and alpha must not leak into K.
  const auto from_cmyk = color_cast<rgbaf_t>(cmyk_float_t{0.1f, 0.2f, 0.3f, 0.4f});
  EXPECT_NEAR(from_cmyk.a(), 1.0f, 1e-6f);

  const auto to_cmyk = color_cast<cmyk_float_t>(rgbaf_t{1.0f, 0.0f, 0.0f, 0.5f});
  EXPECT_NEAR(to_cmyk.k(), 0.0f, 1e-4f);
}

// uint8 ↔ float precision

TEST(ConversionTest, Rgb8ToRgbfKnownValues) {
  auto f = color_cast<rgbf_t>(rgb8_t{0, 128, 255});
  EXPECT_NEAR(f.r(), 0.0f, 1e-4f);
  EXPECT_NEAR(f.g(), 128.0f / 255.0f, 1e-4f);
  EXPECT_NEAR(f.b(), 1.0f, 1e-4f);
}

TEST(ConversionTest, Rgb8RoundTrip) {
  rgb8_t orig(100, 150, 200);
  auto back = color_cast<rgb8_t>(color_cast<rgbf_t>(orig));
  EXPECT_EQ(back.r(), orig.r());
  EXPECT_EQ(back.g(), orig.g());
  EXPECT_EQ(back.b(), orig.b());
}

// Hub + registered edges (8-bit sRGB hub is float sRGB)

TEST(ConversionTest, Rgb8ToLinearRgbfViaRgbfHub) {
  rgb8_t c(255, 128, 0);
  auto lin = color_cast<linear_rgbf_t>(c);
  EXPECT_GT(lin.r(), 0.5f);
  EXPECT_GT(lin.g(), 0.0f);
  EXPECT_FLOAT_EQ(lin.b(), 0.0f);
}

TEST(ConversionTest, Rgb8ToHslViaRegisteredRgbf) {
  rgb8_t red(255, 0, 0);
  auto hsl = color_cast<hsl_float_t>(red);
  EXPECT_NEAR(hsl.h(), 0.0f, 1e-3f);
  EXPECT_GT(hsl.s(), 0.9f);
}

// Multi-hop conversions

TEST(ConversionTest, HslToOklabMultiHop) {
  hsl_float_t hsl(180.0f, 0.5f, 0.5f);
  auto lab = color_cast<oklab_t>(hsl);
  EXPECT_GE(lab.l(), 0.0f);
  EXPECT_LE(lab.l(), 1.0f);
}

TEST(ConversionTest, CielabToOklabViaXyz) {
  cielab_t lab(50.0f, 20.0f, -30.0f);
  auto oklab = color_cast<oklab_t>(lab);
  auto back = color_cast<cielab_t>(oklab);
  EXPECT_NEAR(back.l(), lab.l(), 0.5f);
}

}  // namespace colorcpp::operations::test
