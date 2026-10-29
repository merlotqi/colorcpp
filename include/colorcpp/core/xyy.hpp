/**
 * @file xyy.hpp
 * @brief CIE xyY chromaticity + luminance, D65 white.
 *
 * Channels: x, y in [0, 1] (chromaticity), Y in [0, 2] (luminance, Y = 1 for
 * D65 white). Separates chromaticity from luminance; used by colorimetry and
 * color temperature work. Conversion composes with XYZ (include @ref xyz.hpp
 * with @ref conversion.hpp).
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>
#include <tuple>

namespace colorcpp::core::xyy {

namespace channel {

struct x_tag {};  // chromaticity x
struct y_tag {};  // chromaticity y
struct Y_tag {};  // luminance (capital Y)

// Den=1 keeps values in natural CIE units.
using xyy_x = traits::basic_channel<x_tag, float, 0, 1, 1>;  // [0, 1]
using xyy_y = traits::basic_channel<y_tag, float, 0, 1, 1>;  // [0, 1]
using xyy_Y = traits::basic_channel<Y_tag, float, 0, 2, 1>;  // [0, 2]

}  // namespace channel

namespace model {

struct xyy {};

}  // namespace model

}  // namespace colorcpp::core::xyy

namespace colorcpp::traits {

template <>
struct model_traits<core::xyy::model::xyy> {
  using channels_type = std::tuple<core::xyy::channel::xyy_x, core::xyy::channel::xyy_y, core::xyy::channel::xyy_Y>;
  static constexpr std::string_view prefix = "xyy";
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief CIE xyY with @c x(), @c y() (chromaticity) and @c Y() (luminance).
 * @tparam Model xyy::model::xyy (D65).
 */
template <typename Model>
struct basic_xyy : basic_color<Model> {
  using base = basic_color<Model>;

  using base::base;
  using base::data;

 public:
  COLORCPP_CHANNEL_ACCESSOR(xyy::channel::x_tag, x)

  COLORCPP_CHANNEL_ACCESSOR(xyy::channel::y_tag, y)

  COLORCPP_CHANNEL_ACCESSOR(xyy::channel::Y_tag, Y)
};

/** @brief CIE xyY under D65. */
using xyy_t = basic_xyy<xyy::model::xyy>;

COLORCPP_STREAM_OPERATORS(colorcpp::core::basic_xyy)

}  // namespace colorcpp::core

COLORCPP_ENABLE_STRUCTURED_BINDINGS(colorcpp::core::basic_xyy)
COLORCPP_ENABLE_HASH(colorcpp::core::basic_xyy)
