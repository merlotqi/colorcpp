/**
 * @file test_rec2100.cpp
 * @brief Reference tests for ITU-R BT.2100 PQ and HLG signal encodings.
 *
 * Reference anchors come from independent double-precision evaluation of
 * the ST 2084 (PQ) and BT.2100/ARIB B-67 (HLG OETF) formulas:
 *   PQ:   code = ((c1 + c2 x) / (1 + c3 x))^m2 with x = (L / 10000)^m1,
 *         m1 = 2610/16384, m2 = 2523/32, c1 = 3424/4096,
 *         c2 = 2413/128, c3 = 2392/128 (L in cd/m^2).
 *   HLG:  OETF(E) = sqrt(3E) for E <= 1/12, else
 *         a*ln(12E - b) + c with a = 0.17883277, b = 0.28466892,
 *         c = 0.55991073 (scene-referred, display OOTF out of scope).
 * Library scale: relative Y = 1 maps to the 100 cd/m^2 reference white
 * before PQ, so white encodes to PQ(100) = 0.5080784 — the same anchor the
 * ICtCp path pins.
 */

#include <gtest/gtest.h>

#include <cmath>
#include <colorcpp/colorcpp.hpp>

namespace colorcpp::core::test {

using colorcpp::operations::conversion::color_cast;

TEST(Rec2100Pq, ConstructionAndAccessors) {
  rec2100_pq_rgbf_t c(0.5f, 0.25f, 0.75f);
  EXPECT_FLOAT_EQ(c.r(), 0.5f);
  EXPECT_FLOAT_EQ(c.g(), 0.25f);
  EXPECT_FLOAT_EQ(c.b(), 0.75f);
}

// White (relative Y = 1 -> 100 cd/m^2) must sit on PQ(100); black on 0.
TEST(Rec2100Pq, WhiteAndBlackAnchors) {
  const xyz_t white(0.95047f, 1.0f, 1.08883f);
  const auto pq = color_cast<rec2100_pq_rgbf_t>(white);
  EXPECT_NEAR(pq.r(), 0.5080784f, 1e-5f);
  EXPECT_NEAR(pq.g(), 0.5080784f, 1e-5f);
  EXPECT_NEAR(pq.b(), 0.5080784f, 1e-5f);

  const xyz_t black(0.0f, 0.0f, 0.0f);
  const auto pq0 = color_cast<rec2100_pq_rgbf_t>(black);
  EXPECT_NEAR(pq0.r(), 0.0f, 1e-7f);
  EXPECT_NEAR(pq0.g(), 0.0f, 1e-7f);
  EXPECT_NEAR(pq0.b(), 0.0f, 1e-7f);
}

// Cross-module scale consistency: for white every BT.2020 cone response is
// equal, so the ICtCp intensity and the PQ signal are the same number —
// PQ(100) = 0.5080784 on both paths.
TEST(Rec2100Pq, MatchesIctcpWhiteAnchor) {
  const xyz_t white(0.95047f, 1.0f, 1.08883f);
  const auto pq = color_cast<rec2100_pq_rgbf_t>(white);
  const auto ictcp = color_cast<ictcp_t>(white);
  EXPECT_NEAR(pq.r(), ictcp.i(), 1e-6f);
}

// Direct TF anchors: relative linear BT.2020 x nits -> PQ code.
TEST(Rec2100Pq, TransferFunctionAnchors) {
  const auto code_at = [](float relative) {
    return color_cast<rec2100_pq_rgbf_t>(linear_rec2020_rgbf_t(relative, relative, relative)).r();
  };
  EXPECT_NEAR(code_at(0.0f), 0.0f, 1e-7f);         // 0 nits
  EXPECT_NEAR(code_at(0.01f), 0.1499457f, 1e-5f);  // 1 nit
  EXPECT_NEAR(code_at(0.1f), 0.2996991f, 1e-5f);   // 10 nits
  EXPECT_NEAR(code_at(1.0f), 0.5080784f, 1e-5f);   // 100 nits
  EXPECT_NEAR(code_at(10.0f), 0.7518271f, 1e-5f);  // 1000 nits
}

TEST(Rec2100Pq, RoundTrip) {
  const xyz_t colors[] = {
      xyz_t(0.95047f, 1.0f, 1.08883f),
      xyz_t(0.4124f, 0.2126f, 0.0193f),
      xyz_t(0.2f, 0.4f, 0.6f),
  };
  for (const auto& xyz : colors) {
    const auto back = color_cast<xyz_t>(color_cast<rec2100_pq_rgbf_t>(xyz));
    EXPECT_NEAR(back.x(), xyz.x(), 1e-4f) << "x";
    EXPECT_NEAR(back.y(), xyz.y(), 1e-4f) << "y";
    EXPECT_NEAR(back.z(), xyz.z(), 1e-4f) << "z";
  }
}

TEST(Rec2100Hlg, ConstructionAndAccessors) {
  rec2100_hlga_rgbaf_t c(0.5f, 0.25f, 0.75f, 0.4f);
  EXPECT_FLOAT_EQ(c.r(), 0.5f);
  EXPECT_FLOAT_EQ(c.a(), 0.4f);
}

// Scene-referred OETF: relative linear 1.0 maps to signal 1.0, the
// piecewise breakpoint 1/12 maps to exactly 0.5, and mid-scene 0.5 lands
// on a*ln(12*0.5 - b) + c.
TEST(Rec2100Hlg, TransferFunctionAnchors) {
  const auto signal_at = [](float scene) {
    return color_cast<rec2100_hlg_rgbf_t>(linear_rec2020_rgbf_t(scene, scene, scene)).r();
  };
  EXPECT_NEAR(signal_at(0.0f), 0.0f, 1e-7f);
  EXPECT_NEAR(signal_at(1.0f / 12.0f), 0.5f, 1e-6f);
  EXPECT_NEAR(signal_at(0.5f), 0.8716435f, 1e-5f);
  EXPECT_NEAR(signal_at(1.0f), 1.0f, 1e-6f);
}

TEST(Rec2100Hlg, InverseAnchors) {
  const auto scene_at = [](float signal) {
    return color_cast<linear_rec2020_rgbf_t>(rec2100_hlg_rgbf_t(signal, signal, signal)).r();
  };
  EXPECT_NEAR(scene_at(0.5f), 1.0f / 12.0f, 1e-6f);
  EXPECT_NEAR(scene_at(0.8716435f), 0.5f, 1e-5f);
  EXPECT_NEAR(scene_at(1.0f), 1.0f, 1e-6f);
}

TEST(Rec2100Hlg, RoundTrip) {
  const xyz_t colors[] = {
      xyz_t(0.95047f, 1.0f, 1.08883f),
      xyz_t(0.4124f, 0.2126f, 0.0193f),
      xyz_t(0.2f, 0.4f, 0.6f),
  };
  for (const auto& xyz : colors) {
    const auto back = color_cast<xyz_t>(color_cast<rec2100_hlg_rgbf_t>(xyz));
    EXPECT_NEAR(back.x(), xyz.x(), 1e-4f) << "x";
    EXPECT_NEAR(back.y(), xyz.y(), 1e-4f) << "y";
    EXPECT_NEAR(back.z(), xyz.z(), 1e-4f) << "z";
  }
}

// Alpha survives the signal conversion when the target carries it.
TEST(Rec2100, AlphaPassthrough) {
  const auto pq = color_cast<rec2100_pqa_rgbaf_t>(linear_rec2020_rgbaf_t(1.0f, 1.0f, 1.0f, 0.25f));
  EXPECT_NEAR(pq.r(), 0.5080784f, 1e-5f);
  EXPECT_NEAR(pq.a(), 0.25f, 1e-6f);

  const auto hlg = color_cast<rec2100_hlga_rgbaf_t>(linear_rec2020_rgbaf_t(1.0f, 1.0f, 1.0f, 0.75f));
  EXPECT_NEAR(hlg.r(), 1.0f, 1e-6f);
  EXPECT_NEAR(hlg.a(), 0.75f, 1e-6f);
}

// Graph integration: the signal types hang off linear Rec.2020 (same
// primaries, different transfer function), which already routes to XYZ.
TEST(Rec2100, GraphRoutesThroughLinearRec2020) {
  static_assert(operations::conversion::graph::minimal_conversion_cost<rgbf_t, rec2100_pq_rgbf_t>() == 4);
  static_assert(operations::conversion::graph::minimal_conversion_cost<rec2100_pq_rgbf_t, rec2100_hlg_rgbf_t>() == 2);
  SUCCEED();
}

// color_cast restores the source alpha after routing, which would mask an
// alpha bug inside the conversion function itself — call the function
// directly to pin the function-level contract for direct callers.
TEST(Rec2100, ConversionFunctionCarriesAlphaDirectly) {
  const auto pq = colorcpp::operations::conversion::details::linear_rec2020_to_rec2100_pq<rec2100_pqa_rgbaf_t>(
      linear_rec2020_rgbaf_t(1.0f, 1.0f, 1.0f, 0.25f));
  EXPECT_NEAR(pq.r(), 0.5080784f, 1e-5f);
  EXPECT_NEAR(pq.a(), 0.25f, 1e-6f);
}

}  // namespace colorcpp::core::test
