/**
 * @file test_color_temperature_ohno.cpp
 * @brief Tests for Planckian locus generation and Ohno (2013) CCT / Duv.
 */

#include <gtest/gtest.h>

#include <cmath>
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

// ===== Issue #101: out-of-table inputs silently returned garbage.
//     The nearest-row index is clamped but the triangular/parabolic
//     interpolants extrapolate beyond it, and cct_to_uv_ohno integrated
//     non-positive temperatures (planck_uv(0) -> (0,0), negatives -> garbage). =====

TEST(OhnoTest, SubTableUvClampsToTableMinimum) {
  // uv of 500 K extrapolated below the table (returned 396 K).
  const auto uv500 = planck_uv(500.0f);
  const cct_duv r = uv_to_cct_ohno(uv500.first, uv500.second);
  EXPECT_GE(r.cct, 1000.0f) << "issue #101: CCT must not fall below the table minimum";
  EXPECT_NEAR(r.cct, 1000.0f, 1.0f);
}

TEST(OhnoTest, SuperTableUvClampsToTableMaximum) {
  // uv of 1 MK extrapolated past the table (returned 47513 K).
  const auto uv1m = planck_uv(1000000.0f);
  const cct_duv r = uv_to_cct_ohno(uv1m.first, uv1m.second);
  EXPECT_LE(r.cct, 25000.0f) << "issue #101: CCT must not exceed the table maximum";
  EXPECT_NEAR(r.cct, 25000.0f, 1.0f);
}

TEST(OhnoTest, AbsurdUvStaysWithinTableRange) {
  const cct_duv r = uv_to_cct_ohno(0.9f, 0.9f);
  EXPECT_GE(r.cct, 1000.0f);
  EXPECT_LE(r.cct, 25000.0f);
}

TEST(OhnoTest, NonPositiveForwardCctClampsToTableMinimum) {
  // planck_uv(0) integrates to zero XYZ -> (0,0); negatives yield garbage.
  const auto uv0 = cct_to_uv_ohno(0.0f, 0.0f);
  const auto uvRef = planck_uv(1000.0f);
  EXPECT_NEAR(uv0.first, uvRef.first, 1e-5f) << "issue #101: T=0 must clamp to the table minimum";
  EXPECT_NEAR(uv0.second, uvRef.second, 1e-5f);
  const auto uvn = cct_to_uv_ohno(-100.0f, 0.0f);
  EXPECT_NEAR(uvn.first, uvRef.first, 1e-5f) << "issue #101: negative T must clamp, not return garbage";
  EXPECT_NEAR(uvn.second, uvRef.second, 1e-5f);
}

// ===== Issue #109: two residual accuracy bugs in the Ohno pair. =====
// (1) cct_to_uv_ohno estimated the locus normal from planck_uv(T+1) - planck_uv(T)
//     in float32; at 24000 K the 1 K uv step (~5.2e-7) is below the ~4e-8
//     per-call float noise, so the normal was noise-dominated and high-CCT
//     offsets drifted tangentially (double-precision reference error 8.0e-4
//     at 24000 K / Duv 0.02; round-trip returned cct=25000, duv=+0.412).
// (2) The parabolic branch exploded on the near-duplicate appended end rows
//     (24996/24999/25000) — probes at Duv=-0.005 came back as +0.0216/+0.0407.

namespace ohno_test_detail {

// Double-precision Planck uv over the header's CMF table (issue #109's
// reference integration).
inline std::pair<double, double> PlanckUvReference(double t) {
  constexpr double c1 = 3.741771e-16;
  constexpr double c2 = 1.4388e-2;
  constexpr std::size_t n =
      sizeof(color_temperature::details::CMF_1931_2DEG) / sizeof(color_temperature::details::CMF_1931_2DEG[0]);
  double s_prev = 0, lambda_prev = 0, X = 0, Y = 0, Z = 0;
  for (std::size_t i = 0; i < n; ++i) {
    const double lambda = color_temperature::details::CMF_1931_2DEG[i].nm * 1e-9;
    const double s = c1 * std::pow(lambda, -5.0) / std::expm1(c2 / (lambda * t));
    if (i > 0) {
      const double dl = lambda - lambda_prev;
      X += 0.5 *
           (s * color_temperature::details::CMF_1931_2DEG[i].x_bar +
            s_prev * color_temperature::details::CMF_1931_2DEG[i - 1].x_bar) *
           dl;
      Y += 0.5 *
           (s * color_temperature::details::CMF_1931_2DEG[i].y_bar +
            s_prev * color_temperature::details::CMF_1931_2DEG[i - 1].y_bar) *
           dl;
      Z += 0.5 *
           (s * color_temperature::details::CMF_1931_2DEG[i].z_bar +
            s_prev * color_temperature::details::CMF_1931_2DEG[i - 1].z_bar) *
           dl;
    }
    s_prev = s;
    lambda_prev = lambda;
  }
  const double d = X + 15.0 * Y + 3.0 * Z;
  return {4.0 * X / d, 6.0 * Y / d};
}

// Unit locus normal via a central difference over +/-0.1% of T, in double.
inline std::pair<double, double> LocusNormalReference(double t) {
  const double h = t * 1e-3;
  const auto a = PlanckUvReference(t - h);
  const auto b = PlanckUvReference(t + h);
  const double du = a.first - b.first;
  const double dv = a.second - b.second;
  const double mag = std::hypot(du, dv);
  return {du / mag, dv / mag};
}

}  // namespace ohno_test_detail

