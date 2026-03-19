#pragma once

#include <colorcpp/core/color_base.hpp>
#include <tuple>
#include <type_traits>

// CIELAB (CIE L*a*b*, ISO 11664-4 / ICC):
// A perceptually uniform color space based on the CIE XYZ model.
// Reference white: D65 (Xn=0.95047, Yn=1.00000, Zn=1.08883).
//   L*: [0, 100]    — lightness (0=black, 100=white)
//   a*: [-128, 128] — green(-) / red(+) axis
//   b*: [-128, 128] — blue(-) / yellow(+) axis
//
// CIELCH is the cylindrical form of CIELAB:
//   L*: [0, 100]    — same lightness
//   C*: [0, 200]    — chroma (distance from achromatic axis; sRGB gamut ≈ [0, 130])
//   H:  [0, 360)    — hue angle in degrees
//
// Conversion path: sRGB ↔ Linear sRGB ↔ CIELAB ↔ CIELCH
// (requires linear_rgb.hpp to be included before conversion.hpp)

namespace colorcpp::core::lab {

namespace channel {

struct l_tag {};  // lightness
struct a_tag {};  // green-red axis (CIELAB)
struct b_tag {};  // blue-yellow axis (CIELAB)
struct c_tag {};  // chroma (CIELCH)
struct h_tag {};  // hue angle (CIELCH)

// basic_channel<Tag, T, Min, Max, Den>: value = static_cast<T>(Min) / static_cast<T>(Den) ... Max/Den
// Den=1 keeps values in their natural CIE units rather than normalizing to [0,1].
using lab_l = traits::basic_channel<l_tag, float, 0, 100, 1>;     //  [0,    100]
using lab_a = traits::basic_channel<a_tag, float, -128, 128, 1>;  // [-128,  128]
using lab_b = traits::basic_channel<b_tag, float, -128, 128, 1>;  // [-128,  128]
using lab_c = traits::basic_channel<c_tag, float, 0, 200, 1>;     //  [0,    200]
using lab_h = traits::basic_channel<h_tag, float, 0, 360, 1>;     //  [0,    360]

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
  static constexpr std::size_t channel_size = 3;
};

template <>
struct model_traits<core::lab::model::cielch> {
  using channels_type = std::tuple<core::lab::channel::lab_l, core::lab::channel::lab_c, core::lab::channel::lab_h>;
  static constexpr std::string_view prefix = "lch";
  static constexpr std::size_t channel_size = 3;
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

template <typename Model>
struct basic_lab : basic_color<Model> {
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
  // L* — shared by both CIELAB and CIELCH
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, lab::channel::l_tag>>>
  constexpr auto& l() {
    return channel<lab::channel::l_tag>();
  }
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, lab::channel::l_tag>>>
  constexpr const auto& l() const {
    return channel<lab::channel::l_tag>();
  }

  // a* — CIELAB only
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, lab::channel::a_tag>>>
  constexpr auto& a() {
    return channel<lab::channel::a_tag>();
  }
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, lab::channel::a_tag>>>
  constexpr const auto& a() const {
    return channel<lab::channel::a_tag>();
  }

  // b* — CIELAB only
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, lab::channel::b_tag>>>
  constexpr auto& b() {
    return channel<lab::channel::b_tag>();
  }
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, lab::channel::b_tag>>>
  constexpr const auto& b() const {
    return channel<lab::channel::b_tag>();
  }

  // C* — CIELCH only
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, lab::channel::c_tag>>>
  constexpr auto& c() {
    return channel<lab::channel::c_tag>();
  }
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, lab::channel::c_tag>>>
  constexpr const auto& c() const {
    return channel<lab::channel::c_tag>();
  }

  // H — CIELCH only
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, lab::channel::h_tag>>>
  constexpr auto& h() {
    return channel<lab::channel::h_tag>();
  }
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, lab::channel::h_tag>>>
  constexpr const auto& h() const {
    return channel<lab::channel::h_tag>();
  }
};

using cielab_t = basic_lab<lab::model::cielab>;
using cielch_t = basic_lab<lab::model::cielch>;

}  // namespace colorcpp::core
