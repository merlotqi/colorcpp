/**
 * @file aces.hpp
 * @brief Academy Color Encoding System: ACES2065-1 (AP0), ACEScg (AP1)
 *        and ACEScct signal encodings.
 *
 * - ACES2065-1 (SMPTE ST 2065-1): AP0 primaries, ACES D60 white,
 *   linear — the archival/exchange reference space. AP0 is extremely
 *   wide (its blue primary has a negative y), so XYZ values map to AP0
 *   with both positive and negative components: the type is extended
 *   range.
 * - ACEScg: AP1 primaries, ACES D60 white, linear — the rendering space;
 *   also extended range (out-of-AP1 colours are representable).
 * - ACEScct: ACEScg primaries with the S-2016-003 quasi-log grading
 *   encoding (toe + log2 branch) — a signal, so it clamps at the type
 *   boundary like other display encodings.
 *
 * All three are Bradford-adapted from the ACES D60 white (0.32168,
 * 0.33767) into the library's D65 XYZ hub, so signal (1, 1, 1) maps
 * exactly onto the reference white.
 *
 * @see Academy S-2065-1 / S-2016-003
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>
#include <colorcpp/core/linear_rgb.hpp>

namespace colorcpp::core::aces {

namespace channel {

struct r_tag {};
struct g_tag {};
struct b_tag {};
using a_tag = traits::alpha_tag;

using float_red = traits::basic_channel<r_tag, float, 0, 1>;
using float_green = traits::basic_channel<g_tag, float, 0, 1>;
using float_blue = traits::basic_channel<b_tag, float, 0, 1>;
using float_alpha = traits::basic_channel<a_tag, float, 0, 1>;

}  // namespace channel

namespace model {

struct aces2065 : traits::rgb_model_tag {};
struct aces2065a : traits::rgb_model_tag {};
struct acescg : traits::rgb_model_tag {};
struct acescga : traits::rgb_model_tag {};
struct acescct : traits::rgb_model_tag {};
struct acesccta : traits::rgb_model_tag {};

}  // namespace model

}  // namespace colorcpp::core::aces

namespace colorcpp::traits {

template <>
struct model_traits<core::aces::model::aces2065> {
  using channels_type =
      std::tuple<core::aces::channel::float_red, core::aces::channel::float_green, core::aces::channel::float_blue>;
  static constexpr std::string_view prefix = "aces2065";
  static constexpr bool extended_range = true;  // AP0 spans negatives
};

template <>
struct model_traits<core::aces::model::aces2065a> {
  using channels_type = std::tuple<core::aces::channel::float_red, core::aces::channel::float_green,
                                   core::aces::channel::float_blue, core::aces::channel::float_alpha>;
  static constexpr std::string_view prefix = "aces2065a";
  static constexpr bool extended_range = true;  // AP0 spans negatives
};

template <>
struct model_traits<core::aces::model::acescg> {
  using channels_type =
      std::tuple<core::aces::channel::float_red, core::aces::channel::float_green, core::aces::channel::float_blue>;
  static constexpr std::string_view prefix = "acescg";
  static constexpr bool extended_range = true;  // out-of-AP1 colours are representable
};

template <>
struct model_traits<core::aces::model::acescga> {
  using channels_type = std::tuple<core::aces::channel::float_red, core::aces::channel::float_green,
                                   core::aces::channel::float_blue, core::aces::channel::float_alpha>;
  static constexpr std::string_view prefix = "acescga";
  static constexpr bool extended_range = true;  // out-of-AP1 colours are representable
};

template <>
struct model_traits<core::aces::model::acescct> {
  using channels_type =
      std::tuple<core::aces::channel::float_red, core::aces::channel::float_green, core::aces::channel::float_blue>;
  static constexpr std::string_view prefix = "acescct";
  // grading signal: clamps at the boundary like other display encodings
};

template <>
struct model_traits<core::aces::model::acesccta> {
  using channels_type = std::tuple<core::aces::channel::float_red, core::aces::channel::float_green,
                                   core::aces::channel::float_blue, core::aces::channel::float_alpha>;
  static constexpr std::string_view prefix = "acesccta";
  // grading signal: clamps at the boundary like other display encodings
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief ACES family with @c r(), @c g(), @c b(), and @c a() when alpha is present.
 * @tparam Model aces::model::aces2065, aces2065a, acescg, acescga,
 *         acescct, or acesccta.
 */
template <typename Model>
struct basic_aces : basic_color<Model> {
  using base = basic_color<Model>;

  using base::base;
  using base::data;

 public:
  COLORCPP_CHANNEL_ACCESSOR(aces::channel::r_tag, r)

  COLORCPP_CHANNEL_ACCESSOR(aces::channel::g_tag, g)

  COLORCPP_CHANNEL_ACCESSOR(aces::channel::b_tag, b)

  COLORCPP_CHANNEL_ACCESSOR(aces::channel::a_tag, a)
};

/** @brief ACES2065-1 (AP0, linear) with float channels. */
using aces2065_rgbf_t = basic_aces<aces::model::aces2065>;
/** @brief ACES2065-1 (AP0, linear) with float channels and alpha. */
using aces2065_rgbaf_t = basic_aces<aces::model::aces2065a>;
/** @brief ACEScg (AP1, linear) with float channels. */
using acescg_rgbf_t = basic_aces<aces::model::acescg>;
/** @brief ACEScg (AP1, linear) with float channels and alpha. */
using acescg_rgbaf_t = basic_aces<aces::model::acescga>;
/** @brief ACEScct (AP1, S-2016-003 log) with float channels. */
using acescct_rgbf_t = basic_aces<aces::model::acescct>;
/** @brief ACEScct (AP1, S-2016-003 log) with float channels and alpha. */
using acescct_rgbaf_t = basic_aces<aces::model::acesccta>;

COLORCPP_STREAM_OPERATORS(colorcpp::core::basic_aces)

}  // namespace colorcpp::core

COLORCPP_ENABLE_STRUCTURED_BINDINGS(colorcpp::core::basic_aces)
COLORCPP_ENABLE_HASH(colorcpp::core::basic_aces)
