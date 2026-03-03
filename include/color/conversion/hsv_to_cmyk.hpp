/**
 * @file hsv_to_cmyk.hpp
 * @brief HSV to CMYK color space conversion
 *
 * Provides compile-time conversion from HSV color space to CMYK color space.
 * The conversion is performed in two steps: HSV -> RGB -> CMYK.
 *
 * @author Merlot.Qi
 *
 */

#pragma once

#include <cmath>
#include <color/core/cmyk.hpp>
#include <color/core/hsv.hpp>
#include <color/utils/maths.hpp>
#include <cstdint>

namespace color::conversion {

namespace details {

template <typename T, intptr_t Scale>
constexpr core::cmyk_int_t convert(const core::basic_hsv<T, Scale>& hsv) {
  constexpr double sc = static_cast<double>(Scale);
  double h_f = static_cast<double>(hsv.h);
  double s_f = static_cast<double>(hsv.s) / sc;
  double v_f = static_cast<double>(hsv.v) / sc;

  double c = v_f * s_f;
  double x = c * (1.0 - maths::abs(maths::fmod(h_f / 60.0, 2.0) - 1.0));
  double m = v_f - c;

  double r = 0, g = 0, b = 0;
  int sector = static_cast<int>(h_f / 60.0) % 6;

  switch (sector) {
    case 0:
      r = c;
      g = x;
      b = 0;
      break;
    case 1:
      r = x;
      g = c;
      b = 0;
      break;
    case 2:
      r = 0;
      g = c;
      b = x;
      break;
    case 3:
      r = 0;
      g = x;
      b = c;
      break;
    case 4:
      r = x;
      g = 0;
      b = c;
      break;
    default:
      r = c;
      g = 0;
      b = x;
      break;
  }

  r += m;
  g += m;
  b += m;

  double max_rgb = (r > g) ? ((r > b) ? r : b) : ((g > b) ? g : b);
  double k = 1.0 - max_rgb;

  double c_final = 0, m_final = 0, y_final = 0;
  if (max_rgb > 1e-7) {
    c_final = (1.0 - r - k) / (1.0 - k);
    m_final = (1.0 - g - k) / (1.0 - k);
    y_final = (1.0 - b - k) / (1.0 - k);
  }

  return core::cmyk_int_t(maths::round<int>(c_final * 100.0), maths::round<int>(m_final * 100.0),
                          maths::round<int>(y_final * 100.0), maths::round<int>(k * 100.0));
}

}  // namespace details

template <typename HSVType>
inline constexpr core::cmyk_int_t hsv_to_cmyk_v = details::convert(HSVType{});

template <typename T, intptr_t Scale>
constexpr core::cmyk_int_t convert(const core::basic_hsv<T, Scale>& hsv) {
  return details::convert(hsv);
}

}  // namespace color::conversion
