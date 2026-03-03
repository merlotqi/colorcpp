/**
 * @file hsl_to_hsv.hpp
 * @brief HSL to HSV color space conversion
 *
 * Provides compile-time conversion from HSL color space to HSV color space.
 * Uses direct mathematical transformation between the two color models.
 *
 * @author Merlot.Qi
 *
 */

#pragma once

#include <cmath>
#include <color/core/hsl.hpp>
#include <color/core/hsv.hpp>
#include <color/utils/maths.hpp>
#include <cstdint>

namespace color::conversion {

namespace details {

template <typename T, intptr_t Scale>
constexpr core::hsv_int_t convert(const core::basic_hsl<T, Scale>& hsl) {
  constexpr double sc = static_cast<double>(Scale);
  double h_f = static_cast<double>(hsl.h);
  double s_hsl = static_cast<double>(hsl.s) / sc;
  double l_f = static_cast<double>(hsl.l) / sc;

  double min_l_1l = (l_f < (1.0 - l_f)) ? l_f : (1.0 - l_f);
  double v = l_f + s_hsl * min_l_1l;

  double s_hsv = 0.0;
  if (v > 1e-7) {
    s_hsv = 2.0 * (1.0 - l_f / v);
  }

  return core::hsv_int_t(maths::round<int>(h_f), maths::round<int>(s_hsv * 100.0), maths::round<int>(v * 100.0));
}

}  // namespace details

template <typename HSLType>
inline constexpr core::hsv_int_t hsl_to_hsv_v = details::convert(HSLType{});

template <typename T, intptr_t Scale>
constexpr core::hsv_int_t convert(const core::basic_hsl<T, Scale>& hsl) {
  return details::convert(hsl);
}

}  // namespace color::conversion
