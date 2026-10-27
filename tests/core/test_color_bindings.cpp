/**
 * @file test_color_bindings.cpp
 * @brief Tuple protocol (structured bindings) and ADL visibility of get / << / >>.
 */

#include <gtest/gtest.h>

#include <colorcpp/core/rgb.hpp>
#include <colorcpp/core/xyz.hpp>
#include <sstream>

namespace hostile {

// Declaring any operator<< in a user namespace hides old global-scope ones.
struct widget {};
std::ostream& operator<<(std::ostream& os, const widget&) { return os; }

TEST(ColorBindingAdlTest, StreamOperatorsAreFoundByAdlDespiteUserOverloads) {
  std::ostringstream os;
  const colorcpp::core::xyz_t c(0.5f, 0.6f, 0.7f);
  os << c;
  EXPECT_FALSE(os.fail());

  std::istringstream is("0.1 0.2 0.3");
  colorcpp::core::xyz_t d;
  is >> d;
  EXPECT_FALSE(is.fail());
  EXPECT_FLOAT_EQ(d.x(), 0.1f);
  EXPECT_FLOAT_EQ(d.y(), 0.2f);
  EXPECT_FLOAT_EQ(d.z(), 0.3f);
}

TEST(ColorBindingAdlTest, CoreGetIsAvailable) {
  colorcpp::core::rgbaf_t c(0.1f, 0.2f, 0.3f, 0.4f);
  EXPECT_FLOAT_EQ(colorcpp::core::get<2>(c), 0.3f);
  EXPECT_FLOAT_EQ(colorcpp::core::get<colorcpp::core::rgb::channel::r_tag>(c), 0.1f);
  colorcpp::core::get<0>(c) = 0.9f;
  EXPECT_FLOAT_EQ(c.r(), 0.9f);
}

}  // namespace hostile

TEST(ColorBindingTest, StructuredBindingsDecomposeDerivedTypes) {
  colorcpp::core::rgbaf_t c(0.1f, 0.2f, 0.3f, 0.4f);
  auto [r, g, b, a] = c;
  EXPECT_FLOAT_EQ(r, 0.1f);
  EXPECT_FLOAT_EQ(g, 0.2f);
  EXPECT_FLOAT_EQ(b, 0.3f);
  EXPECT_FLOAT_EQ(a, 0.4f);

  const colorcpp::core::xyz_t xyz(0.5f, 0.6f, 0.7f);
  const auto& [x, y, z] = xyz;
  EXPECT_FLOAT_EQ(x, 0.5f);
  EXPECT_FLOAT_EQ(y, 0.6f);
  EXPECT_FLOAT_EQ(z, 0.7f);
}

TEST(ColorBindingTest, StructuredBindingsExposeMutableReferences) {
  colorcpp::core::rgbaf_t c(0.1f, 0.2f, 0.3f, 0.4f);
  auto& [r, g, b, a] = c;
  g = 0.5f;
  EXPECT_FLOAT_EQ(c.g(), 0.5f);
}
