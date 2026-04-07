/**
 * @file hsv.hpp
 * @brief HSV / HSVA: hue in degrees [0, 360], saturation and value in [0, 1] (not percentages).
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>

namespace colorcpp::core::hsv {

namespace channel {

struct h_tag {};  // hue
struct s_tag {};  // saturation
struct v_tag {};  // value
struct a_tag {};  // alpha

using f32_hue = traits::basic_channel<h_tag, float, 0, 360, 1>;
using f32_saturation = traits::basic_channel<s_tag, float, 0, 1>;  // [0, 1] — not percentage
using f32_value = traits::basic_channel<v_tag, float, 0, 1>;       // [0, 1] — not percentage
using f32_alpha = traits::basic_channel<a_tag, float, 0, 1>;       // [0, 1] — consistent with RGB alpha

}  // namespace channel

namespace model {

struct hsv {};
struct hsva {};

}  // namespace model

}  // namespace colorcpp::core::hsv

namespace colorcpp::traits {

template <>
struct model_traits<core::hsv::model::hsv> {
  using channels_type =
      std::tuple<core::hsv::channel::f32_hue, core::hsv::channel::f32_saturation, core::hsv::channel::f32_value>;

  static constexpr std::string_view prefix = "hsv";
  static constexpr std::size_t channel_size = 3;
};

template <>
struct model_traits<core::hsv::model::hsva> {
  using channels_type = std::tuple<core::hsv::channel::f32_hue, core::hsv::channel::f32_saturation,
                                   core::hsv::channel::f32_value, core::hsv::channel::f32_alpha>;

  static constexpr std::string_view prefix = "hsva";
  static constexpr std::size_t channel_size = 4;
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief HSV/HSVA with @c h(), @c s(), @c v(), and @c a() when alpha is present.
 * @tparam Model hsv::model::hsv or hsva.
 */
template <typename Model>
struct basic_hsv : basic_color<Model> {
  using base = basic_color<Model>;

  using base::base;
  using base::data;

 private:
  template <typename Tag>
  constexpr auto& channel() {
    constexpr std::size_t idx = traits::channel_index_v<Model, Tag>;
    return data[idx];
  }

  template <typename Tag>
  constexpr const auto& channel() const {
    constexpr std::size_t idx = traits::channel_index_v<Model, Tag>;
    return data[idx];
  }

 public:
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hsv::channel::h_tag>>>
  constexpr auto& h() {
    return channel<hsv::channel::h_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hsv::channel::h_tag>>>
  constexpr const auto& h() const {
    return channel<hsv::channel::h_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hsv::channel::s_tag>>>
  constexpr auto& s() {
    return channel<hsv::channel::s_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hsv::channel::s_tag>>>
  constexpr const auto& s() const {
    return channel<hsv::channel::s_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hsv::channel::v_tag>>>
  constexpr auto& v() {
    return channel<hsv::channel::v_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hsv::channel::v_tag>>>
  constexpr const auto& v() const {
    return channel<hsv::channel::v_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hsv::channel::a_tag>>>
  constexpr auto& a() {
    return channel<hsv::channel::a_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hsv::channel::a_tag>>>
  constexpr const auto& a() const {
    return channel<hsv::channel::a_tag>();
  }
};

/** @brief HSV with float channels. */
using hsv_float_t = basic_hsv<hsv::model::hsv>;
/** @brief HSVA with float channels. */
using hsva_float_t = basic_hsv<hsv::model::hsva>;

}  // namespace colorcpp::core

// I/O operators for basic_hsv
template <typename Model>
std::ostream& operator<<(std::ostream& os, const colorcpp::core::basic_hsv<Model>& c) {
  return colorcpp::core::io::operator<<(os, static_cast<const colorcpp::core::basic_color<Model>&>(c));
}

template <typename Model>
std::istream& operator>>(std::istream& is, colorcpp::core::basic_hsv<Model>& c) {
  return colorcpp::core::io::operator>>(is, static_cast<colorcpp::core::basic_color<Model>&>(c));
}
