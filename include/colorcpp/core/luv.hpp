/**
 * @file luv.hpp
 * @brief CIE L*u*v* (ISO 11664-5), D65 reference white.
 *
 * Channels: L* [0, 100], u* and v* [-400, 400]. The u* and v* range is
 * nominal: it must be wide enough for every real color (sRGB red already
 * has u* ≈ 175, ProPhoto red ≈ 331), so conversions never clip to it.
 * Uses the CIE 1976 u'v' uniformity diagram (u' = 4X/(X+15Y+3Z),
 * v' = 9Y/(X+15Y+3Z)). Conversion composes with XYZ
 * (include @ref xyz.hpp with @ref conversion.hpp).
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>
#include <tuple>

namespace colorcpp::core::luv {

namespace channel {

struct l_tag {};  // lightness
struct u_tag {};  // red-green opponent axis
struct v_tag {};  // yellow-blue opponent axis

// Den=1 keeps values in their natural CIE units. The u/v bounds are nominal:
// real colors reach u* ≈ 331 (ProPhoto red), so ±128 clipped conversions.
using luv_l = traits::basic_channel<l_tag, float, 0, 100, 1>;     // [0,    100]
using luv_u = traits::basic_channel<u_tag, float, -400, 400, 1>;  // [-400,  400]
using luv_v = traits::basic_channel<v_tag, float, -400, 400, 1>;  // [-400,  400]

}  // namespace channel

namespace model {

struct cieluv {};

}  // namespace model

}  // namespace colorcpp::core::luv

namespace colorcpp::traits {

template <>
struct model_traits<core::luv::model::cieluv> {
  using channels_type = std::tuple<core::luv::channel::luv_l, core::luv::channel::luv_u, core::luv::channel::luv_v>;
  static constexpr std::string_view prefix = "luv";
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief CIE L*u*v* with @c l(), @c u(), @c v().
 * @tparam Model luv::model::cieluv (D65).
 */
template <typename Model>
struct basic_luv : basic_color<Model> {
  using base = basic_color<Model>;

  using base::base;
  using base::data;

 public:
  COLORCPP_CHANNEL_ACCESSOR(luv::channel::l_tag, l)

  COLORCPP_CHANNEL_ACCESSOR(luv::channel::u_tag, u)

  COLORCPP_CHANNEL_ACCESSOR(luv::channel::v_tag, v)
};

/** @brief CIE L*u*v* under D65. */
using luv_t = basic_luv<luv::model::cieluv>;

COLORCPP_STREAM_OPERATORS(colorcpp::core::basic_luv)

}  // namespace colorcpp::core

COLORCPP_ENABLE_STRUCTURED_BINDINGS(colorcpp::core::basic_luv)
COLORCPP_ENABLE_HASH(colorcpp::core::basic_luv)
