/**
 * @file scrgb.hpp
 * @brief scRGB (IEC 61966-2-1): linear sRGB primaries with an extended
 *        range for rendering headroom.
 *
 * scRGB shares the sRGB primaries and D65 white, so its XYZ matrix is the
 * linear-sRGB matrix; what distinguishes the type is the signal semantics
 * — negatives and values above 1 are first-class (compositing headroom),
 * carried through the graph without clamping (extended range, issue #76).
 *
 * Following colour-science, the reference white is the diffuse white
 * (Y = 1): the spec's absolute "1.0 = 80 cd/m^2" figure is not modeled —
 * absolute anchoring in this library happens at the PQ step
 * (@ref colorcpp::operations::conversion::details::kRelativeWhiteNits),
 * and treating scRGB as relative keeps white at 1.0 like every other
 * linear type.
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>
#include <colorcpp/core/linear_rgb.hpp>

namespace colorcpp::core::scrgb {

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

struct scrgb : traits::rgb_model_tag {};
struct scrgba : traits::rgb_model_tag {};

}  // namespace model

}  // namespace colorcpp::core::scrgb

namespace colorcpp::traits {

template <>
struct model_traits<core::scrgb::model::scrgb> {
  using channels_type =
      std::tuple<core::scrgb::channel::float_red, core::scrgb::channel::float_green, core::scrgb::channel::float_blue>;
  static constexpr std::string_view prefix = "scrgb";
  static constexpr bool extended_range = true;  // negatives / >1 pass through
};

template <>
struct model_traits<core::scrgb::model::scrgba> {
  using channels_type = std::tuple<core::scrgb::channel::float_red, core::scrgb::channel::float_green,
                                   core::scrgb::channel::float_blue, core::scrgb::channel::float_alpha>;
  static constexpr std::string_view prefix = "scrgba";
  static constexpr bool extended_range = true;  // negatives / >1 pass through
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief scRGB family with @c r(), @c g(), @c b(), and @c a() when alpha is present.
 * @tparam Model scrgb::model::scrgb or scrgba.
 */
template <typename Model>
struct basic_scrgb : basic_color<Model> {
  using base = basic_color<Model>;

  using base::base;
  using base::data;

 public:
  COLORCPP_CHANNEL_ACCESSOR(scrgb::channel::r_tag, r)

  COLORCPP_CHANNEL_ACCESSOR(scrgb::channel::g_tag, g)

  COLORCPP_CHANNEL_ACCESSOR(scrgb::channel::b_tag, b)

  COLORCPP_CHANNEL_ACCESSOR(scrgb::channel::a_tag, a)
};

/** @brief scRGB with float channels (extended range). */
using scrgb_rgbf_t = basic_scrgb<scrgb::model::scrgb>;
/** @brief scRGB with float channels and alpha (extended range). */
using scrgb_rgbaf_t = basic_scrgb<scrgb::model::scrgba>;

COLORCPP_STREAM_OPERATORS(colorcpp::core::basic_scrgb)

}  // namespace colorcpp::core

COLORCPP_ENABLE_STRUCTURED_BINDINGS(colorcpp::core::basic_scrgb)
COLORCPP_ENABLE_HASH(colorcpp::core::basic_scrgb)
