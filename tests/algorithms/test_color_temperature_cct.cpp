/**
 * @file test_color_temperature_cct.cpp
 * @brief Tests for McCamy (1992), Robertson (1968) CCT and mired helpers.
 */

#include <gtest/gtest.h>

#include <colorcpp/algorithms/color_temperature/mccamy.hpp>
#include <colorcpp/algorithms/color_temperature/ohno.hpp>
#include <colorcpp/algorithms/color_temperature/robertson.hpp>
#include <colorcpp/core/xyy.hpp>

namespace colorcpp::algorithms::test {

using namespace color_temperature;

// McCamy (1992): closed-form cubic. Reference: colour-science
// xy_to_CCT_McCamy1992 doctest.
TEST(McCamyTest, D65ChromaticityMatchesColorScienceReference) {
  const core::xyy_t xyy(0.3127f, 0.3290f, 1.0f);
  EXPECT_NEAR(xy_to_cct_mccamy(xyy), 6505.0806f, 0.1f);
}

TEST(McCamyTest, OnPlanckianLocusGivesApproximatelyItsTemperature) {
  // xy of the 6504 K Planckian point (5 nm locus).
  const core::xyy_t xyy(0.3134653f, 0.3235699f, 1.0f);
  EXPECT_NEAR(xy_to_cct_mccamy(xyy), 6503.2f, 1.0f);
}

// Robertson (1968). Reference: colour-science uv_to_CCT_Robertson1968 doctest.
TEST(RobertsonTest, AnchorMatchesColorScienceReference) {
  const cct_duv r = uv_to_cct_robertson1968(0.193741375998230f, 0.315221043940594f);
  EXPECT_NEAR(r.cct, 6500.0162f, 0.1f);
  EXPECT_NEAR(r.duv, 0.0083333f, 1e-5f);
}

TEST(RobertsonTest, RoundTrip) {
  const cct_duv r = uv_to_cct_robertson1968(0.193741375998230f, 0.315221043940594f);
  const auto uv = cct_to_uv_robertson1968(r.cct, r.duv);
  EXPECT_NEAR(uv.first, 0.1937414f, 1e-4f);
  EXPECT_NEAR(uv.second, 0.3152210f, 1e-4f);
}

TEST(RobertsonTest, OnLocusPointHasNearZeroDuv) {
  // 6504 K Planckian point in CIE 1960 uv.
  const cct_duv r = uv_to_cct_robertson1968(0.2004283f, 0.3103337f);
  EXPECT_NEAR(r.cct, 6504.0f, 15.0f);
  EXPECT_NEAR(r.duv, 0.0f, 1e-3f);
}

// Mired (micro reciprocal degree) helpers: 1e6 / CCT.
TEST(MiredTest, CCTToMiredAndBack) {
  EXPECT_NEAR(cct_to_mired(6504.0f), 153.7515f, 1e-3f);
  EXPECT_NEAR(mired_to_cct(153.7515f), 6504.0f, 0.1f);
}

// Cross-method consistency: Ohno and Robertson estimate the same physical
// quantity from the same chromaticity (tables/approximations differ slightly).
TEST(CCTMethodConsistency, OhnoAndRobertsonAgreeOnOffLocusPoint) {
  const cct_duv ohno = uv_to_cct_ohno(0.1978f, 0.3122f);
  const cct_duv robertson = uv_to_cct_robertson1968(0.1978f, 0.3122f);
  EXPECT_NEAR(ohno.cct, robertson.cct, 10.0f);
  EXPECT_NEAR(ohno.duv, robertson.duv, 5e-4f);
}

}  // namespace colorcpp::algorithms::test
