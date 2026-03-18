#pragma once

#include <colorcpp/core/color_base.hpp>
#include <tuple>
#include <type_traits>

// Oklab (Björn Ottosson, 2020): a perceptually uniform color space designed
// as a practical replacement for CIELAB. Channel ranges follow CSS Color Level 4:
//   L: [0, 1]       — lightness (0=black, 1=white)
//   a: [-0.5, 0.5]  — green(-) / red(+) axis
//   b: [-0.5, 0.5]  — blue(-) / yellow(+) axis
//
// OkLCH is the cylindrical form of Oklab:
//   L: [0, 1]       — same lightness
//   C: [0, 0.4]     — chroma (distance from achromatic axis; sRGB max ≈ 0.32)
//   H: [0, 360)     — hue angle in degrees
//
// Conversion path: sRGB ↔ Linear sRGB ↔ Oklab ↔ OkLCH
// (requires linear_rgb.hpp to be included before conversion.hpp)

namespace colorcpp::core::oklab {

namespace channel {

struct l_tag {};  // lightness
struct a_tag {};  // green-red axis (Oklab)
struct b_tag {};  // blue-yellow axis (Oklab)
struct c_tag {};  // chroma (OkLCH)
struct h_tag {};  // hue angle (OkLCH)

// basic_channel<Tag, T, Min, Max, Den>: value = Min/Den … Max/Den
using okl = traits::basic_channel<l_tag, float, 0, 1>;          //  [0,    1]
using oka = traits::basic_channel<a_tag, float, -50, 50, 100>;  // [-0.5, 0.5]
using okb = traits::basic_channel<b_tag, float, -50, 50, 100>;  // [-0.5, 0.5]
using okc = traits::basic_channel<c_tag, float, 0, 40, 100>;    //  [0,   0.4]
using okh = traits::basic_channel<h_tag, float, 0, 360, 1>;     //  [0, 360]

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
  static constexpr std::size_t channel_size = 3;
};

template <>
struct model_traits<core::oklab::model::oklch> {
  using channels_type = std::tuple<core::oklab::channel::okl, core::oklab::channel::okc, core::oklab::channel::okh>;
  static constexpr std::string_view prefix = "oklch";
  static constexpr std::size_t channel_size = 3;
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

template <typename Model>
struct basic_oklab : basic_color<Model> {
  using base = basic_color<Model>;

  using base::base;
  using base::data;

 private:
  template <typename Tag>
  constexpr auto& channel() {
    constexpr std::size_t idx = traits::channel_index_v<Model, Tag>;
    return std::get<idx>(data);
  }

  template <typename Tag>
  constexpr const auto& channel() const {
    constexpr std::size_t idx = traits::channel_index_v<Model, Tag>;
    return std::get<idx>(data);
  }

 public:
  // L — shared by both Oklab and OkLCH
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, oklab::channel::l_tag>>>
  constexpr auto& l() {
    return channel<oklab::channel::l_tag>();
  }
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, oklab::channel::l_tag>>>
  constexpr const auto& l() const {
    return channel<oklab::channel::l_tag>();
  }

  // a — Oklab only
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, oklab::channel::a_tag>>>
  constexpr auto& a() {
    return channel<oklab::channel::a_tag>();
  }
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, oklab::channel::a_tag>>>
  constexpr const auto& a() const {
    return channel<oklab::channel::a_tag>();
  }

  // b — Oklab only
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, oklab::channel::b_tag>>>
  constexpr auto& b() {
    return channel<oklab::channel::b_tag>();
  }
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, oklab::channel::b_tag>>>
  constexpr const auto& b() const {
    return channel<oklab::channel::b_tag>();
  }

  // c — OkLCH only
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, oklab::channel::c_tag>>>
  constexpr auto& c() {
    return channel<oklab::channel::c_tag>();
  }
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, oklab::channel::c_tag>>>
  constexpr const auto& c() const {
    return channel<oklab::channel::c_tag>();
  }

  // h — OkLCH only
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, oklab::channel::h_tag>>>
  constexpr auto& h() {
    return channel<oklab::channel::h_tag>();
  }
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, oklab::channel::h_tag>>>
  constexpr const auto& h() const {
    return channel<oklab::channel::h_tag>();
  }
};

using oklab_t = basic_oklab<oklab::model::oklab>;
using oklch_t = basic_oklab<oklab::model::oklch>;

}  // namespace colorcpp::core
