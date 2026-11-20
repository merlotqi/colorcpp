/**
 * @file test_appearance_cam16_ucs.cpp
 * @brief Reference-vector tests for CAM16-UCS (J\'a\'b\') and its delta E.
 *
 * The coordinate expectations are derived from colour-science's
 * XYZ_to_UCS_Li2017 doctest (BSD-3-Clause): that test pins J\'a\'b\' with
 * the CAM02-LCD coefficients (c1 = 0.007, c2 = 0.0053); the published
 * inverses recover (J, M, h) exactly, and re-running the Luo 2006
 * transform with the CAM16-UCS coefficients (c2 = 0.0228) yields the
 * expectations below. J\' is unchanged because both coefficient sets
 * share c1 = 0.007.
 */

#include <gtest/gtest.h>

#include <cmath>
#include <colorcpp/algorithms/appearance.hpp>

namespace colorcpp::algorithms::test {

using colorcpp::algorithms::appearance::cam16_to_ucs;
using colorcpp::algorithms::appearance::cam16_to_xyz;
using colorcpp::algorithms::appearance::delta_e_cam16_ucs;
using colorcpp::algorithms::appearance::surround;
using colorcpp::algorithms::appearance::ucs_to_cam16_jmh;
using colorcpp::algorithms::appearance::viewing_conditions;
using colorcpp::algorithms::appearance::xyz_to_cam16;
using colorcpp::core::xyz_t;

namespace {

constexpr xyz_t XYZ(float x, float y, float z) { return xyz_t(x, y, z); }

// colour-science XYZ_to_UCS_Li2017 input with the explicit D65 whitepoint.
const xyz_t kXyz = XYZ(0.20654008f, 0.12197225f, 0.05136952f);
const xyz_t kWhite = XYZ(0.95047f, 1.0f, 1.08883f);
// IEC 61966-2-1 defaults used by colour-science: 64/PI * 0.2 cd/m^2, Y_b = 20%.
constexpr float kLa = 4.074366543152521f;

}  // namespace

TEST(Cam16Ucs, ForwardReference) {
  const auto vc = viewing_conditions::create(kWhite, kLa, 0.20f, surround::average);
  const auto spec = xyz_to_cam16(kXyz, vc);

  // (J, M, h) recovered from colour-science's LCD-space J\'a\'b\' via the
  // published inverses — an independent check of the forward transform too.
  EXPECT_NEAR(spec.J, 33.44058343f, 1e-3f);
  EXPECT_NEAR(spec.M, 49.00041080f, 1e-3f);
  EXPECT_NEAR(spec.h, 19.45394390f, 0.01f);

  const auto ucs = cam16_to_ucs(spec);
  EXPECT_NEAR(ucs.Jp, 46.06573617f, 1e-3f);
  EXPECT_NEAR(ucs.ap, 31.02084967f, 1e-3f);
  EXPECT_NEAR(ucs.bp, 10.95700456f, 1e-3f);
}

TEST(Cam16Ucs, InverseRoundTrip) {
  const auto vc = viewing_conditions::create(kWhite, kLa, 0.20f, surround::average);
  const auto spec = xyz_to_cam16(kXyz, vc);
  const auto jmh = ucs_to_cam16_jmh(cam16_to_ucs(spec));
  EXPECT_NEAR(jmh.J, spec.J, 1e-3f);
  EXPECT_NEAR(jmh.M, spec.M, 1e-3f);
  EXPECT_NEAR(jmh.h, spec.h, 0.01f);
}

TEST(Cam16Ucs, DeltaE) {
  const colorcpp::algorithms::appearance::cam16_ucs a{46.06573617f, 31.02084967f, 10.95700456f};
  EXPECT_FLOAT_EQ(delta_e_cam16_ucs(a, a), 0.0f);
  const colorcpp::algorithms::appearance::cam16_ucs origin{0.0f, 0.0f, 0.0f};
  EXPECT_NEAR(delta_e_cam16_ucs(a, origin), 56.607430f, 0.01f);
  // Euclidean: one unit of J\' alone.
  const colorcpp::algorithms::appearance::cam16_ucs one{1.0f, 0.0f, 0.0f};
  EXPECT_NEAR(delta_e_cam16_ucs(one, origin), 1.0f, 1e-6f);
}

// cam16_to_xyz is exercised indirectly through the round trip; keep one
// direct XYZ-level assertion so the inverse cannot silently diverge from
// the forward while the UCS algebra still round-trips.
TEST(Cam16Ucs, ForwardInverseXyzRoundTrip) {
  const auto vc = viewing_conditions::create(kWhite, kLa, 0.20f, surround::average);
  const auto spec = xyz_to_cam16(kXyz, vc);
  const auto back = cam16_to_xyz(spec.J, spec.C, spec.h, vc);
  EXPECT_NEAR(back.x(), kXyz.x(), 1e-4f);
  EXPECT_NEAR(back.y(), kXyz.y(), 1e-4f);
  EXPECT_NEAR(back.z(), kXyz.z(), 1e-4f);
}

}  // namespace colorcpp::algorithms::test
