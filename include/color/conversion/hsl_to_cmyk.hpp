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
constexpr core::cmyk_int_t convert(const core::basic_hsla<T, Scale>& hsl) {
  float h_f{0}, s_f{0}, l_f{0};

  if constexpr (std::is_integral_v<T> && Scale == 1) {
    h_f = static_cast<float>(hsl.h);
    s_f = static_cast<float>(hsl.s) / 100.0f;
    l_f = static_cast<float>(hsl.l) / 100.0f;
  } else {
    constexpr float sc = static_cast<float>(Scale);
    h_f = static_cast<float>(hsl.h);
    s_f = static_cast<float>(hsl.s) / sc;
    l_f = static_cast<float>(hsl.l) / sc;
  }

  float chroma = (1.0f - maths::abs(2.0f * l_f - 1.0f)) * s_f;
  float x = chroma * (1.0f - maths::abs(maths::fmod(h_f / 60.0f, 2.0f) - 1.0f));
  float m = l_f - chroma / 2.0f;

  float r = 0, g = 0, b = 0;
  int sector = static_cast<int>(h_f / 60.0f) % 6;

  switch (sector) {
    case 0:
      r = chroma;
      g = x;
      b = 0;
      break;
    case 1:
      r = x;
      g = chroma;
      b = 0;
      break;
    case 2:
      r = 0;
      g = chroma;
      b = x;
      break;
    case 3:
      r = 0;
      g = x;
      b = chroma;
      break;
    case 4:
      r = x;
      g = 0;
      b = chroma;
      break;
    default:
      r = chroma;
      g = 0;
      b = x;
      break;
  }

  r += m;
  g += m;
  b += m;

  float max_rgb = (r > g) ? ((r > b) ? r : b) : ((g > b) ? g : b);
  float k = 1.0f - max_rgb;

  float c_final = 0, m_final = 0, y_final = 0;
  if (k < 1.0f - 1e-7f) {
    float inv_k = 1.0f / (1.0f - k);
    c_final = (1.0f - r - k) * inv_k;
    m_final = (1.0f - g - k) * inv_k;
    y_final = (1.0f - b - k) * inv_k;
  }

  return core::cmyk_int_t(maths::round<int>(c_final * 100.0f), maths::round<int>(m_final * 100.0f),
                          maths::round<int>(y_final * 100.0f), maths::round<int>(k * 100.0f));
}

}  // namespace details

template <typename HSLType>
inline constexpr core::cmyk_int_t hsl_to_cmyk_v = details::convert(HSLType{});

template <typename T, intptr_t Scale>
constexpr core::cmyk_int_t convert(const core::basic_hsla<T, Scale>& hsl) {
  return details::convert(hsl);
}

}  // namespace color::conversion
