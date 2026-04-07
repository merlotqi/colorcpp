/**
 * @file hwb.hpp
 * @brief HWB / HWBA: hue in degrees [0, 360], whiteness and blackness in [0, 1].
 *
 * HWB is a color model defined by the CSS Color Level 4 specification.
 * It's designed to be more intuitive for humans than HSL/HSV.
 * @see https://www.w3.org/TR/css-color-4/#the-hwb-notation
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>

namespace colorcpp::core::hwb {

namespace channel {

struct h_tag {};  // hue
struct w_tag {};  // whiteness
struct b_tag {};  // blackness
struct a_tag {};  // alpha

using f32_hue = traits::basic_channel<h_tag, float, 0, 360, 1>;
using f32_whiteness = traits::basic_channel<w_tag, float, 0, 1>;
using f32_blackness = traits::basic_channel<b_tag, float, 0, 1>;
using f32_alpha = traits::basic_channel<a_tag, float, 0, 1>;

}  // namespace channel

namespace model {

struct hwb {};
struct hwba {};

}  // namespace model

}  // namespace colorcpp::core::hwb

namespace colorcpp::traits {

template <>
struct model_traits<core::hwb::model::hwb> {
  using channels_type =
      std::tuple<core::hwb::channel::f32_hue, core::hwb::channel::f32_whiteness, core::hwb::channel::f32_blackness>;

  static constexpr std::string_view prefix = "hwb";
  static constexpr std::size_t channel_size = 3;
};

template <>
struct model_traits<core::hwb::model::hwba> {
  using channels_type = std::tuple<core::hwb::channel::f32_hue, core::hwb::channel::f32_whiteness,
                                   core::hwb::channel::f32_blackness, core::hwb::channel::f32_alpha>;
  static constexpr std::string_view prefix = "hwba";
  static constexpr std::size_t channel_size = 4;
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief HWB/HWBA with @c h(), @c w(), @c b(), and @c a() when alpha is present.
 * @tparam Model hwb::model::hwb or hwba.
 */
template <typename Model>
struct basic_hwb : basic_color<Model> {
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
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hwb::channel::h_tag>>>
  constexpr auto& h() {
    return channel<hwb::channel::h_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hwb::channel::h_tag>>>
  constexpr const auto& h() const {
    return channel<hwb::channel::h_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hwb::channel::w_tag>>>
  constexpr auto& w() {
    return channel<hwb::channel::w_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hwb::channel::w_tag>>>
  constexpr const auto& w() const {
    return channel<hwb::channel::w_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hwb::channel::b_tag>>>
  constexpr auto& b() {
    return channel<hwb::channel::b_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hwb::channel::b_tag>>>
  constexpr const auto& b() const {
    return channel<hwb::channel::b_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hwb::channel::a_tag>>>
  constexpr auto& a() {
    return channel<hwb::channel::a_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hwb::channel::a_tag>>>
  constexpr const auto& a() const {
    return channel<hwb::channel::a_tag>();
  }
};

/** @brief HWB with float channels. */
using hwb_float_t = basic_hwb<hwb::model::hwb>;
/** @brief HWBA with float channels. */
using hwba_float_t = basic_hwb<hwb::model::hwba>;

}  // namespace colorcpp::core

// I/O operators for basic_hwb
template <typename Model>
std::ostream& operator<<(std::ostream& os, const colorcpp::core::basic_hwb<Model>& c) {
  return colorcpp::core::io::operator<<(os, static_cast<const colorcpp::core::basic_color<Model>&>(c));
}

template <typename Model>
std::istream& operator>>(std::istream& is, colorcpp::core::basic_hwb<Model>& c) {
  return colorcpp::core::io::operator>>(is, static_cast<colorcpp::core::basic_color<Model>&>(c));
}
