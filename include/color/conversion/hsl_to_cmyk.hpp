/**
 * @file hsl_to_cmyk.hpp
 * @brief HSL to CMYK color space conversion
 *
 * Provides compile-time conversion from HSL color space to CMYK color space.
 * The conversion is performed in two steps: HSL -> RGB -> CMYK.
 *
 * @author Merlot.Qi
 *
 */

#pragma once

#include <cmath>
#include <color/core/cmyk.hpp>
#include <color/core/hsl.hpp>
#include <color/utils/maths.hpp>
#include <cstdint>

namespace color::conversion {

namespace details {
template <typename T, intptr_t Scale>
constexpr core::cmyk_int_t convert(const core::basic_hsl<T, Scale>& hsl) {
  constexpr double sc = static_cast<double>(Scale);
  double h_f = static_cast<double>(hsl.h);
  double s_f = static_cast<double>(hsl.s) / sc;
  double l_f = static_cast<double>(hsl.l) / sc;

  double c = (1.0 - maths::abs(2.0 * l_f - 1.0)) * s_f;
  double x = c * (1.0 - maths::abs(maths::fmod(h_f / 60.0, 2.0) - 1.0));
  double m = l_f - c / 2.0;

  double r = 0, g = 0, b = 0;
  int sector = static_cast<int>(h_f / 60.0) % 6;

  switch (sector) {
    case 0:
      r = c;
      g = x;
      b = 0;
      break;  // 0-60
    case 1:
      r = x;
      g = c;
      b = 0;
      break;  // 60-120
    case 2:
      r = 0;
      g = c;
      b = x;
      break;  // 120-180
    case 3:
      r = 0;
      g = x;
      b = c;
      break;  // 180-240
    case 4:
      r = x;
      g = 0;
      b = c;
      break;  // 240-300
    default:
      r = c;
      g = 0;
      b = x;
      break;  // 300-360
  }

  r += m;
  g += m;
  b += m;

  double k = 1.0 - (r > g ? (r > b ? r : b) : (g > b ? g : b));

  double c_final = 0, m_final = 0, y_final = 0;
  if (k < 1.0 - 1e-7) {
    double inv_k = 1.0 / (1.0 - k);
    c_final = (1.0 - r - k) * inv_k;
    m_final = (1.0 - g - k) * inv_k;
    y_final = (1.0 - b - k) * inv_k;
  }

  return core::cmyk_int_t(maths::round<int>(c_final * 100.0), maths::round<int>(m_final * 100.0),
                          maths::round<int>(y_final * 100.0), maths::round<int>(k * 100.0));
}

}  // namespace details

template <typename HSLType>
inline constexpr core::cmyk_int_t hsl_to_cmyk_v = details::convert(HSLType{});

template <typename T, intptr_t Scale>
constexpr core::cmyk_int_t convert(const core::basic_hsl<T, Scale>& hsl) {
  return details::convert(hsl);
}

}  // namespace color::conversion
