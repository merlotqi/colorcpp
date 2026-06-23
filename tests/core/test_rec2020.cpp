/**
 * @file test_rec2020.cpp
 * @brief Tests for Rec.2020 (BT.2020) color model and conversion.
 *
 * Rec.2020 uses BT.709-style piecewise transfer function and D65 white point.
 */

#include <gtest/gtest.h>

#include <colorcpp/core/rec2020.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/io/css.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::core::test {

using namespace colorcpp::operations::conversion;

// --- Construction ---

TEST(Rec2020Test, DefaultConstruction) {
  rec2020_rgbf_t c;
  EXPECT_FLOAT_EQ(c.r(), 0.0f);
  EXPECT_FLOAT_EQ(c.g(), 0.0f);
  EXPECT_FLOAT_EQ(c.b(), 0.0f);
}

TEST(Rec2020Test, ParameterizedConstruction) {
  rec2020_rgbf_t c(0.2f, 0.4f, 0.6f);
  EXPECT_FLOAT_EQ(c.r(), 0.2f);
  EXPECT_FLOAT_EQ(c.g(), 0.4f);
  EXPECT_FLOAT_EQ(c.b(), 0.6f);
}

TEST(Rec2020ATest, ParameterizedWithAlpha) {
  rec2020_rgbaf_t c(0.2f, 0.4f, 0.6f, 0.8f);
  EXPECT_FLOAT_EQ(c.r(), 0.2f);
  EXPECT_FLOAT_EQ(c.g(), 0.4f);
  EXPECT_FLOAT_EQ(c.b(), 0.6f);
  EXPECT_FLOAT_EQ(c.a(), 0.8f);
}

// --- Boundary values ---

TEST(Rec2020Test, BoundaryValuesMin) {
  rec2020_rgbf_t c(0.0f, 0.0f, 0.0f);
  EXPECT_FLOAT_EQ(c.r(), 0.0f);
  EXPECT_FLOAT_EQ(c.g(), 0.0f);
  EXPECT_FLOAT_EQ(c.b(), 0.0f);
}

TEST(Rec2020Test, BoundaryValuesMax) {
  rec2020_rgbf_t c(1.0f, 1.0f, 1.0f);
  EXPECT_FLOAT_EQ(c.r(), 1.0f);
  EXPECT_FLOAT_EQ(c.g(), 1.0f);
  EXPECT_FLOAT_EQ(c.b(), 1.0f);
}

TEST(Rec2020Test, OutOfRangeThrows) {
  EXPECT_THROW(rec2020_rgbf_t(-0.1f, 0.5f, 0.5f), std::out_of_range);
  EXPECT_THROW(rec2020_rgbf_t(1.1f, 0.5f, 0.5f), std::out_of_range);
  EXPECT_THROW(rec2020_rgbf_t(0.5f, -0.1f, 0.5f), std::out_of_range);
  EXPECT_THROW(rec2020_rgbf_t(0.5f, 1.1f, 0.5f), std::out_of_range);
  EXPECT_THROW(rec2020_rgbf_t(0.5f, 0.5f, -0.1f), std::out_of_range);
  EXPECT_THROW(rec2020_rgbf_t(0.5f, 0.5f, 1.1f), std::out_of_range);
}

// --- Member and indexed access ---

TEST(Rec2020Test, MutableMemberAccess) {
  rec2020_rgbf_t c(0.1f, 0.2f, 0.3f);
  c.r() = 0.4f;
  c.g() = 0.5f;
  c.b() = 0.6f;
  EXPECT_FLOAT_EQ(c.r(), 0.4f);
  EXPECT_FLOAT_EQ(c.g(), 0.5f);
  EXPECT_FLOAT_EQ(c.b(), 0.6f);
}

TEST(Rec2020Test, IndexAccess) {
  rec2020_rgbf_t c(0.2f, 0.4f, 0.6f);
  EXPECT_FLOAT_EQ(c.get_index<0>(), 0.2f);
  EXPECT_FLOAT_EQ(c.get_index<1>(), 0.4f);
  EXPECT_FLOAT_EQ(c.get_index<2>(), 0.6f);
}

TEST(Rec2020Test, ConstCorrectness) {
  const rec2020_rgbf_t c(0.3f, 0.5f, 0.7f);
  EXPECT_FLOAT_EQ(c.r(), 0.3f);
  EXPECT_FLOAT_EQ(c.g(), 0.5f);
  EXPECT_FLOAT_EQ(c.b(), 0.7f);
}

// --- Copy and move semantics ---

TEST(Rec2020Test, CopyConstruction) {
  rec2020_rgbf_t original(0.2f, 0.4f, 0.6f);
  rec2020_rgbf_t copy(original);
  EXPECT_FLOAT_EQ(copy.r(), 0.2f);
  EXPECT_FLOAT_EQ(copy.g(), 0.4f);
  EXPECT_FLOAT_EQ(copy.b(), 0.6f);
}

TEST(Rec2020Test, CopyAssignment) {
  rec2020_rgbf_t original(0.2f, 0.4f, 0.6f);
  rec2020_rgbf_t copy;
  copy = original;
  EXPECT_FLOAT_EQ(copy.r(), 0.2f);
  EXPECT_FLOAT_EQ(copy.g(), 0.4f);
  EXPECT_FLOAT_EQ(copy.b(), 0.6f);
}

TEST(Rec2020Test, Equality) {
  rec2020_rgbf_t a(0.2f, 0.4f, 0.6f);
  rec2020_rgbf_t b(0.2f, 0.4f, 0.6f);
  rec2020_rgbf_t c(0.3f, 0.4f, 0.6f);
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

// --- Linear variants ---

TEST(LinearRec2020Test, Construction) {
  linear_rec2020_rgbf_t c(0.1f, 0.5f, 0.9f);
  EXPECT_FLOAT_EQ(c.r(), 0.1f);
  EXPECT_FLOAT_EQ(c.g(), 0.5f);
  EXPECT_FLOAT_EQ(c.b(), 0.9f);
}

TEST(LinearRec2020ATest, ConstructionWithAlpha) {
  linear_rec2020_rgbaf_t c(0.1f, 0.5f, 0.9f, 0.3f);
  EXPECT_FLOAT_EQ(c.r(), 0.1f);
  EXPECT_FLOAT_EQ(c.g(), 0.5f);
  EXPECT_FLOAT_EQ(c.b(), 0.9f);
  EXPECT_FLOAT_EQ(c.a(), 0.3f);
}

}  // namespace colorcpp::core::test
