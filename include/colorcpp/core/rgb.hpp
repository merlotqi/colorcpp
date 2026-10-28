/**
 * @file rgb.hpp
 * @brief sRGB-style RGB and RGBA: 8-bit channels or float channels in [0, 1].
 *
 * Order: R, G, B (optional A). Gamma-encoded for 8-bit and float types here; use @ref linear_rgb.hpp
 * for scene-linear sRGB primaries.
 */

#pragma once

#include <cassert>
#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>
#include <cstdint>
#include <tuple>
#include <type_traits>

namespace colorcpp::core::rgb {

namespace channel {

struct r_tag {};
struct g_tag {};
struct b_tag {};
using a_tag = traits::alpha_tag;

using u8_red = traits::basic_channel<r_tag, uint8_t, 0, 255, 1>;
using u8_green = traits::basic_channel<g_tag, uint8_t, 0, 255, 1>;
using u8_blue = traits::basic_channel<b_tag, uint8_t, 0, 255, 1>;
using u8_alpha = traits::basic_channel<a_tag, uint8_t, 0, 255, 1>;

using float_red = traits::basic_channel<r_tag, float, 0, 1>;
using float_green = traits::basic_channel<g_tag, float, 0, 1>;
using float_blue = traits::basic_channel<b_tag, float, 0, 1>;
using float_alpha = traits::basic_channel<a_tag, float, 0, 1>;

}  // namespace channel

namespace model {

struct rgb8 : traits::rgb_model_tag {};
struct rgba8 : traits::rgb_model_tag {};

struct rgb_float : traits::rgb_model_tag {};
struct rgba_float : traits::rgb_model_tag {};

}  // namespace model

}  // namespace colorcpp::core::rgb

namespace colorcpp::traits {

template <>
struct model_traits<core::rgb::model::rgb8> {
  using channels_type =
      std::tuple<core::rgb::channel::u8_red, core::rgb::channel::u8_green, core::rgb::channel::u8_blue>;
  static constexpr std::string_view prefix = "rgb";
};

template <>
struct model_traits<core::rgb::model::rgba8> {
  using channels_type = std::tuple<core::rgb::channel::u8_red, core::rgb::channel::u8_green,
                                   core::rgb::channel::u8_blue, core::rgb::channel::u8_alpha>;
  static constexpr std::string_view prefix = "rgba";
};

template <>
struct model_traits<core::rgb::model::rgb_float> {
  using channels_type =
      std::tuple<core::rgb::channel::float_red, core::rgb::channel::float_green, core::rgb::channel::float_blue>;
  static constexpr std::string_view prefix = "rgbf";
};

template <>
struct model_traits<core::rgb::model::rgba_float> {
  using channels_type = std::tuple<core::rgb::channel::float_red, core::rgb::channel::float_green,
                                   core::rgb::channel::float_blue, core::rgb::channel::float_alpha>;
  static constexpr std::string_view prefix = "rgbaf";
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief RGB/RGBA color with @c r(), @c g(), @c b(), and @c a() when alpha is present.
 * @tparam Model One of rgb::model::rgb8, rgba8, rgb_float, rgba_float.
 */
template <typename Model>
struct basic_rgba : basic_color<Model> {
  using base = basic_color<Model>;

  using base::base;
  using base::data;

 public:
  COLORCPP_CHANNEL_ACCESSOR(rgb::channel::r_tag, r)

  COLORCPP_CHANNEL_ACCESSOR(rgb::channel::g_tag, g)

  COLORCPP_CHANNEL_ACCESSOR(rgb::channel::b_tag, b)

  COLORCPP_CHANNEL_ACCESSOR(rgb::channel::a_tag, a)
};

/** @brief 8-bit RGB, channels 0–255. */
using rgb8_t = basic_rgba<rgb::model::rgb8>;
/** @brief 8-bit RGBA, alpha 0–255. */
using rgba8_t = basic_rgba<rgb::model::rgba8>;

/** @brief Float RGB in [0, 1]. */
using rgbf_t = basic_rgba<rgb::model::rgb_float>;
/** @brief Float RGBA in [0, 1]. */
using rgbaf_t = basic_rgba<rgb::model::rgba_float>;

COLORCPP_STREAM_OPERATORS(colorcpp::core::basic_rgba)

}  // namespace colorcpp::core

COLORCPP_ENABLE_STRUCTURED_BINDINGS(colorcpp::core::basic_rgba)
COLORCPP_ENABLE_HASH(colorcpp::core::basic_rgba)
