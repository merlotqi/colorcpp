/**
 * @file test_spectral.cpp
 * @brief Reference tests for the spectral module: SPD sampling and
 *        trapezoidal spectrum integration against the CIE 1931 2° CMFs.
 *
 * Reference anchors from an independent double-precision evaluation:
 *   - constant-1 spectrum over the library's CMF grid (360-830 nm, 5 nm,
 *     dl in nm) gives the bare CMF integrals;
 *   - Planck's law at the CIE A temperature (2856.67 K) lands on the
 *     published CIE A chromaticity (0.44757, 0.40745) within the 5 nm
 *     sampling error;
 *   - illuminant A and D65 spectral distributions (colour-science
 *     datasets, 300-780 nm, 5 nm) integrate to the published CIE A
 *     tristimulus values (1.09850, 1.0, 0.35585) and, for D65, the
 *     library's own WHITEPOINT_D65 constant.
 */

#include <gtest/gtest.h>

#include <colorcpp/algorithms/chromatic_adaptation.hpp>
#include <colorcpp/algorithms/spectral.hpp>

namespace colorcpp::algorithms::test {

using colorcpp::algorithms::spectral::integrate_spectrum;
using colorcpp::algorithms::spectral::observer;
using colorcpp::algorithms::spectral::spd;
using colorcpp::algorithms::spectral::spd_to_xyz;

namespace {

// Illuminant A and D65 spectral distributions (colour-science
// colour/colorimetry/datasets/illuminants/sds.py, BSD-3-Clause):
// 300-780 nm at 5 nm.
constexpr float kIlluminantA[][2] = {
    {300.0f, 0.930483f}, {305.0f, 1.12821f}, {310.0f, 1.35769f}, {315.0f, 1.62219f}, {320.0f, 1.92508f},
    {325.0f, 2.2698f},   {330.0f, 2.65981f}, {335.0f, 3.09861f}, {340.0f, 3.58968f}, {345.0f, 4.13648f},
    {350.0f, 4.74238f},  {355.0f, 5.4107f},  {360.0f, 6.14462f}, {365.0f, 6.9472f},  {370.0f, 7.82135f},
    {375.0f, 8.7698f},   {380.0f, 9.7951f},  {385.0f, 10.8996f}, {390.0f, 12.0853f}, {395.0f, 13.3543f},
    {400.0f, 14.708f},   {405.0f, 16.148f},  {410.0f, 17.6753f}, {415.0f, 19.2907f}, {420.0f, 20.995f},
    {425.0f, 22.7883f},  {430.0f, 24.6709f}, {435.0f, 26.6425f}, {440.0f, 28.7027f}, {445.0f, 30.8508f},
    {450.0f, 33.0859f},  {455.0f, 35.4068f}, {460.0f, 37.8121f}, {465.0f, 40.3002f}, {470.0f, 42.8693f},
    {475.0f, 45.5174f},  {480.0f, 48.2423f}, {485.0f, 51.0418f}, {490.0f, 53.9132f}, {495.0f, 56.8539f},
    {500.0f, 59.8611f},  {505.0f, 62.932f},  {510.0f, 66.0635f}, {515.0f, 69.2525f}, {520.0f, 72.4959f},
    {525.0f, 75.7903f},  {530.0f, 79.1326f}, {535.0f, 82.5193f}, {540.0f, 85.947f},  {545.0f, 89.4124f},
    {550.0f, 92.912f},   {555.0f, 96.4423f}, {560.0f, 100.0f},   {565.0f, 103.582f}, {570.0f, 107.184f},
    {575.0f, 110.803f},  {580.0f, 114.436f}, {585.0f, 118.08f},  {590.0f, 121.731f}, {595.0f, 125.386f},
    {600.0f, 129.043f},  {605.0f, 132.697f}, {610.0f, 136.346f}, {615.0f, 139.988f}, {620.0f, 143.618f},
    {625.0f, 147.235f},  {630.0f, 150.836f}, {635.0f, 154.418f}, {640.0f, 157.979f}, {645.0f, 161.516f},
    {650.0f, 165.028f},  {655.0f, 168.51f},  {660.0f, 171.963f}, {665.0f, 175.383f}, {670.0f, 178.769f},
    {675.0f, 182.118f},  {680.0f, 185.429f}, {685.0f, 188.701f}, {690.0f, 191.931f}, {695.0f, 195.118f},
    {700.0f, 198.261f},  {705.0f, 201.359f}, {710.0f, 204.409f}, {715.0f, 207.411f}, {720.0f, 210.365f},
    {725.0f, 213.268f},  {730.0f, 216.12f},  {735.0f, 218.92f},  {740.0f, 221.667f}, {745.0f, 224.361f},
    {750.0f, 227.0f},    {755.0f, 229.585f}, {760.0f, 232.115f}, {765.0f, 234.589f}, {770.0f, 237.008f},
    {775.0f, 239.37f},   {780.0f, 241.675f},
};

constexpr float kIlluminantD65[][2] = {
    {300.0f, 0.0341f},  {305.0f, 1.6643f},  {310.0f, 3.2945f},  {315.0f, 11.7652f}, {320.0f, 20.236f},
    {325.0f, 28.6447f}, {330.0f, 37.0535f}, {335.0f, 38.5011f}, {340.0f, 39.9488f}, {345.0f, 42.4302f},
    {350.0f, 44.9117f}, {355.0f, 45.775f},  {360.0f, 46.6383f}, {365.0f, 49.3637f}, {370.0f, 52.0891f},
    {375.0f, 51.0323f}, {380.0f, 49.9755f}, {385.0f, 52.3118f}, {390.0f, 54.6482f}, {395.0f, 68.7015f},
    {400.0f, 82.7549f}, {405.0f, 87.1204f}, {410.0f, 91.486f},  {415.0f, 92.4589f}, {420.0f, 93.4318f},
    {425.0f, 90.057f},  {430.0f, 86.6823f}, {435.0f, 95.7736f}, {440.0f, 104.865f}, {445.0f, 110.936f},
    {450.0f, 117.008f}, {455.0f, 117.41f},  {460.0f, 117.812f}, {465.0f, 116.336f}, {470.0f, 114.861f},
    {475.0f, 115.392f}, {480.0f, 115.923f}, {485.0f, 112.367f}, {490.0f, 108.811f}, {495.0f, 109.082f},
    {500.0f, 109.354f}, {505.0f, 108.578f}, {510.0f, 107.802f}, {515.0f, 106.296f}, {520.0f, 104.79f},
    {525.0f, 106.239f}, {530.0f, 107.689f}, {535.0f, 106.047f}, {540.0f, 104.405f}, {545.0f, 104.225f},
    {550.0f, 104.046f}, {555.0f, 102.023f}, {560.0f, 100.0f},   {565.0f, 98.1671f}, {570.0f, 96.3342f},
    {575.0f, 96.0611f}, {580.0f, 95.788f},  {585.0f, 92.2368f}, {590.0f, 88.6856f}, {595.0f, 89.3459f},
    {600.0f, 90.0062f}, {605.0f, 89.8026f}, {610.0f, 89.5991f}, {615.0f, 88.6489f}, {620.0f, 87.6987f},
    {625.0f, 85.4936f}, {630.0f, 83.2886f}, {635.0f, 83.4939f}, {640.0f, 83.6992f}, {645.0f, 81.863f},
    {650.0f, 80.0268f}, {655.0f, 80.1207f}, {660.0f, 80.2146f}, {665.0f, 81.2462f}, {670.0f, 82.2778f},
    {675.0f, 80.281f},  {680.0f, 78.2842f}, {685.0f, 74.0027f}, {690.0f, 69.7213f}, {695.0f, 70.6652f},
    {700.0f, 71.6091f}, {705.0f, 72.979f},  {710.0f, 74.349f},  {715.0f, 67.9765f}, {720.0f, 61.604f},
    {725.0f, 65.7448f}, {730.0f, 69.8856f}, {735.0f, 72.4863f}, {740.0f, 75.087f},  {745.0f, 69.3398f},
    {750.0f, 63.5927f}, {755.0f, 55.0054f}, {760.0f, 46.4182f}, {765.0f, 56.6118f}, {770.0f, 66.8054f},
    {775.0f, 65.0941f}, {780.0f, 63.3828f},
};

spd from_table(const float (&table)[97][2]) {
  std::vector<float> values;
  values.reserve(97);
  for (const auto& row : table) values.push_back(row[1]);
  return spd(300.0f, 5.0f, std::move(values));
}

}  // namespace

TEST(Spectral, SpdSampling) {
  const spd s(400.0f, 10.0f, {0.0f, 1.0f, 2.0f, 3.0f});  // 400, 410, 420, 430 nm
  EXPECT_EQ(s.size(), 4u);
  EXPECT_FALSE(s.empty());
  EXPECT_FLOAT_EQ(s.nm_at(0), 400.0f);
  EXPECT_FLOAT_EQ(s.nm_at(3), 430.0f);

  EXPECT_FLOAT_EQ(s.sample(400.0f), 0.0f);
  EXPECT_FLOAT_EQ(s.sample(415.0f), 1.5f);  // linear interpolation
  EXPECT_FLOAT_EQ(s.sample(430.0f), 3.0f);
  // Outside the grid clamps to the edge values (np.interp semantics, which
  // is what the colour-science oracle does).
  EXPECT_FLOAT_EQ(s.sample(350.0f), 0.0f);
  EXPECT_FLOAT_EQ(s.sample(500.0f), 3.0f);
}

// A constant spectrum integrates to the bare CMF integrals (dl in nm).
TEST(Spectral, ConstantSpectrumIntegration) {
  const auto v = integrate_spectrum([](double) { return 1.0; });
  EXPECT_NEAR(v.x, 106.865372, 1e-4);
  EXPECT_NEAR(v.y, 106.857028, 1e-4);
  EXPECT_NEAR(v.z, 106.891806, 1e-4);
}

// Planck's law at the CIE A temperature lands on the published CIE A
// chromaticity (0.44757, 0.40745); the computed anchor below is the same
// integral in double precision on this library's 5 nm grid.
TEST(Spectral, PlanckSpectrumMatchesIlluminantAChromaticity) {
  const auto v = integrate_spectrum([](double nm) {
    const double lambda = nm * 1e-9;
    return 3.741771e-16 * std::pow(lambda, -5.0) / std::expm1(1.4388e-2 / (lambda * 2856.67));
  });
  const double sum = v.x + v.y + v.z;
  const double x = v.x / sum;
  const double y = v.y / sum;
  EXPECT_NEAR(x, 0.447487, 1e-5);
  EXPECT_NEAR(y, 0.407415, 1e-5);
  // Published CIE A chromaticity (agree to the 5 nm sampling error).
  EXPECT_NEAR(x, 0.44757, 2e-4);
  EXPECT_NEAR(y, 0.40745, 2e-4);
}

// Illuminant A integrates to the published CIE A tristimulus values
// (1.09850, 1.00000, 0.35585) after Y normalization.
TEST(Spectral, IlluminantAIntegration) {
  const auto raw = spd_to_xyz(from_table(kIlluminantA));
  ASSERT_GT(raw.y(), 0.0f);
  const float inv = 1.0f / raw.y();
  EXPECT_NEAR(raw.x() * inv, 1.0985017, 1e-4);
  EXPECT_NEAR(raw.z() * inv, 0.3558489, 1e-4);

  const double xd = static_cast<double>(raw.x()) * inv;
  const double yd = static_cast<double>(raw.y()) * inv;
  const double zd = static_cast<double>(raw.z()) * inv;
  const double sum = xd + yd + zd;
  EXPECT_NEAR(xd / sum, 0.447573, 1e-5);
  EXPECT_NEAR(yd / sum, 0.407440, 1e-5);
}

// The cross-module invariant: integrating D65 reproduces the library's own
// WHITEPOINT_D65 constant (they agree to the D65 dataset's rounding).
TEST(Spectral, D65MatchesLibraryWhitePoint) {
  const auto raw = spd_to_xyz(from_table(kIlluminantD65));
  ASSERT_GT(raw.y(), 0.0f);
  const float inv = 1.0f / raw.y();
  const auto& ref = colorcpp::algorithms::chromatic_adaptation::WHITEPOINT_D65;
  EXPECT_NEAR(raw.x() * inv, ref.x(), 2e-4);
  EXPECT_NEAR(raw.y() * inv, ref.y(), 1e-6f);
  EXPECT_NEAR(raw.z() * inv, ref.z(), 2e-4);
}

// The 1964 10° observer integrates differently from the 2° table: same
// constant spectrum, different CMF sums.
TEST(Spectral, Observer1964ConstantIntegration) {
  const auto v = integrate_spectrum([](double) { return 1.0; }, observer::cie_1964_10deg);
  EXPECT_NEAR(v.x, 116.647394, 1e-4);
  EXPECT_NEAR(v.y, 116.660381, 1e-4);
  EXPECT_NEAR(v.z, 116.671647, 1e-4);
}

// D65 under the 10° observer: the normalized result matches colour-science's
// published CIE illuminant tristimulus values for the 1964 observer
// (94.81, 100.00, 107.32) — the same cross-check P1 does for WHITEPOINT_D65.
TEST(Spectral, Observer1964D65WhitePoint) {
  const auto raw = spd_to_xyz(from_table(kIlluminantD65), observer::cie_1964_10deg);
  ASSERT_GT(raw.y(), 0.0f);
  const float inv = 1.0f / raw.y();
  EXPECT_NEAR(raw.x() * inv, 0.9481201, 1e-4);
  EXPECT_NEAR(raw.z() * inv, 1.0732439, 1e-4);

  // Published dataset (rounded to 0.01 in colour-science).
  EXPECT_NEAR(raw.x() * inv * 100.0f, 94.81f, 0.01f);
  EXPECT_NEAR(raw.z() * inv * 100.0f, 107.32f, 0.01f);

  const double xd = static_cast<double>(raw.x()) * inv;
  const double yd = static_cast<double>(raw.y()) * inv;
  const double zd = static_cast<double>(raw.z()) * inv;
  const double sum = xd + yd + zd;
  EXPECT_NEAR(xd / sum, 0.3138053, 1e-5);
  EXPECT_NEAR(yd / sum, 0.3309763, 1e-5);
}

// The two observers genuinely disagree on the same spectrum — the tables
// are not decorative aliases.
TEST(Spectral, ObserversDiffer) {
  const auto spd_d65 = from_table(kIlluminantD65);
  const auto r2 = spd_to_xyz(spd_d65, observer::cie_1931_2deg);
  const auto r10 = spd_to_xyz(spd_d65, observer::cie_1964_10deg);
  ASSERT_GT(r2.y(), 0.0f);
  ASSERT_GT(r10.y(), 0.0f);
  const float x2 = r2.x() / (r2.x() + r2.y() + r2.z());
  const float x10 = r10.x() / (r10.x() + r10.y() + r10.z());
  EXPECT_GT(std::abs(x2 - x10), 1e-3f);
}

}  // namespace colorcpp::algorithms::test
