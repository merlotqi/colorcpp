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
using a_tag = traits::alpha_tag;

using f32_red = traits::basic_channel<r_tag, float, 0, 1, 1>;
using f32_green = traits::basic_channel<g_tag, float, 0, 1, 1>;
using f32_blue = traits::basic_channel<b_tag, float, 0, 1, 1>;
using f32_alpha = traits::basic_channel<a_tag, float, 0, 1, 1>;

}  // namespace channel

namespace model {

struct prophoto_rgb : traits::rgb_model_tag {};
struct prophoto_rgba : traits::rgb_model_tag {};
struct linear_prophoto_rgbf : traits::rgb_model_tag {};
struct linear_prophoto_rgbaf : traits::rgb_model_tag {};

}  // namespace model

}  // namespace colorcpp::core::prophoto_rgb

namespace colorcpp::traits {

template <>
struct model_traits<core::prophoto_rgb::model::prophoto_rgb> {
  using channels_type = std::tuple<core::prophoto_rgb::channel::f32_red, core::prophoto_rgb::channel::f32_green,
                                   core::prophoto_rgb::channel::f32_blue>;
  static constexpr std::string_view prefix = "prophoto-rgb";
};

template <>
struct model_traits<core::prophoto_rgb::model::prophoto_rgba> {
  using channels_type = std::tuple<core::prophoto_rgb::channel::f32_red, core::prophoto_rgb::channel::f32_green,
                                   core::prophoto_rgb::channel::f32_blue, core::prophoto_rgb::channel::f32_alpha>;
  static constexpr std::string_view prefix = "prophoto-rgba";
};

template <>
struct model_traits<core::prophoto_rgb::model::linear_prophoto_rgbf> {
  using channels_type = std::tuple<core::prophoto_rgb::channel::f32_red, core::prophoto_rgb::channel::f32_green,
                                   core::prophoto_rgb::channel::f32_blue>;
  static constexpr std::string_view prefix = "linear-prophoto-rgb";
};

template <>
struct model_traits<core::prophoto_rgb::model::linear_prophoto_rgbaf> {
  using channels_type = std::tuple<core::prophoto_rgb::channel::f32_red, core::prophoto_rgb::channel::f32_green,
                                   core::prophoto_rgb::channel::f32_blue, core::prophoto_rgb::channel::f32_alpha>;
  static constexpr std::string_view prefix = "linear-prophoto-rgba";
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

 public:
  COLORCPP_CHANNEL_ACCESSOR(prophoto_rgb::channel::r_tag, r)

  COLORCPP_CHANNEL_ACCESSOR(prophoto_rgb::channel::g_tag, g)

  COLORCPP_CHANNEL_ACCESSOR(prophoto_rgb::channel::b_tag, b)

  COLORCPP_CHANNEL_ACCESSOR(prophoto_rgb::channel::a_tag, a)
};

/** @brief ProPhoto RGB with float channels. */
using prophoto_rgbf_t = basic_prophoto_rgb<prophoto_rgb::model::prophoto_rgb>;
/** @brief ProPhoto RGBA with float channels. */
using prophoto_rgbaf_t = basic_prophoto_rgb<prophoto_rgb::model::prophoto_rgba>;
/** @brief Linear ProPhoto RGB with float channels. */
using linear_prophoto_rgbf_t = basic_prophoto_rgb<prophoto_rgb::model::linear_prophoto_rgbf>;
/** @brief Linear ProPhoto RGBA with float channels. */
using linear_prophoto_rgbaf_t = basic_prophoto_rgb<prophoto_rgb::model::linear_prophoto_rgbaf>;

COLORCPP_STREAM_OPERATORS(colorcpp::core::basic_prophoto_rgb)

}  // namespace colorcpp::core

COLORCPP_ENABLE_STRUCTURED_BINDINGS(colorcpp::core::basic_prophoto_rgb)
COLORCPP_ENABLE_HASH(colorcpp::core::basic_prophoto_rgb)
