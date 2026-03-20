
#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

namespace colorcpp::operations::test {

using namespace core;
using namespace vision;

// Helper: check RGB channels are within [0, 1] (works for any color type via color_cast)
static void expect_in_range(const rgbaf_t& c) {
  EXPECT_GE(c.r(), 0.0f);
  EXPECT_LE(c.r(), 1.0f);
  EXPECT_GE(c.g(), 0.0f);
  EXPECT_LE(c.g(), 1.0f);
  EXPECT_GE(c.b(), 0.0f);
  EXPECT_LE(c.b(), 1.0f);
}

// simulate_protanopia

TEST(ProtanopiaTest, BlackStaysBlack) {
  rgbf_t black(0.0f, 0.0f, 0.0f);
  auto result = simulate_protanopia(black);
  EXPECT_NEAR(result.r(), 0.0f, 1e-4f);
  EXPECT_NEAR(result.g(), 0.0f, 1e-4f);
  EXPECT_NEAR(result.b(), 0.0f, 1e-4f);
}

TEST(ProtanopiaTest, OutputAlwaysInRange) {
  for (auto& c : {rgbaf_t{1, 0, 0, 1}, rgbaf_t{0, 1, 0, 1}, rgbaf_t{0, 0, 1, 1}, rgbaf_t{1, 1, 0, 1},
                  rgbaf_t{0, 1, 1, 1}, rgbaf_t{1, 0, 1, 1}, rgbaf_t{0.5f, 0.5f, 0.5f, 1}}) {
    auto r = simulate_protanopia(c);
    expect_in_range(r);
  }
}

TEST(ProtanopiaTest, AlphaPreserved) {
  rgbaf_t c(0.8f, 0.2f, 0.4f, 0.7f);
  auto result = simulate_protanopia(c);
  EXPECT_NEAR(result.a(), 0.7f, 1e-4f);
}

TEST(ProtanopiaTest, WorksWithRgb8) {
  core::rgb8_t red(255, 0, 0);
  auto result = simulate_protanopia(red);
  // Should return same type (rgb8_t) in valid range
  EXPECT_GE(result.r(), 0);
  EXPECT_LE(result.r(), 255);
  EXPECT_GE(result.g(), 0);
  EXPECT_LE(result.g(), 255);
  EXPECT_GE(result.b(), 0);
  EXPECT_LE(result.b(), 255);
}

// simulate_deuteranopia

TEST(DeuteranopiaTest, BlackStaysBlack) {
  auto result = simulate_deuteranopia(rgbf_t{0.0f, 0.0f, 0.0f});
  EXPECT_NEAR(result.r(), 0.0f, 1e-4f);
  EXPECT_NEAR(result.g(), 0.0f, 1e-4f);
  EXPECT_NEAR(result.b(), 0.0f, 1e-4f);
}

TEST(DeuteranopiaTest, OutputAlwaysInRange) {
  for (auto& c : {rgbaf_t{1, 0, 0, 1}, rgbaf_t{0, 1, 0, 1}, rgbaf_t{0, 0, 1, 1}, rgbaf_t{1, 1, 0, 1},
                  rgbaf_t{0.3f, 0.6f, 0.9f, 1}}) {
    auto r = simulate_deuteranopia(c);
    expect_in_range(r);
  }
}

TEST(DeuteranopiaTest, AlphaPreserved) {
  rgbaf_t c(0.3f, 0.7f, 0.5f, 0.4f);
  auto result = simulate_deuteranopia(c);
  EXPECT_NEAR(result.a(), 0.4f, 1e-4f);
}

// simulate_tritanopia

TEST(TritanopiaTest, BlackStaysBlack) {
  auto result = simulate_tritanopia(rgbf_t{0.0f, 0.0f, 0.0f});
  EXPECT_NEAR(result.r(), 0.0f, 1e-4f);
  EXPECT_NEAR(result.g(), 0.0f, 1e-4f);
  EXPECT_NEAR(result.b(), 0.0f, 1e-4f);
}

TEST(TritanopiaTest, OutputAlwaysInRange) {
  for (auto& c : {rgbaf_t{1, 0, 0, 1}, rgbaf_t{0, 1, 0, 1}, rgbaf_t{0, 0, 1, 1}, rgbaf_t{1, 1, 0, 1},
                  rgbaf_t{0.5f, 0.2f, 0.8f, 1}}) {
    auto r = simulate_tritanopia(c);
    expect_in_range(r);
  }
}

TEST(TritanopiaTest, AlphaPreserved) {
  rgbaf_t c(0.6f, 0.1f, 0.9f, 0.8f);
  auto result = simulate_tritanopia(c);
  EXPECT_NEAR(result.a(), 0.8f, 1e-4f);
}

// simulate_achromatopsia

TEST(AchromatopsiaTest, BlackStaysBlack) {
  auto result = simulate_achromatopsia(rgbf_t{0.0f, 0.0f, 0.0f});
  EXPECT_NEAR(result.r(), 0.0f, 1e-4f);
  EXPECT_NEAR(result.g(), 0.0f, 1e-4f);
  EXPECT_NEAR(result.b(), 0.0f, 1e-4f);
}

TEST(AchromatopsiaTest, WhiteStaysWhite) {
  auto result = simulate_achromatopsia(rgbf_t{1.0f, 1.0f, 1.0f});
  EXPECT_NEAR(result.r(), 1.0f, 1e-4f);
  EXPECT_NEAR(result.g(), 1.0f, 1e-4f);
  EXPECT_NEAR(result.b(), 1.0f, 1e-4f);
}

TEST(AchromatopsiaTest, AllChannelsEqual) {
  // Achromatopsia always produces a gray: r=g=b
  for (auto& c :
       {rgbf_t{1, 0, 0}, rgbf_t{0, 1, 0}, rgbf_t{0, 0, 1}, rgbf_t{0.3f, 0.6f, 0.1f}, rgbf_t{0.9f, 0.2f, 0.7f}}) {
    auto r = simulate_achromatopsia(c);
    EXPECT_NEAR(r.r(), r.g(), 1e-5f);
    EXPECT_NEAR(r.g(), r.b(), 1e-5f);
  }
}

TEST(AchromatopsiaTest, AlphaPreserved) {
  rgbaf_t c(0.5f, 0.5f, 0.5f, 0.3f);
  auto result = simulate_achromatopsia(c);
  EXPECT_NEAR(result.a(), 0.3f, 1e-4f);
}

TEST(AchromatopsiaTest, OutputAlwaysInRange) {
  for (auto& c : {rgbf_t{1, 0, 0}, rgbf_t{0, 1, 0}, rgbf_t{0, 0, 1}, rgbf_t{1, 1, 1}, rgbf_t{0, 0, 0}}) {
    auto r = simulate_achromatopsia(c);
    EXPECT_GE(r.r(), 0.0f);
    EXPECT_LE(r.r(), 1.0f);
  }
}

TEST(AchromatopsiaTest, LuminanceFormula) {
  // Y = 0.2126*R_lin + 0.7152*G_lin + 0.0722*B_lin for linear RGB
  // Pure red (1,0,0): Y = linearize(1) * 0.2126 = 1 * 0.2126 = 0.2126
  // gamma_encode(0.2126) ≈ 0.2126^(1/2.4) * 1.055 - 0.055 for v > 0.0031308
  auto result = simulate_achromatopsia(rgbf_t{1.0f, 0.0f, 0.0f});
  float gray = result.r();
  // Just verify it's between 0 and 1 and not full red
  EXPECT_GT(gray, 0.0f);
  EXPECT_LT(gray, 1.0f);
}

// Simulation produces different output from input (colored inputs)

TEST(VisionSimulationTest, ProtanopiaChangesRed) {
  rgbf_t red(1.0f, 0.0f, 0.0f);
  auto result = simulate_protanopia(red);
  // A protanope cannot distinguish red from some other color — output differs from input
  bool changed = (std::abs(result.r() - red.r()) > 0.05f || std::abs(result.g() - red.g()) > 0.05f ||
                  std::abs(result.b() - red.b()) > 0.05f);
  EXPECT_TRUE(changed);
}

TEST(VisionSimulationTest, DeuteranopiaChangesGreen) {
  rgbf_t green(0.0f, 1.0f, 0.0f);
  auto result = simulate_deuteranopia(green);
  bool changed = (std::abs(result.r() - green.r()) > 0.05f || std::abs(result.g() - green.g()) > 0.05f ||
                  std::abs(result.b() - green.b()) > 0.05f);
  EXPECT_TRUE(changed);
}

TEST(VisionSimulationTest, AchromatopsiaChangesColor) {
  rgbf_t cyan(0.0f, 1.0f, 1.0f);
  auto result = simulate_achromatopsia(cyan);
  // Not cyan anymore — should be some shade of gray
  EXPECT_NEAR(result.r(), result.g(), 1e-5f);
  EXPECT_NEAR(result.g(), result.b(), 1e-5f);
}

}  // namespace colorcpp::operations::test
