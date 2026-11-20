/**
 * @file test_appearance_cam16.cpp
 * @brief Reference-vector tests for the CAM16 colour appearance model.
 *
 * Reference vectors are the colour-science project's CAM16 doctests
 * (colour/appearance/tests/test_cam16.py, BSD-3-Clause), whose inputs use
 * the Y = 100 XYZ scale: converted here to this library's Y = 1 relative
 * scale. L_A stays in cd/m^2 and Y_b becomes a 0-1 factor.
 */

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <colorcpp/algorithms/appearance.hpp>

namespace colorcpp::algorithms::test {

using colorcpp::algorithms::appearance::surround;
using colorcpp::algorithms::appearance::viewing_conditions;
using colorcpp::algorithms::appearance::xyz_to_cam16;
using colorcpp::core::xyz_t;

namespace {

// xyz_t's constructor is explicit, so aggregate tables go through a helper.
constexpr xyz_t XYZ(float x, float y, float z) { return xyz_t(x, y, z); }

struct Cam16Sample {
  xyz_t xyz;
  xyz_t white;
  float la;  // adapted luminance, cd/m^2 (absolute)
  float yb;  // background luminance factor, 0-1
  float J, C, h, s, Q, M, H;
};

// Half-tolerance helpers: the model runs in double but the matrix and the
// stored expectations are single-precision, so scale the bound with the
// magnitude of the expected value (case 5 has C = 458).
void expect_near_scaled(float actual, float expected, float abs_floor, float rel) {
  EXPECT_NEAR(actual, expected, std::max(abs_floor, std::abs(expected) * rel))
      << "expected=" << expected << " actual=" << actual;
}

const Cam16Sample kRefs[] = {
    // colour-science test_XYZ_to_CAM16 cases 1-5.
    {XYZ(0.1901f, 0.2000f, 0.2178f), XYZ(0.9505f, 1.0000f, 1.0888f), 318.31f, 0.20f, 41.73120791f, 0.10335574f,
     217.06795977f, 2.34501507f, 195.37170899f, 0.10743677f, 275.59498615f},
    {XYZ(0.5706f, 0.4306f, 0.3196f), XYZ(0.9505f, 1.0000f, 1.0888f), 31.83f, 0.20f, 65.42828069f, 49.67956420f,
     17.48659243f, 52.94308868f, 152.06985268f, 42.62473321f, 398.03047943f},
    {XYZ(0.0353f, 0.0656f, 0.0214f), XYZ(1.0985f, 1.0000f, 0.3558f), 318.31f, 0.20f, 21.36052893f, 50.99381895f,
     178.86724266f, 61.57953092f, 139.78582768f, 53.00732582f, 223.01823806f},
    {XYZ(0.1901f, 0.2000f, 0.2178f), XYZ(1.0985f, 1.0000f, 0.3558f), 318.31f, 0.20f, 41.36326063f, 52.81154022f,
     258.88676291f, 53.12406914f, 194.52011798f, 54.89682038f, 311.24768647f},
    {XYZ(0.6145276998f, 0.0700421901f, 0.822406738f), XYZ(0.9505f, 1.0000f, 1.0888f), 4.074366543152521f, 0.20f,
     21.03801957f, 457.78881613f, 350.06445098f, 241.50642846f, 56.74143988f, 330.94646237f, 376.43915877f},
};

}  // namespace

TEST(Cam16, ForwardReferenceVectors) {
  for (const auto& r : kRefs) {
    const auto vc = viewing_conditions::create(r.white, r.la, r.yb, surround::average);
    const auto m = xyz_to_cam16(r.xyz, vc);
    // Measured double-vs-reference maxima: J 8e-6, C 9e-6, h 6e-4, s 1e-4,
    // Q 0, M 9e-6, H 8e-4 — bounds sit ~10x above to absorb libm drift.
    expect_near_scaled(m.J, r.J, 1e-4f, 1e-5f);
    expect_near_scaled(m.C, r.C, 1e-4f, 1e-5f);
    expect_near_scaled(m.h, r.h, 0.01f, 0.0f);
    expect_near_scaled(m.s, r.s, 0.01f, 1e-5f);
    expect_near_scaled(m.Q, r.Q, 0.01f, 1e-5f);
    expect_near_scaled(m.M, r.M, 1e-4f, 1e-5f);
    expect_near_scaled(m.H, r.H, 0.01f, 0.0f);
  }
}

// Hue quadrature nodes and wrap anchors (colour-science HUE_DATA_FOR_HUE_QUADRATURE,
// shared with CIECAM02). The five reference vectors exercise the wrap bin and
// the green/blue/red bins; these anchors pin the yellow-region node and the
// bin boundaries those vectors never reach.
TEST(Cam16, HueQuadratureAnchors) {
  using colorcpp::algorithms::appearance::cam16_hue_quadrature;
  // H lives on a 400 ring: at the red node the wrap bin yields 400 and the
  // first bin yields 0 — the same hue quadrature, so compare modulo 400.
  EXPECT_NEAR(std::fmod(cam16_hue_quadrature(20.14f), 400.0f), 0.0f, 1e-3f);
  EXPECT_NEAR(cam16_hue_quadrature(90.0f), 100.0f, 1e-3f);
  EXPECT_NEAR(cam16_hue_quadrature(164.25f), 200.0f, 1e-3f);
  EXPECT_NEAR(cam16_hue_quadrature(237.53f), 300.0f, 1e-3f);
  EXPECT_NEAR(cam16_hue_quadrature(0.0f), 385.9f, 1e-3f);
  // Continuity across the 360/0 seam: H(360-) = 385.9 = H(0).
  EXPECT_NEAR(cam16_hue_quadrature(359.999f), 385.9f, 0.01f);
  // Monotonic across a full turn modulo the 400 wrap.
  EXPECT_LT(cam16_hue_quadrature(45.0f), cam16_hue_quadrature(120.0f));
  EXPECT_LT(cam16_hue_quadrature(120.0f), cam16_hue_quadrature(300.0f));
}

// Every input channel keeps its own colour: a pure red XYZ under the standard
// conditions must land in the red hue region (h near 20-40 deg region per the
// nodes), not collapse to gray (C > 0).
TEST(Cam16, SaturatedInputKeepsChroma) {
  const xyz_t white{0.9505f, 1.0f, 1.0888f};
  const auto vc = viewing_conditions::create(white, 318.31f, 0.20f, surround::average);
  // sRGB red in XYZ (D65).
  const xyz_t red = XYZ(0.4124f, 0.2126f, 0.0193f);
  const auto m = xyz_to_cam16(red, vc);
  EXPECT_GT(m.C, 40.0f);
  EXPECT_GT(m.J, 30.0f);
  EXPECT_LT(m.J, 80.0f);
}

}  // namespace colorcpp::algorithms::test
