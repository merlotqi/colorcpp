/**
 * @file channel_compare.hpp
 * @brief Channel-wise comparison utilities for color comparison.
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/operations/compare/detail/float_utils.hpp>
#include <tuple>
#include <type_traits>

namespace colorcpp::operations::compare::detail {

/**
 * @brief Compare two values exactly.
 * @tparam T Value type.
 * @param a First value.
 * @param b Second value.
 * @return True if values are exactly equal.
 */
template <typename T>
constexpr bool channel_exact(T a, T b) noexcept {
  return a == b;
}

/**
 * @brief Compare two values within epsilon tolerance.
 * @tparam T Value type.
 * @param a First value.
 * @param b Second value.
 * @param eps Epsilon tolerance.
 * @return True if values are within epsilon.
 */
template <typename T>
constexpr bool channel_epsilon(T a, T b, T eps) noexcept {
  if constexpr (std::is_floating_point_v<T>) {
    return float_equal(a, b, eps);
  } else {
    // For integer types, use exact comparison with tolerance
    const T diff = (a > b) ? (a - b) : (b - a);
    return diff <= eps;
  }
}

/**
 * @brief Compare two values within relative tolerance with an absolute floor.
 *
 * The tolerance is a real ratio (@c double), not @p T: on integer channels a
 * value_type tolerance would truncate every ratio below 100% to zero.
 *
 * @tparam T Value type.
 * @param a First value.
 * @param b Second value.
 * @param rel Relative tolerance (as fraction of the larger value).
 * @param abs_floor Absolute tolerance that dominates when larger (default 0: pure relative).
 * @return True if values are within the combined tolerance.
 */
template <typename T>
constexpr bool channel_relative(T a, T b, double rel, double abs_floor = 0.0) noexcept {
  if constexpr (std::is_floating_point_v<T>) {
    return float_relative(a, b, static_cast<T>(rel), static_cast<T>(abs_floor));
  } else {
    // Negative rel or floor never matches, including for identical values.
    if (rel < 0.0 || abs_floor < 0.0) return false;
    // Integer channels: do the arithmetic in double so sub-100% ratios survive.
    const double diff = static_cast<double>(a > b ? a - b : b - a);
    const double max_val = static_cast<double>(a > b ? a : b);
    const double rel_bound = rel * max_val;
    const double tol = (rel_bound > abs_floor) ? rel_bound : abs_floor;
    return diff <= tol;
  }
}

/**
 * @brief Whether a channel type is a circular hue (its tag aliases @ref traits::hue_tag).
 */
template <typename Channel>
inline constexpr bool is_hue_channel_v = std::is_same_v<typename Channel::tag_type, traits::hue_tag>;

/**
 * @brief Circular distance between two hue values on the ring [0, range].
 *
 * Linear for values inside one turn; wraps out-of-range inputs down first, so
 * 0 and range are distance 0 and 359.9 vs 0 is distance 0.1. Arithmetic only
 * (no fmod) to stay constexpr in C++17.
 */
template <typename T>
constexpr T circular_diff(T a, T b, T range) noexcept {
  T d = (a > b) ? (a - b) : (b - a);
  if (range > T(0)) {
    while (d > range) d -= range;
    if (d > range - d) d = range - d;
  }
  return d;
}

/**
 * @brief Whether the color carries a shared chroma channel (HSL/HSV S, CIELCH/OkLCH C).
 */
template <typename Color>
inline constexpr bool has_chroma_channel_v =
    traits::details::has_channel_tag_impl<typename Color::channels_tuple, traits::chroma_tag>::value;

/**
 * @brief Achromatic iff the chroma channel is exactly zero.
 *
 * At zero chroma the stored hue is meaningless (a gray at h = 0 is the same
 * gray at h = 180), so policies may skip the hue comparison for pairs where
 * both sides are achromatic. Models without a chroma channel (HWB, RGB) are
 * never achromatic here and keep plain hue-ring semantics.
 */
template <typename Color>
constexpr bool is_achromatic(const Color& c) noexcept {
  if constexpr (has_chroma_channel_v<Color>) {
    return c.template get<traits::chroma_tag>() == typename Color::value_type(0);
  } else {
    return false;
  }
}

/**
 * @brief Compare one channel of two colors, dispatching on hue vs linear semantics.
 *
 * Linear channels are handed to @p linear_comp as the raw value pair. Hue
 * channels are compared on the ring: both raw values and their circular
 * distance are handed to @p hue_comp. When both colors are achromatic
 * (@ref is_achromatic), the hue pair short-circuits to true — the stored hue
 * carries no information without chroma.
 */
template <std::size_t Is, typename Color, typename LinearComp, typename HueComp>
constexpr bool compare_one_channel(const Color& a, const Color& b, LinearComp linear_comp, HueComp hue_comp) noexcept {
  using channel_t = std::tuple_element_t<Is, typename Color::channels_tuple>;
  const auto va = a.template get_index<Is>();
  const auto vb = b.template get_index<Is>();
  if constexpr (is_hue_channel_v<channel_t>) {
    if constexpr (has_chroma_channel_v<Color>) {
      if (is_achromatic(a) && is_achromatic(b)) return true;
    }
    using T = typename channel_t::value_type;
    constexpr T range = channel_t::max - channel_t::min;
    return hue_comp(va, vb, circular_diff(va, vb, range));
  } else {
    return linear_comp(va, vb);
  }
}

/**
 * @brief Helper to compare all channels of two colors.
 * @tparam LinearComp Comparison for non-hue channels (two values).
 * @tparam HueComp Comparison for hue channels (value, value, circular distance).
 * @tparam Color Color type.
 * @tparam Is Index sequence.
 * @param a First color.
 * @param b Second color.
 * @param linear_comp Linear-channel predicate.
 * @param hue_comp Hue-channel predicate.
 * @return True if all channels match according to the predicates.
 */
template <typename LinearComp, typename HueComp, typename Color, std::size_t... Is>
constexpr bool compare_channels_impl(const Color& a, const Color& b, LinearComp linear_comp, HueComp hue_comp,
                                     std::index_sequence<Is...>) noexcept {
  return (compare_one_channel<Is>(a, b, linear_comp, hue_comp) && ...);
}

/**
 * @brief True if @p Color exposes the channel-color interface used here
 *        (@c channels_tuple, channel count, indexed access).
 *
 * Guards policy entry points so non-color arguments fail with one clear
 * static_assert instead of a cascade of deep template errors.
 */
template <typename Color, typename = void>
inline constexpr bool is_channel_color_v = false;

template <typename Color>
inline constexpr bool
    is_channel_color_v<Color, std::void_t<typename Color::channels_tuple, decltype(Color::channels)>> = true;

/**
 * @brief Compare all channels of two colors (linear + hue-aware).
 * @param linear_comp Predicate for non-hue channels: (value_a, value_b).
 * @param hue_comp Predicate for hue channels: (value_a, value_b, circular_diff).
 * @return True if all channels match according to the predicates.
 */
template <typename LinearComp, typename HueComp, typename Color>
constexpr bool compare_channels(const Color& a, const Color& b, LinearComp linear_comp, HueComp hue_comp) noexcept {
  static_assert(is_channel_color_v<Color>, "colorcpp: channel comparison requires a color type (e.g. core::rgb8_t)");
  return compare_channels_impl(a, b, linear_comp, hue_comp, std::make_index_sequence<Color::channels>{});
}

}  // namespace colorcpp::operations::compare::detail
