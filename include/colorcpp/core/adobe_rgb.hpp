/**
 * @file adobe_rgb.hpp
 * @brief Adobe RGB (A98-RGB) color space (D65 white point, pure Gamma 2.2).
 *
 * Adobe RGB (1998) uses the same D65 white point as sRGB but wider primaries
 * covering approximately 50% of the CIE 1931 visible gamut (vs ~35% for sRGB).
 * It's a standard working space in photography and design.
 *
 * Transfer function: pure Gamma 2.2 (no linear segment like sRGB).
 *
 * @see https://www.adobe.com/digitalimag/pdfs/AdobeRGB1998.pdf
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>
#include <colorcpp/core/linear_rgb.hpp>

namespace colorcpp::core::adobe_rgb {

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

struct adobe_rgb : traits::rgb_model_tag {};
struct adobe_rgba : traits::rgb_model_tag {};
struct linear_adobe_rgbf : traits::rgb_model_tag {};
struct linear_adobe_rgbaf : traits::rgb_model_tag {};

}  // namespace model

}  // namespace colorcpp::core::adobe_rgb

namespace colorcpp::traits {

template <>
struct model_traits<core::adobe_rgb::model::adobe_rgb> {
  using channels_type = std::tuple<core::adobe_rgb::channel::f32_red, core::adobe_rgb::channel::f32_green,
                                   core::adobe_rgb::channel::f32_blue>;
  static constexpr std::string_view prefix = "adobe-rgb";
};

template <>
struct model_traits<core::adobe_rgb::model::adobe_rgba> {
  using channels_type = std::tuple<core::adobe_rgb::channel::f32_red, core::adobe_rgb::channel::f32_green,
                                   core::adobe_rgb::channel::f32_blue, core::adobe_rgb::channel::f32_alpha>;
  static constexpr std::string_view prefix = "adobe-rgba";
};

template <>
struct model_traits<core::adobe_rgb::model::linear_adobe_rgbf> {
  using channels_type = std::tuple<core::adobe_rgb::channel::f32_red, core::adobe_rgb::channel::f32_green,
                                   core::adobe_rgb::channel::f32_blue>;
  static constexpr std::string_view prefix = "linear-adobe-rgb";
};

template <>
struct model_traits<core::adobe_rgb::model::linear_adobe_rgbaf> {
  using channels_type = std::tuple<core::adobe_rgb::channel::f32_red, core::adobe_rgb::channel::f32_green,
                                   core::adobe_rgb::channel::f32_blue, core::adobe_rgb::channel::f32_alpha>;
  static constexpr std::string_view prefix = "linear-adobe-rgba";
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief Adobe RGB / Adobe RGBA with @c r(), @c g(), @c b(), and @c a() when alpha is present.
 * @tparam Model adobe_rgb::model::adobe_rgb or adobe_rgba.
 */
template <typename Model>
struct basic_adobe_rgb : basic_color<Model> {
  using base = basic_color<Model>;

  using base::base;
  using base::data;

 public:
  COLORCPP_CHANNEL_ACCESSOR(adobe_rgb::channel::r_tag, r)

  COLORCPP_CHANNEL_ACCESSOR(adobe_rgb::channel::g_tag, g)

  COLORCPP_CHANNEL_ACCESSOR(adobe_rgb::channel::b_tag, b)

  COLORCPP_CHANNEL_ACCESSOR(adobe_rgb::channel::a_tag, a)
};

/** @brief Adobe RGB with float channels. */
using adobe_rgbf_t = basic_adobe_rgb<adobe_rgb::model::adobe_rgb>;
/** @brief Adobe RGBA with float channels. */
using adobe_rgbaf_t = basic_adobe_rgb<adobe_rgb::model::adobe_rgba>;
/** @brief Linear Adobe RGB with float channels. */
using linear_adobe_rgbf_t = basic_adobe_rgb<adobe_rgb::model::linear_adobe_rgbf>;
/** @brief Linear Adobe RGBA with float channels. */
using linear_adobe_rgbaf_t = basic_adobe_rgb<adobe_rgb::model::linear_adobe_rgbaf>;

COLORCPP_STREAM_OPERATORS(colorcpp::core::basic_adobe_rgb)

}  // namespace colorcpp::core

COLORCPP_ENABLE_STRUCTURED_BINDINGS(colorcpp::core::basic_adobe_rgb)
