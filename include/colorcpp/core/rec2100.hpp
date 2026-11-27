/**
 * @file rec2100.hpp
 * @brief ITU-R BT.2100 signal encodings: PQ (ST 2084) and HLG on BT.2020
 *        primaries with the D65 white point.
 *
 * BT.2100 defines two signal encodings of the same BT.2020 primaries:
 *   - PQ (Perceptual Quantizer, SMPTE ST 2084): absolute — code values map
 *     to cd/m^2 via the PQ EOTF. Under this library's scale (relative
 *     Y = 1 maps to the 100 cd/m^2 reference white) diffuse white encodes
 *     to PQ(100) = 0.5080784.
 *   - HLG (Hybrid Log-Gamma, ARIB B-67): scene-referred — the OETF maps
 *     scene linear light [0, 1] to signal [0, 1]. The display OOTF
 *     (BT.2100 §7.2, which depends on the display peak luminance and
 *     system gamma) is deliberately out of scope; apply it downstream if
 *     display-referred HLG is required.
 *
 * Channels are the signal code values in [0, 1] (alpha rides along). The
 * linear counterparts live in @ref linear_rec2020_rgbf_t — same primaries,
 * no transfer function.
 *
 * @see https://www.itu.int/rec/R-REC-BT.2100
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>
#include <colorcpp/core/linear_rgb.hpp>

namespace colorcpp::core::rec2100 {

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

struct rec2100_pq : traits::rgb_model_tag {};
struct rec2100_pqa : traits::rgb_model_tag {};
struct rec2100_hlg : traits::rgb_model_tag {};
struct rec2100_hlga : traits::rgb_model_tag {};

}  // namespace model

}  // namespace colorcpp::core::rec2100

namespace colorcpp::traits {

template <>
struct model_traits<core::rec2100::model::rec2100_pq> {
  using channels_type =
      std::tuple<core::rec2100::channel::f32_red, core::rec2100::channel::f32_green, core::rec2100::channel::f32_blue>;
  static constexpr std::string_view prefix = "rec2100-pq";
};

template <>
struct model_traits<core::rec2100::model::rec2100_pqa> {
  using channels_type = std::tuple<core::rec2100::channel::f32_red, core::rec2100::channel::f32_green,
                                   core::rec2100::channel::f32_blue, core::rec2100::channel::f32_alpha>;
  static constexpr std::string_view prefix = "rec2100-pqa";
};

template <>
struct model_traits<core::rec2100::model::rec2100_hlg> {
  using channels_type =
      std::tuple<core::rec2100::channel::f32_red, core::rec2100::channel::f32_green, core::rec2100::channel::f32_blue>;
  static constexpr std::string_view prefix = "rec2100-hlg";
};

template <>
struct model_traits<core::rec2100::model::rec2100_hlga> {
  using channels_type = std::tuple<core::rec2100::channel::f32_red, core::rec2100::channel::f32_green,
                                   core::rec2100::channel::f32_blue, core::rec2100::channel::f32_alpha>;
  static constexpr std::string_view prefix = "rec2100-hlga";
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief BT.2100 PQ/HLG family with @c r(), @c g(), @c b(), and @c a() when alpha is present.
 * @tparam Model rec2100::model::rec2100_pq, rec2100_pqa, rec2100_hlg, or rec2100_hlga.
 */
template <typename Model>
struct basic_rec2100 : basic_color<Model> {
  using base = basic_color<Model>;

  using base::base;
  using base::data;

 public:
  COLORCPP_CHANNEL_ACCESSOR(rec2100::channel::r_tag, r)

  COLORCPP_CHANNEL_ACCESSOR(rec2100::channel::g_tag, g)

  COLORCPP_CHANNEL_ACCESSOR(rec2100::channel::b_tag, b)

  COLORCPP_CHANNEL_ACCESSOR(rec2100::channel::a_tag, a)
};

/** @brief BT.2100 PQ signal with float channels. */
using rec2100_pq_rgbf_t = basic_rec2100<rec2100::model::rec2100_pq>;
/** @brief BT.2100 PQ signal with float channels and alpha. */
using rec2100_pqa_rgbaf_t = basic_rec2100<rec2100::model::rec2100_pqa>;
/** @brief BT.2100 HLG signal with float channels. */
using rec2100_hlg_rgbf_t = basic_rec2100<rec2100::model::rec2100_hlg>;
/** @brief BT.2100 HLG signal with float channels and alpha. */
using rec2100_hlga_rgbaf_t = basic_rec2100<rec2100::model::rec2100_hlga>;

COLORCPP_STREAM_OPERATORS(colorcpp::core::basic_rec2100)

}  // namespace colorcpp::core

COLORCPP_ENABLE_STRUCTURED_BINDINGS(colorcpp::core::basic_rec2100)
COLORCPP_ENABLE_HASH(colorcpp::core::basic_rec2100)
