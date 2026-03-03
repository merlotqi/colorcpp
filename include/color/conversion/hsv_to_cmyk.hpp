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
constexpr core::cmyk_int_t convert(const core::basic_hsva<T, Scale>& hsv) {
  float h_f{0}, s_f{0}, v_f{0};

  if constexpr (std::is_integral_v<T> && Scale == 1) {
    h_f = static_cast<float>(hsv.h);
    s_f = static_cast<float>(hsv.s) / 100.0f;
    v_f = static_cast<float>(hsv.v) / 100.0f;
  } else {
    constexpr float sc = static_cast<float>(Scale);
    h_f = static_cast<float>(hsv.h);
    s_f = static_cast<float>(hsv.s) / sc;
    v_f = static_cast<float>(hsv.v) / sc;
  }

  float chroma = v_f * s_f;
  float x = chroma * (1.0f - maths::abs(maths::fmod(h_f / 60.0f, 2.0f) - 1.0f));
  float m = v_f - chroma;

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

template <typename HSVType>
inline constexpr core::cmyk_int_t hsv_to_cmyk_v = details::convert(HSVType{});

template <typename T, intptr_t Scale>
constexpr core::cmyk_int_t convert(const core::basic_hsva<T, Scale>& hsv) {
  return details::convert(hsv);
}

}  // namespace color::conversion
