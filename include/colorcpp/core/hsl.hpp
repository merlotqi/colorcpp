/**
 * @file hsl.hpp
 * @brief HSL / HSLA: hue in degrees [0, 360], saturation and lightness in [0, 1] (not percentages).
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>

namespace colorcpp::core::hsl {

namespace channel {

struct h_tag {};  // hue
struct s_tag {};  // saturation
struct l_tag {};  // lightness
struct a_tag {};  // alpha

using f32_hue = traits::basic_channel<h_tag, float, 0, 360, 1>;
using f32_saturation = traits::basic_channel<s_tag, float, 0, 1>;  // [0, 1] — not percentage
using f32_lightness = traits::basic_channel<l_tag, float, 0, 1>;   // [0, 1] — not percentage
using f32_alpha = traits::basic_channel<a_tag, float, 0, 1>;       // [0, 1] — consistent with RGB alpha

}  // namespace channel

namespace model {

struct hsl {};
struct hsla {};

}  // namespace model

}  // namespace colorcpp::core::hsl

namespace colorcpp::traits {

template <>
struct model_traits<core::hsl::model::hsl> {
  using channels_type =
      std::tuple<core::hsl::channel::f32_hue, core::hsl::channel::f32_saturation, core::hsl::channel::f32_lightness>;

  static constexpr std::string_view prefix = "hsl";
  static constexpr std::size_t channel_size = 3;
};

template <>
struct model_traits<core::hsl::model::hsla> {
  using channels_type = std::tuple<core::hsl::channel::f32_hue, core::hsl::channel::f32_saturation,
                                   core::hsl::channel::f32_lightness, core::hsl::channel::f32_alpha>;
  static constexpr std::string_view prefix = "hsla";
  static constexpr std::size_t channel_size = 4;
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief HSL/HSLA with @c h(), @c s(), @c l(), and @c a() when alpha is present.
 * @tparam Model hsl::model::hsl or hsla.
 */
template <typename Model>
struct basic_hsl : basic_color<Model> {
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
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hsl::channel::h_tag>>>
  constexpr auto& h() {
    return channel<hsl::channel::h_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hsl::channel::h_tag>>>
  constexpr const auto& h() const {
    return channel<hsl::channel::h_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hsl::channel::s_tag>>>
  constexpr auto& s() {
    return channel<hsl::channel::s_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hsl::channel::s_tag>>>
  constexpr const auto& s() const {
    return channel<hsl::channel::s_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hsl::channel::l_tag>>>
  constexpr auto& l() {
    return channel<hsl::channel::l_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hsl::channel::l_tag>>>
  constexpr const auto& l() const {
    return channel<hsl::channel::l_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hsl::channel::a_tag>>>
  constexpr auto& a() {
    return channel<hsl::channel::a_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, hsl::channel::a_tag>>>
  constexpr const auto& a() const {
    return channel<hsl::channel::a_tag>();
  }
};

/** @brief HSL with float channels. */
using hsl_float_t = basic_hsl<hsl::model::hsl>;
/** @brief HSLA with float channels. */
using hsla_float_t = basic_hsl<hsl::model::hsla>;

}  // namespace colorcpp::core

// I/O operators for basic_hsl
template <typename Model>
std::ostream& operator<<(std::ostream& os, const colorcpp::core::basic_hsl<Model>& c) {
  return colorcpp::core::io::operator<<(os, static_cast<const colorcpp::core::basic_color<Model>&>(c));
}

template <typename Model>
std::istream& operator>>(std::istream& is, colorcpp::core::basic_hsl<Model>& c) {
  return colorcpp::core::io::operator>>(is, static_cast<colorcpp::core::basic_color<Model>&>(c));
}
