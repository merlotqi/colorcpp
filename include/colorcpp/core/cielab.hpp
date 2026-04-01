/**
 * @file cielab.hpp
 * @brief CIELAB and CIELCH (ISO 11664-4 / ICC), D65 reference white.
 *
 * CIELAB: L* [0, 100], a* and b* [-128, 128]. CIELCH: L*, C* [0, 200], H [0, 360).
 * Conversions compose with linear sRGB and XYZ (include @ref linear_rgb.hpp with @ref conversion.hpp).
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>
#include <tuple>
#include <type_traits>

namespace colorcpp::core::lab {

namespace channel {

struct l_tag {};  // lightness
struct a_tag {};  // green-red axis (CIELAB)
struct b_tag {};  // blue-yellow axis (CIELAB)
struct c_tag {};  // chroma (CIELCH)
struct h_tag {};  // hue angle (CIELCH)

// basic_channel<Tag, T, Min, Max, Den>: value = static_cast<T>(Min) / static_cast<T>(Den) ... Max/Den
// Den=1 keeps values in their natural CIE units rather than normalizing to [0,1].
using lab_l = traits::basic_channel<l_tag, float, 0, 100, 1>;     // [0,    100]
using lab_a = traits::basic_channel<a_tag, float, -128, 128, 1>;  // [-128,  128]
using lab_b = traits::basic_channel<b_tag, float, -128, 128, 1>;  // [-128,  128]
using lab_c = traits::basic_channel<c_tag, float, 0, 200, 1>;     // [0,    200]
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

/**
 * @brief CIELAB or CIELCH: @c l(); @c a(), @c b() for LAB; @c c(), @c h() for LCH.
 * @tparam Model lab::model::cielab or cielch.
 */
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

/** @brief CIE L*a*b* (D65). */
using cielab_t = basic_lab<lab::model::cielab>;
/** @brief CIE L*C*h* cylindrical LAB (D65). */
using cielch_t = basic_lab<lab::model::cielch>;

}  // namespace colorcpp::core

// I/O operators for basic_lab
template <typename Model>
std::ostream& operator<<(std::ostream& os, const colorcpp::core::basic_lab<Model>& c) {
  return colorcpp::core::io::operator<<(os, static_cast<const colorcpp::core::basic_color<Model>&>(c));
}

template <typename Model>
std::istream& operator>>(std::istream& is, colorcpp::core::basic_lab<Model>& c) {
  return colorcpp::core::io::operator>>(is, static_cast<colorcpp::core::basic_color<Model>&>(c));
}
