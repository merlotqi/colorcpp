/**
 * @file hwb.hpp
 * @brief HWB (Hue-Whiteness-Blackness) conversion functions.
 * @see CSS Color Level 4 §4.5
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/hwb.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

/**
 * @brief Convert HWB to RGB.
 */
template <typename To, typename From>
constexpr To hwb_to_rgb(const From& src) {
  float h = to_unit<From, 0>(src.template get_index<0>());  // [0,1] from [0,360]
  float w = to_unit<From, 1>(src.template get_index<1>());  // [0,1]
  float b = to_unit<From, 2>(src.template get_index<2>());  // [0,1]
  float a = get_src_alpha(src);

  // If whiteness + blackness >= 1, the color is achromatic
  if (w + b >= 1.0f) {
    float grayf = w / (w + b);
    if constexpr (To::channels >= 4)
      return pack_to<To>(from_unit<To, 0>(grayf), from_unit<To, 1>(grayf), from_unit<To, 2>(grayf), from_unit<To, 3>(a));
    else
      return pack_to<To>(from_unit<To, 0>(grayf), from_unit<To, 1>(grayf), from_unit<To, 2>(grayf));
  }

  // Convert HWB to HSV first
  float s = 1.0f - w / (1.0f - b);
  float v = 1.0f - b;

  // Now convert HSV to RGB
  int i = static_cast<int>(h * 6.0f);
  float f = h * 6.0f - static_cast<float>(i);
  float p = v * (1.0f - s);
  float q = v * (1.0f - f * s);
  float t = v * (1.0f - (1.0f - f) * s);

  float r = 0, g = 0, bl = 0;
  switch (i % 6) {
    case 0:
      r = v;
      g = t;
      bl = p;
      break;
    case 1:
      r = q;
      g = v;
      bl = p;
      break;
    case 2:
      r = p;
      g = v;
      bl = t;
      break;
    case 3:
      r = p;
      g = q;
      bl = v;
      break;
    case 4:
      r = t;
      g = p;
      bl = v;
      break;
    case 5:
      r = v;
      g = p;
      bl = q;
      break;
  }

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(bl), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(bl));
}

/**
 * @brief Convert RGB to HWB.
 */
template <typename To, typename From>
constexpr To rgb_to_hwb(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());
  float a = get_src_alpha(src);

  float w = std::min({r, g, b});
  float bk = 1.0f - std::max({r, g, b});

  // Hue: same as HSV hue calculation
  float max = std::max({r, g, b}), min = std::min({r, g, b});
  float d = max - min;
  float h = 0.0f;

  if (max != min) {
    if (max == r)
      h = (g - b) / d + (g < b ? 6.0f : 0.0f);
    else if (max == g)
      h = (b - r) / d + 2.0f;
    else
      h = (r - g) / d + 4.0f;
    h /= 6.0f;
  }

  // Convert h from [0,1] to [0,360]
  h = h * 360.0f;

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_value<To, 0>(h), from_unit<To, 1>(w), from_unit<To, 2>(bk), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_value<To, 0>(h), from_unit<To, 1>(w), from_unit<To, 2>(bk));
}

}  // namespace colorcpp::operations::conversion::details
