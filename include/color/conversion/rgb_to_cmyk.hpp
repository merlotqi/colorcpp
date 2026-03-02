/**
 * @file rgb_to_cmyk.hpp
 * @brief RGB to CMYK color space conversion
 *
 * Provides compile-time conversion from RGB color space to CMYK color space.
 * Uses the standard RGB to CMYK transformation algorithm.
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
constexpr core::cmyk_int_t convert(const core::basic_rgb<T, Scale>& rgb) {
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

  double max_rgb = (r_f > g_f) ? ((r_f > b_f) ? r_f : b_f) : ((g_f > b_f) ? g_f : b_f);
  double k = 1.0 - max_rgb;

  double c_final = 0.0;
  double m_final = 0.0;
  double y_final = 0.0;

  if (max_rgb > 1e-7) {
    double inv_max = 1.0 / max_rgb;
    c_final = (max_rgb - r_f) * inv_max;
    m_final = (max_rgb - g_f) * inv_max;
    y_final = (max_rgb - b_f) * inv_max;
  }

  return core::cmyk_int_t(maths::round<int>(c_final * 100.0), maths::round<int>(m_final * 100.0),
                          maths::round<int>(y_final * 100.0), maths::round<int>(k * 100.0));
}

}  // namespace details

template <typename RGBType>
inline constexpr core::cmyk_int_t rgb_to_cmyk_v = details::convert(RGBType{});

template <typename T, intptr_t Scale>
constexpr core::cmyk_int_t convert(const core::basic_rgb<T, Scale>& rgb) {
  return details::convert(rgb);
}

}  // namespace color::conversion
