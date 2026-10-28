/**
 * @file oklab.hpp
 * @brief Oklab and OkLCH (Björn Ottosson); ranges aligned with CSS Color Level 4.
 *
 * Oklab: L [0, 1], a and b in [-0.5, 0.5]. OkLCH: L, C [0, 0.4], H [0, 360).
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>
#include <tuple>
#include <type_traits>

namespace colorcpp::core::oklab {

namespace channel {

struct l_tag {};  // lightness
struct a_tag {};  // green-red axis (Oklab)
struct b_tag {};  // blue-yellow axis (Oklab)
using c_tag = traits::chroma_tag;  // chroma (OkLCH, shared chroma tag)
using h_tag = traits::hue_tag;  // hue angle (OkLCH, shared ring tag)

// basic_channel<Tag, T, Min, Max, Den>: value = Min/Den … Max/Den
// a/b/c nominal ranges are deliberately wider than any in-gamut value: CSS has no
// upper chroma bound (0.4 / ±0.4 are only the relative-syntax 100% references) and
// wide-gamut intermediates exceed the old ±0.5 / 0.4 limits.
using okl = traits::basic_channel<l_tag, float, 0, 1>;    // [0,    1]
using oka = traits::basic_channel<a_tag, float, -1, 1>;   // [-1,   1]
using okb = traits::basic_channel<b_tag, float, -1, 1>;   // [-1,   1]
using okc = traits::basic_channel<c_tag, float, 0, 1>;    // [0,    1]
using okh = traits::basic_channel<h_tag, float, 0, 360>;  // [0, 360]

}  // namespace channel

namespace model {

struct oklab {};
struct oklch {};

}  // namespace model

}  // namespace colorcpp::core::oklab

namespace colorcpp::traits {

template <>
struct model_traits<core::oklab::model::oklab> {
  using channels_type = std::tuple<core::oklab::channel::okl, core::oklab::channel::oka, core::oklab::channel::okb>;
  static constexpr std::string_view prefix = "oklab";
};

template <>
struct model_traits<core::oklab::model::oklch> {
  using channels_type = std::tuple<core::oklab::channel::okl, core::oklab::channel::okc, core::oklab::channel::okh>;
  static constexpr std::string_view prefix = "oklch";
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief Oklab or OkLCH: @c l(); @c a(), @c b() for Oklab; @c c(), @c h() for OkLCH.
 * @tparam Model oklab::model::oklab or oklch.
 */
template <typename Model>
struct basic_oklab : basic_color<Model> {
  using base = basic_color<Model>;

  using base::base;
  using base::data;

 public:
  // L — shared by both Oklab and OkLCH
  COLORCPP_CHANNEL_ACCESSOR(oklab::channel::l_tag, l)

  // a — Oklab only
  COLORCPP_CHANNEL_ACCESSOR(oklab::channel::a_tag, a)

  // b — Oklab only
  COLORCPP_CHANNEL_ACCESSOR(oklab::channel::b_tag, b)

  // c — OkLCH only
  COLORCPP_CHANNEL_ACCESSOR(oklab::channel::c_tag, c)

  // h — OkLCH only
  COLORCPP_CHANNEL_ACCESSOR(oklab::channel::h_tag, h)
};

/** @brief Oklab rectangular coordinates. */
using oklab_t = basic_oklab<oklab::model::oklab>;
/** @brief OkLCH cylindrical coordinates. */
using oklch_t = basic_oklab<oklab::model::oklch>;

COLORCPP_STREAM_OPERATORS(colorcpp::core::basic_oklab)

}  // namespace colorcpp::core

COLORCPP_ENABLE_STRUCTURED_BINDINGS(colorcpp::core::basic_oklab)
COLORCPP_ENABLE_HASH(colorcpp::core::basic_oklab)
