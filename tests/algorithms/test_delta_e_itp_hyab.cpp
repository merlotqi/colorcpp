/**
 * @file test_delta_e_itp_hyab.cpp
 * @brief Tests for IC_T_C P colour encoding and ΔE_ITP / ΔE_HyAB.
 */

#include <gtest/gtest.h>

#include <colorcpp/algorithms/delta_e/hyab.hpp>
#include <colorcpp/algorithms/delta_e/itp.hpp>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/core/xyz.hpp>

namespace colorcpp::algorithms::test {

using namespace delta_e;

// XYZ -> ICtCp (Dolby 2016 / ITU-R BT.2100 PQ). Reference: independent
// double-precision evaluation of the BT.2100 equations with the library's
// scale convention (relative Y = 1 maps to the 100 cd/m^2 reference white,
// so PQ receives absolute nits — white encodes to PQ(100) exactly).
TEST(ICTCPTest, XYZMatchesAbsoluteScaleReference) {
  const core::xyz_t xyz(0.20654008f, 0.12197225f, 0.05136952f);
  const auto ictcp = xyz_to_ictcp(xyz);
  EXPECT_NEAR(ictcp.i(), 0.3175477f, 1e-5f);
  EXPECT_NEAR(ictcp.ct(), -0.0157026f, 1e-5f);
  EXPECT_NEAR(ictcp.cp(), 0.1732075f, 1e-5f);
}

// The scale anchor: D65 white (Y = 1) must land on the ST 2084 code value
// for 100 cd/m^2 — PQ(100) = 0.5080784 — with achromatic chroma.
TEST(ICTCPTest, WhiteEncodesToPq100Nits) {
  const core::xyz_t white(0.95047f, 1.0f, 1.08883f);
  const auto ictcp = xyz_to_ictcp(white);
  EXPECT_NEAR(ictcp.i(), 0.5080784f, 1e-5f);
  EXPECT_NEAR(ictcp.ct(), 0.0f, 1e-5f);
  EXPECT_NEAR(ictcp.cp(), 0.0f, 1e-5f);
}

TEST(ICTCPTest, RoundTripXYZ) {
  const core::xyz_t xyz(0.20654008f, 0.12197225f, 0.05136952f);
  const auto back = ictcp_to_xyz(xyz_to_ictcp(xyz));
  EXPECT_NEAR(back.x(), xyz.x(), 1e-4f);
  EXPECT_NEAR(back.y(), xyz.y(), 1e-4f);
  EXPECT_NEAR(back.z(), xyz.z(), 1e-4f);
}

TEST(ICTCPTest, BlackEncodesToZero) {
  const auto ictcp = xyz_to_ictcp(core::xyz_t(0.0f, 0.0f, 0.0f));
  EXPECT_NEAR(ictcp.i(), 0.0f, 1e-9f);
  EXPECT_NEAR(ictcp.ct(), 0.0f, 1e-9f);
  EXPECT_NEAR(ictcp.cp(), 0.0f, 1e-9f);
}

// ΔE_ITP (ITU-R BT.2124). Reference: colour-science delta_E_ITP doctest.
TEST(DeltaEITPTest, ReferencePairMatchesColorScience) {
  const ictcp_t a(0.4885468072f, -0.04739350675f, 0.07475401302f);
  const ictcp_t b(0.4899203231f, -0.04567508203f, 0.07361341775f);
  EXPECT_NEAR(delta_e_itp(a, b), 1.4265722f, 1e-4f);
}

TEST(DeltaEITPTest, SameEncodingIsZero) {
  const ictcp_t a(0.4885468072f, -0.04739350675f, 0.07475401302f);
  EXPECT_NEAR(delta_e_itp(a, a), 0.0f, 1e-6f);
}

TEST(DeltaEITPTest, SymmetricAndNonNegative) {
  const ictcp_t a(0.4885468072f, -0.04739350675f, 0.07475401302f);
  const ictcp_t b(0.4899203231f, -0.04567508203f, 0.07361341775f);
  EXPECT_NEAR(delta_e_itp(a, b), delta_e_itp(b, a), 1e-5f);
  EXPECT_GE(delta_e_itp(a, b), 0.0f);
}

// HyAB (Abasi et al. 2020): |ΔL| + √(Δa² + Δb²) in CIELAB.
// Reference: colour-science delta_E_HyAB doctest.
TEST(DeltaEHyABTest, ReferencePairMatchesColorScience) {
  const core::cielab_t a(39.91531343f, 51.16658481f, 146.12933781f);
  const core::cielab_t b(53.12207516f, -39.92365056f, 249.54831278f);
  EXPECT_NEAR(delta_e_hyab(a, b), 151.0215481f, 1e-3f);
}

TEST(DeltaEHyABTest, SameColorIsZero) {
  const core::cielab_t a(50.0f, 10.0f, -20.0f);
  EXPECT_NEAR(delta_e_hyab(a, a), 0.0f, 1e-5f);
}

TEST(DeltaEHyABTest, PureLightnessDifferenceEqualsDeltaL) {
  const core::cielab_t a(20.0f, 30.0f, 40.0f);
  const core::cielab_t b(50.0f, 30.0f, 40.0f);
  EXPECT_NEAR(delta_e_hyab(a, b), 30.0f, 1e-4f);
}

TEST(DeltaEHyABTest, IsSymmetric) {
  const core::cielab_t a(39.91531343f, 51.16658481f, 146.12933781f);
  const core::cielab_t b(53.12207516f, -39.92365056f, 249.54831278f);
  EXPECT_NEAR(delta_e_hyab(a, b), delta_e_hyab(b, a), 1e-4f);
}

}  // namespace colorcpp::algorithms::test
