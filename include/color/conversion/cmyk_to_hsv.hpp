/**
 * @file cmyk_to_hsv.hpp
 * @brief CMYK to HSV color space conversion
 *
 * Provides compile-time conversion from CMYK color space to HSV color space.
 * The conversion is performed in two steps: CMYK -> RGB -> HSV.
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
constexpr core::hsva_int_t convert(const core::basic_cmyk<T, Scale>& cmyk) {
  float c_f{0}, m_f{0}, y_f{0}, k_f{0};

  if constexpr (std::is_integral_v<T> && Scale == 1) {
    c_f = static_cast<float>(cmyk.c) / 100.0f;
    m_f = static_cast<float>(cmyk.m) / 100.0f;
    y_f = static_cast<float>(cmyk.y) / 100.0f;
    k_f = static_cast<float>(cmyk.k) / 100.0f;
  } else {
    constexpr float sc = static_cast<float>(Scale);
    c_f = static_cast<float>(cmyk.c) / sc;
    m_f = static_cast<float>(cmyk.m) / sc;
    y_f = static_cast<float>(cmyk.y) / sc;
    k_f = static_cast<float>(cmyk.k) / sc;
  }

  float r = (1.0f - c_f) * (1.0f - k_f);
  float g = (1.0f - m_f) * (1.0f - k_f);
  float b = (1.0f - y_f) * (1.0f - k_f);

  float max_val = (r > g) ? ((r > b) ? r : b) : ((g > b) ? g : b);
  float min_val = (r < g) ? ((r < b) ? r : b) : ((g < b) ? g : b);
  float delta = max_val - min_val;

  float h = 0.0f;
  if (delta > 1e-7f) {
    if (max_val == r) {
      h = 60.0f * maths::fmod((g - b) / delta, 6.0f);
    } else if (max_val == g) {
      h = 60.0f * ((b - r) / delta + 2.0f);
    } else {
      h = 60.0f * ((r - g) / delta + 4.0f);
    }
    if (h < 0.0f) h += 360.0f;
  }

  float s = (max_val < 1e-7f) ? 0.0f : (delta / max_val);
  float v = max_val;

  return core::hsva_int_t(maths::round<int>(h), maths::round<int>(s * 100.0f), maths::round<int>(v * 100.0f), 255);
}

}  // namespace details

template <typename CMYKType>
inline constexpr core::hsva_int_t cmyk_to_hsv_v = details::convert(CMYKType{});

template <typename T, intptr_t Scale>
constexpr core::hsva_int_t convert(const core::basic_cmyk<T, Scale>& cmyk) {
  return details::convert(cmyk);
}

}  // namespace color::conversion
