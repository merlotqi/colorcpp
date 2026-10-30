
#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

namespace colorcpp::algorithms::test {

using namespace core;
using namespace gamut;
using namespace conversion;

// is_in_srgb_gamut

TEST(GamutTest, BlackIsInGamut) { EXPECT_TRUE(is_in_srgb_gamut(core::rgbf_t{0.0f, 0.0f, 0.0f})); }

TEST(GamutTest, WhiteIsInGamut) { EXPECT_TRUE(is_in_srgb_gamut(core::rgbf_t{1.0f, 1.0f, 1.0f})); }

TEST(GamutTest, MidGrayIsInGamut) { EXPECT_TRUE(is_in_srgb_gamut(core::rgbf_t{0.5f, 0.5f, 0.5f})); }

TEST(GamutTest, PrimaryColorsAreInGamut) {
  EXPECT_TRUE(is_in_srgb_gamut(core::rgbf_t{1.0f, 0.0f, 0.0f}));  // red
  EXPECT_TRUE(is_in_srgb_gamut(core::rgbf_t{0.0f, 1.0f, 0.0f}));  // green
  EXPECT_TRUE(is_in_srgb_gamut(core::rgbf_t{0.0f, 0.0f, 1.0f}));  // blue
}

TEST(GamutTest, Rgb8ColorsAreInGamut) {
  // All rgb8_t values are by definition in sRGB gamut
  EXPECT_TRUE(is_in_srgb_gamut(core::rgb8_t{255, 128, 0}));
  EXPECT_TRUE(is_in_srgb_gamut(core::rgb8_t{0, 0, 0}));
}

TEST(GamutTest, HighChromaOklabIsOutOfGamut) {
  // OKLab (0.5, 0.4, 0) has very high chroma → out of sRGB gamut
  // (linear R would be negative when converted back)
  core::oklab_t out_of_gamut(0.5f, 0.4f, 0.0f);
  EXPECT_FALSE(is_in_srgb_gamut(out_of_gamut));
}

TEST(GamutTest, AchromaticOklabIsInGamut) {
  // Pure achromatic OKLab: a=0, b=0, any valid L in [0,1]
  EXPECT_TRUE(is_in_srgb_gamut(core::oklab_t{0.5f, 0.0f, 0.0f}));
  EXPECT_TRUE(is_in_srgb_gamut(core::oklab_t{0.0f, 0.0f, 0.0f}));
  EXPECT_TRUE(is_in_srgb_gamut(core::oklab_t{1.0f, 0.0f, 0.0f}));
}

// gamut_clip: clip method

TEST(GamutClipTest, InGamutColorUnchangedByClip) {
  rgbaf_t in_gamut(0.5f, 0.3f, 0.7f, 1.0f);
  auto clipped = gamut_clip(in_gamut, clip_method::clip);
  EXPECT_NEAR(clipped.r(), in_gamut.r(), 1e-4f);
  EXPECT_NEAR(clipped.g(), in_gamut.g(), 1e-4f);
  EXPECT_NEAR(clipped.b(), in_gamut.b(), 1e-4f);
}

TEST(GamutClipTest, ClipMethodProducesInGamutResult) {
  core::oklab_t out_of_gamut(0.5f, 0.4f, 0.0f);
  auto result = gamut_clip(out_of_gamut, clip_method::clip);
  EXPECT_TRUE(is_in_srgb_gamut(result));
}

TEST(GamutClipTest, ClipMethodPreservesBlack) {
  auto result = gamut_clip(rgbaf_t{0.0f, 0.0f, 0.0f, 1.0f}, clip_method::clip);
  EXPECT_NEAR(result.r(), 0.0f, 1e-4f);
  EXPECT_NEAR(result.g(), 0.0f, 1e-4f);
  EXPECT_NEAR(result.b(), 0.0f, 1e-4f);
}

TEST(GamutClipTest, ClipMethodPreservesWhite) {
  auto result = gamut_clip(rgbaf_t{1.0f, 1.0f, 1.0f, 1.0f}, clip_method::clip);
  EXPECT_NEAR(result.r(), 1.0f, 1e-4f);
  EXPECT_NEAR(result.g(), 1.0f, 1e-4f);
  EXPECT_NEAR(result.b(), 1.0f, 1e-4f);
}

// gamut_clip: chroma_reduce method

TEST(GamutClipTest, ChromaReduceProducesInGamutResult) {
  core::oklab_t out_of_gamut(0.5f, 0.4f, 0.0f);
  auto result = gamut_clip(out_of_gamut, clip_method::chroma_reduce);
  EXPECT_TRUE(is_in_srgb_gamut(result));
}

TEST(GamutClipTest, ChromaReduceInGamutUnchanged) {
  rgbf_t in_gamut(0.4f, 0.6f, 0.2f);
  auto result = gamut_clip(in_gamut, clip_method::chroma_reduce);
  EXPECT_NEAR(result.r(), in_gamut.r(), 1e-3f);
  EXPECT_NEAR(result.g(), in_gamut.g(), 1e-3f);
  EXPECT_NEAR(result.b(), in_gamut.b(), 1e-3f);
}

TEST(GamutClipTest, ChromaReducePreservesBlack) {
  core::oklab_t black(0.0f, 0.0f, 0.0f);
  auto result = gamut_clip(black, clip_method::chroma_reduce);
  EXPECT_TRUE(is_in_srgb_gamut(result));
}

// gamut_clip: css_color4 method (default)

TEST(GamutClipTest, CssColor4ProducesInGamutResult) {
  core::oklab_t out_of_gamut(0.5f, 0.4f, 0.0f);
  auto result = gamut_clip(out_of_gamut);  // default = css_color4
  EXPECT_TRUE(is_in_srgb_gamut(result));
}

TEST(GamutClipTest, CssColor4InGamutUnchanged) {
  rgbaf_t in_gamut(0.5f, 0.5f, 0.5f, 1.0f);
  auto result = gamut_clip(in_gamut, clip_method::css_color4);
  EXPECT_NEAR(result.r(), 0.5f, 1e-3f);
  EXPECT_NEAR(result.g(), 0.5f, 1e-3f);
  EXPECT_NEAR(result.b(), 0.5f, 1e-3f);
}

TEST(GamutClipTest, CssColor4PreservesLightnessExtreme) {
  // OKLab with L=1 (full white) → white regardless of chroma
  // The algorithm short-circuits: L>=1 → {1,1,1,alpha}
  oklab_t pure_white(1.0f, 0.0f, 0.0f);
  auto result = gamut_clip(pure_white, clip_method::css_color4);
  // Result is oklab_t; cast to RGB to verify white
  auto rgb = conversion::color_cast<rgbf_t>(result);
  EXPECT_NEAR(rgb.r(), 1.0f, 1e-3f);
  EXPECT_NEAR(rgb.g(), 1.0f, 1e-3f);
  EXPECT_NEAR(rgb.b(), 1.0f, 1e-3f);
}

TEST(GamutClipTest, CssColor4PreservesLightnessWithinJnd) {
  // CSS Color 4 §14.2: bisection accepts clipping whose ΔE_OK stays under one
  // JND (0.02), so the mapped result's lightness may shift by at most JND.
  // Chroma reduction itself is intentional and not JND-bounded.
  // Regression: storing linear sRGB into rgbaf_t without encoding shifted L
  // by ~0.12 — far beyond the JND bound.
  core::oklab_t in(0.5f, 0.4f, 0.0f);
  auto result = gamut_clip(in, clip_method::css_color4);
  EXPECT_TRUE(is_in_srgb_gamut(result));
  EXPECT_LE(std::abs(result.get_index<0>() - in.get_index<0>()), 0.02f + 1e-3f);
}

TEST(GamutClipTest, ChromaReducePreservesLightness) {
  // chroma_reduce returns the bisected in-gamut candidate without channel
  // clipping, so lightness must round-trip exactly.
  core::oklab_t out_of_gamut(0.5f, 0.4f, 0.0f);
  auto result = gamut_clip(out_of_gamut, clip_method::chroma_reduce);
  EXPECT_NEAR(result.get_index<0>(), 0.5f, 1e-4f);
}

TEST(GamutClipTest, ChromaReducePreservesAlpha) {
  // Wide-gamut input carrying alpha: chroma reduction must not reset it.
  display_p3af_t out_of_gamut(1.0f, 0.0f, 1.0f, 0.5f);
  ASSERT_FALSE(is_in_srgb_gamut(out_of_gamut));
  auto result = gamut_clip(out_of_gamut, clip_method::chroma_reduce);
  auto rgb = color_cast<rgbaf_t>(result);
  EXPECT_NEAR(rgb.a(), 0.5f, 1e-3f);
}

// All methods preserve alpha

TEST(GamutClipTest, AllMethodsProduceValidRgbOutput) {
  // Verify all three methods don't produce NaN or out-of-range values
  core::oklab_t oog(0.6f, 0.3f, 0.2f);

  for (auto method : {clip_method::clip, clip_method::chroma_reduce, clip_method::css_color4}) {
    auto result = gamut_clip(oog, method);
    EXPECT_TRUE(is_in_srgb_gamut(result)) << "method=" << (int)method;
  }
}

// ============================================================================
// Multi-colorspace gamut tests
// ============================================================================

TEST(MultiColorspaceGamutTest, IsInGamutSrgb) {
  // sRGB colors should be in sRGB gamut
  EXPECT_TRUE(is_in_gamut(core::rgbf_t{1.0f, 0.0f, 0.0f}, gamut_type::srgb));
  EXPECT_TRUE(is_in_gamut(core::rgbf_t{0.0f, 1.0f, 0.0f}, gamut_type::srgb));
  EXPECT_TRUE(is_in_gamut(core::rgbf_t{0.0f, 0.0f, 1.0f}, gamut_type::srgb));
}

TEST(MultiColorspaceGamutTest, IsInGamutDisplayP3) {
  // Display P3 has wider gamut than sRGB
  // sRGB colors should be in Display P3 gamut
  EXPECT_TRUE(is_in_gamut(core::rgbf_t{1.0f, 0.0f, 0.0f}, gamut_type::display_p3));
  EXPECT_TRUE(is_in_gamut(core::rgbf_t{0.0f, 1.0f, 0.0f}, gamut_type::display_p3));
  EXPECT_TRUE(is_in_gamut(core::rgbf_t{0.0f, 0.0f, 1.0f}, gamut_type::display_p3));
}

TEST(MultiColorspaceGamutTest, IsInGamutAdobeRGB) {
  // Adobe RGB has wider gamut than sRGB
  // sRGB colors should be in Adobe RGB gamut
  EXPECT_TRUE(is_in_gamut(core::rgbf_t{1.0f, 0.0f, 0.0f}, gamut_type::adobe_rgb));
  EXPECT_TRUE(is_in_gamut(core::rgbf_t{0.0f, 1.0f, 0.0f}, gamut_type::adobe_rgb));
  EXPECT_TRUE(is_in_gamut(core::rgbf_t{0.0f, 0.0f, 1.0f}, gamut_type::adobe_rgb));
}

TEST(MultiColorspaceGamutTest, IsInDisplayP3Gamut) {
  // Test convenience function
  EXPECT_TRUE(is_in_displayp3_gamut(core::rgbf_t{1.0f, 0.0f, 0.0f}));
  EXPECT_TRUE(is_in_displayp3_gamut(core::rgbf_t{0.0f, 1.0f, 0.0f}));
}

TEST(MultiColorspaceGamutTest, IsInAdobeRGBGamut) {
  // Test convenience function
  EXPECT_TRUE(is_in_adobergb_gamut(core::rgbf_t{1.0f, 0.0f, 0.0f}));
  EXPECT_TRUE(is_in_adobergb_gamut(core::rgbf_t{0.0f, 1.0f, 0.0f}));
}

TEST(MultiColorspaceGamutTest, GamutClipToGamut) {
  // Test clipping to different gamuts
  core::oklab_t out_of_gamut(0.5f, 0.4f, 0.0f);

  auto srgb_result = gamut_clip_to_gamut(out_of_gamut, gamut_type::srgb);
  EXPECT_TRUE(is_in_srgb_gamut(srgb_result));

  auto p3_result = gamut_clip_to_gamut(out_of_gamut, gamut_type::display_p3);
  EXPECT_TRUE(is_in_displayp3_gamut(p3_result));

  auto argb_result = gamut_clip_to_gamut(out_of_gamut, gamut_type::adobe_rgb);
  EXPECT_TRUE(is_in_adobergb_gamut(argb_result));
}

// ============================================================================
// Gamut distance tests
// ============================================================================

TEST(GamutDistanceTest, InGamutColorHasZeroDistance) {
  rgbf_t in_gamut(0.5f, 0.3f, 0.7f);
  float distance = gamut_distance(in_gamut);
  EXPECT_NEAR(distance, 0.0f, 1e-4f);
}

TEST(GamutDistanceTest, OutOfGamutColorHasPositiveDistance) {
  core::oklab_t out_of_gamut(0.5f, 0.4f, 0.0f);
  float distance = gamut_distance(out_of_gamut);
  EXPECT_GT(distance, 0.0f);
}

TEST(GamutDistanceTest, BlackHasZeroDistance) {
  rgbf_t black(0.0f, 0.0f, 0.0f);
  float distance = gamut_distance(black);
  EXPECT_NEAR(distance, 0.0f, 1e-4f);
}

TEST(GamutDistanceTest, WhiteHasZeroDistance) {
  rgbf_t white(1.0f, 1.0f, 1.0f);
  float distance = gamut_distance(white);
  EXPECT_NEAR(distance, 0.0f, 1e-4f);
}

TEST(GamutDistanceTest, IsNearGamutBoundary) {
  // Color very close to gamut boundary
  rgbf_t near_boundary(0.99f, 0.0f, 0.0f);
  bool is_near = is_near_gamut_boundary(near_boundary, 0.1f);
  // Red at 0.99 is not near boundary
  EXPECT_FALSE(is_near);
}

// ============================================================================
// Gamut mapping result tests
// ============================================================================

TEST(GamutMappingResultTest, InGamutColorResult) {
  rgbf_t in_gamut(0.5f, 0.3f, 0.7f);
  auto result = gamut_clip_with_info(in_gamut, preserve_mode::perceptual);

  EXPECT_TRUE(result.was_in_gamut);
  EXPECT_NEAR(result.delta_e, 0.0f, 1e-4f);
  EXPECT_NEAR(result.original_chroma, result.mapped_chroma, 1e-4f);
  EXPECT_TRUE(result.is_valid());
  EXPECT_NEAR(result.chroma_reduction_percent(), 0.0f, 1e-4f);
}

TEST(GamutMappingResultTest, OutOfGamutColorResult) {
  core::oklab_t out_of_gamut(0.5f, 0.4f, 0.0f);
  auto result = gamut_clip_with_info(out_of_gamut, preserve_mode::perceptual);

  EXPECT_FALSE(result.was_in_gamut);
  EXPECT_GT(result.delta_e, 0.0f);
  EXPECT_LT(result.mapped_chroma, result.original_chroma);
  EXPECT_TRUE(result.is_valid());
  EXPECT_GT(result.chroma_reduction_percent(), 0.0f);
}

// ============================================================================
// Preserve mode tests
// ============================================================================

TEST(PreserveModeTest, LightnessPreservation) {
  core::oklab_t out_of_gamut(0.5f, 0.4f, 0.0f);
  auto result = gamut_clip_preserve_lightness(out_of_gamut);
  EXPECT_TRUE(is_in_srgb_gamut(result));

  // Lightness should be preserved (approximately)
  auto lab_result = color_cast<core::oklab_t>(result);
  EXPECT_NEAR(lab_result.l(), 0.5f, 0.01f);
}

TEST(PreserveModeTest, HuePreservation) {
  core::oklab_t out_of_gamut(0.5f, 0.4f, 0.0f);
  auto result = gamut_clip_preserve_hue(out_of_gamut);
  EXPECT_TRUE(is_in_srgb_gamut(result));
}

TEST(PreserveModeTest, SaturationPreservation) {
  core::oklab_t out_of_gamut(0.5f, 0.4f, 0.0f);
  auto result = gamut_clip_preserve_saturation(out_of_gamut);
  EXPECT_TRUE(is_in_srgb_gamut(result));
}

TEST(PreserveModeTest, PerceptualPreservation) {
  core::oklab_t out_of_gamut(0.5f, 0.4f, 0.0f);
  auto result = gamut_clip_perceptual(out_of_gamut);
  EXPECT_TRUE(is_in_srgb_gamut(result));
}

TEST(PreserveModeTest, AllPreserveModesProduceValidResults) {
  core::oklab_t out_of_gamut(0.6f, 0.3f, 0.2f);

  for (auto mode :
       {preserve_mode::lightness, preserve_mode::hue, preserve_mode::saturation, preserve_mode::perceptual}) {
    auto result = gamut_clip_preserve(out_of_gamut, mode);
    EXPECT_TRUE(is_in_srgb_gamut(result)) << "mode=" << (int)mode;
  }
}

TEST(PreserveModeTest, GamutClipWithInfo) {
  core::oklab_t out_of_gamut(0.6f, 0.3f, 0.2f);

  for (auto mode :
       {preserve_mode::lightness, preserve_mode::hue, preserve_mode::saturation, preserve_mode::perceptual}) {
    auto result = gamut_clip_with_info(out_of_gamut, mode);
    EXPECT_TRUE(is_in_srgb_gamut(result.mapped_color)) << "mode=" << (int)mode;
    EXPECT_TRUE(result.is_valid()) << "mode=" << (int)mode;
  }
}

// --- Issue #57: preserve-saturation bisection was a no-op, grays misread
//     OkLab L as sRGB, in-gamut colors were rebuilt, alpha was dropped. ---

TEST(PreserveModeTest, SaturationPreservationKeepsChroma) {
  // C = 0.24 at hue 30° only fits sRGB for L in a narrow band (~0.60–0.65).
  // Preserving chroma means moving L into that band, not channel-clipping.
  const float C = 0.24f;
  constexpr float kRad = 30.0f * 3.14159265f / 180.0f;
  const core::oklab_t input(0.7f, C * std::cos(kRad), C * std::sin(kRad));

  const auto result = gamut_clip_preserve_saturation(input);
  EXPECT_TRUE(is_in_srgb_gamut(result));

  const auto lab = color_cast<core::oklab_t>(result);
  const float chroma = std::sqrt(lab.a() * lab.a() + lab.b() * lab.b());
  EXPECT_NEAR(chroma, C, 1e-3f) << "chroma must survive, only lightness may move";
  EXPECT_NEAR(lab.l(), 0.646f, 0.03f) << "lightness must land on the feasible band edge";
}

TEST(PreserveModeTest, SaturationInGamutReturnedUnchanged) {
  // In-gamut input must come back bit-identical, not rebuilt via OkLab.
  const core::rgbf_t in(0.3f, 0.6f, 0.9f);
  const auto result = gamut_clip_preserve_saturation(in);
  EXPECT_EQ(result.r(), in.r());
  EXPECT_EQ(result.g(), in.g());
  EXPECT_EQ(result.b(), in.b());
}

TEST(PreserveModeTest, SaturationGrayUnchanged) {
  // Gray OkLab L = 0.5 must stay 0.5 (not be reinterpreted as sRGB gray 0.5 → 0.5982).
  const core::oklab_t gray(0.5f, 0.0f, 0.0f);
  const auto result = gamut_clip_preserve_saturation(gray);
  const auto lab = color_cast<core::oklab_t>(result);
  EXPECT_NEAR(lab.l(), 0.5f, 1e-3f);
}

TEST(PreserveModeTest, PreserveAlpha) {
  // Rebuilding through 3-channel OkLab used to reset alpha to 1.0.
  const core::rgbaf_t oog(1.5f, 0.1f, 0.1f, 0.5f);
  EXPECT_FLOAT_EQ(gamut_clip_preserve_lightness(oog).a(), 0.5f);
  EXPECT_FLOAT_EQ(gamut_clip_preserve_hue(oog).a(), 0.5f);
  EXPECT_FLOAT_EQ(gamut_clip_preserve_saturation(oog).a(), 0.5f);
}

// --- Issue #58: perceptual mode stored *linear* sRGB in rgbaf_t (gamma-
//     encoded), so results were far too dark. CSS Color 4 reference vector. ---

TEST(PreserveModeTest, PerceptualMatchesCssReference) {
  // color.js / CSS Color 4 gamut mapping for oklch(0.7 0.3 30):
  // rgb(1, 0.3451, 0.2646). Storing the linear value (0.0976) in rgbaf_t
  // used to read back as encoded 0.0976 instead of 0.3451.
  const float C = 0.3f;
  constexpr float kRad = 30.0f * 3.14159265f / 180.0f;
  const core::oklab_t input(0.7f, C * std::cos(kRad), C * std::sin(kRad));

  const auto result = color_cast<core::rgbaf_t>(gamut_clip_perceptual(input));
  EXPECT_NEAR(result.r(), 1.0f, 2e-3f);
  EXPECT_NEAR(result.g(), 0.3451f, 2e-3f);
  EXPECT_NEAR(result.b(), 0.2646f, 2e-3f);
}

// --- ProPhoto RGB gamut ---

TEST(MultiColorspaceGamutTest, ProPhotoInGamut) {
  // Colors near sRGB primaries should all be within ProPhoto's massive gamut
  EXPECT_TRUE(is_in_gamut(rgbf_t{1.0f, 0.0f, 0.0f}, gamut_type::prophoto_rgb));
  EXPECT_TRUE(is_in_gamut(rgbf_t{0.0f, 1.0f, 0.0f}, gamut_type::prophoto_rgb));
  EXPECT_TRUE(is_in_gamut(rgbf_t{0.0f, 0.0f, 1.0f}, gamut_type::prophoto_rgb));
  EXPECT_TRUE(is_in_gamut(rgbf_t{0.0f, 0.0f, 0.0f}, gamut_type::prophoto_rgb));
  EXPECT_TRUE(is_in_gamut(rgbf_t{1.0f, 1.0f, 1.0f}, gamut_type::prophoto_rgb));
}

TEST(MultiColorspaceGamutTest, ProPhotoConvenienceFunction) {
  EXPECT_TRUE(is_in_prophoto_gamut(rgbf_t{0.5f, 0.5f, 0.5f}));
}

TEST(MultiColorspaceGamutTest, GamutClipToProPhoto) {
  // Clipping a valid sRGB color to ProPhoto should return it (ProPhoto is larger)
  auto result = gamut_clip_to_gamut(rgbf_t{0.3f, 0.6f, 0.9f}, gamut_type::prophoto_rgb);
  EXPECT_NEAR(result.template get_index<0>(), 0.3f, 1e-2f);
  EXPECT_NEAR(result.template get_index<1>(), 0.6f, 1e-2f);
  EXPECT_NEAR(result.template get_index<2>(), 0.9f, 1e-2f);
}

TEST(MultiColorspaceGamutTest, ProPhotoClippedResultIsInGamut) {
  // Clip an out-of-gamut color to ProPhoto, verify in-gamut
  auto clipped = gamut_clip_to_gamut(core::oklab_t{0.5f, 0.4f, 0.0f}, gamut_type::prophoto_rgb);
  EXPECT_TRUE(is_in_gamut(clipped, gamut_type::prophoto_rgb));
}

// --- Issue #48: the P3/Adobe/ProPhoto "converters" applied only transfer
//     curves (and clamped first), so every membership check was true. ---

TEST(MultiColorspaceGamutTest, Rec2020GreenOutsideNarrowerGamuts) {
  // Rec.2020 green (xy 0.170, 0.797) is more saturated than the P3 and
  // Adobe RGB green primaries (linear g ≈ 1.37 / 1.43), but ProPhoto's
  // green primary (0.1596, 0.8404) still contains it (linear g ≈ 0.93).
  const core::rec2020_rgbf_t green(0.0f, 1.0f, 0.0f);
  EXPECT_FALSE(is_in_gamut(green, gamut_type::display_p3));
  EXPECT_FALSE(is_in_gamut(green, gamut_type::adobe_rgb));
  EXPECT_TRUE(is_in_gamut(green, gamut_type::prophoto_rgb));
}

TEST(MultiColorspaceGamutTest, HyperchromaticOutsideAllGamuts) {
  // oklch(0.6 0.9 140) lies outside sRGB, Display P3, Adobe RGB and ProPhoto.
  const float C = 0.9f;
  constexpr float kRad = 140.0f * 3.14159265f / 180.0f;
  const core::oklab_t c(0.6f, C * std::cos(kRad), C * std::sin(kRad));
  EXPECT_FALSE(is_in_gamut(c, gamut_type::srgb));
  EXPECT_FALSE(is_in_gamut(c, gamut_type::display_p3));
  EXPECT_FALSE(is_in_gamut(c, gamut_type::adobe_rgb));
  EXPECT_FALSE(is_in_gamut(c, gamut_type::prophoto_rgb));
}

TEST(MultiColorspaceGamutTest, P3RedOutsideSrgb) {
  // Sanity: P3 red (1,0,0) maps to linear sRGB ≈ (1.22, -0.04, -0.02).
  const core::display_p3f_t red(1.0f, 0.0f, 0.0f);
  EXPECT_TRUE(is_in_gamut(red, gamut_type::display_p3));
  EXPECT_FALSE(is_in_gamut(red, gamut_type::srgb));
}

TEST(MultiColorspaceGamutTest, GamutClipToP3MapsOutOfRangeColor) {
  // gamut_clip_to_gamut(c, display_p3) used to return c unchanged because the
  // membership check always passed; the result must actually be mapped.
  const float C = 0.9f;
  constexpr float kRad = 140.0f * 3.14159265f / 180.0f;
  const core::oklab_t c(0.6f, C * std::cos(kRad), C * std::sin(kRad));
  const auto clipped = gamut_clip_to_gamut(c, gamut_type::display_p3);
  EXPECT_TRUE(is_in_gamut(clipped, gamut_type::display_p3));
  const auto lab = color_cast<core::oklab_t>(clipped);
  EXPECT_NE(lab.a(), c.a()) << "chroma must be reduced, not returned unchanged";
}

// --- Issue #65: the clip result was rebuilt through core::rgbaf_t, whose sRGB
//     clamp silently re-clipped wide-gamut targets. The returned color must
//     sit on the *target* gamut's boundary, never inside sRGB. ---

TEST(MultiColorspaceGamutTest, GamutClipToP3LandsOnP3BoundaryNotSrgb) {
  // oklch(0.6 0.9 140): outside both sRGB and P3. The P3 boundary chroma at
  // this L and hue is ≈ 0.2275; the sRGB boundary is ≈ 0.1933. Pre-fix the
  // rgbaf_t rebuild clamped the search result back to chroma ≈ 0.2052
  // (fully inside sRGB) and drifted L to 0.603.
  constexpr float kRad = 140.0f * 3.14159265f / 180.0f;
  const core::oklab_t c(0.6f, 0.9f * std::cos(kRad), 0.9f * std::sin(kRad));
  const auto clipped = gamut_clip_to_gamut(c, gamut_type::display_p3);

  EXPECT_TRUE(is_in_gamut(clipped, gamut_type::display_p3));
  EXPECT_FALSE(is_in_gamut(clipped, gamut_type::srgb))
      << "clip-to-P3 result must reach beyond sRGB, not be re-clamped into it";

  const auto lab = color_cast<core::oklab_t>(clipped);
  const float L = lab.get_index<0>();
  const float chroma = std::sqrt(lab.get_index<1>() * lab.get_index<1>() + lab.get_index<2>() * lab.get_index<2>());
  EXPECT_NEAR(L, 0.6f, 5e-3f) << "a chroma-only clip must preserve lightness";
  EXPECT_NEAR(chroma, 0.2275f, 3e-3f) << "must land on the P3 boundary, not sRGB's";

  // True boundary check: pushing chroma further at the same hue leaves P3.
  const core::oklab_t beyond(L, (chroma + 0.01f) * std::cos(kRad), (chroma + 0.01f) * std::sin(kRad));
  EXPECT_FALSE(is_in_gamut(beyond, gamut_type::display_p3));
}

TEST(MultiColorspaceGamutTest, GamutClipToP3PreservesAlpha) {
  // Out-of-gamut P3 coordinates with alpha: the clip must keep alpha and
  // return a P3 value (never re-clamped through sRGB).
  const core::display_p3af_t c(1.2f, -0.05f, 0.1f, 0.5f);
  const auto clipped = gamut_clip_to_gamut(c, gamut_type::display_p3);
  EXPECT_TRUE(is_in_gamut(clipped, gamut_type::display_p3));
  EXPECT_NEAR(clipped.a(), 0.5f, 1e-6f);
}

// --- Issue #67: is_in_gamut routed the color through oklab_t, whose storage
//     range clipped wide-gamut values before membership was computed — 100 of
//     ProPhoto's 21³ own-cube grid points (including pure blue) were reported
//     outside their own gamut. Membership must never see a clipped value. ---

TEST(MultiColorspaceGamutTest, ProPhotoBlueIsInsideOwnGamut) {
  EXPECT_TRUE(is_in_gamut(core::prophoto_rgbf_t{0.0f, 0.0f, 1.0f}, gamut_type::prophoto_rgb))
      << "pre-fix: OkLab a was clipped to -1, landing outside ProPhoto";
}

TEST(MultiColorspaceGamutTest, ProPhotoGridFullyInsideOwnGamut) {
  int rejected = 0;
  for (int i = 0; i <= 20; ++i) {
    for (int j = 0; j <= 20; ++j) {
      for (int k = 0; k <= 20; ++k) {
        const core::prophoto_rgbf_t c(static_cast<float>(i) / 20.0f, static_cast<float>(j) / 20.0f,
                                      static_cast<float>(k) / 20.0f);
        if (!is_in_gamut(c, gamut_type::prophoto_rgb)) ++rejected;
      }
    }
  }
  EXPECT_EQ(rejected, 0) << "points on ProPhoto's own [0,1]³ cube must be inside ProPhoto";
}

}  // namespace colorcpp::algorithms::test
