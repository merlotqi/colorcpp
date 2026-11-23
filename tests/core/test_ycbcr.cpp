/**
 * @file test_ycbcr.cpp
 * @brief Reference tests for YCbCr (BT.601 / BT.709 / BT.2020) with
 *        limited/full range and 8/10-bit quantization.
 *
 * Reference anchors come from an independent double-precision evaluation of
 * the ITU-R definitions: luma weights Kr/Kg/Kb (0.299/0.587/0.114,
 * 0.2126/0.7152/0.0722, 0.2627/0.6780/0.0593), normalized chroma
 * Cb = (B - Y) / (2 (1 - Kb)), Cr = (R - Y) / (2 (1 - Kr)), and studio-swing
 * quantization with half-away-from-zero rounding:
 *   full 8    : round(Y * 255), round(C * 255 + 128)
 *   limited 8 : 16 + round(Y * 219), 128 + round(C * 224)
 *   full 10   : round(Y * 1023), round(C * 1023 + 512)
 *   limited 10: 64 + round(Y * 876), 512 + round(C * 896)
 * Colored anchors carry a +/-1 code tolerance for float32 accumulation.
 */

#include <gtest/gtest.h>

#include <cmath>
#include <colorcpp/colorcpp.hpp>

namespace colorcpp::core::test {

using colorcpp::core::rgb8_t;
using colorcpp::core::rgbf_t;
using colorcpp::core::ycbcr2020_t;
using colorcpp::core::ycbcr601_t;
using colorcpp::core::ycbcr709_t;
using colorcpp::core::ycbcr::bit_depth;
using colorcpp::core::ycbcr::codes;
using colorcpp::core::ycbcr::dequantize;
using colorcpp::core::ycbcr::quantize;
using colorcpp::core::ycbcr::range;
using colorcpp::operations::conversion::color_cast;

namespace {

// rgbf_t's constructor is explicit, so aggregate tables go through a helper.
constexpr rgbf_t RGB(float r, float g, float b) { return rgbf_t(r, g, b); }

void expect_codes(const codes& got, int y, int cb, int cr, int tol) {
  EXPECT_LE(std::abs(got.y - y), tol) << "Y got " << got.y << " want " << y;
  EXPECT_LE(std::abs(got.cb - cb), tol) << "Cb got " << got.cb << " want " << cb;
  EXPECT_LE(std::abs(got.cr - cr), tol) << "Cr got " << got.cr << " want " << cr;
}

}  // namespace

// The studio-swing neutral anchors hold for every matrix: white sits at the
// top of the luma swing, black at the bottom, chroma at neutral.
TEST(YCbCr, LimitedRangeNeutralAnchors) {
  expect_codes(quantize(ycbcr601_t{1.0f, 0.0f, 0.0f}, range::limited, bit_depth::depth8), 235, 128, 128, 0);
  expect_codes(quantize(ycbcr709_t{1.0f, 0.0f, 0.0f}, range::limited, bit_depth::depth8), 235, 128, 128, 0);
  expect_codes(quantize(ycbcr2020_t{1.0f, 0.0f, 0.0f}, range::limited, bit_depth::depth8), 235, 128, 128, 0);
  expect_codes(quantize(ycbcr709_t{0.0f, 0.0f, 0.0f}, range::limited, bit_depth::depth8), 16, 128, 128, 0);
  expect_codes(quantize(ycbcr601_t{1.0f, 0.0f, 0.0f}, range::limited, bit_depth::depth10), 940, 512, 512, 0);
  expect_codes(quantize(ycbcr709_t{0.0f, 0.0f, 0.0f}, range::limited, bit_depth::depth10), 64, 512, 512, 0);
}

TEST(YCbCr, FullRangeNeutralAnchors) {
  expect_codes(quantize(ycbcr709_t{1.0f, 0.0f, 0.0f}, range::full, bit_depth::depth8), 255, 128, 128, 0);
  expect_codes(quantize(ycbcr709_t{0.0f, 0.0f, 0.0f}, range::full, bit_depth::depth8), 0, 128, 128, 0);
  expect_codes(quantize(ycbcr709_t{1.0f, 0.0f, 0.0f}, range::full, bit_depth::depth10), 1023, 512, 512, 0);
  // Out-of-range float values clamp into the code space instead of
  // overflowing it.
  expect_codes(quantize(ycbcr709_t{1.5f, 0.0f, 0.0f}, range::full, bit_depth::depth8), 255, 128, 128, 0);
  expect_codes(quantize(ycbcr709_t{-0.2f, 0.0f, 0.0f}, range::limited, bit_depth::depth8), 0, 128, 128,
               0);  // 16 - 44 clamps to 0
}

// The same RGB primary quantizes differently under each matrix — pins the
// per-standard luma weights against a copy-paste of any other standard.
TEST(YCbCr, MatrixAnchorsPerStandard) {
  // limited 8
  expect_codes(quantize(color_cast<ycbcr601_t>(rgbf_t{1, 0, 0}), range::limited, bit_depth::depth8), 81, 90, 240, 0);
  expect_codes(quantize(color_cast<ycbcr709_t>(rgbf_t{1, 0, 0}), range::limited, bit_depth::depth8), 63, 102, 240, 0);
  expect_codes(quantize(color_cast<ycbcr2020_t>(rgbf_t{1, 0, 0}), range::limited, bit_depth::depth8), 74, 97, 240, 0);
  expect_codes(quantize(color_cast<ycbcr601_t>(rgbf_t{0, 1, 0}), range::limited, bit_depth::depth8), 145, 54, 34, 0);
  expect_codes(quantize(color_cast<ycbcr709_t>(rgbf_t{0, 1, 0}), range::limited, bit_depth::depth8), 173, 42, 26, 0);
  expect_codes(quantize(color_cast<ycbcr601_t>(rgbf_t{0, 0, 1}), range::limited, bit_depth::depth8), 41, 240, 110, 0);
  expect_codes(quantize(color_cast<ycbcr709_t>(rgbf_t{0, 0, 1}), range::limited, bit_depth::depth8), 32, 240, 118, 0);

  // full 8 (601 red is the classic JPEG value)
  expect_codes(quantize(color_cast<ycbcr601_t>(rgbf_t{1, 0, 0}), range::full, bit_depth::depth8), 76, 85, 255, 0);
  expect_codes(quantize(color_cast<ycbcr709_t>(rgbf_t{1, 0, 0}), range::full, bit_depth::depth8), 54, 99, 255, 0);
  expect_codes(quantize(color_cast<ycbcr2020_t>(rgbf_t{1, 0, 0}), range::full, bit_depth::depth8), 67, 92, 255, 0);

  // 10-bit
  expect_codes(quantize(color_cast<ycbcr709_t>(rgbf_t{1, 0, 0}), range::limited, bit_depth::depth10), 250, 409, 960, 0);
  expect_codes(quantize(color_cast<ycbcr709_t>(rgbf_t{1, 0, 0}), range::full, bit_depth::depth10), 217, 395, 1023, 0);
}

// dequantize inverts quantize within half a code, and the float matrix
// conversion round-trips well inside one 8-bit step.
TEST(YCbCr, QuantizeDequantizeRoundTrip) {
  const rgbf_t colors[] = {RGB(1, 0, 0),           RGB(0, 1, 0), RGB(0, 0, 1),
                           RGB(1, 1, 1),           RGB(0, 0, 0), RGB(0.04f, 0.08f, 0.12f),
                           RGB(0.78f, 0.39f, 0.2f)};
  for (const auto& rgb : colors) {
    const auto y709 = color_cast<ycbcr709_t>(rgb);
    const auto back = color_cast<rgbf_t>(y709);
    EXPECT_NEAR(back.r(), rgb.r(), 2e-3f) << "r";
    EXPECT_NEAR(back.g(), rgb.g(), 2e-3f) << "g";
    EXPECT_NEAR(back.b(), rgb.b(), 2e-3f) << "b";

    const auto q = quantize(y709, range::limited, bit_depth::depth8);
    const auto dq = dequantize<ycbcr709_t>(q, range::limited, bit_depth::depth8);
    EXPECT_NEAR(dq.y(), y709.y(), 0.5f / 219.0f + 1e-4f) << "Y quantization step";
    EXPECT_NEAR(dq.cb(), y709.cb(), 0.5f / 224.0f + 1e-4f) << "Cb quantization step";
    EXPECT_NEAR(dq.cr(), y709.cr(), 0.5f / 224.0f + 1e-4f) << "Cr quantization step";
  }
}

// rgb8_t reaches YCbCr through the registered graph edges (rgb8 -> rgbf ->
// ycbcr); the 8-bit codes must match the direct float path.
TEST(YCbCr, ColorCastFromRgb8RoutesThroughGraph) {
  const auto y = color_cast<ycbcr709_t>(rgb8_t{255, 0, 0});
  expect_codes(quantize(y, range::full, bit_depth::depth8), 54, 99, 255, 0);
  const auto neutral = color_cast<ycbcr601_t>(rgb8_t{255, 255, 255});
  expect_codes(quantize(neutral, range::limited, bit_depth::depth8), 235, 128, 128, 0);
}

// The two ranges produce different codes for the same float value — the
// range parameter is not decorative.
TEST(YCbCr, FullAndLimitedRangesDiffer) {
  const auto c = ycbcr709_t{1.0f, 0.0f, 0.0f};
  const auto full = quantize(c, range::full, bit_depth::depth8);
  const auto limited = quantize(c, range::limited, bit_depth::depth8);
  EXPECT_EQ(full.y, 255);
  EXPECT_EQ(limited.y, 235);
  EXPECT_NE(full.y, limited.y);
}

}  // namespace colorcpp::core::test
