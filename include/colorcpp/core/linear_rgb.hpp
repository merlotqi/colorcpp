#pragma once

#include <colorcpp/core/color_base.hpp>
#include <tuple>
#include <type_traits>

// Linear sRGB: same channel layout as rgbf/rgbaf but semantically represents
// scene-linear light values (no gamma encoding). Keeping it as a separate type
// (not just a tag on rgbf) ensures that color_cast inserts the correct
// linearize/delinearize step when converting to/from gamma-encoded sRGB.

namespace colorcpp::core::linear_rgb {

namespace channel {

struct r_tag {};
struct g_tag {};
struct b_tag {};
struct a_tag {};

using float_red = traits::basic_channel<r_tag, float, 0, 1>;
using float_green = traits::basic_channel<g_tag, float, 0, 1>;
using float_blue = traits::basic_channel<b_tag, float, 0, 1>;
using float_alpha = traits::basic_channel<a_tag, float, 0, 1>;

}  // namespace channel

namespace model {

struct linear_rgbf {};
struct linear_rgbaf {};

}  // namespace model

}  // namespace colorcpp::core::linear_rgb

namespace colorcpp::traits {

template <>
struct model_traits<core::linear_rgb::model::linear_rgbf> {
  using channels_type = std::tuple<core::linear_rgb::channel::float_red, core::linear_rgb::channel::float_green,
                                   core::linear_rgb::channel::float_blue>;
  static constexpr std::string_view prefix = "linear-rgb";
  static constexpr std::size_t channel_size = 3;
};

template <>
struct model_traits<core::linear_rgb::model::linear_rgbaf> {
  using channels_type = std::tuple<core::linear_rgb::channel::float_red, core::linear_rgb::channel::float_green,
                                   core::linear_rgb::channel::float_blue, core::linear_rgb::channel::float_alpha>;
  static constexpr std::string_view prefix = "linear-rgba";
  static constexpr std::size_t channel_size = 4;
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

template <typename Model>
struct basic_linear_rgba : basic_color<Model> {
  using base = basic_color<Model>;

  using base::base;
  using base::data;

 private:
  template <typename Tag>
  constexpr auto& channel() {
    constexpr std::size_t idx = traits::channel_index_v<Model, Tag>;
    return std::get<idx>(data);
  }

  template <typename Tag>
  constexpr const auto& channel() const {
    constexpr std::size_t idx = traits::channel_index_v<Model, Tag>;
    return std::get<idx>(data);
  }

 public:
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, linear_rgb::channel::r_tag>>>
  constexpr auto& r() {
    return channel<linear_rgb::channel::r_tag>();
  }
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, linear_rgb::channel::r_tag>>>
  constexpr const auto& r() const {
    return channel<linear_rgb::channel::r_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, linear_rgb::channel::g_tag>>>
  constexpr auto& g() {
    return channel<linear_rgb::channel::g_tag>();
  }
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, linear_rgb::channel::g_tag>>>
  constexpr const auto& g() const {
    return channel<linear_rgb::channel::g_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, linear_rgb::channel::b_tag>>>
  constexpr auto& b() {
    return channel<linear_rgb::channel::b_tag>();
  }
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, linear_rgb::channel::b_tag>>>
  constexpr const auto& b() const {
    return channel<linear_rgb::channel::b_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, linear_rgb::channel::a_tag>>>
  constexpr auto& a() {
    return channel<linear_rgb::channel::a_tag>();
  }
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, linear_rgb::channel::a_tag>>>
  constexpr const auto& a() const {
    return channel<linear_rgb::channel::a_tag>();
  }
};

using linear_rgbf_t = basic_linear_rgba<linear_rgb::model::linear_rgbf>;
using linear_rgbaf_t = basic_linear_rgba<linear_rgb::model::linear_rgbaf>;

}  // namespace colorcpp::core
