/**
 * @file display_p3.hpp
 * @brief Display P3 conversion functions.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/display_p3.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

template <typename To, typename From>
constexpr To display_p3_to_linear_display_p3(const From& src) {
  auto linearize = [](float v) noexcept {
    return (v <= 0.04045f) ? (v / 12.92f) : std::pow((v + 0.055f) / 1.055f, 2.4f);
  };
  float r = linearize(to_unit<From, 0>(src.template get_index<0>()));
  float g = linearize(to_unit<From, 1>(src.template get_index<1>()));
  float b = linearize(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

template <typename To, typename From>
constexpr To linear_display_p3_to_display_p3(const From& src) {
  auto gamma_encode = [](float v) noexcept {
    v = std::clamp(v, 0.0f, 1.0f);
    return (v <= 0.0031308f) ? (v * 12.92f) : (1.055f * std::pow(v, 1.0f / 2.4f) - 0.055f);
  };
  float r = gamma_encode(to_unit<From, 0>(src.template get_index<0>()));
  float g = gamma_encode(to_unit<From, 1>(src.template get_index<1>()));
  float b = gamma_encode(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

}  // namespace colorcpp::operations::conversion::details
