/**
 * @file hsv_to_rgb.hpp
 * @brief HSV to RGB color space conversion
 *
 * Provides compile-time conversion from HSV color space to RGB color space.
 * Uses the standard HSV to RGB transformation algorithm.
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
constexpr core::rgb8_t convert(const core::basic_hsv<T, Scale>& hsv) {
  constexpr double sc = static_cast<double>(Scale);

  double s_f = static_cast<double>(hsv.s) / sc;
  double v_f = static_cast<double>(hsv.v) / sc;

  double h_f = static_cast<double>(hsv.h);

  double c = v_f * s_f;
  double h_prime = h_f / 60.0;
  double x = c * (1.0 - maths::abs(maths::fmod(h_prime, 2.0) - 1.0));
  double m = v_f - c;

  double rt = 0, gt = 0, bt = 0;
  int sector = static_cast<int>(h_prime) % 6;

  switch (sector) {
    case 0:
      rt = c;
      gt = x;
      bt = 0;
      break;
    case 1:
      rt = x;
      gt = c;
      bt = 0;
      break;
    case 2:
      rt = 0;
      gt = c;
      bt = x;
      break;
    case 3:
      rt = 0;
      gt = x;
      bt = c;
      break;
    case 4:
      rt = x;
      gt = 0;
      bt = c;
      break;
    default:
      rt = c;
      gt = 0;
      bt = x;
      break;
  }

  return core::rgb8_t(maths::round<uint8_t>((rt + m) * 255.0), maths::round<uint8_t>((gt + m) * 255.0),
                      maths::round<uint8_t>((bt + m) * 255.0));
}
}  // namespace details

template <typename HSVType>
inline constexpr core::rgb8_t hsv_to_rgb_v = details::convert(HSVType{});

template <typename T, intptr_t Scale>
constexpr core::rgb8_t convert(const core::basic_hsv<T, Scale>& hsv) {
  return details::convert(hsv);
}

}  // namespace color::conversion
