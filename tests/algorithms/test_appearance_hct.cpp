/**
 * @file test_appearance_hct.cpp
 * @brief Reference and property tests for HCT (CAM16 hue/chroma + CIE L* tone).
 *
 * The viewing-condition anchors are material-color-utilities'
 * kDefaultViewingConditions (Apache-2.0, numeric constants); the round-trip
 * anchors and the property grid mirror that project's hct_solver_test.cc /
 * hct_test.cc assertions. The solver itself is an independent Newton
 * implementation on top of this library's validated CAM16 inverse.
 */

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <colorcpp/algorithms/appearance.hpp>
#include <colorcpp/colorcpp.hpp>

namespace colorcpp::algorithms::test {

using colorcpp::algorithms::appearance::hct;
using colorcpp::algorithms::appearance::hct_to_xyz;
using colorcpp::algorithms::appearance::hct_viewing_conditions;
using colorcpp::algorithms::appearance::xyz_to_hct;
using colorcpp::core::rgba8_t;
using colorcpp::core::xyz_t;

namespace {

// Circular hue distance in degrees.
float hue_dist(float a, float b) {
  float d = std::fmod(std::abs(a - b), 360.0f);
  return d > 180.0f ? 360.0f - d : d;
}

bool on_rgba8_boundary(const rgba8_t& c) {
  return c.r() == 0 || c.r() == 255 || c.g() == 0 || c.g() == 255 || c.b() == 0 || c.b() == 255;
}

}  // namespace

// material-color-utilities kDefaultViewingConditions: D65 white, L_A from
// 200/pi * Y(L*=50), background L* = 50, average surround.
TEST(Hct, DefaultViewingConditionsMatchMaterial) {
  const auto vc = hct_viewing_conditions();
  EXPECT_NEAR(vc.n, 0.184186503, 1e-5);
  EXPECT_NEAR(vc.z, 1.909169555, 1e-5);
  EXPECT_NEAR(vc.N_bb, 1.016919255, 1e-4);
  EXPECT_NEAR(vc.F_L, 0.388481468, 1e-5);
  EXPECT_NEAR(vc.A_w, 29.981000900, 2e-3);
  EXPECT_NEAR(vc.c, 0.69f, 1e-6);
  EXPECT_NEAR(vc.N_c, 1.0f, 1e-6);
}

// material hct_solver_test.cc Red/Green/Blue: (h, c, t) of an sRGB color
// must solve back to the same 8-bit triple.
TEST(Hct, RoundTripAnchors) {
  const rgba8_t anchors[] = {
      rgba8_t{254, 3, 21, 255},
      rgba8_t{21, 254, 3, 255},
      rgba8_t{3, 21, 254, 255},
  };
  for (const auto& c : anchors) {
    const auto xyz = colorcpp::operations::conversion::color_cast<xyz_t>(c);
    const auto h = xyz_to_hct(xyz);
    const auto back = colorcpp::operations::conversion::color_cast<rgba8_t>(hct_to_xyz(h));
    EXPECT_EQ(back.r(), c.r()) << "r of (" << static_cast<int>(c.r()) << "," << static_cast<int>(c.g()) << ","
                               << static_cast<int>(c.b()) << ")";
    EXPECT_EQ(back.g(), c.g());
    EXPECT_EQ(back.b(), c.b());
  }
}

// material hct_test.cc Correctness: after an 8-bit round trip the recovered
// hue stays within 4 deg, tone within 0.5, chroma never exceeds the target
// by 2.5; a chroma loss beyond 2.5 only happens at the gamut boundary.
TEST(Hct, PropertyGridRoundTrip) {
  for (float hue : {15.0f, 45.0f, 75.0f, 105.0f, 135.0f, 165.0f, 195.0f, 225.0f, 255.0f, 285.0f, 315.0f, 345.0f}) {
    for (float chroma : {0.0f, 20.0f, 40.0f, 60.0f, 80.0f, 100.0f}) {
      for (float tone : {20.0f, 40.0f, 60.0f, 80.0f}) {
        const hct target{hue, chroma, tone};
        const auto xyz = hct_to_xyz(target);
        const auto px = colorcpp::operations::conversion::color_cast<rgba8_t>(xyz);
        const auto back = xyz_to_hct(colorcpp::operations::conversion::color_cast<xyz_t>(px));
        if (chroma > 0.0f) {
          EXPECT_LE(hue_dist(back.hue, hue), 4.0f) << "hue=" << hue << " c=" << chroma << " t=" << tone;
        }
        EXPECT_LE(back.chroma, chroma + 2.5f) << "hue=" << hue << " c=" << chroma << " t=" << tone;
        if (back.chroma < chroma - 2.5f) {
          EXPECT_TRUE(on_rgba8_boundary(px)) << "hue=" << hue << " c=" << chroma << " t=" << tone;
        }
        EXPECT_NEAR(back.tone, tone, 0.5f) << "hue=" << hue << " c=" << chroma << " t=" << tone;
      }
    }
  }
}

// material hct_test.cc LimitedToSRGB: an impossible chroma collapses to the
// gamut boundary while hue and tone survive.
TEST(Hct, ImpossibleChromaClampsToBoundary) {
  const hct target{120.0f, 200.0f, 50.0f};
  const auto px = colorcpp::operations::conversion::color_cast<rgba8_t>(hct_to_xyz(target));
  const auto back = xyz_to_hct(colorcpp::operations::conversion::color_cast<xyz_t>(px));
  EXPECT_LE(hue_dist(back.hue, 120.0f), 4.0f);
  EXPECT_LT(back.chroma, 200.0f);
  EXPECT_NEAR(back.tone, 50.0f, 0.5f);
}

// Grays and the L* endpoints: chroma 0 is achromatic at any hue; tone 0/100
// are black/white regardless of chroma.
TEST(Hct, GrayAndToneEndpoints) {
  const auto gray = colorcpp::operations::conversion::color_cast<rgba8_t>(hct_to_xyz(hct{200.0f, 0.0f, 50.0f}));
  EXPECT_EQ(gray.r(), gray.g());
  EXPECT_EQ(gray.g(), gray.b());

  const auto black = colorcpp::operations::conversion::color_cast<rgba8_t>(hct_to_xyz(hct{10.0f, 80.0f, 0.0f}));
  EXPECT_EQ(black.r(), 0);
  EXPECT_EQ(black.g(), 0);
  EXPECT_EQ(black.b(), 0);

  const auto white = colorcpp::operations::conversion::color_cast<rgba8_t>(hct_to_xyz(hct{300.0f, 80.0f, 100.0f}));
  EXPECT_EQ(white.r(), 255);
  EXPECT_EQ(white.g(), 255);
  EXPECT_EQ(white.b(), 255);

  // Material's SolveToInt treats chroma < 0.0001 as zero and returns the
  // exact sRGB gray; the CAM16-neutral path would keep the white point's
  // small residual chroma and land off-neutral.
  const auto near_gray = colorcpp::operations::conversion::color_cast<rgba8_t>(hct_to_xyz(hct{200.0f, 1e-5f, 50.0f}));
  EXPECT_EQ(near_gray.r(), near_gray.g());
  EXPECT_EQ(near_gray.g(), near_gray.b());
}

// The CIE L* formulas switch at L* = 8 (Y100 = 0.8856): below it both
// directions are linear, above they are cubic. The grid and anchors only
// exercise tones >= 20, so pin the linear region with exact grays (an
// achromatic round trip survives the 8-bit quantization at these levels).
TEST(Hct, LowToneLinearRegion) {
  for (float tone : {2.0f, 5.0f}) {
    const auto gray = colorcpp::operations::conversion::color_cast<rgba8_t>(hct_to_xyz(hct{200.0f, 0.0f, tone}));
    EXPECT_EQ(gray.r(), gray.g());
    EXPECT_EQ(gray.g(), gray.b());
    const auto back = xyz_to_hct(colorcpp::operations::conversion::color_cast<xyz_t>(gray));
    EXPECT_NEAR(back.tone, tone, 0.5f) << "tone=" << tone;
  }
}

}  // namespace colorcpp::algorithms::test
