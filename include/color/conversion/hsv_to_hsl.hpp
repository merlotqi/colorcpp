/**
 * @file hsv_to_hsl.hpp
 * @brief HSV to HSL color space conversion
 *
 * Provides compile-time conversion from HSV color space to HSL color space.
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
constexpr core::hsl_int_t convert(const core::basic_hsv<T, Scale>& hsv) {
  constexpr double sc = static_cast<double>(Scale);
  double h_f = static_cast<double>(hsv.h);
  double s_hsv = static_cast<double>(hsv.s) / sc;
  double v_f = static_cast<double>(hsv.v) / sc;

  double l = v_f * (1.0 - s_hsv / 2.0);

  double s_hsl = 0.0;
  double min_l = (l < (1.0 - l)) ? l : (1.0 - l);

  if (min_l > 1e-7) {
    s_hsl = (v_f - l) / min_l;
  }

  return core::hsl_int_t(maths::round<int>(h_f), maths::round<int>(s_hsl * 100.0), maths::round<int>(l * 100.0));
}

}  // namespace details

template <typename HSVType>
inline constexpr core::hsl_int_t hsv_to_hsl_v = details::convert(HSVType{});

template <typename T, intptr_t Scale>
constexpr core::hsl_int_t convert(const core::basic_hsv<T, Scale>& hsv) {
  return details::convert(hsv);
}

}  // namespace color::conversion
