/**
 * @file rec2020.hpp
 * @brief Rec.2020 (BT.2020) color space (D65 white point, gamma 2.4 transfer).
 *
 * ITU-R BT.2020 (Rec.2020) is the UHDTV standard, covering approximately 75% of
 * the CIE 1931 visible gamut. It uses the same D65 white point as sRGB but with
 * significantly wider primaries.
 *
 * Transfer function: pure gamma 2.4 (BT.1886), per CSS Color 4 section 10.8.
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
using a_tag = traits::alpha_tag;

using f32_red = traits::basic_channel<r_tag, float, 0, 1, 1>;
using f32_green = traits::basic_channel<g_tag, float, 0, 1, 1>;
using f32_blue = traits::basic_channel<b_tag, float, 0, 1, 1>;
using f32_alpha = traits::basic_channel<a_tag, float, 0, 1, 1>;

}  // namespace channel

namespace model {

struct rec2020 : traits::rgb_model_tag {};
struct rec2020a : traits::rgb_model_tag {};
struct linear_rec2020f : traits::rgb_model_tag {};
struct linear_rec2020af : traits::rgb_model_tag {};

}  // namespace model

}  // namespace colorcpp::core::rec2020

namespace colorcpp::traits {

template <>
struct model_traits<core::rec2020::model::rec2020> {
  using channels_type =
      std::tuple<core::rec2020::channel::f32_red, core::rec2020::channel::f32_green, core::rec2020::channel::f32_blue>;
  static constexpr std::string_view prefix = "rec2020";
};

template <>
struct model_traits<core::rec2020::model::rec2020a> {
  using channels_type = std::tuple<core::rec2020::channel::f32_red, core::rec2020::channel::f32_green,
                                   core::rec2020::channel::f32_blue, core::rec2020::channel::f32_alpha>;
  static constexpr std::string_view prefix = "rec2020a";
};

template <>
struct model_traits<core::rec2020::model::linear_rec2020f> {
  using channels_type =
      std::tuple<core::rec2020::channel::f32_red, core::rec2020::channel::f32_green, core::rec2020::channel::f32_blue>;
  static constexpr std::string_view prefix = "linear-rec2020";
};

template <>
struct model_traits<core::rec2020::model::linear_rec2020af> {
  using channels_type = std::tuple<core::rec2020::channel::f32_red, core::rec2020::channel::f32_green,
                                   core::rec2020::channel::f32_blue, core::rec2020::channel::f32_alpha>;
  static constexpr std::string_view prefix = "linear-rec2020a";
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

 public:
  COLORCPP_CHANNEL_ACCESSOR(rec2020::channel::r_tag, r)

  COLORCPP_CHANNEL_ACCESSOR(rec2020::channel::g_tag, g)

  COLORCPP_CHANNEL_ACCESSOR(rec2020::channel::b_tag, b)

  COLORCPP_CHANNEL_ACCESSOR(rec2020::channel::a_tag, a)
};

/** @brief Rec.2020 with float channels. */
using rec2020_rgbf_t = basic_rec2020<rec2020::model::rec2020>;
/** @brief Rec.2020A with float channels. */
using rec2020_rgbaf_t = basic_rec2020<rec2020::model::rec2020a>;
/** @brief Linear Rec.2020 with float channels. */
using linear_rec2020_rgbf_t = basic_rec2020<rec2020::model::linear_rec2020f>;
/** @brief Linear Rec.2020A with float channels. */
using linear_rec2020_rgbaf_t = basic_rec2020<rec2020::model::linear_rec2020af>;

COLORCPP_STREAM_OPERATORS(colorcpp::core::basic_rec2020)

}  // namespace colorcpp::core

COLORCPP_ENABLE_STRUCTURED_BINDINGS(colorcpp::core::basic_rec2020)
COLORCPP_ENABLE_HASH(colorcpp::core::basic_rec2020)
