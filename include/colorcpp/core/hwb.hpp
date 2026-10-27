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

struct h_tag {};                  // hue
struct w_tag {};                  // whiteness
struct b_tag {};                  // blackness
using a_tag = traits::alpha_tag;  // alpha

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
};

template <>
struct model_traits<core::hwb::model::hwba> {
  using channels_type = std::tuple<core::hwb::channel::f32_hue, core::hwb::channel::f32_whiteness,
                                   core::hwb::channel::f32_blackness, core::hwb::channel::f32_alpha>;
  static constexpr std::string_view prefix = "hwba";
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

 public:
  COLORCPP_CHANNEL_ACCESSOR(hwb::channel::h_tag, h)

  COLORCPP_CHANNEL_ACCESSOR(hwb::channel::w_tag, w)

  COLORCPP_CHANNEL_ACCESSOR(hwb::channel::b_tag, b)

  COLORCPP_CHANNEL_ACCESSOR(hwb::channel::a_tag, a)
};

/** @brief HWB with float channels. */
using hwb_float_t = basic_hwb<hwb::model::hwb>;
/** @brief HWBA with float channels. */
using hwba_float_t = basic_hwb<hwb::model::hwba>;

COLORCPP_STREAM_OPERATORS(colorcpp::core::basic_hwb)

}  // namespace colorcpp::core

COLORCPP_ENABLE_STRUCTURED_BINDINGS(colorcpp::core::basic_hwb)
