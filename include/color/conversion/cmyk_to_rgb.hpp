/**
 * @file cmyk_to_rgb.hpp
 * @brief CMYK to RGB color space conversion
 *
 * Provides compile-time conversion from CMYK color space to RGB color space.
 * Uses the standard CMYK to RGB transformation formula.
 *
 * @author Merlot.Qi
 *
 */

#pragma once

#include <cmath>
#include <cstdint>

#include "../core/cmyk.hpp"
#include "../core/rgb.hpp"
#include "../utils/maths.hpp"

namespace color::conversion {

namespace details {

template <typename T, intptr_t Scale>
constexpr core::rgb8_t convert(const core::basic_cmyk<T, Scale>& cmyk) {
  constexpr double sc = static_cast<double>(Scale);

  double c_f = static_cast<double>(cmyk.c) / sc;
  double m_f = static_cast<double>(cmyk.m) / sc;
  double y_f = static_cast<double>(cmyk.y) / sc;
  double k_f = static_cast<double>(cmyk.k) / sc;

  double r = (1.0 - c_f) * (1.0 - k_f);
  double g = (1.0 - m_f) * (1.0 - k_f);
  double b = (1.0 - y_f) * (1.0 - k_f);

  return core::rgb8_t(maths::round<uint8_t>(r * 255.0), maths::round<uint8_t>(g * 255.0),
                      maths::round<uint8_t>(b * 255.0));
}

}  // namespace details

template <typename CMYKType>
inline constexpr core::rgb8_t cmyk_to_rgb_v = details::convert(CMYKType{});

template <typename T, intptr_t Scale>
constexpr core::rgb8_t convert(const core::basic_cmyk<T, Scale>& cmyk) {
  return details::convert(cmyk);
}

}  // namespace color::conversion
