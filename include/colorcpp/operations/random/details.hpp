/**
 * @file details.hpp
 * @brief Helper traits for random color generation.
 *
 * Used by @ref basic_random_generator and related templates; see @ref random.hpp for thread-safety and gamut notes.
 */

#pragma once

#include <colorcpp/core/cielab.hpp>
#include <colorcpp/core/luv.hpp>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/xyz.hpp>
#include <cstddef>
#include <tuple>
#include <type_traits>

namespace colorcpp::operations::random::details {

namespace impl {

// SFINAE helper to detect if a color type has an h() method (hue channel)
template <typename Color, typename = void>
struct has_hue_method : std::false_type {};

template <typename Color>
struct has_hue_method<Color, std::void_t<decltype(std::declval<const Color&>().h())>> : std::true_type {};

}  // namespace impl

/**
 * @brief Per-channel sampling bounds for random generation.
 *
 * Defaults to the channel's own [min, max]. Device-independent spaces override
 * it below: their channel ranges are *nominal* — deliberately widened so
 * conversions never clip (issue #67) — which makes them useless as sampling
 * boxes (most of Lab's ±400 a*, e.g., lies outside every display gamut).
 * Instead, each override samples that channel's perceptual working range.
 */
template <typename Channel>
struct sample_channel {
  static constexpr float min = static_cast<float>(Channel::min);
  static constexpr float max = static_cast<float>(Channel::max);
};

// OkLab / OkLCH: L [0, 1], a/b ±0.5, C [0, 0.4] (CSS relative-syntax 100%
// references — well inside every supported gamut).
template <>
struct sample_channel<core::oklab::channel::okl> {
  static constexpr float min = 0.0f;
  static constexpr float max = 1.0f;
};
template <>
struct sample_channel<core::oklab::channel::oka> {
  static constexpr float min = -0.5f;
  static constexpr float max = 0.5f;
};
template <>
struct sample_channel<core::oklab::channel::okb> {
  static constexpr float min = -0.5f;
  static constexpr float max = 0.5f;
};
template <>
struct sample_channel<core::oklab::channel::okc> {
  static constexpr float min = 0.0f;
  static constexpr float max = 0.4f;
};

// CIELAB / CIELCH: L* [0, 100], a*/b* ±128, C* [0, 200] — the pre-#56
// perceptual box, before the nominal ranges widened to ±400 / 400.
template <>
struct sample_channel<core::lab::channel::lab_l> {
  static constexpr float min = 0.0f;
  static constexpr float max = 100.0f;
};
template <>
struct sample_channel<core::lab::channel::lab_a> {
  static constexpr float min = -128.0f;
  static constexpr float max = 128.0f;
};
template <>
struct sample_channel<core::lab::channel::lab_b> {
  static constexpr float min = -128.0f;
  static constexpr float max = 128.0f;
};
template <>
struct sample_channel<core::lab::channel::lab_c> {
  static constexpr float min = 0.0f;
  static constexpr float max = 200.0f;
};

// CIELUV: u*/v* ±128 (nominal ±400 since #56); L* [0, 100] already matches.
template <>
struct sample_channel<core::luv::channel::luv_u> {
  static constexpr float min = -128.0f;
  static constexpr float max = 128.0f;
};
template <>
struct sample_channel<core::luv::channel::luv_v> {
  static constexpr float min = -128.0f;
  static constexpr float max = 128.0f;
};

// XYZ: nominal [-1, 2] (#56, keeps adaptation intermediates); the physical
// range of displayable colors is [0, ~1.09] — sample [0, 1] per axis.
template <>
struct sample_channel<core::xyz::channel::xyz_x> {
  static constexpr float min = 0.0f;
  static constexpr float max = 1.0f;
};
template <>
struct sample_channel<core::xyz::channel::xyz_y> {
  static constexpr float min = 0.0f;
  static constexpr float max = 1.0f;
};
template <>
struct sample_channel<core::xyz::channel::xyz_z> {
  static constexpr float min = 0.0f;
  static constexpr float max = 1.0f;
};

/**
 * @brief Color traits for random generation.
 * @tparam Color Color type.
 */
template <typename Color>
struct color_traits {
  using value_type = typename Color::value_type;
  using channels_tuple = typename Color::channels_tuple;
  static constexpr std::size_t size = Color::channels;

  // Detect hue channel by checking if h() method exists
  static constexpr bool has_hue = impl::has_hue_method<Color>::value;

  template <std::size_t I>
  static constexpr value_type min_at() {
    using channel_t = std::tuple_element_t<I, channels_tuple>;
    return channel_t::min;
  }

  template <std::size_t I>
  static constexpr value_type max_at() {
    using channel_t = std::tuple_element_t<I, channels_tuple>;
    return channel_t::max;
  }

  // Sampling bounds used by basic_random_generator: the channel range for
  // device spaces, the perceptual working range where sample_channel
  // overrides it (issue #67).
  template <std::size_t I>
  static constexpr value_type sample_min_at() {
    using channel_t = std::tuple_element_t<I, channels_tuple>;
    return static_cast<value_type>(sample_channel<channel_t>::min);
  }

  template <std::size_t I>
  static constexpr value_type sample_max_at() {
    using channel_t = std::tuple_element_t<I, channels_tuple>;
    return static_cast<value_type>(sample_channel<channel_t>::max);
  }

  static constexpr value_type hue_max() { return max_at<0>(); }
};

}  // namespace colorcpp::operations::random::details
