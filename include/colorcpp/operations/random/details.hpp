/**
 * @file details.hpp
 * @brief Helper traits for random color generation.
 *
 * Used by @ref basic_random_generator and related templates; see @ref random.hpp for thread-safety and gamut notes.
 */

#pragma once

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

  static constexpr value_type hue_max() { return max_at<0>(); }
};

}  // namespace colorcpp::operations::random::details
