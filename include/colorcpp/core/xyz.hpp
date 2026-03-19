#pragma once

#include <colorcpp/core/color_base.hpp>
#include <tuple>
#include <type_traits>

// CIE XYZ (CIE 1931, D65 reference white):
// The device-independent "hub" color space that underlies all CIE-based models.
// Using D65 for consistency with linear sRGB, CIELAB, and OkLab in this library.
//   X: [0, 2]  — mix of cone response curves (white point Xn ≈ 0.95047)
//   Y: [0, 2]  — luminance; exactly 1.0 for D65 white
//   Z: [0, 2]  — quasi-blue (white point Zn ≈ 1.08883)
//
// The range [0, 2] covers sRGB, P3-D65, and Rec2020 gamuts.
// HDR values beyond Y=1 are representable up to the ceiling of 2.
//
// Conversion path:  sRGB ↔ Linear sRGB ↔ XYZ ↔ CIELAB
//                                         XYZ ↔ OkLab  (direct, no gamut clip)

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

template <typename Model>
struct basic_xyz : basic_color<Model> {
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

using xyz_t = basic_xyz<xyz::model::xyze>;

}  // namespace colorcpp::core
