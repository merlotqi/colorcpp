/**
 * @file test_model_traits.cpp
 * @brief model_traits contract: the channel count is derived from channels_type alone.
 */

#include <gtest/gtest.h>

#include <colorcpp/core/cmyk.hpp>
#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/core/xyz.hpp>

namespace colorcpp::traits::test {

struct width_tag {};
struct height_tag {};

// Pure tag type: the description lives in its model_traits specialization.
struct model_pair {};

// Alpha at index 1 of 3 channels, no has_alpha member: alpha must be derived
// from the shared alpha_tag in channels_type alone.
struct model_alpha_mid {};

}  // namespace colorcpp::traits::test

namespace colorcpp::traits {

// Deliberately declares NO channel_size member: the channel count must be
// derivable from channels_type alone.
template <>
struct model_traits<test::model_pair> {
  using channels_type =
      std::tuple<basic_channel<test::width_tag, float, 0, 100, 1>, basic_channel<test::height_tag, float, -50, 50, 1>>;
  static constexpr std::string_view prefix = "pair";
};

// Deliberately declares NO has_alpha member.
template <>
struct model_traits<test::model_alpha_mid> {
  using channels_type =
      std::tuple<basic_channel<test::width_tag, float, 0, 100, 1>, basic_channel<alpha_tag, float, 0, 1, 1>,
                 basic_channel<test::height_tag, float, -50, 50, 1>>;
  static constexpr std::string_view prefix = "amid";
};

}  // namespace colorcpp::traits

namespace colorcpp::traits::test {

TEST(ModelTraitsTest, ChannelCountIsDerivedFromChannelsTuple) {
  static_assert(model_channel_count_v<model_pair> == 2);
  static_assert(model_channel_count_v<core::xyz::model::xyze> == 3);
  SUCCEED();
}

TEST(ModelTraitsTest, ModelWithoutChannelSizeMemberIsStillAModel) {
  static_assert(is_model_traits_v<model_pair>);
  static_assert(core::basic_color<model_pair>::channels == 2);

  core::basic_color<model_pair> c{};
  c.get_index<1>() = 25.0f;
  EXPECT_FLOAT_EQ(c.get_index<1>(), 25.0f);
}

TEST(ModelAlphaTest, AlphaIsDerivedFromSharedAlphaTag) {
  static_assert(model_has_alpha_v<model_alpha_mid>);
  static_assert(!model_has_alpha_v<model_pair>);
  SUCCEED();
}

TEST(ModelAlphaTest, AlphaIndexIsDerivedNotHardcoded) {
  static_assert(channel_index_v<model_alpha_mid, alpha_tag> == 1);
  SUCCEED();
}

TEST(ModelAlphaTest, BuiltInModelsFollowTheTag) {
  static_assert(model_has_alpha_v<core::rgb::model::rgba8>);
  static_assert(model_has_alpha_v<core::hsl::model::hsla>);
  static_assert(!model_has_alpha_v<core::rgb::model::rgb8>);
  static_assert(!model_has_alpha_v<core::cmyk::model::cmyk_float>);  // channel 3 is K
  static_assert(!model_has_alpha_v<core::oklab::model::oklab>);      // a is the green-red axis
  SUCCEED();
}

TEST(ModelAlphaTest, SharedTagAccessesAlphaOnAnyFamily) {
  core::rgba8_t c(1, 2, 3, 4);
  EXPECT_EQ((core::get<alpha_tag>(c)), 4);
  // Family-local a_tag names remain supported as aliases of the shared tag.
  EXPECT_EQ((core::get<core::rgb::channel::a_tag>(c)), 4);

  core::get<alpha_tag>(c) = 9;
  EXPECT_EQ(c.a(), 9);

  const core::hsla_float_t h(0.1f, 0.2f, 0.3f, 0.5f);
  EXPECT_FLOAT_EQ((core::get<alpha_tag>(h)), 0.5f);
}

TEST(ModelTraitsTest, DefaultDenominatorIsOne) {
  // basic_channel<Tag, T, Min, Max> must mean [Min, Max] — not [Min/Max, 1].
  using hue_like = basic_channel<width_tag, float, 0, 360>;
  static_assert(hue_like::min == 0.0f);
  static_assert(hue_like::max == 360.0f);

  using byte_like = basic_channel<height_tag, uint8_t, 0, 255>;
  static_assert(byte_like::min == 0);
  static_assert(byte_like::max == 255);
  SUCCEED();
}

}  // namespace colorcpp::traits::test
