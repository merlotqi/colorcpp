/**
 * @file dci_p3.hpp
 * @brief DCI-P3 (theatrical) colour space: P3 primaries, D62 white,
 *        gamma 2.6.
 *
 * Distinct from Display P3 (@ref display_p3f_t), which shares the
 * primaries but uses the D65 white and the sRGB transfer function:
 * DCI-P3 carries the cinema D62 white point, which is Bradford-adapted
 * into the library's D65 XYZ hub (the ProPhoto D50 precedent), and the
 * gamma 2.6 theatrical EOTF.
 *
 * @see SMPTE RP 431-2 (DCI P3 projection practice)
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>
#include <colorcpp/core/linear_rgb.hpp>

namespace colorcpp::core::dci_p3 {

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

struct dci_p3 : traits::rgb_model_tag {};
struct dci_p3a : traits::rgb_model_tag {};

}  // namespace model

}  // namespace colorcpp::core::dci_p3

namespace colorcpp::traits {

template <>
struct model_traits<core::dci_p3::model::dci_p3> {
  using channels_type = std::tuple<core::dci_p3::channel::float_red, core::dci_p3::channel::float_green,
                                   core::dci_p3::channel::float_blue>;
  static constexpr std::string_view prefix = "dci-p3";
};

template <>
struct model_traits<core::dci_p3::model::dci_p3a> {
  using channels_type = std::tuple<core::dci_p3::channel::float_red, core::dci_p3::channel::float_green,
                                   core::dci_p3::channel::float_blue, core::dci_p3::channel::float_alpha>;
  static constexpr std::string_view prefix = "dci-p3a";
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief DCI-P3 family with @c r(), @c g(), @c b(), and @c a() when alpha is present.
 * @tparam Model dci_p3::model::dci_p3 or dci_p3a.
 */
template <typename Model>
struct basic_dci_p3 : basic_color<Model> {
  using base = basic_color<Model>;

  using base::base;
  using base::data;

 public:
  COLORCPP_CHANNEL_ACCESSOR(dci_p3::channel::r_tag, r)

  COLORCPP_CHANNEL_ACCESSOR(dci_p3::channel::g_tag, g)

  COLORCPP_CHANNEL_ACCESSOR(dci_p3::channel::b_tag, b)

  COLORCPP_CHANNEL_ACCESSOR(dci_p3::channel::a_tag, a)
};

/** @brief DCI-P3 signal with float channels. */
using dci_p3_rgbf_t = basic_dci_p3<dci_p3::model::dci_p3>;
/** @brief DCI-P3 signal with float channels and alpha. */
using dci_p3_rgbaf_t = basic_dci_p3<dci_p3::model::dci_p3a>;

COLORCPP_STREAM_OPERATORS(colorcpp::core::basic_dci_p3)

}  // namespace colorcpp::core

COLORCPP_ENABLE_STRUCTURED_BINDINGS(colorcpp::core::basic_dci_p3)
COLORCPP_ENABLE_HASH(colorcpp::core::basic_dci_p3)
