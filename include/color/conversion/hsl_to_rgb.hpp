/**
 * @file hsl_to_rgb.hpp
 * @brief HSL to RGB color space conversion
 *
 * Provides compile-time conversion from HSL color space to RGB color space.
 * Uses the standard HSL to RGB transformation algorithm.
 *
 * @author Merlot.Qi
 *
 */

#pragma once

#include <cmath>
#include <cstdint>

#include "../core/hsl.hpp"
#include "../core/rgb.hpp"
#include "../utils/maths.hpp"

namespace color::conversion {

namespace details {

template <typename T, intptr_t Scale>
constexpr core::rgb8_t convert(const core::basic_hsl<T, Scale>& hsl) {
  constexpr double sc = static_cast<double>(Scale);
  double h_f = static_cast<double>(hsl.h);
  double s_f = static_cast<double>(hsl.s) / sc;
  double l_f = static_cast<double>(hsl.l) / sc;

  double c = (1.0 - maths::abs(2.0 * l_f - 1.0)) * s_f;
  double x = c * (1.0 - maths::abs(maths::fmod(h_f / 60.0, 2.0) - 1.0));
  double m = l_f - c / 2.0;

  double rt = 0, gt = 0, bt = 0;

  int sector = static_cast<int>(h_f / 60.0) % 6;
  switch (sector) {
    case 0:
      rt = c;
      gt = x;
      bt = 0;
      break;  // 0-60°
    case 1:
      rt = x;
      gt = c;
      bt = 0;
      break;  // 60-120°
    case 2:
      rt = 0;
      gt = c;
      bt = x;
      break;  // 120-180°
    case 3:
      rt = 0;
      gt = x;
      bt = c;
      break;  // 180-240°
    case 4:
      rt = x;
      gt = 0;
      bt = c;
      break;  // 240-300°
    default:
      rt = c;
      gt = 0;
      bt = x;
      break;  // 300-360°
  }

  return core::rgb8_t(maths::round<uint8_t>((rt + m) * 255.0), maths::round<uint8_t>((gt + m) * 255.0),
                      maths::round<uint8_t>((bt + m) * 255.0));
}

}  // namespace details

template <typename HSLType>
inline constexpr core::rgb8_t hsl_to_rgb_v = details::convert(HSLType{});

template <typename T, intptr_t Scale>
constexpr core::rgb8_t convert(const core::basic_hsl<T, Scale>& hsl) {
  return details::convert(hsl);
}

}  // namespace color::conversion
