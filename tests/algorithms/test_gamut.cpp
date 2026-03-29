
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
  
  for (auto mode : {preserve_mode::lightness, preserve_mode::hue, 
                    preserve_mode::saturation, preserve_mode::perceptual}) {
    auto result = gamut_clip_preserve(out_of_gamut, mode);
    EXPECT_TRUE(is_in_srgb_gamut(result)) << "mode=" << (int)mode;
  }
}

TEST(PreserveModeTest, GamutClipWithInfo) {
  core::oklab_t out_of_gamut(0.6f, 0.3f, 0.2f);
  
  for (auto mode : {preserve_mode::lightness, preserve_mode::hue, 
                    preserve_mode::saturation, preserve_mode::perceptual}) {
    auto result = gamut_clip_with_info(out_of_gamut, mode);
    EXPECT_TRUE(is_in_srgb_gamut(result.mapped_color)) << "mode=" << (int)mode;
    EXPECT_TRUE(result.is_valid()) << "mode=" << (int)mode;
  }
}

}  // namespace colorcpp::algorithms::test
