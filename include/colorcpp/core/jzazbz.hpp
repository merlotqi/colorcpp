/**
 * @file jzazbz.hpp
 * @brief Jzazbz perceptual colour space for HDR / wide gamut (Safdar et al. 2017), D65 white.
 *
 * Channels: Jz [0, 1] (lightness), az and bz [-1, 1] (opponent axes).
 * Input XYZ must be under D65. Uses a modified SMPTE ST 2084 PQ encoding
 * (exponent m2 re-optimized to 1.7 * 2523 / 32). Conversion composes with
 * XYZ (include @ref xyz.hpp with @ref conversion.hpp).
 *
 * @see Safdar, Cui, Kim, Luo (2017). Perceptually uniform color space for
 *      image signals including high dynamic range and wide gamut. Optics Express.
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>
#include <tuple>

namespace colorcpp::core::jzazbz {

namespace channel {

struct jz_tag {};  // lightness
struct az_tag {};  // red-green opponent axis
struct bz_tag {};  // yellow-blue opponent axis

// Den=1 keeps values in their natural units.
using jzazbz_jz = traits::basic_channel<jz_tag, float, 0, 1, 1>;   // [0,  1]
using jzazbz_az = traits::basic_channel<az_tag, float, -1, 1, 1>;  // [-1, 1]
using jzazbz_bz = traits::basic_channel<bz_tag, float, -1, 1, 1>;  // [-1, 1]

}  // namespace channel

namespace model {

struct jzazbz {};

}  // namespace model

}  // namespace colorcpp::core::jzazbz

namespace colorcpp::traits {

template <>
struct model_traits<core::jzazbz::model::jzazbz> {
  using channels_type =
      std::tuple<core::jzazbz::channel::jzazbz_jz, core::jzazbz::channel::jzazbz_az, core::jzazbz::channel::jzazbz_bz>;
  static constexpr std::string_view prefix = "jzazbz";
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief Jzazbz with @c jz(), @c az(), @c bz().
 * @tparam Model jzazbz::model::jzazbz (D65).
 */
template <typename Model>
struct basic_jzazbz : basic_color<Model> {
  using base = basic_color<Model>;

  using base::base;
  using base::data;

 public:
  COLORCPP_CHANNEL_ACCESSOR(jzazbz::channel::jz_tag, jz)

  COLORCPP_CHANNEL_ACCESSOR(jzazbz::channel::az_tag, az)

  COLORCPP_CHANNEL_ACCESSOR(jzazbz::channel::bz_tag, bz)
};

/** @brief Jzazbz (Safdar 2017) under D65. */
using jzazbz_t = basic_jzazbz<jzazbz::model::jzazbz>;

COLORCPP_STREAM_OPERATORS(colorcpp::core::basic_jzazbz)

}  // namespace colorcpp::core

COLORCPP_ENABLE_STRUCTURED_BINDINGS(colorcpp::core::basic_jzazbz)
COLORCPP_ENABLE_HASH(colorcpp::core::basic_jzazbz)
