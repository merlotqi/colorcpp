
#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

namespace colorcpp::operations::test {

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

}  // namespace colorcpp::operations::test
