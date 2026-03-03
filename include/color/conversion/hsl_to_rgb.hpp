/**
 * @file hsl_to_rgb.hpp
 * @brief HSL to RGB color space conversion
 *
 * Provides compile-time conversion from HSL color space to RGB color space.
 * Uses the standard HSL to RGB transformation algorithm.
 *
 * @author Merlot.Qi
 *
 */

#pragma once

#include <cmath>
#include <color/core/hsl.hpp>
#include <color/core/rgb.hpp>
#include <color/utils/maths.hpp>
#include <cstdint>

namespace color::conversion {

namespace details {

template <typename T, intptr_t Scale>
constexpr core::rgba8_t convert(const core::basic_hsla<T, Scale>& hsl) {
  float h_f{0}, s_f{0}, l_f{0}, a_f{0};

  if constexpr (std::is_integral_v<T> && Scale == 1) {
    h_f = static_cast<float>(hsl.h);
    s_f = static_cast<float>(hsl.s) / 100.0f;
    l_f = static_cast<float>(hsl.l) / 100.0f;
    a_f = static_cast<float>(hsl.a) / 255.0f;
  } else {
    constexpr float sc = static_cast<float>(Scale);
    h_f = static_cast<float>(hsl.h);
    s_f = static_cast<float>(hsl.s) / sc;
    l_f = static_cast<float>(hsl.l) / sc;
    a_f = static_cast<float>(hsl.a) / sc;
  }

  // 2. (Chroma/Intermediate value)
  float c = (1.0f - maths::abs(2.0f * l_f - 1.0f)) * s_f;
  float x = c * (1.0f - maths::abs(maths::fmod(h_f / 60.0f, 2.0f) - 1.0f));
  float m = l_f - c / 2.0f;

  float rt = 0, gt = 0, bt = 0;

  int sector = static_cast<int>(h_f / 60.0f) % 6;
  switch (sector) {
    case 0:
      rt = c;
      gt = x;
      bt = 0;
      break;  // 0-60°
    case 1:
      rt = x;
      gt = c;
      bt = 0;
      break;  // 60-120°
    case 2:
      rt = 0;
      gt = c;
      bt = x;
      break;  // 120-180°
    case 3:
      rt = 0;
      gt = x;
      bt = c;
      break;  // 180-240°
    case 4:
      rt = x;
      gt = 0;
      bt = c;
      break;  // 240-300°
    default:
      rt = c;
      gt = 0;
      bt = x;
      break;  // 300-360°
  }

  return core::rgba8_t(maths::round<uint8_t>((rt + m) * 255.0f), maths::round<uint8_t>((gt + m) * 255.0f),
                       maths::round<uint8_t>((bt + m) * 255.0f), maths::round<uint8_t>(a_f * 255.0f));
}

}  // namespace details

template <typename HSLType>
inline constexpr core::rgba8_t hsl_to_rgb_v = details::convert(HSLType{});

template <typename T, intptr_t Scale>
constexpr core::rgba8_t convert(const core::basic_hsla<T, Scale>& hsl) {
  return details::convert(hsl);
}

}  // namespace color::conversion
