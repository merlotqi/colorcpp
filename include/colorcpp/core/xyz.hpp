/**
 * @file xyz.hpp
 * @brief CIE XYZ (1931), D65 white. Hub space for LAB and OkLab conversions.
 *
 * Channels X, Y, Z use nominal range [0, 2] (float); Y = 1 for D65 white.
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>
#include <tuple>
#include <type_traits>

namespace colorcpp::core::xyz {

namespace channel {

struct x_tag {};
struct y_tag {};
struct z_tag {};

// Den=1 keeps values in natural CIE units (not normalized to [0,1]).
using xyz_x = traits::basic_channel<x_tag, float, 0, 2, 1>;  // [0, 2]
using xyz_y = traits::basic_channel<y_tag, float, 0, 2, 1>;  // [0, 2]
using xyz_z = traits::basic_channel<z_tag, float, 0, 2, 1>;  // [0, 2]

}  // namespace channel

namespace model {

struct xyze {};  // "e" suffix: D65 (equal-energy-adjacent) reference white

}  // namespace model

}  // namespace colorcpp::core::xyz

namespace colorcpp::traits {

template <>
struct model_traits<core::xyz::model::xyze> {
  using channels_type = std::tuple<core::xyz::channel::xyz_x, core::xyz::channel::xyz_y, core::xyz::channel::xyz_z>;
  static constexpr std::string_view prefix = "xyz";
  static constexpr std::size_t channel_size = 3;
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief CIE XYZ with @c x(), @c y(), @c z().
 * @tparam Model xyz::model::xyze (D65).
 */
template <typename Model>
struct basic_xyz : basic_color<Model> {
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
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, xyz::channel::x_tag>>>
  constexpr auto& x() {
    return channel<xyz::channel::x_tag>();
  }
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, xyz::channel::x_tag>>>
  constexpr const auto& x() const {
    return channel<xyz::channel::x_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, xyz::channel::y_tag>>>
  constexpr auto& y() {
    return channel<xyz::channel::y_tag>();
  }
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, xyz::channel::y_tag>>>
  constexpr const auto& y() const {
    return channel<xyz::channel::y_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, xyz::channel::z_tag>>>
  constexpr auto& z() {
    return channel<xyz::channel::z_tag>();
  }
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, xyz::channel::z_tag>>>
  constexpr const auto& z() const {
    return channel<xyz::channel::z_tag>();
  }
};

/** @brief CIE XYZ under D65. */
using xyz_t = basic_xyz<xyz::model::xyze>;

}  // namespace colorcpp::core

// I/O operators for basic_xyz
template <typename Model>
std::ostream& operator<<(std::ostream& os, const colorcpp::core::basic_xyz<Model>& c) {
  return colorcpp::core::io::operator<<(os, static_cast<const colorcpp::core::basic_color<Model>&>(c));
}

template <typename Model>
std::istream& operator>>(std::istream& is, colorcpp::core::basic_xyz<Model>& c) {
  return colorcpp::core::io::operator>>(is, static_cast<colorcpp::core::basic_color<Model>&>(c));
}
