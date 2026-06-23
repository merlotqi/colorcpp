/**
 * @file rec2020.hpp
 * @brief Rec.2020 (BT.2020) color space (D65 white point, BT.709-style piecewise transfer).
 *
 * ITU-R BT.2020 (Rec.2020) is the UHDTV standard, covering approximately 75% of
 * the CIE 1931 visible gamut. It uses the same D65 white point as sRGB but with
 * significantly wider primaries.
 *
 * Transfer function: BT.709-style piecewise (α=1.0993, β=0.0181).
 *
 * @see https://www.itu.int/rec/R-REC-BT.2020
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>
#include <colorcpp/core/linear_rgb.hpp>

namespace colorcpp::core::rec2020 {

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

struct rec2020 {};
struct rec2020a {};
struct linear_rec2020f {};
struct linear_rec2020af {};

}  // namespace model

}  // namespace colorcpp::core::rec2020

namespace colorcpp::traits {

template <>
struct model_traits<core::rec2020::model::rec2020> {
  using channels_type = std::tuple<core::rec2020::channel::f32_red, core::rec2020::channel::f32_green,
                                   core::rec2020::channel::f32_blue>;
  static constexpr std::string_view prefix = "rec2020";
  static constexpr std::size_t channel_size = 3;
};

template <>
struct model_traits<core::rec2020::model::rec2020a> {
  using channels_type = std::tuple<core::rec2020::channel::f32_red, core::rec2020::channel::f32_green,
                                   core::rec2020::channel::f32_blue, core::rec2020::channel::f32_alpha>;
  static constexpr std::string_view prefix = "rec2020a";
  static constexpr std::size_t channel_size = 4;
};

template <>
struct model_traits<core::rec2020::model::linear_rec2020f> {
  using channels_type = std::tuple<core::rec2020::channel::f32_red, core::rec2020::channel::f32_green,
                                   core::rec2020::channel::f32_blue>;
  static constexpr std::string_view prefix = "linear-rec2020";
  static constexpr std::size_t channel_size = 3;
};

template <>
struct model_traits<core::rec2020::model::linear_rec2020af> {
  using channels_type = std::tuple<core::rec2020::channel::f32_red, core::rec2020::channel::f32_green,
                                   core::rec2020::channel::f32_blue, core::rec2020::channel::f32_alpha>;
  static constexpr std::string_view prefix = "linear-rec2020a";
  static constexpr std::size_t channel_size = 4;
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief Rec.2020 / Rec.2020A with @c r(), @c g(), @c b(), and @c a() when alpha is present.
 * @tparam Model rec2020::model::rec2020, rec2020a, linear_rec2020f, or linear_rec2020af.
 */
template <typename Model>
struct basic_rec2020 : basic_color<Model> {
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
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rec2020::channel::r_tag>>>
  constexpr auto& r() { return channel<rec2020::channel::r_tag>(); }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rec2020::channel::r_tag>>>
  constexpr const auto& r() const { return channel<rec2020::channel::r_tag>(); }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rec2020::channel::g_tag>>>
  constexpr auto& g() { return channel<rec2020::channel::g_tag>(); }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rec2020::channel::g_tag>>>
  constexpr const auto& g() const { return channel<rec2020::channel::g_tag>(); }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rec2020::channel::b_tag>>>
  constexpr auto& b() { return channel<rec2020::channel::b_tag>(); }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rec2020::channel::b_tag>>>
  constexpr const auto& b() const { return channel<rec2020::channel::b_tag>(); }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rec2020::channel::a_tag>>>
  constexpr auto& a() { return channel<rec2020::channel::a_tag>(); }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rec2020::channel::a_tag>>>
  constexpr const auto& a() const { return channel<rec2020::channel::a_tag>(); }
};

/** @brief Rec.2020 with float channels. */
using rec2020_rgbf_t = basic_rec2020<rec2020::model::rec2020>;
/** @brief Rec.2020A with float channels. */
using rec2020_rgbaf_t = basic_rec2020<rec2020::model::rec2020a>;
/** @brief Linear Rec.2020 with float channels. */
using linear_rec2020_rgbf_t = basic_rec2020<rec2020::model::linear_rec2020f>;
/** @brief Linear Rec.2020A with float channels. */
using linear_rec2020_rgbaf_t = basic_rec2020<rec2020::model::linear_rec2020af>;

}  // namespace colorcpp::core

// I/O operators for basic_rec2020
template <typename Model>
std::ostream& operator<<(std::ostream& os, const colorcpp::core::basic_rec2020<Model>& c) {
  return colorcpp::core::io::operator<<(os, static_cast<const colorcpp::core::basic_color<Model>&>(c));
}

template <typename Model>
std::istream& operator>>(std::istream& is, colorcpp::core::basic_rec2020<Model>& c) {
  return colorcpp::core::io::operator>>(is, static_cast<colorcpp::core::basic_color<Model>&>(c));
}
