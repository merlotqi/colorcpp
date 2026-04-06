/**
 * @file hsv.hpp
 * @brief HSV (Hue-Saturation-Value) conversion functions.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/hsv.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

/**
 * @brief Convert HSV to RGB.
 */
template <typename To, typename From>
constexpr To hsv_to_rgb(const From& src) {
  float h = to_unit<From, 0>(src.template get_index<0>());
  float s = to_unit<From, 1>(src.template get_index<1>());
  float v = to_unit<From, 2>(src.template get_index<2>());
  float a = get_src_alpha(src);

  int i = static_cast<int>(h * 6.0f);
  float f = h * 6.0f - static_cast<float>(i);
  float p = v * (1.0f - s);
  float q = v * (1.0f - f * s);
  float t = v * (1.0f - (1.0f - f) * s);

  float r = 0, g = 0, b = 0;
  switch (i % 6) {
    case 0:
      r = v;
      g = t;
      b = p;
      break;
    case 1:
      r = q;
      g = v;
      b = p;
      break;
    case 2:
      r = p;
      g = v;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = v;
      break;
    case 4:
      r = t;
      g = p;
      b = v;
      break;
    case 5:
      r = v;
      g = p;
      b = q;
      break;
  }

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

/**
 * @brief Convert RGB to HSV.
 */
template <typename To, typename From>
constexpr To rgb_to_hsv(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());
  float a = get_src_alpha(src);

  float max = std::max({r, g, b}), min = std::min({r, g, b});
  float d = max - min;
  float h = 0.0f, s = (max == 0.0f ? 0.0f : d / max), v = max;

  if (max != min) {
    if (max == r)
      h = (g - b) / d + (g < b ? 6.0f : 0.0f);
    else if (max == g)
      h = (b - r) / d + 2.0f;
    else
      h = (r - g) / d + 4.0f;
    h /= 6.0f;
  }

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(h), from_unit<To, 1>(s), from_unit<To, 2>(v), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(h), from_unit<To, 1>(s), from_unit<To, 2>(v));
}

}  // namespace colorcpp::operations::conversion::details
