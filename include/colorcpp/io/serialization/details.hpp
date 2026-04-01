/**
 * @file details.hpp
 * @brief Internal helpers for serialization: channel iteration, value extraction.
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/io/serialization/traits.hpp>
#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>

namespace colorcpp::io::serialization::details {

/**
 * @brief Convert a channel value to double for serialization.
 */
template <typename T>
constexpr double to_double(T val) {
  if constexpr (std::is_same_v<T, uint8_t>) {
    return static_cast<double>(val) / 255.0;
  } else if constexpr (std::is_same_v<T, int8_t>) {
    return static_cast<double>(val);
  } else {
    return static_cast<double>(val);
  }
}

/**
 * @brief Convert double back to a channel value.
 */
template <typename T>
constexpr T from_double(double val) {
  if constexpr (std::is_same_v<T, uint8_t>) {
    double v = val * 255.0;
    if (v < 0.0) v = 0.0;
    if (v > 255.0) v = 255.0;
    return static_cast<T>(static_cast<int>(v + 0.5));
  } else if constexpr (std::is_same_v<T, int8_t>) {
    if (val < -128.0) val = -128.0;
    if (val > 127.0) val = 127.0;
    return static_cast<T>(static_cast<int>(val));
  } else {
    return static_cast<T>(val);
  }
}

/**
 * @brief Extract all channel values from a color as doubles.
 */
template <typename Color, std::size_t... Is>
void extract_channels_impl(const Color& c, double* out, std::index_sequence<Is...>) {
  ((out[Is] = to_double(c.template get_index<Is>())), ...);
}

/**
 * @brief Extract all channel values from a color into an array.
 */
template <typename Color>
void extract_channels(const Color& c, double* out) {
  extract_channels_impl(c, out, std::make_index_sequence<Color::channels>{});
}

/**
 * @brief Apply double values to construct a color.
 */
template <typename Color, std::size_t... Is>
Color apply_channels_impl(const double* vals, std::index_sequence<Is...>) {
  return Color{from_double<typename Color::template channel_value_t<Is>>(vals[Is])...};
}

/**
 * @brief Construct a color from an array of double values.
 */
template <typename Color>
Color apply_channels(const double* vals) {
  return apply_channels_impl<Color>(vals, std::make_index_sequence<Color::channels>{});
}

/**
 * @brief Get default channel names for a model (generic: ch0, ch1, ...).
 */
inline std::string default_channel_name(std::size_t index) { return "ch" + std::to_string(index); }

}  // namespace colorcpp::io::serialization::details
