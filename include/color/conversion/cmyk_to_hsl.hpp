/**
 * @file cmyk_to_hsl.hpp
 * @brief CMYK to HSL color space conversion
 *
 * Provides compile-time conversion from CMYK color space to HSL color space.
 * The conversion is performed in two steps: CMYK -> RGB -> HSL.
 *
 * @author Merlot.Qi
 *
 */

#pragma once

#include <cmath>
#include <cstdint>

#include "../core/cmyk.hpp"
#include "../core/hsl.hpp"
#include "../utils/maths.hpp"

namespace color::conversion {

namespace details {

template <typename T, intptr_t Scale>
constexpr core::hsl_int_t convert(const core::basic_cmyk<T, Scale>& cmyk) {
  constexpr double sc = static_cast<double>(Scale);
  double c_f = static_cast<double>(cmyk.c) / sc;
  double m_f = static_cast<double>(cmyk.m) / sc;
  double y_f = static_cast<double>(cmyk.y) / sc;
  double k_f = static_cast<double>(cmyk.k) / sc;

  double r = (1.0 - c_f) * (1.0 - k_f);
  double g = (1.0 - m_f) * (1.0 - k_f);
  double b = (1.0 - y_f) * (1.0 - k_f);

  double max_v = (r > g) ? ((r > b) ? r : b) : ((g > b) ? g : b);
  double min_v = (r < g) ? ((r < b) ? r : b) : ((g < b) ? g : b);
  double delta = max_v - min_v;

  double h = 0.0;
  if (delta > 1e-7) {
    if (max_v == r) {
      h = 60.0 * maths::fmod((g - b) / delta, 6.0);
    } else if (max_v == g) {
      h = 60.0 * ((b - r) / delta + 2.0);
    } else {
      h = 60.0 * ((r - g) / delta + 4.0);
    }
    if (h < 0.0) h += 360.0;
  }

  double l = (max_v + min_v) / 2.0;

  double s = 0.0;
  double denom = 1.0 - maths::abs(2.0 * l - 1.0);
  if (denom > 1e-7) {
    s = delta / denom;
  }

  return core::hsl_int_t(maths::round<int>(h), maths::round<int>(s * 100.0), maths::round<int>(l * 100.0));
}

}  // namespace details

template <typename CMYKType>
inline constexpr core::hsl_int_t cmyk_to_hsl_v = details::convert(CMYKType{});

template <typename T, intptr_t Scale>
constexpr core::hsl_int_t convert(const core::basic_cmyk<T, Scale>& cmyk) {
  return details::convert(cmyk);
}

}  // namespace color::conversion
