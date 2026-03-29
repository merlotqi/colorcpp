/**
 * @file test_compare.cpp
 * @brief Unit tests for color comparison module.
 */

#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

namespace colorcpp::operations::test {

using namespace core;
using namespace compare;

// ============================================================================
// Operator comparison tests
// ============================================================================

TEST(OperatorCompareTest, SameColorsAreEqual) {
  rgb8_t a{128, 64, 32};
  rgb8_t b{128, 64, 32};
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);
}

TEST(OperatorCompareTest, DifferentColorsAreNotEqual) {
  rgb8_t a{128, 64, 32};
  rgb8_t b{128, 64, 33};
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
}

// ============================================================================
// Exact comparison tests
// ============================================================================

TEST(ExactCompareTest, SameColorsAreEqual) {
  rgb8_t a{128, 64, 32};
  rgb8_t b{128, 64, 32};
  EXPECT_TRUE(exact_equal(a, b));
  EXPECT_TRUE(equal(a, b));
  EXPECT_TRUE(a == b);
}

TEST(ExactCompareTest, DifferentColorsAreNotEqual) {
  rgb8_t a{128, 64, 32};
  rgb8_t b{128, 64, 33};
  EXPECT_FALSE(exact_equal(a, b));
  EXPECT_FALSE(equal(a, b));
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
}

TEST(ExactCompareTest, FloatColorsExact) {
  rgbf_t a{0.5f, 0.3f, 0.7f};
  rgbf_t b{0.5f, 0.3f, 0.7f};
  EXPECT_TRUE(exact_equal(a, b));
}

TEST(ExactCompareTest, FloatColorsSlightDifference) {
  rgbf_t a{0.5f, 0.3f, 0.7f};
  rgbf_t b{0.5000001f, 0.3f, 0.7f};
  EXPECT_FALSE(exact_equal(a, b));
}

// ============================================================================
// Epsilon comparison tests
// ============================================================================

TEST(EpsilonCompareTest, WithinEpsilon) {
  rgbf_t a{0.5f, 0.3f, 0.7f};
  rgbf_t b{0.5000001f, 0.3f, 0.7f};
  EXPECT_TRUE(epsilon_equal(a, b, 0.001f));
  EXPECT_TRUE((equal<epsilon_policy>(a, b, 0.001f)));
}

TEST(EpsilonCompareTest, OutsideEpsilon) {
  rgbf_t a{0.5f, 0.3f, 0.7f};
  rgbf_t b{0.51f, 0.3f, 0.7f};
  EXPECT_FALSE(epsilon_equal(a, b, 0.001f));
}

TEST(EpsilonCompareTest, IntegerColors) {
  rgb8_t a{100, 50, 200};
  rgb8_t b{101, 50, 200};
  EXPECT_TRUE(epsilon_equal(a, b, 1));
  EXPECT_FALSE(epsilon_equal(a, b, 0));
}

// ============================================================================
// Relative comparison tests
// ============================================================================

TEST(RelativeCompareTest, WithinRelative) {
  rgbf_t a{0.5f, 0.3f, 0.8f};
  rgbf_t b{0.505f, 0.3f, 0.8f};
  EXPECT_TRUE(relative_equal(a, b, 0.02f));
  EXPECT_TRUE((equal<relative_policy>(a, b, 0.02f)));
}

TEST(RelativeCompareTest, OutsideRelative) {
  rgbf_t a{0.5f, 0.3f, 0.8f};
  rgbf_t b{0.55f, 0.3f, 0.8f};
  EXPECT_FALSE(relative_equal(a, b, 0.001f));
}

// ============================================================================
// Perceptual comparison tests
// ============================================================================

TEST(PerceptualCompareTest, IdenticalColors) {
  rgb8_t a{128, 64, 32};
  rgb8_t b{128, 64, 32};
  EXPECT_TRUE(perceptual_equal(a, b, 0.01f));
  EXPECT_TRUE((equal<perceptual_policy>(a, b, 0.01f)));
}

TEST(PerceptualCompareTest, SimilarColors) {
  rgb8_t a{128, 64, 32};
  rgb8_t b{130, 65, 33};
  EXPECT_TRUE(perceptual_equal(a, b, 5.0f));
}

TEST(PerceptualCompareTest, DifferentColors) {
  rgb8_t a{255, 0, 0};
  rgb8_t b{0, 255, 0};
  EXPECT_FALSE(perceptual_equal(a, b, 1.0f));
}

// ============================================================================
// Cross-type tests
// ============================================================================

TEST(CompareCrossTypeTest, Rgb8VsRgbaf) {
  rgb8_t a{128, 64, 32};
  rgbaf_t b{128.0f / 255.0f, 64.0f / 255.0f, 32.0f / 255.0f, 1.0f};
  // Different types - should use conversion
  auto b_rgb8 = conversion::color_cast<rgb8_t>(b);
  EXPECT_TRUE(exact_equal(a, b_rgb8));
}

// ============================================================================
// Constexpr tests
// ============================================================================

TEST(ConstexprCompareTest, ExactConstexpr) {
  constexpr rgb8_t a{100, 50, 200};
  constexpr rgb8_t b{100, 50, 200};
  static_assert(exact_equal(a, b), "Should be constexpr");
}

TEST(ConstexprCompareTest, EpsilonConstexpr) {
  constexpr rgbf_t a{0.5f, 0.3f, 0.7f};
  constexpr rgbf_t b{0.5f, 0.3f, 0.7f};
  static_assert(epsilon_equal(a, b, 0.001f), "Should be constexpr");
}

}  // namespace colorcpp::operations::test
