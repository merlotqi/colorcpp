/**
 * @file ictcp.hpp
 * @brief ICtCp perceptual colour encoding (ITU-R BT.2100 PQ / Dolby 2016).
 *
 * Intensity I in [0, 1] is PQ-coded (absolute luminance; under this
 * library's scale relative Y = 1 maps to the 100 cd/m^2 reference white, so
 * white encodes to PQ(100) = 0.5080784), Ct/Cp are the tritan/protan
 * opponent chroma axes. Formerly an internal detail of
 * algorithms/delta_e/itp.hpp; as a core model it joins the conversion graph.
 *
 * @see Lu et al. (2016). A Perceptual EOTF for High Dynamic Range Content.
 * @see ITU-R BT.2100 / BT.2124.
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>

namespace colorcpp::core::ictcp {

namespace channel {

struct i_tag {};
struct ct_tag {};
struct cp_tag {};

using f32_i = traits::basic_channel<i_tag, float, 0, 1>;     // PQ intensity [0, 1]
using f32_ct = traits::basic_channel<ct_tag, float, -1, 1>;  // tritan axis (nominal)
using f32_cp = traits::basic_channel<cp_tag, float, -1, 1>;  // protan axis (nominal)

}  // namespace channel

namespace model {

struct ictcp {};

}  // namespace model

}  // namespace colorcpp::core::ictcp

namespace colorcpp::traits {

template <>
struct model_traits<core::ictcp::model::ictcp> {
  using channels_type =
      std::tuple<core::ictcp::channel::f32_i, core::ictcp::channel::f32_ct, core::ictcp::channel::f32_cp>;
  static constexpr std::string_view prefix = "ictcp";
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief ICtCp family with @c i(), @c ct(), and @c cp() accessors.
 * @tparam Model ictcp::model::ictcp.
 */
template <typename Model>
struct basic_ictcp : basic_color<Model> {
  using base = basic_color<Model>;

  using base::base;
  using base::data;

 public:
  COLORCPP_CHANNEL_ACCESSOR(ictcp::channel::i_tag, i)

  COLORCPP_CHANNEL_ACCESSOR(ictcp::channel::ct_tag, ct)

  COLORCPP_CHANNEL_ACCESSOR(ictcp::channel::cp_tag, cp)
};

/** @brief ICtCp (BT.2100 PQ). */
using ictcp_t = basic_ictcp<ictcp::model::ictcp>;

COLORCPP_STREAM_OPERATORS(colorcpp::core::basic_ictcp)

}  // namespace colorcpp::core

COLORCPP_ENABLE_STRUCTURED_BINDINGS(colorcpp::core::basic_ictcp)
COLORCPP_ENABLE_HASH(colorcpp::core::basic_ictcp)
