/**
 * @file display_p3.hpp
 * @brief Display P3 color space (DCI-P3 primaries with D65 white point).
 *
 * Display P3 uses the DCI-P3 primaries with a D65 white point and the sRGB transfer function.
 * It's commonly used in modern displays and is part of CSS Color Level 4.
 *
 * @see https://www.w3.org/TR/css-color-4/#valdef-color-display-p3
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/linear_rgb.hpp>
#include <colorcpp/core/io.hpp>

namespace colorcpp::core::display_p3 {

namespace channel {

struct r_tag {};
struct g_tag {};
struct b_tag {};
struct a_tag {};

using f32_red = traits::basic_channel<r_tag, float, 0, 1, 1>;
using f32_green = traits::basic_channel<g_tag, float, 0, 1, 1>;
using f32_blue = traits::basic_channel<b_tag, float, 0, 1, 1>;
using f32_alpha = traits::basic_channel<a_tag, float, 0, 1, 1>;

}  // namespace channel

namespace model {

struct display_p3 {};
struct display_p3a {};
struct linear_display_p3f {};
struct linear_display_p3af {};

}  // namespace model

}  // namespace colorcpp::core::display_p3

namespace colorcpp::traits {

template <>
struct model_traits<core::display_p3::model::display_p3> {
  using channels_type = std::tuple<core::display_p3::channel::f32_red, core::display_p3::channel::f32_green,
                                   core::display_p3::channel::f32_blue>;

  static constexpr std::string_view prefix = "display_p3";
  static constexpr std::size_t channel_size = 3;
};

template <>
struct model_traits<core::display_p3::model::display_p3a> {
  using channels_type = std::tuple<core::display_p3::channel::f32_red, core::display_p3::channel::f32_green,
                                   core::display_p3::channel::f32_blue, core::display_p3::channel::f32_alpha>;
  static constexpr std::string_view prefix = "display_p3a";
  static constexpr std::size_t channel_size = 4;
};

template <>
struct model_traits<core::display_p3::model::linear_display_p3f> {
  using channels_type = std::tuple<core::display_p3::channel::f32_red, core::display_p3::channel::f32_green,
                                   core::display_p3::channel::f32_blue>;

  static constexpr std::string_view prefix = "linear_display_p3f";
  static constexpr std::size_t channel_size = 3;
};

template <>
struct model_traits<core::display_p3::model::linear_display_p3af> {
  using channels_type = std::tuple<core::display_p3::channel::f32_red, core::display_p3::channel::f32_green,
                                   core::display_p3::channel::f32_blue, core::display_p3::channel::f32_alpha>;
  static constexpr std::string_view prefix = "linear_display_p3af";
  static constexpr std::size_t channel_size = 4;
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief Display P3/Display P3A with @c r(), @c g(), @c b(), and @c a() when alpha is present.
 * @tparam Model display_p3::model::display_p3 or display_p3a.
 */
template <typename Model>
struct basic_display_p3 : basic_color<Model> {
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
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, display_p3::channel::r_tag>>>
  constexpr auto& r() {
    return channel<display_p3::channel::r_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, display_p3::channel::r_tag>>>
  constexpr const auto& r() const {
    return channel<display_p3::channel::r_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, display_p3::channel::g_tag>>>
  constexpr auto& g() {
    return channel<display_p3::channel::g_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, display_p3::channel::g_tag>>>
  constexpr const auto& g() const {
    return channel<display_p3::channel::g_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, display_p3::channel::b_tag>>>
  constexpr auto& b() {
    return channel<display_p3::channel::b_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, display_p3::channel::b_tag>>>
  constexpr const auto& b() const {
    return channel<display_p3::channel::b_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, display_p3::channel::a_tag>>>
  constexpr auto& a() {
    return channel<display_p3::channel::a_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, display_p3::channel::a_tag>>>
  constexpr const auto& a() const {
    return channel<display_p3::channel::a_tag>();
  }
};

/** @brief Display P3 with float channels. */
using display_p3f_t = basic_display_p3<display_p3::model::display_p3>;
/** @brief Display P3A with float channels. */
using display_p3af_t = basic_display_p3<display_p3::model::display_p3a>;

/** @brief Linear Display P3 with float channels. */
using linear_display_p3f_t = basic_display_p3<display_p3::model::linear_display_p3f>;
/** @brief Linear Display P3A with float channels. */
using linear_display_p3af_t = basic_display_p3<display_p3::model::linear_display_p3af>;

}  // namespace colorcpp::core

// I/O operators for basic_display_p3
template <typename Model>
std::ostream& operator<<(std::ostream& os, const colorcpp::core::basic_display_p3<Model>& c) {
  return colorcpp::core::io::operator<<(os, static_cast<const colorcpp::core::basic_color<Model>&>(c));
}

template <typename Model>
std::istream& operator>>(std::istream& is, colorcpp::core::basic_display_p3<Model>& c) {
  return colorcpp::core::io::operator>>(is, static_cast<colorcpp::core::basic_color<Model>&>(c));
}
