/**
 * @file prophoto_rgb.hpp
 * @brief ProPhoto RGB (ROMM RGB) color space (D50 white point, Gamma 1.8).
 *
 * ProPhoto RGB is a wide-gamut RGB color space developed by Kodak, using D50 as
 * its reference white point. It covers approximately 90% of the CIE L*a*b* gamut,
 * making it one of the largest RGB working spaces. It is the internal working space
 * of Adobe Lightroom and is specified by ICC as ROMM RGB (Reference Output Medium
 * Metric RGB).
 *
 * Transfer function: Gamma 1.8 with a linear segment below 1/512.
 * White point: D50 (0.3457, 0.3585)
 *
 * NOTE: Because the existing conversion graph uses XYZ(D65) as its central hub,
 * conversions between linear ProPhoto RGB and XYZ incorporate a Bradford chromatic
 * adaptation step (D50 <-> D65).
 *
 * @see https://www.color.org/ROMMRGB.pdf
 * @see http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>
#include <colorcpp/core/linear_rgb.hpp>

namespace colorcpp::core::prophoto_rgb {

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

struct prophoto_rgb {};
struct prophoto_rgba {};
struct linear_prophoto_rgbf {};
struct linear_prophoto_rgbaf {};

}  // namespace model

}  // namespace colorcpp::core::prophoto_rgb

namespace colorcpp::traits {

template <>
struct model_traits<core::prophoto_rgb::model::prophoto_rgb> {
  using channels_type = std::tuple<core::prophoto_rgb::channel::f32_red, core::prophoto_rgb::channel::f32_green,
                                   core::prophoto_rgb::channel::f32_blue>;
  static constexpr std::string_view prefix = "prophoto-rgb";
  static constexpr std::size_t channel_size = 3;
};

template <>
struct model_traits<core::prophoto_rgb::model::prophoto_rgba> {
  using channels_type = std::tuple<core::prophoto_rgb::channel::f32_red, core::prophoto_rgb::channel::f32_green,
                                   core::prophoto_rgb::channel::f32_blue, core::prophoto_rgb::channel::f32_alpha>;
  static constexpr std::string_view prefix = "prophoto-rgba";
  static constexpr std::size_t channel_size = 4;
};

template <>
struct model_traits<core::prophoto_rgb::model::linear_prophoto_rgbf> {
  using channels_type = std::tuple<core::prophoto_rgb::channel::f32_red, core::prophoto_rgb::channel::f32_green,
                                   core::prophoto_rgb::channel::f32_blue>;
  static constexpr std::string_view prefix = "linear-prophoto-rgb";
  static constexpr std::size_t channel_size = 3;
};

template <>
struct model_traits<core::prophoto_rgb::model::linear_prophoto_rgbaf> {
  using channels_type = std::tuple<core::prophoto_rgb::channel::f32_red, core::prophoto_rgb::channel::f32_green,
                                   core::prophoto_rgb::channel::f32_blue, core::prophoto_rgb::channel::f32_alpha>;
  static constexpr std::string_view prefix = "linear-prophoto-rgba";
  static constexpr std::size_t channel_size = 4;
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief ProPhoto RGB / ProPhoto RGBA with @c r(), @c g(), @c b(), and @c a() when alpha is present.
 * @tparam Model prophoto_rgb::model::prophoto_rgb or prophoto_rgba.
 */
template <typename Model>
struct basic_prophoto_rgb : basic_color<Model> {
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
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, prophoto_rgb::channel::r_tag>>>
  constexpr auto& r() {
    return channel<prophoto_rgb::channel::r_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, prophoto_rgb::channel::r_tag>>>
  constexpr const auto& r() const {
    return channel<prophoto_rgb::channel::r_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, prophoto_rgb::channel::g_tag>>>
  constexpr auto& g() {
    return channel<prophoto_rgb::channel::g_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, prophoto_rgb::channel::g_tag>>>
  constexpr const auto& g() const {
    return channel<prophoto_rgb::channel::g_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, prophoto_rgb::channel::b_tag>>>
  constexpr auto& b() {
    return channel<prophoto_rgb::channel::b_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, prophoto_rgb::channel::b_tag>>>
  constexpr const auto& b() const {
    return channel<prophoto_rgb::channel::b_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, prophoto_rgb::channel::a_tag>>>
  constexpr auto& a() {
    return channel<prophoto_rgb::channel::a_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, prophoto_rgb::channel::a_tag>>>
  constexpr const auto& a() const {
    return channel<prophoto_rgb::channel::a_tag>();
  }
};

/** @brief ProPhoto RGB with float channels. */
using prophoto_rgbf_t = basic_prophoto_rgb<prophoto_rgb::model::prophoto_rgb>;
/** @brief ProPhoto RGBA with float channels. */
using prophoto_rgbaf_t = basic_prophoto_rgb<prophoto_rgb::model::prophoto_rgba>;
/** @brief Linear ProPhoto RGB with float channels. */
using linear_prophoto_rgbf_t = basic_prophoto_rgb<prophoto_rgb::model::linear_prophoto_rgbf>;
/** @brief Linear ProPhoto RGBA with float channels. */
using linear_prophoto_rgbaf_t = basic_prophoto_rgb<prophoto_rgb::model::linear_prophoto_rgbaf>;

}  // namespace colorcpp::core

// I/O operators for basic_prophoto_rgb
template <typename Model>
std::ostream& operator<<(std::ostream& os, const colorcpp::core::basic_prophoto_rgb<Model>& c) {
  return colorcpp::core::io::operator<<(os, static_cast<const colorcpp::core::basic_color<Model>&>(c));
}

template <typename Model>
std::istream& operator>>(std::istream& is, colorcpp::core::basic_prophoto_rgb<Model>& c) {
  return colorcpp::core::io::operator>>(is, static_cast<colorcpp::core::basic_color<Model>&>(c));
}