// (1a) Forward normal accuracy: cct_to_uv_ohno's offset point must sit on the
// double-precision reference normal. The old 1 K step produced 7.4e-5 /
// 2.3e-4 / 8.0e-4 error at 6500 / 20000 / 24000 K (issue #109).
TEST(OhnoTest, ForwardOffsetMatchesDoubleReference) {
  using namespace ohno_test_detail;
  for (double t : {2000.0, 6500.0, 15000.0, 24000.0}) {
    for (double duv : {-0.02, 0.02}) {
      const auto uv_ref = PlanckUvReference(t);
      const auto n = LocusNormalReference(t);
      const double eu = uv_ref.first - duv * n.second;
      const double ev = uv_ref.second + duv * n.first;
      const auto uv = cct_to_uv_ohno(static_cast<float>(t), static_cast<float>(duv));
      const double err = std::hypot(uv.first - eu, uv.second - ev);
      EXPECT_LE(err, 2e-5) << "issue #109: normal direction error at T=" << t << " Duv=" << duv
                           << " (old 1 K step: 7.4e-5..8.0e-4)";
    }
  }
}

// (1b) User-visible symptom: forward offset -> inverse must recover CCT/Duv.
// The old direction error mapped to >300 K CCT error at 20000 K and a
// clamped cct=25000 / duv=+0.412 at 24000 K, Duv=+0.02.
TEST(OhnoTest, HighCctOffsetRoundTrips) {
  for (double t : {6500.0, 20000.0, 24000.0}) {
    for (double duv : {-0.02, 0.02}) {
      const auto uv = cct_to_uv_ohno(static_cast<float>(t), static_cast<float>(duv));
      const cct_duv r = uv_to_cct_ohno(uv.first, uv.second);
      EXPECT_NEAR(r.cct, t, 150.0) << "issue #109: round-trip CCT at T=" << t << " Duv=" << duv;
      EXPECT_NEAR(r.duv, duv, 5e-3) << "issue #109: round-trip Duv at T=" << t << " Duv=" << duv;
    }
  }
}

// (2) The last rows before the appended 24999/25000 pair are spaced 98 K / 3 K
// / 1 K — the parabola through that near-duplicate triplet exploded (vertex
// clamped to 25000, value flipped sign: Duv_in=-0.005 -> duv=+0.0216/+0.0407).
TEST(OhnoTest, TableEndProbesPreserveDuvSign) {
  using namespace ohno_test_detail;
  const planckian_table table;
  ASSERT_GT(table.size(), 4u);
  for (std::size_t row : {table.size() - 3, table.size() - 2}) {
    const float t = table.temperature(row);
    const auto uv_ref = PlanckUvReference(t);
    const auto n = LocusNormalReference(t);
    for (double duv : {-0.05, -0.02, -0.005, 0.005, 0.02, 0.05}) {
      const float u = static_cast<float>(uv_ref.first - duv * n.second);
      const float v = static_cast<float>(uv_ref.second + duv * n.first);
      const cct_duv r = uv_to_cct_ohno(u, v);
      EXPECT_NEAR(r.duv, duv, 1e-3) << "issue #109: table-end row " << row << " T=" << t << " Duv_in=" << duv;
      EXPECT_GE(r.cct, 24800.0f) << "issue #109: table-end CCT clamped to 25000 for Duv_in=" << duv;
      EXPECT_LE(r.cct, 25000.0f);
    }
  }
}

// (2b) Pin: the parabolic branch stays ACTIVE for non-degenerate mid-table
// rows (pass-immediately pin of current Ohno 2014 behavior — the guards must
// not disable the parabola everywhere). Row ~19792 has ~190 K spacing on both
// sides; at Duv=+0.05 the parabola returns 0.050512 where the triangular
// fallback alone would give ~0.05000 (5.1e-4 apart).
TEST(OhnoTest, ParabolicBranchActiveAtLargeDuvOnNormalRows) {
  using namespace ohno_test_detail;
  const planckian_table table;
  // Row nearest 19792 K (deterministic table build: geometric temperatures).
  std::size_t row = 0;
  float best = 1e9f;
  for (std::size_t i = 1; i + 1 < table.size(); ++i) {
    const float d = std::abs(table.temperature(i) - 19791.6f);
    if (d < best) {
      best = d;
      row = i;
    }
  }
  const float t = table.temperature(row);
  EXPECT_NEAR(t, 19791.6f, 1.0f);
  const auto uv_ref = PlanckUvReference(t);
  const auto n = LocusNormalReference(t);
  const float u = static_cast<float>(uv_ref.first - 0.05 * n.second);
  const float v = static_cast<float>(uv_ref.second + 0.05 * n.first);
  const cct_duv r = uv_to_cct_ohno(u, v);
  EXPECT_NEAR(r.duv, 0.0505120f, 1e-4f)
      << "issue #109: parabola must remain the active branch when rows are well spaced";
  EXPECT_NEAR(r.cct, 19805.6f, 5.0f);
}

}  // namespace colorcpp::algorithms::test
