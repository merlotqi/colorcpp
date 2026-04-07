/**
 * @file cmyk.hpp
 * @brief CMYK (Cyan-Magenta-Yellow-Key) conversion functions.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/cmyk.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

/**
 * @brief Convert CMYK to RGB.
 * @note CMYK channels are C, M, Y, K — channel 3 is K (black), NOT alpha.
 */
template <typename To, typename From>
constexpr To cmyk_to_rgb(const From& src) {
  float c = to_unit<From, 0>(src.template get_index<0>());
  float m = to_unit<From, 1>(src.template get_index<1>());
  float y = to_unit<From, 2>(src.template get_index<2>());
  float k = to_unit<From, 3>(src.template get_index<3>());

  float r = (1.0f - c) * (1.0f - k);
  float g = (1.0f - m) * (1.0f - k);
  float b = (1.0f - y) * (1.0f - k);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(1.0f));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

/**
 * @brief Convert RGB to CMYK.
 */
template <typename To, typename From>
constexpr To rgb_to_cmyk(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());

  float k = 1.0f - std::max({r, g, b});
  float denom = 1.0f - k;
  float c = (denom > 0.0f) ? (1.0f - r - k) / denom : 0.0f;
  float m = (denom > 0.0f) ? (1.0f - g - k) / denom : 0.0f;
  float y = (denom > 0.0f) ? (1.0f - b - k) / denom : 0.0f;

  return pack_to<To>(from_unit<To, 0>(c), from_unit<To, 1>(m), from_unit<To, 2>(y), from_unit<To, 3>(k));
}

}  // namespace colorcpp::operations::conversion::details
