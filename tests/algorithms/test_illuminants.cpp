/**
 * @file test_illuminants.cpp
 * @brief Reference tests for the standard illuminant datasets and the
 *        CIE daylight locus.
 *
 * Chromaticity anchors come from an independent double-precision
 * evaluation: each dataset spd integrated against the library's 2° CMFs
 * (trapezoid, edge-clamped sampling) — the results agree with
 * colour-science's published illuminant chromaticities to <=5e-5.
 * Locus anchors come from the Wyszecki & Stiles / CIE 15:2004
 * relationships (including colour-science's own doctest point).
 */

#include <gtest/gtest.h>

#include <colorcpp/algorithms/spectral.hpp>

namespace colorcpp::algorithms::test {

using colorcpp::algorithms::spectral::illuminants::cie_d_xy;
using colorcpp::algorithms::spectral::spd;
using colorcpp::algorithms::spectral::spd_to_xyz;
namespace illuminants = colorcpp::algorithms::spectral::illuminants;

namespace {

void expect_xy(const spd& s, float ex, float ey, float tol) {
  const auto xyz = spd_to_xyz(s);
  ASSERT_GT(xyz.y(), 0.0f);
  const float sum = xyz.x() + xyz.y() + xyz.z();
  EXPECT_NEAR(xyz.x() / sum, ex, tol) << "x";
  EXPECT_NEAR(xyz.y() / sum, ey, tol) << "y";
}

}  // namespace

// Wyszecki & Stiles quadratic locus + colour-science doctest points.
TEST(Illuminants, DaylightLocus) {
  EXPECT_NEAR(cie_d_xy(6504.38938305f).first, 0.3127078f, 1e-6f);
  EXPECT_NEAR(cie_d_xy(6504.38938305f).second, 0.3291128f, 1e-6f);
  EXPECT_NEAR(cie_d_xy(4000.0f).first, 0.3823436f, 1e-6f);
  EXPECT_NEAR(cie_d_xy(4000.0f).second, 0.3837663f, 1e-6f);
  EXPECT_NEAR(cie_d_xy(7000.0f).first, 0.3053574f, 1e-6f);
  EXPECT_NEAR(cie_d_xy(10000.0f).first, 0.2787996f, 1e-6f);
  EXPECT_NEAR(cie_d_xy(25000.0f).first, 0.2498537f, 1e-6f);
  // y(x) relation at the D65-ish abscissa.
  EXPECT_NEAR(cie_d_xy(6504.38938305f).second, 0.3291051f, 1e-4f);

  // Outside the physical domain the input clamps deterministically.
  const auto low = cie_d_xy(1000.0f);
  const auto high = cie_d_xy(100000.0f);
  EXPECT_FLOAT_EQ(low.first, cie_d_xy(4000.0f).first);
  EXPECT_FLOAT_EQ(high.first, cie_d_xy(25000.0f).first);
}

// Integrated chromaticities of the shipped datasets (independent
// double-precision evaluation; colour-science's published xy values agree
// to <=5e-5).
TEST(Illuminants, ChromaticitiesOfShippedDatasets) {
  expect_xy(illuminants::a(), 0.4475733f, 0.4074398f, 1e-5f);
  expect_xy(illuminants::d50(), 0.3456690f, 0.3584972f, 1e-5f);
  expect_xy(illuminants::d55(), 0.3324250f, 0.3474286f, 1e-5f);
  expect_xy(illuminants::d60(), 0.3216176f, 0.3376206f, 1e-5f);
  expect_xy(illuminants::d65(), 0.3127120f, 0.3290093f, 1e-5f);
  expect_xy(illuminants::d75(), 0.2990232f, 0.3148537f, 1e-5f);
  expect_xy(illuminants::f2(), 0.3720657f, 0.3751179f, 1e-5f);
  expect_xy(illuminants::f11(), 0.3805355f, 0.3769117f, 1e-5f);
  expect_xy(illuminants::led_b1(), 0.4559512f, 0.4077988f, 1e-5f);
  expect_xy(illuminants::led_rgb1(), 0.4557462f, 0.4211208f, 1e-5f);
}

// Every shipped factory returns a non-empty, finite-valued spectrum.
TEST(Illuminants, AllFactoriesProduceData) {
  spd (*factories[])() = {
      &illuminants::a,    &illuminants::d50,  &illuminants::d55,  &illuminants::d60,
      &illuminants::d65,  &illuminants::d75,  &illuminants::f1,   &illuminants::f2,
      &illuminants::f3,   &illuminants::f4,   &illuminants::f5,   &illuminants::f6,
      &illuminants::f7,   &illuminants::f8,   &illuminants::f9,   &illuminants::f10,
      &illuminants::f11,  &illuminants::f12,  &illuminants::led_b1,  &illuminants::led_b2,
      &illuminants::led_b3, &illuminants::led_b4, &illuminants::led_b5, &illuminants::led_bh1,
      &illuminants::led_rgb1, &illuminants::led_v1, &illuminants::led_v2,
  };
  for (const auto f : factories) {
    const spd s = f();
    ASSERT_FALSE(s.empty()) << "factory returned an empty spectrum";
    const auto xyz = spd_to_xyz(s);
    EXPECT_TRUE(std::isfinite(xyz.x()) && std::isfinite(xyz.y()) && std::isfinite(xyz.z()));
    EXPECT_GT(xyz.y(), 0.0f);
  }
}

// CIE 15:2004 Appendix C: S(λ) = S0(λ) + M1·S1(λ) + M2·S2(λ), with M1/M2
// derived from the daylight locus and rounded to 3 decimals. The synthesized
// spectra approximate the tabulated D-series datasets (those are the CIE's
// tabulated values, the formula is a fit) and hit S(560) = 100 exactly.
TEST(Illuminants, DaylightSeriesSynthesis) {
  const spd d65 = illuminants::d_series(6504.38938305f);
  ASSERT_EQ(d65.size(), 107u);
  EXPECT_FLOAT_EQ(d65.start_nm, 300.0f);
  EXPECT_FLOAT_EQ(d65.step_nm, 5.0f);
  EXPECT_FLOAT_EQ(d65.nm_at(d65.size() - 1), 830.0f);
  EXPECT_NEAR(d65.sample(560.0f), 100.0f, 1e-4f);

  auto max_diff = [](const spd& a, const spd& b) {
    float m = 0.0f;
    const std::size_t n = std::min(a.size(), b.size());
    for (std::size_t i = 0; i < n; ++i) {
      m = std::max(m, std::abs(a.values[i] - b.values[i]));
    }
    return m;
  };
  EXPECT_LT(max_diff(d65, illuminants::d65()), 0.06f);
  EXPECT_LT(max_diff(illuminants::d_series(5003.0f), illuminants::d50()), 0.02f);
  EXPECT_LT(max_diff(illuminants::d_series(7504.0f), illuminants::d75()), 0.01f);
}

// The synthesized SPD's integrated chromaticity lands near the requested
// daylight point; the residual (~1e-4) is the CIE basis-fit error.
TEST(Illuminants, DaylightSeriesChromaticity) {
  const auto xyz = spd_to_xyz(illuminants::d_series_xy(0.31271f, 0.32902f));
  const float sum = xyz.x() + xyz.y() + xyz.z();
  EXPECT_NEAR(xyz.x() / sum, 0.31271f, 3e-4f);
  EXPECT_NEAR(xyz.y() / sum, 0.32902f, 3e-4f);
}

// CCT outside [4000, 25000] clamps to the nearest daylight point.
TEST(Illuminants, DaylightSeriesClampsCct) {
  const spd low = illuminants::d_series(1000.0f);
  const spd at_min = illuminants::d_series(4000.0f);
  ASSERT_EQ(low.size(), at_min.size());
  for (std::size_t i = 0; i < low.size(); ++i) {
    EXPECT_FLOAT_EQ(low.values[i], at_min.values[i]);
  }
  EXPECT_NEAR(low.sample(560.0f), 100.0f, 1e-4f);
}

}  // namespace colorcpp::algorithms::test
