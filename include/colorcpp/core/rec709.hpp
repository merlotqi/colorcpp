/**
 * @file rec709.hpp
 * @brief Rec.709 signal encoding: sRGB primaries with the BT.1886
 *        pure gamma 2.4 transfer function.
 *
 * Rec.709 shares its primaries and D65 white point with sRGB — only the
 * transfer function differs: pure gamma 2.4 (BT.1886 display EOTF with
 * zero black lift, also the curve CSS Color 4 section 10.8 assigns to
 * rec2020) instead of the sRGB piecewise curve. The linear counterpart
 * is @ref linear_rgbf_t (same primaries, same white, no transfer
 * function), so the conversion edge is a pure TF swap.
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>
#include <colorcpp/core/linear_rgb.hpp>

namespace colorcpp::core::rec709 {

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

struct rec709 : traits::rgb_model_tag {};
struct rec709a : traits::rgb_model_tag {};

}  // namespace model

}  // namespace colorcpp::core::rec709

namespace colorcpp::traits {

template <>
struct model_traits<core::rec709::model::rec709> {
  using channels_type = std::tuple<core::rec709::channel::float_red, core::rec709::channel::float_green,
                                   core::rec709::channel::float_blue>;
  static constexpr std::string_view prefix = "rec709";
};

template <>
struct model_traits<core::rec709::model::rec709a> {
  using channels_type = std::tuple<core::rec709::channel::float_red, core::rec709::channel::float_green,
                                   core::rec709::channel::float_blue, core::rec709::channel::float_alpha>;
  static constexpr std::string_view prefix = "rec709a";
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief Rec.709 family with @c r(), @c g(), @c b(), and @c a() when alpha is present.
 * @tparam Model rec709::model::rec709 or rec709a.
 */
template <typename Model>
struct basic_rec709 : basic_color<Model> {
  using base = basic_color<Model>;

  using base::base;
  using base::data;

 public:
  COLORCPP_CHANNEL_ACCESSOR(rec709::channel::r_tag, r)

  COLORCPP_CHANNEL_ACCESSOR(rec709::channel::g_tag, g)

  COLORCPP_CHANNEL_ACCESSOR(rec709::channel::b_tag, b)

  COLORCPP_CHANNEL_ACCESSOR(rec709::channel::a_tag, a)
};

/** @brief Rec.709 signal with float channels. */
using rec709_rgbf_t = basic_rec709<rec709::model::rec709>;
/** @brief Rec.709 signal with float channels and alpha. */
using rec709_rgbaf_t = basic_rec709<rec709::model::rec709a>;

COLORCPP_STREAM_OPERATORS(colorcpp::core::basic_rec709)

}  // namespace colorcpp::core

COLORCPP_ENABLE_STRUCTURED_BINDINGS(colorcpp::core::basic_rec709)
COLORCPP_ENABLE_HASH(colorcpp::core::basic_rec709)
