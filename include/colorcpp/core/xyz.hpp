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

 public:
  COLORCPP_CHANNEL_ACCESSOR(xyz::channel::x_tag, x)

  COLORCPP_CHANNEL_ACCESSOR(xyz::channel::y_tag, y)

  COLORCPP_CHANNEL_ACCESSOR(xyz::channel::z_tag, z)
};

/** @brief CIE XYZ under D65. */
using xyz_t = basic_xyz<xyz::model::xyze>;

COLORCPP_STREAM_OPERATORS(colorcpp::core::basic_xyz)

}  // namespace colorcpp::core

COLORCPP_ENABLE_STRUCTURED_BINDINGS(colorcpp::core::basic_xyz)
COLORCPP_ENABLE_HASH(colorcpp::core::basic_xyz)
