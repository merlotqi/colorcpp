/**
 * @file detail.hpp
 * @brief Helper functions for color space conversion.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <tuple>
#include <type_traits>
#include <utility>

namespace colorcpp::operations::conversion::details {

template <typename T>
struct extract_model;

template <template <typename...> class Color, typename Model, typename... Args>
struct extract_model<Color<Model, Args...>> {
  using type = Model;
};

template <typename T>
using extract_model_t = typename extract_model<T>::type;

// V is deduced from the call argument (e.g. uint8_t for rgb8, float for rgbf).
// Using a deduced V instead of Color::value_type makes to_unit work correctly
// with heterogeneous-channel models where each channel has its own scalar type.
template <typename Color, std::size_t I, typename V>
constexpr float to_unit(V v) {
  using channel_t = std::tuple_element_t<I, typename Color::channels_tuple>;
  return static_cast<float>(v - channel_t::min) / static_cast<float>(channel_t::max - channel_t::min);
}

// Maps a unit-normalized value u ∈ [0,1] back to channel I's [min, max] range.
// The clamp is applied in unit space to guard against floating-point arithmetic
// producing values like 1.0000001; it does NOT clip out-of-gamut channel values.
// Returns channel I's exact value_type (supports heterogeneous-channel models).
template <typename Color, std::size_t I>
constexpr typename Color::template channel_value_t<I> from_unit(float v) {
  using channel_t = std::tuple_element_t<I, typename Color::channels_tuple>;
  using value_t = typename Color::template channel_value_t<I>;
  v = std::clamp(v, 0.0f, 1.0f);
  float val = v * static_cast<float>(channel_t::max - channel_t::min) + static_cast<float>(channel_t::min);
  if constexpr (std::is_integral_v<value_t>)
    return static_cast<value_t>(std::round(val));
  else
    return static_cast<value_t>(val);
}

// Maps a raw channel value (already in the channel's natural units, possibly signed)
// directly to the target type, clamping to the channel's [min, max].
// Use this instead of from_unit when the value is NOT in [0,1] unit space —
// e.g., when writing Oklab a/b channels that live in [-0.5, 0.5].
template <typename Color, std::size_t I>
constexpr typename Color::template channel_value_t<I> from_value(float v) {
  using channel_t = std::tuple_element_t<I, typename Color::channels_tuple>;
  using value_t = typename Color::template channel_value_t<I>;
  float clamped = std::clamp(v, static_cast<float>(channel_t::min), static_cast<float>(channel_t::max));
  if constexpr (std::is_integral_v<value_t>)
    return static_cast<value_t>(std::round(clamped));
  else
    return static_cast<value_t>(clamped);
}

/**
 * @brief Returns the alpha channel as a unit [0,1] value.
 *
 * @warning Only use for types where channel 3 is alpha:
 *   - RGBA, HSLA, HSVA, HWBA, OkLabA, CIELABA, Display P3A, etc.
 *
 * @note Do NOT use for:
 *   - CMYK (channel 3 is K/black, not alpha)
 *   - Any type where channel 3 is not alpha
 *
 * @tparam From Source color type (must have channel 3 as alpha if 4+ channels).
 * @param src Source color value.
 * @return Alpha value in [0, 1] range, or 1.0 if no alpha channel.
 */
template <typename From>
constexpr float get_src_alpha(const From& src) {
  if constexpr (From::channels >= 4) {
    // Note: This assumes channel 3 is alpha.
    // For CMYK, channel 3 is K (black) - do not use this function!
    return to_unit<From, 3>(src.template get_index<3>());
  } else {
    return 1.0f;
  }
}

template <typename To, std::size_t... Is, typename... Args>
constexpr To pack_to_impl(std::index_sequence<Is...>, Args&&... args) {
  auto tp = std::forward_as_tuple(std::forward<Args>(args)...);
  return To{std::get<Is>(std::move(tp))...};
}

template <typename To, typename... Args>
constexpr To pack_to(Args&&... args) {
  if constexpr (sizeof...(args) > To::channels) {
    return pack_to_impl<To>(std::make_index_sequence<To::channels>{}, std::forward<Args>(args)...);
  } else {
    return To{std::forward<Args>(args)...};
  }
}

// Helper: read channel I from src if it exists, else return a default (for alpha expansion).
// Uses if constexpr so get_index<I> is never instantiated on a type with fewer channels.
template <typename To, typename From, std::size_t I>
constexpr typename To::template channel_value_t<I> channel_or_default(const From& src, float default_val) {
  if constexpr (I < From::channels)
    return from_unit<To, I>(to_unit<From, I>(src.template get_index<I>()));
  else
    return from_unit<To, I>(default_val);
}

template <typename To, typename From, std::size_t... Is>
constexpr To same_model_cast_impl(const From& src, std::index_sequence<Is...>) {
  return pack_to<To>(channel_or_default<To, From, Is>(src, 1.0f)...);
}

/** 8-bit sRGB (3 channels) → float sRGB (3 channels). */
template <typename To, typename From>
constexpr To integral_srgb_to_float_srgb(const From& src) {
  static_assert(From::channels == 3 && To::channels == 3);
  return pack_to<To>(from_unit<To, 0>(to_unit<From, 0>(src.template get_index<0>())),
                     from_unit<To, 1>(to_unit<From, 1>(src.template get_index<1>())),
                     from_unit<To, 2>(to_unit<From, 2>(src.template get_index<2>())));
}

/** Float sRGB (3 channels) → 8-bit sRGB (3 channels). */
template <typename To, typename From>
constexpr To float_srgb_to_integral_srgb(const From& src) {
  static_assert(From::channels == 3 && To::channels == 3);
  return pack_to<To>(from_unit<To, 0>(to_unit<From, 0>(src.template get_index<0>())),
                     from_unit<To, 1>(to_unit<From, 1>(src.template get_index<1>())),
                     from_unit<To, 2>(to_unit<From, 2>(src.template get_index<2>())));
}

/** 8-bit sRGBA → float sRGBA. */
template <typename To, typename From>
constexpr To integral_srgba_to_float_srgba(const From& src) {
  static_assert(From::channels == 4 && To::channels == 4);
  return pack_to<To>(from_unit<To, 0>(to_unit<From, 0>(src.template get_index<0>())),
                     from_unit<To, 1>(to_unit<From, 1>(src.template get_index<1>())),
                     from_unit<To, 2>(to_unit<From, 2>(src.template get_index<2>())),
                     from_unit<To, 3>(to_unit<From, 3>(src.template get_index<3>())));
}

/** Float sRGBA → 8-bit sRGBA. */
template <typename To, typename From>
constexpr To float_srgba_to_integral_srgba(const From& src) {
  static_assert(From::channels == 4 && To::channels == 4);
  return pack_to<To>(from_unit<To, 0>(to_unit<From, 0>(src.template get_index<0>())),
                     from_unit<To, 1>(to_unit<From, 1>(src.template get_index<1>())),
                     from_unit<To, 2>(to_unit<From, 2>(src.template get_index<2>())),
                     from_unit<To, 3>(to_unit<From, 3>(src.template get_index<3>())));
}

}  // namespace colorcpp::operations::conversion::details
