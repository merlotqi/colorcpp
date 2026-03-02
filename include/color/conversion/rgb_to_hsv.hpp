/**
 * @file rgb_to_hsv.hpp
 * @brief RGB to HSV color space conversion
 *
 * Provides compile-time conversion from RGB color space to HSV color space.
 * Uses the standard RGB to HSV transformation algorithm.
 *
 * @author Merlot.Qi
 *
 */

#pragma once

#include <cmath>
#include <cstdint>

#include "../core/hsv.hpp"
#include "../core/rgb.hpp"
#include "../utils/maths.hpp"

namespace color::conversion {

namespace details {

template <typename T, intptr_t Scale>
constexpr core::hsv_int_t convert(const core::basic_rgb<T, Scale>& rgb) {
  double r_f{0}, g_f{0}, b_f{0};
  if constexpr (std::is_integral_v<T> && Scale == 1) {
    r_f = static_cast<double>(rgb.r) / 255.0;
    g_f = static_cast<double>(rgb.g) / 255.0;
    b_f = static_cast<double>(rgb.b) / 255.0;
  } else {
    constexpr double sc = static_cast<double>(Scale);
    r_f = static_cast<double>(rgb.r) / sc;
    g_f = static_cast<double>(rgb.g) / sc;
    b_f = static_cast<double>(rgb.b) / sc;
  }

  double max_v = (r_f > g_f) ? ((r_f > b_f) ? r_f : b_f) : ((g_f > b_f) ? g_f : b_f);
  double min_v = (r_f < g_f) ? ((r_f < b_f) ? r_f : b_f) : ((g_f < b_f) ? g_f : b_f);
  double delta = max_v - min_v;

  double h = 0.0;
  if (delta > 1e-7) {
    if (max_v == r_f) {
      h = 60.0 * maths::fmod((g_f - b_f) / delta, 6.0);
    } else if (max_v == g_f) {
      h = 60.0 * ((b_f - r_f) / delta + 2.0);
    } else {
      h = 60.0 * ((r_f - g_f) / delta + 4.0);
    }
    if (h < 0.0) h += 360.0;
  }

  double s = (max_v > 1e-7) ? (delta / max_v) : 0.0;

  double v = max_v;

  return core::hsv_int_t(maths::round<int>(h), maths::round<int>(s * 100.0), maths::round<int>(v * 100.0));
}

}  // namespace details

template <typename RGBType>
inline constexpr core::hsv_int_t rgb_to_hsv_v = details::convert(RGBType{});

template <typename T, intptr_t Scale>
constexpr core::hsv_int_t convert(const core::basic_rgb<T, Scale>& rgb) {
  return details::convert(rgb);
}

}  // namespace color::conversion
