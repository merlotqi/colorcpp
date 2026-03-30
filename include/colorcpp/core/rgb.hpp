/**
 * @file rgb.hpp
 * @brief sRGB-style RGB and RGBA: 8-bit channels or float channels in [0, 1].
 *
 * Order: R, G, B (optional A). Gamma-encoded for 8-bit and float types here; use @ref linear_rgb.hpp
 * for scene-linear sRGB primaries.
 */

#pragma once

#include <cassert>
#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>
#include <cstdint>
#include <tuple>
#include <type_traits>

namespace colorcpp::core::rgb {

namespace channel {

struct r_tag {};
struct g_tag {};
struct b_tag {};
struct a_tag {};

using u8_red = traits::basic_channel<r_tag, uint8_t, 0, 255, 1>;
using u8_green = traits::basic_channel<g_tag, uint8_t, 0, 255, 1>;
using u8_blue = traits::basic_channel<b_tag, uint8_t, 0, 255, 1>;
using u8_alpha = traits::basic_channel<a_tag, uint8_t, 0, 255, 1>;

using float_red = traits::basic_channel<r_tag, float, 0, 1>;
using float_green = traits::basic_channel<g_tag, float, 0, 1>;
using float_blue = traits::basic_channel<b_tag, float, 0, 1>;
using float_alpha = traits::basic_channel<a_tag, float, 0, 1>;

}  // namespace channel

namespace model {

struct rgb8 {};
struct rgba8 {};

struct rgb_float {};
struct rgba_float {};

}  // namespace model

}  // namespace colorcpp::core::rgb

namespace colorcpp::traits {

template <>
struct model_traits<core::rgb::model::rgb8> {
  using channels_type =
      std::tuple<core::rgb::channel::u8_red, core::rgb::channel::u8_green, core::rgb::channel::u8_blue>;
  static constexpr std::string_view prefix = "rgb";
  static constexpr std::size_t channel_size = 3;
};

template <>
struct model_traits<core::rgb::model::rgba8> {
  using channels_type = std::tuple<core::rgb::channel::u8_red, core::rgb::channel::u8_green,
                                   core::rgb::channel::u8_blue, core::rgb::channel::u8_alpha>;
  static constexpr std::string_view prefix = "rgba";
  static constexpr std::size_t channel_size = 4;
};

template <>
struct model_traits<core::rgb::model::rgb_float> {
  using channels_type =
      std::tuple<core::rgb::channel::float_red, core::rgb::channel::float_green, core::rgb::channel::float_blue>;
  static constexpr std::string_view prefix = "rgbf";
  static constexpr std::size_t channel_size = 3;
};

template <>
struct model_traits<core::rgb::model::rgba_float> {
  using channels_type = std::tuple<core::rgb::channel::float_red, core::rgb::channel::float_green,
                                   core::rgb::channel::float_blue, core::rgb::channel::float_alpha>;
  static constexpr std::string_view prefix = "rgbaf";
  static constexpr std::size_t channel_size = 4;
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief RGB/RGBA color with @c r(), @c g(), @c b(), and @c a() when alpha is present.
 * @tparam Model One of rgb::model::rgb8, rgba8, rgb_float, rgba_float.
 */
template <typename Model>
struct basic_rgba : basic_color<Model> {
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
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rgb::channel::r_tag>>>
  constexpr auto& r() {
    return channel<rgb::channel::r_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rgb::channel::r_tag>>>
  constexpr const auto& r() const {
    return channel<rgb::channel::r_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rgb::channel::g_tag>>>
  constexpr auto& g() {
    return channel<rgb::channel::g_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rgb::channel::g_tag>>>
  constexpr const auto& g() const {
    return channel<rgb::channel::g_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rgb::channel::b_tag>>>
  constexpr auto& b() {
    return channel<rgb::channel::b_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rgb::channel::b_tag>>>
  constexpr const auto& b() const {
    return channel<rgb::channel::b_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rgb::channel::a_tag>>>
  constexpr auto& a() {
    return channel<rgb::channel::a_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rgb::channel::a_tag>>>
  constexpr const auto& a() const {
    return channel<rgb::channel::a_tag>();
  }
};

/** @brief 8-bit RGB, channels 0–255. */
using rgb8_t = basic_rgba<rgb::model::rgb8>;
/** @brief 8-bit RGBA, alpha 0–255. */
using rgba8_t = basic_rgba<rgb::model::rgba8>;

/** @brief Float RGB in [0, 1]. */
using rgbf_t = basic_rgba<rgb::model::rgb_float>;
/** @brief Float RGBA in [0, 1]. */
using rgbaf_t = basic_rgba<rgb::model::rgba_float>;

}  // namespace colorcpp::core

// I/O operators for basic_rgba
template <typename Model>
std::ostream& operator<<(std::ostream& os, const colorcpp::core::basic_rgba<Model>& c) {
  return colorcpp::core::io::operator<<(os, static_cast<const colorcpp::core::basic_color<Model>&>(c));
}

template <typename Model>
std::istream& operator>>(std::istream& is, colorcpp::core::basic_rgba<Model>& c) {
  return colorcpp::core::io::operator>>(is, static_cast<colorcpp::core::basic_color<Model>&>(c));
}
