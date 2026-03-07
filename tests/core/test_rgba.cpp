#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

TEST(core_rgba, rgba8) {
  using namespace colorcpp::core;
  
  auto rgb = rgb8_t{255, 0, 0};
  ASSERT_EQ(rgb.r(), 255);

  auto getr = rgb.get<rgb::channel::r_tag>();
  ASSERT_EQ(getr, 255);

  auto stdr = std::get<0>(rgb);
  ASSERT_EQ(stdr, 255);

  auto stdchannelr = std::get<rgb::channel::r_tag>(rgb);
  ASSERT_EQ(stdchannelr, 255);

  auto red = colorcpp::constants::red;
  ASSERT_EQ(red.r(), 255);
  ASSERT_EQ(red.g(), 0);
  ASSERT_EQ(red.b(), 0);
  ASSERT_EQ(red.a(), 255);
}
