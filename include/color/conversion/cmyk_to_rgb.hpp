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
#include <color/core/cmyk.hpp>
#include <color/core/rgb.hpp>
#include <color/utils/maths.hpp>
#include <cstdint>
namespace color::conversion {

namespace details {

template <typename T, intptr_t Scale>
constexpr core::rgba8_t convert(const core::basic_cmyk<T, Scale>& cmyk) {
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

  return core::rgba8_t(maths::round<uint8_t>(r * 255.0f), maths::round<uint8_t>(g * 255.0f),
                       maths::round<uint8_t>(b * 255.0f), 255);
}

}  // namespace details

template <typename CMYKType>
inline constexpr core::rgba8_t cmyk_to_rgb_v = details::convert(CMYKType{});

template <typename T, intptr_t Scale>
constexpr core::rgba8_t convert(const core::basic_cmyk<T, Scale>& cmyk) {
  return details::convert(cmyk);
}

}  // namespace color::conversion
