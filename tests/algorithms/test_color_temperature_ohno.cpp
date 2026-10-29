/**
 * @file test_color_temperature_ohno.cpp
 * @brief Tests for Planckian locus generation and Ohno (2013) CCT / Duv.
 */

#include <gtest/gtest.h>

#include <colorcpp/algorithms/color_temperature/ohno.hpp>
#include <colorcpp/algorithms/color_temperature/planckian.hpp>
#include <colorcpp/core/xyz.hpp>

namespace colorcpp::algorithms::test {

using namespace color_temperature;

// Planckian locus anchors: colour-science CCT_to_uv_Planck1900 doctests with
// the embedded 5 nm CIE 1931 2° CMFs (verified against the 1 nm dataset to
// ~3e-7 in uv).
TEST(PlanckianTest, UVAt6504KMatchesReference) {
  const auto uv = planck_uv(6504.0f);
  EXPECT_NEAR(uv.first, 0.2004280f, 5e-5f);
  EXPECT_NEAR(uv.second, 0.3103334f, 5e-5f);
}

TEST(PlanckianTest, UVAt1000KMatchesReference) {
  const auto uv = planck_uv(1000.0f);
  EXPECT_NEAR(uv.first, 0.4480106f, 1e-4f);
  EXPECT_NEAR(uv.second, 0.3546250f, 1e-4f);
}

TEST(PlanckianTest, DefaultTableSpansRequestedRange) {
  const planckian_table table;
  EXPECT_GT(table.size(), 500u);
  EXPECT_FLOAT_EQ(table.temperature(0), 1000.0f);
  EXPECT_FLOAT_EQ(table.temperature(table.size() - 1), 25000.0f);
  // Temperatures strictly increase.
  for (std::size_t i = 1; i < table.size(); ++i) {
    EXPECT_GT(table.temperature(i), table.temperature(i - 1));
  }
}

// Ohno (2013) anchors: colour-science uv_to_CCT_Ohno2013 with the library
// configuration (5 nm CMFs, 1000–25000 K, spacing 1.005) computed from the
// validated pure-Python replication.
TEST(OhnoTest, OnLocusPointYieldsItsTemperature) {
  // Planckian point at 6504 K.
  const cct_duv r = uv_to_cct_ohno(0.2004283f, 0.3103337f);
  EXPECT_NEAR(r.cct, 6504.2f, 2.0f);
  EXPECT_NEAR(r.duv, 0.0f, 1e-4f);
}

TEST(OhnoTest, OffLocusPointMatchesColorScienceReference) {
  // colour-science doctest: uv(0.1978, 0.3122) -> (6507.47, 0.003223).
  const cct_duv r = uv_to_cct_ohno(0.1978f, 0.3122f);
  EXPECT_NEAR(r.cct, 6507.5f, 2.0f);
  EXPECT_NEAR(r.duv, 0.003223f, 2e-5f);
}

TEST(OhnoTest, LowTemperatureAnchor) {
  const cct_duv r = uv_to_cct_ohno(0.4480106f, 0.3546250f);
  EXPECT_NEAR(r.cct, 1000.2f, 3.0f);
  EXPECT_NEAR(r.duv, 0.0f, 2e-4f);
}

TEST(OhnoTest, NegativeDuvIsSigned) {
  // Mirror of the off-locus point across the locus: Duv must flip sign.
  const cct_duv r = uv_to_cct_ohno(0.1978f, 0.3055f);
  EXPECT_LT(r.duv, 0.0f);
}

// CCT + Duv -> uv (colour-science CCT_to_uv_Ohno2013 doctest):
// (6507.4342201047066, 0.003223690901513) -> (0.1978, 0.31220).
TEST(OhnoTest, CCTToUVMatchesColorScienceReference) {
  const auto uv = cct_to_uv_ohno(6507.4342f, 0.0032237f);
  EXPECT_NEAR(uv.first, 0.1978000f, 1e-4f);
  EXPECT_NEAR(uv.second, 0.3122000f, 1e-4f);
}

TEST(OhnoTest, RoundTripUV) {
  const cct_duv r = uv_to_cct_ohno(0.1978f, 0.3122f);
  const auto uv = cct_to_uv_ohno(r.cct, r.duv);
  EXPECT_NEAR(uv.first, 0.1978f, 1e-4f);
  EXPECT_NEAR(uv.second, 0.3122f, 1e-4f);
}

TEST(OhnoTest, XYZConvenienceMatchesUvPath) {
  // Same off-locus sample as the uv anchor, entered as XYZ.
  const core::xyz_t xyz(0.95035049f, 1.0f, 1.08935705f);
  const cct_duv r = xyz_to_cct_ohno(xyz);
  EXPECT_NEAR(r.cct, 6507.5f, 2.0f);
  EXPECT_NEAR(r.duv, 0.003223f, 2e-5f);
}

}  // namespace colorcpp::algorithms::test
