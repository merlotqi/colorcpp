/**
 * @file hsv.hpp
 * @brief HSV / HSVA: hue in degrees [0, 360], saturation and value in [0, 1] (not percentages).
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>

namespace colorcpp::core::hsv {

namespace channel {

struct h_tag {};                  // hue
struct s_tag {};                  // saturation
struct v_tag {};                  // value
using a_tag = traits::alpha_tag;  // alpha

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
};

template <>
struct model_traits<core::hsv::model::hsva> {
  using channels_type = std::tuple<core::hsv::channel::f32_hue, core::hsv::channel::f32_saturation,
                                   core::hsv::channel::f32_value, core::hsv::channel::f32_alpha>;

  static constexpr std::string_view prefix = "hsva";
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

 public:
  COLORCPP_CHANNEL_ACCESSOR(hsv::channel::h_tag, h)

  COLORCPP_CHANNEL_ACCESSOR(hsv::channel::s_tag, s)

  COLORCPP_CHANNEL_ACCESSOR(hsv::channel::v_tag, v)

  COLORCPP_CHANNEL_ACCESSOR(hsv::channel::a_tag, a)
};

/** @brief HSV with float channels. */
using hsv_float_t = basic_hsv<hsv::model::hsv>;
/** @brief HSVA with float channels. */
using hsva_float_t = basic_hsv<hsv::model::hsva>;

COLORCPP_STREAM_OPERATORS(colorcpp::core::basic_hsv)

}  // namespace colorcpp::core

COLORCPP_ENABLE_STRUCTURED_BINDINGS(colorcpp::core::basic_hsv)
