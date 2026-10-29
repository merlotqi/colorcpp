/**
 * @file hsl.hpp
 * @brief HSL / HSLA: hue in degrees [0, 360], saturation and lightness in [0, 1] (not percentages).
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>

namespace colorcpp::core::hsl {

namespace channel {

using h_tag = traits::hue_tag;     // hue (shared ring tag)
using s_tag = traits::chroma_tag;  // saturation (shared chroma tag)
struct l_tag {};                   // lightness
using a_tag = traits::alpha_tag;   // alpha

using f32_hue = traits::basic_channel<h_tag, float, 0, 360, 1>;
using f32_saturation = traits::basic_channel<s_tag, float, 0, 1>;  // [0, 1] — not percentage
using f32_lightness = traits::basic_channel<l_tag, float, 0, 1>;   // [0, 1] — not percentage
using f32_alpha = traits::basic_channel<a_tag, float, 0, 1>;       // [0, 1] — consistent with RGB alpha

}  // namespace channel

namespace model {

struct hsl {};
struct hsla {};

}  // namespace model

}  // namespace colorcpp::core::hsl

namespace colorcpp::traits {

template <>
struct model_traits<core::hsl::model::hsl> {
  using channels_type =
      std::tuple<core::hsl::channel::f32_hue, core::hsl::channel::f32_saturation, core::hsl::channel::f32_lightness>;

  static constexpr std::string_view prefix = "hsl";
};

template <>
struct model_traits<core::hsl::model::hsla> {
  using channels_type = std::tuple<core::hsl::channel::f32_hue, core::hsl::channel::f32_saturation,
                                   core::hsl::channel::f32_lightness, core::hsl::channel::f32_alpha>;
  static constexpr std::string_view prefix = "hsla";
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief HSL/HSLA with @c h(), @c s(), @c l(), and @c a() when alpha is present.
 * @tparam Model hsl::model::hsl or hsla.
 */
template <typename Model>
struct basic_hsl : basic_color<Model> {
  using base = basic_color<Model>;

  using base::base;
  using base::data;

 public:
  COLORCPP_CHANNEL_ACCESSOR(hsl::channel::h_tag, h)

  COLORCPP_CHANNEL_ACCESSOR(hsl::channel::s_tag, s)

  COLORCPP_CHANNEL_ACCESSOR(hsl::channel::l_tag, l)

  COLORCPP_CHANNEL_ACCESSOR(hsl::channel::a_tag, a)
};

/** @brief HSL with float channels. */
using hsl_float_t = basic_hsl<hsl::model::hsl>;
/** @brief HSLA with float channels. */
using hsla_float_t = basic_hsl<hsl::model::hsla>;

COLORCPP_STREAM_OPERATORS(colorcpp::core::basic_hsl)

}  // namespace colorcpp::core

COLORCPP_ENABLE_STRUCTURED_BINDINGS(colorcpp::core::basic_hsl)
COLORCPP_ENABLE_HASH(colorcpp::core::basic_hsl)
