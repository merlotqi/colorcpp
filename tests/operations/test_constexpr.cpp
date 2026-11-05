/**
 * @file test_constexpr.cpp
 * @brief Constexpr portability of gamma/cbrt conversion paths (issue #75).
 *
 * std::pow / std::cbrt are not constexpr in C++17/20 (P0533R3 is C++26).
 * GCC folds them as builtins during constant evaluation; Clang/MSVC do not,
 * so `constexpr color_cast` through the Adobe gamma and CIELAB/CIELUV cbrt
 * paths failed to compile there. The conversion functions now branch on
 * is_constant_evaluated(): runtime keeps std::pow/std::cbrt (libm speed and
 * semantics), constant evaluation uses constexpr-safe shims.
 *
 * Two layers of coverage:
 *  - Runtime sweep: each shim is compared against its libm reference across
 *    edge and interior samples — the mutation anchor for the shims.
 *  - static_assert: the actual constant-evaluation claim; passes on every
 *    compiler that provides __builtin_is_constant_evaluated (GCC 9+,
 *    Clang 8+, MSVC 2019 16.4+), which the CI matrix exercises.
 */

#include <gtest/gtest.h>

#include <cmath>
#include <colorcpp/colorcpp.hpp>
#include <limits>

using namespace colorcpp;
using namespace colorcpp::operations::conversion;

// ============================================================
// Runtime equivalence: shims must track libm to float precision
// ============================================================

TEST(ConstexprShimTest, CbrtMatchesStdLibm) {
  const float samples[] = {0.0f,   std::numeric_limits<float>::min(),
                           1e-38f, 1e-6f,
                           0.001f, 0.01f,
                           0.1f,   0.25f,
                           0.5f,   0.75f,
                           1.0f,   1.5f,
                           2.0f,   8.0f,
                           27.0f,  1000.0f,
                           1e10f,  1e20f};
  for (const float v : samples) {
    const float expect = std::cbrt(v);
    EXPECT_NEAR(details::constexpr_cbrt(v), expect, std::fabs(expect) * 1e-5f + 1e-6f) << "v=" << v;
  }
  // cbrt is defined on negatives (odd function), unlike sqrt — no domain error
  EXPECT_NEAR(details::constexpr_cbrt(-8.0f), -2.0f, 1e-6f);
  EXPECT_TRUE(std::isnan(details::constexpr_cbrt(std::numeric_limits<float>::quiet_NaN())));
}

TEST(ConstexprShimTest, AdobeLinearizeMatchesStdPow) {
  // v^(563/256), spec exponent (issue #68)
  const float samples[] = {0.0f, 1e-6f, 0.001f, 0.01f, 0.05f, 0.1f,  0.2f, 0.25f, 0.4f,
                           0.5f, 0.6f,  0.75f,  0.9f,  0.95f, 0.99f, 1.0f, 1.5f,  2.0f};
  for (const float v : samples) {
    const float expect = std::pow(v, 563.0f / 256.0f);
    EXPECT_NEAR(details::constexpr_adobe_linearize(v), expect, std::fabs(expect) * 1e-5f + 1e-6f) << "v=" << v;
  }
  EXPECT_TRUE(std::isnan(details::constexpr_adobe_linearize(-0.5f))) << "std::pow domain error is NaN";
}

TEST(ConstexprShimTest, AdobeGammaEncodeMatchesStdPow) {
  // v^(256/563), exact inverse exponent (issue #68); call site clamps to [0,1]
  const float samples[] = {0.0f, 1e-6f, 0.001f, 0.01f, 0.05f, 0.1f,  0.2f,  0.2159f, 0.25f,
                           0.4f, 0.5f,  0.6f,   0.75f, 0.9f,  0.95f, 0.99f, 1.0f};
  for (const float v : samples) {
    const float expect = std::pow(v, 256.0f / 563.0f);
    EXPECT_NEAR(details::constexpr_adobe_gamma_encode(v), expect, std::fabs(expect) * 1e-5f + 1e-6f) << "v=" << v;
  }
}

TEST(ConstexprShimTest, AdobeTransferRoundTripsNearIdentity) {
  const float samples[] = {0.0f, 0.01f, 0.1f, 0.25f, 0.5f, 0.75f, 0.9f, 1.0f};
  for (const float v : samples) {
    const float back = details::constexpr_adobe_gamma_encode(details::constexpr_adobe_linearize(v));
    EXPECT_NEAR(back, v, 1e-5f) << "v=" << v;
  }
}

// ============================================================
// Constant evaluation: the claim itself (issue #75)
// ============================================================

namespace {

// Adobe gamma: linearize then encode, evaluated at compile time.
constexpr auto kAdobeLinearized = color_cast<linear_adobe_rgbf_t>(adobe_rgbf_t(0.5f, 0.5f, 0.5f));
static_assert(kAdobeLinearized.r() > 0.21f && kAdobeLinearized.r() < 0.23f,
              "Adobe linearize (std::pow 563/256) must constant-evaluate");
static_assert(kAdobeLinearized.g() == kAdobeLinearized.r() && kAdobeLinearized.b() == kAdobeLinearized.r(),
              "channel symmetry through the transfer function");

constexpr auto kAdobeBack = color_cast<adobe_rgbf_t>(kAdobeLinearized);
static_assert(kAdobeBack.r() > 0.49f && kAdobeBack.r() < 0.51f,
              "Adobe encode (std::pow 256/563) must constant-evaluate");

// CIELAB / CIELUV cbrt paths. XYZ (0.5,0.5,0.5) is not achromatic
// (Xn ≠ Zn), so the neutral check uses white scaled to Y = 0.5.
constexpr auto kLab = color_cast<cielab_t>(xyz_t(0.5f, 0.5f, 0.5f));
static_assert(kLab.l() > 70.0f && kLab.l() < 80.0f, "CIELAB cbrt must constant-evaluate");

constexpr auto kLabGray = color_cast<cielab_t>(xyz_t(0.5f * 0.95047f, 0.5f, 0.5f * 1.08883f));
static_assert(kLabGray.l() > 70.0f && kLabGray.l() < 80.0f, "scaled-white L*");
static_assert(kLabGray.a() > -0.01f && kLabGray.a() < 0.01f && kLabGray.b() > -0.01f && kLabGray.b() < 0.01f,
              "scaled white stays achromatic");

constexpr auto kLuv = color_cast<luv_t>(xyz_t(0.5f, 0.5f, 0.5f));
static_assert(kLuv.l() > 70.0f && kLuv.l() < 80.0f, "CIELUV cbrt must constant-evaluate");

// OKLab cbrt path (same portable_cbrt).
constexpr auto kOklab = color_cast<oklab_t>(xyz_t(0.5f, 0.5f, 0.5f));
static_assert(kOklab.l() > 0.7f && kOklab.l() < 0.9f, "OKLab cbrt must constant-evaluate");

}  // namespace

TEST(ConstexprShimTest, StaticAssertsAreLive) {
  // The static_asserts above run at compile time on every CI compiler; this
  // test body only anchors the suite so the file registers as a test run.
  EXPECT_NEAR(kAdobeLinearized.r(), std::pow(0.5f, 563.0f / 256.0f), 1e-5f);
  EXPECT_NEAR(kLab.l(), 116.0f * std::cbrt(0.5f) - 16.0f, 1e-3f);
}
