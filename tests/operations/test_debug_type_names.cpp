/**
 * @file test_debug_type_names.cpp
 * @brief Tests for compile-time color type names used in conversion debugging.
 */

#include <gtest/gtest.h>

#include <colorcpp/core/jzazbz.hpp>
#include <colorcpp/core/luv.hpp>
#include <colorcpp/core/rec2020.hpp>
#include <colorcpp/core/xyy.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::operations::test {

using conversion::color_type_name_v;

// Regression: names that already exist must stay stable.
TEST(ColorTypeNameTest, ExistingNamesStayStable) {
  EXPECT_EQ(color_type_name_v<core::xyz_t>, "xyz");
  EXPECT_EQ(color_type_name_v<core::cielab_t>, "cielab");
  EXPECT_EQ(color_type_name_v<core::display_p3f_t>, "display_p3");
}

TEST(ColorTypeNameTest, XyyName) { EXPECT_EQ(color_type_name_v<core::xyy_t>, "xyy"); }

TEST(ColorTypeNameTest, LuvName) { EXPECT_EQ(color_type_name_v<core::luv_t>, "luv"); }

TEST(ColorTypeNameTest, JzazbzName) { EXPECT_EQ(color_type_name_v<core::jzazbz_t>, "jzazbz"); }

TEST(ColorTypeNameTest, Rec2020Names) {
  EXPECT_EQ(color_type_name_v<core::rec2020_rgbf_t>, "rec2020");
  EXPECT_EQ(color_type_name_v<core::rec2020_rgbaf_t>, "rec2020a");
  EXPECT_EQ(color_type_name_v<core::linear_rec2020_rgbf_t>, "linear_rec2020");
  EXPECT_EQ(color_type_name_v<core::linear_rec2020_rgbaf_t>, "linear_rec2020a");
}

TEST(ColorTypeNameTest, LinearDisplayP3Names) {
  EXPECT_EQ(color_type_name_v<core::linear_display_p3f_t>, "linear_display_p3");
  EXPECT_EQ(color_type_name_v<core::linear_display_p3af_t>, "linear_display_p3a");
}

}  // namespace colorcpp::operations::test
