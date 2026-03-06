#include <gtest/gtest.h>
#include <colorcpp/colorcpp.hpp>
#include "colorcpp/core/rgb.hpp"

TEST(core_rgba, rgba8)
{
  using namespace color::core;

  rgba8_t color = make_rgba8(12, 12, 13);
  ASSERT_EQ(color.r, 12);
  ASSERT_EQ(color.g, 12);
  ASSERT_EQ(color.b, 13);
  ASSERT_EQ(color.a, 255);

}

TEST(color_rgba, rgbaf)
{
  using namespace color::core;

  rgba_float_t color = make_rgbaf(0.1, 0.1, 0.2);
  ASSERT_EQ(color.r, 0.1);
  ASSERT_EQ(color.g, 0.1);
  ASSERT_EQ(color.b, 0.2);
  ASSERT_EQ(color.a, 1.0f);
}
