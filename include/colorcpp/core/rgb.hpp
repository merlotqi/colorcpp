/**
 * @file rgb.hpp
 * @brief RGBA color space core implementation
 *
 * Provides basic RGBA color template class and common color aliases.
 * Supports 8-bit integer RGBA and floating-point RGBA representations.
 *
 * @author Merlot.Qi
 *
 */

#pragma once

#include <cassert>
#include <colorcpp/core/color_base.hpp>
#include <colorcpp/traits/concepts.hpp>
#include <cstdint>

namespace colorcpp::core::rgb {

namespace channel {
struct r_tag {};
struct g_tag {};
struct b_tag {};
struct a_tag {};

template <typename Tag, typename T, T Min, T Max>
struct packed_channel {
  using tag_type = Tag;
  using value_type = T;
  static constexpr T min = Min;
  static constexpr T max = Max;
};

using u8_r = packed_channel<r_tag, uint8_t, 0, 255>;
using u8_g = packed_channel<g_tag, uint8_t, 0, 255>;
using u8_b = packed_channel<b_tag, uint8_t, 0, 255>;
using u8_a = packed_channel<a_tag, uint8_t, 0, 255>;

using f32_r = packed_channel<r_tag, float, 0.0f, 1.0f>;
using f32_g = packed_channel<g_tag, float, 0.0f, 1.0f>;
using f32_b = packed_channel<b_tag, float, 0.0f, 1.0f>;
using f32_a = packed_channel<a_tag, float, 0.0f, 1.0f>;

}  // namespace channel

namespace model {
struct rgb8 {};
struct rgba8 {};
struct rgb_f32 {};
}  // namespace model

}  // namespace colorcpp::core::rgb

namespace colorcpp::traits {
namespace ch = colorcpp::core::rgb::channel;
namespace md = colorcpp::core::rgb::model;

template <>
struct model_traits<md::rgb8> {
  using channels_type = std::tuple<ch::u8_r, ch::u8_g, ch::u8_b, ch::u8_a>;
  static constexpr std::size_t channel_size = 4;
};

template <>
struct model_traits<md::rgb_f32> {
  using channels_type = std::tuple<ch::f32_r, ch::f32_g, ch::f32_b>;
  static constexpr std::size_t channel_size = 3;
};
}  // namespace colorcpp::traits

namespace colorcpp::core {

using rgb8_t = basic_color<rgb::model::rgb8>;
using rgbf_t = basic_color<rgb::model::rgb_f32>;

}  // namespace colorcpp::core
