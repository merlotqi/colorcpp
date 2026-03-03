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
#include <color/core/cmyk.hpp>
#include <color/core/rgb.hpp>
#include <color/utils/maths.hpp>
#include <cstdint>

namespace color::conversion {

namespace details {

template <typename T, intptr_t Scale>
constexpr core::cmyk_int_t convert(const core::basic_rgba<T, Scale>& rgb) {
  float r_f{0}, g_f{0}, b_f{0};

  if constexpr (std::is_integral_v<T> && Scale == 1) {
    r_f = static_cast<float>(rgb.r) / 255.0f;
    g_f = static_cast<float>(rgb.g) / 255.0f;
    b_f = static_cast<float>(rgb.b) / 255.0f;
  } else {
    constexpr float sc = static_cast<float>(Scale);
    r_f = static_cast<float>(rgb.r) / sc;
    g_f = static_cast<float>(rgb.g) / sc;
    b_f = static_cast<float>(rgb.b) / sc;
  }

  float max_rgb = (r_f > g_f) ? ((r_f > b_f) ? r_f : b_f) : ((g_f > b_f) ? g_f : b_f);
  float k = 1.0f - max_rgb;

  float c_final = 0.0f;
  float m_final = 0.0f;
  float y_final = 0.0f;

  if (max_rgb > 1e-7f) {
    float inv_max = 1.0f / max_rgb;
    c_final = (max_rgb - r_f) * inv_max;
    m_final = (max_rgb - g_f) * inv_max;
    y_final = (max_rgb - b_f) * inv_max;
  }

  return core::cmyk_int_t(maths::round<int>(c_final * 100.0f), maths::round<int>(m_final * 100.0f),
                          maths::round<int>(y_final * 100.0f), maths::round<int>(k * 100.0f));
}

}  // namespace details

template <typename RGBType>
inline constexpr core::cmyk_int_t rgb_to_cmyk_v = details::convert(RGBType{});

template <typename T, intptr_t Scale>
constexpr core::cmyk_int_t convert(const core::basic_rgba<T, Scale>& rgb) {
  return details::convert(rgb);
}

}  // namespace color::conversion
