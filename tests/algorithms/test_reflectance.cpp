/**
 * @file test_reflectance.cpp
 * @brief Reference tests for reflectance integration and Y normalization.
 *
 * Anchors: a perfect reflecting diffuser under illuminant A gives the
 * published CIE tristimulus values (109.850, 100, 35.585); under D65 it
 * reproduces the library's WHITEPOINT_D65; under equal-energy illuminant E
 * it gives (100.008, 100, 100.033). A rectangular band reflectance checks
 * the double integral, and scaling the illuminant checks that k = 100/Y(S)
 * removes the absolute magnitude.
 */

#include <gtest/gtest.h>

#include <cstddef>
#include <utility>
#include <vector>

#include <colorcpp/algorithms/spectral.hpp>

namespace colorcpp::algorithms::test {

using colorcpp::algorithms::spectral::observer;
using colorcpp::algorithms::spectral::reflectance_to_xyz;
using colorcpp::algorithms::spectral::spd;
namespace illuminants = colorcpp::algorithms::spectral::illuminants;

namespace {

constexpr std::size_t kCmfSamples = 95;  // 360..830 nm, 5 nm

// Flat spectrum over the full CMF grid.
spd flat(float value) {
  return spd(360.0f, 5.0f, std::vector<float>(kCmfSamples, value));
}

// Reflectance 1 on [500, 600] nm with 5 nm ramps, 0 elsewhere.
spd band() {
  std::vector<float> values(kCmfSamples, 0.0f);
  for (std::size_t i = 0; i < values.size(); ++i) {
    const float nm = 360.0f + 5.0f * static_cast<float>(i);
    if (nm >= 500.0f && nm <= 600.0f) {
      values[i] = 1.0f;
    }
  }
  return spd(360.0f, 5.0f, std::move(values));
}

spd scaled(const spd& s, float factor) {
  std::vector<float> values = s.values;
  for (float& v : values) {
    v *= factor;
  }
  return spd(s.start_nm, s.step_nm, std::move(values));
}

}  // namespace

TEST(Reflectance, PerfectDiffuserUnderA) {
  const auto xyz = reflectance_to_xyz(flat(1.0f), illuminants::a());
  EXPECT_NEAR(xyz.x(), 109.850f, 1e-3f);
  EXPECT_NEAR(xyz.y(), 100.0f, 1e-3f);
  EXPECT_NEAR(xyz.z(), 35.585f, 1e-3f);
}

TEST(Reflectance, PerfectDiffuserUnderD65) {
  const auto xyz = reflectance_to_xyz(flat(1.0f), illuminants::d65());
  EXPECT_NEAR(xyz.y(), 100.0f, 1e-4f);
  EXPECT_NEAR(xyz.x() / 100.0f, 0.95047f, 1e-3f);
  EXPECT_NEAR(xyz.z() / 100.0f, 1.08883f, 1e-3f);
}

TEST(Reflectance, PerfectDiffuserUnderEqualEnergy) {
  const auto xyz = reflectance_to_xyz(flat(1.0f), flat(1.0f));
  EXPECT_NEAR(xyz.x(), 100.0078f, 1e-2f);
  EXPECT_NEAR(xyz.y(), 100.0f, 1e-4f);
  EXPECT_NEAR(xyz.z(), 100.0325f, 1e-2f);
}

TEST(Reflectance, GreyScalesLinearly) {
  const auto white = reflectance_to_xyz(flat(1.0f), illuminants::a());
  const auto grey = reflectance_to_xyz(flat(0.5f), illuminants::a());
  EXPECT_NEAR(grey.x(), 0.5f * white.x(), 1e-4f);
  EXPECT_NEAR(grey.y(), 0.5f * white.y(), 1e-4f);
  EXPECT_NEAR(grey.z(), 0.5f * white.z(), 1e-4f);
}

TEST(Reflectance, BandUnderEqualEnergy) {
  const auto xyz = reflectance_to_xyz(band(), flat(1.0f));
  EXPECT_NEAR(xyz.x(), 47.3671f, 1e-2f);
  EXPECT_NEAR(xyz.y(), 77.9593f, 1e-2f);
  EXPECT_NEAR(xyz.z(), 4.80268f, 1e-2f);
}

TEST(Reflectance, NormalizationRemovesIlluminantScale) {
  const auto one = reflectance_to_xyz(band(), illuminants::a());
  const auto two = reflectance_to_xyz(band(), scaled(illuminants::a(), 2.0f));
  EXPECT_NEAR(two.x(), one.x(), 1e-4f);
  EXPECT_NEAR(two.y(), one.y(), 1e-4f);
  EXPECT_NEAR(two.z(), one.z(), 1e-4f);
}

TEST(Reflectance, ZeroIlluminantIsBlack) {
  const auto xyz = reflectance_to_xyz(flat(1.0f), flat(0.0f));
  EXPECT_EQ(xyz.x(), 0.0f);
  EXPECT_EQ(xyz.y(), 0.0f);
  EXPECT_EQ(xyz.z(), 0.0f);
}

TEST(Reflectance, WideFieldObserver) {
  const auto xyz = reflectance_to_xyz(flat(1.0f), illuminants::d65(), observer::cie_1964_10deg);
  EXPECT_NEAR(xyz.y(), 100.0f, 1e-4f);
  EXPECT_NEAR(xyz.x() / 100.0f, 0.9481201f, 2e-3f);
  EXPECT_NEAR(xyz.z() / 100.0f, 1.0732439f, 2e-3f);
}

}  // namespace colorcpp::algorithms::test
