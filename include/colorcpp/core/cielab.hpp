/**
 * @file cielab.hpp
 * @brief CIELAB and CIELCH (ISO 11664-4 / ICC), D65 reference white.
 *
 * CIELAB: L* [0, 200], a* and b* [-400, 400]. CIELCH: L* [0, 200], C* [0, 400], H [0, 360).
 * The ranges are nominal: they must be wide enough for every real color
 * (ProPhoto green has a* ≈ -201, C* ≈ 263) and for above-white XYZ
 * (Y = 1.5 → L* ≈ 116.8), so conversions never clip to them (issue #67).
 * Conversions compose with linear sRGB and XYZ
 * (include @ref linear_rgb.hpp with @ref conversion.hpp).
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>
#include <tuple>

namespace colorcpp::core::lab {

namespace channel {

struct l_tag {};                   // lightness
struct a_tag {};                   // green-red axis (CIELAB)
struct b_tag {};                   // blue-yellow axis (CIELAB)
using c_tag = traits::chroma_tag;  // chroma (CIELCH, shared chroma tag)
using h_tag = traits::hue_tag;     // hue angle (CIELCH, shared ring tag)

// basic_channel<Tag, T, Min, Max, Den>: value = static_cast<T>(Min) / static_cast<T>(Den) ... Max/Den
// Den=1 keeps values in their natural CIE units rather than normalizing to [0,1].
// The a/b/c and L bounds are nominal (real colors reach a* ≈ -201, C* ≈ 263;
// above-white XYZ Y = 2 → L* ≈ 130): ±128 / 200 / 100 clipped conversions.
using lab_l = traits::basic_channel<l_tag, float, 0, 200, 1>;     // [0,    200]
using lab_a = traits::basic_channel<a_tag, float, -400, 400, 1>;  // [-400,  400]
using lab_b = traits::basic_channel<b_tag, float, -400, 400, 1>;  // [-400,  400]
using lab_c = traits::basic_channel<c_tag, float, 0, 400, 1>;     // [0,    400]
using lab_h = traits::basic_channel<h_tag, float, 0, 360, 1>;     // [0,    360]

}  // namespace channel

namespace model {

struct cielab {};
struct cielch {};

}  // namespace model

}  // namespace colorcpp::core::lab

namespace colorcpp::traits {

template <>
struct model_traits<core::lab::model::cielab> {
  using channels_type = std::tuple<core::lab::channel::lab_l, core::lab::channel::lab_a, core::lab::channel::lab_b>;
  static constexpr std::string_view prefix = "lab";
};

template <>
struct model_traits<core::lab::model::cielch> {
  using channels_type = std::tuple<core::lab::channel::lab_l, core::lab::channel::lab_c, core::lab::channel::lab_h>;
  static constexpr std::string_view prefix = "lch";
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief CIELAB or CIELCH: @c l(); @c a(), @c b() for LAB; @c c(), @c h() for LCH.
 * @tparam Model lab::model::cielab or cielch.
 */
template <typename Model>
struct basic_lab : basic_color<Model> {
  using base = basic_color<Model>;

  using base::base;
  using base::data;

 public:
  // L* — shared by both CIELAB and CIELCH
  COLORCPP_CHANNEL_ACCESSOR(lab::channel::l_tag, l)

  // a* — CIELAB only
  COLORCPP_CHANNEL_ACCESSOR(lab::channel::a_tag, a)

  // b* — CIELAB only
  COLORCPP_CHANNEL_ACCESSOR(lab::channel::b_tag, b)

  // C* — CIELCH only
  COLORCPP_CHANNEL_ACCESSOR(lab::channel::c_tag, c)

  // H — CIELCH only
  COLORCPP_CHANNEL_ACCESSOR(lab::channel::h_tag, h)
};

/** @brief CIE L*a*b* (D65). */
using cielab_t = basic_lab<lab::model::cielab>;
/** @brief CIE L*C*h* cylindrical LAB (D65). */
using cielch_t = basic_lab<lab::model::cielch>;

COLORCPP_STREAM_OPERATORS(colorcpp::core::basic_lab)

}  // namespace colorcpp::core

COLORCPP_ENABLE_STRUCTURED_BINDINGS(colorcpp::core::basic_lab)
COLORCPP_ENABLE_HASH(colorcpp::core::basic_lab)
