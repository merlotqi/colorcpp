/**
 * @file hsv_to_rgb.hpp
 * @brief HSV to RGB color space conversion
 *
 * Provides compile-time conversion from HSV color space to RGB color space.
 * Uses the standard HSV to RGB transformation algorithm.
 *
 * @author Merlot.Qi
 *
 */

#pragma once

#include <cmath>
#include <color/core/hsv.hpp>
#include <color/core/rgb.hpp>
#include <color/utils/maths.hpp>
#include <cstdint>

namespace color::conversion {

namespace details {

template <typename T, intptr_t Scale>
constexpr core::rgba8_t convert(const core::basic_hsva<T, Scale>& hsv) {
  float h_f{0}, s_f{0}, v_f{0}, a_f{0};

  if constexpr (std::is_integral_v<T> && Scale == 1) {
    h_f = static_cast<float>(hsv.h);
    s_f = static_cast<float>(hsv.s) / 100.0f;
    v_f = static_cast<float>(hsv.v) / 100.0f;
    a_f = static_cast<float>(hsv.a) / 255.0f;
  } else {
    constexpr float sc = static_cast<float>(Scale);
    h_f = static_cast<float>(hsv.h);
    s_f = static_cast<float>(hsv.s) / sc;
    v_f = static_cast<float>(hsv.v) / sc;
    a_f = static_cast<float>(hsv.a) / sc;
  }

  float c = v_f * s_f;
  float h_prime = h_f / 60.0f;
  float x = c * (1.0f - maths::abs(maths::fmod(h_prime, 2.0f) - 1.0f));
  float m = v_f - c;

  float rt = 0, gt = 0, bt = 0;
  int sector = static_cast<int>(h_prime) % 6;

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

template <typename HSVType>
inline constexpr core::rgba8_t hsv_to_rgb_v = details::convert(HSVType{});

template <typename T, intptr_t Scale>
constexpr core::rgba8_t convert(const core::basic_hsva<T, Scale>& hsv) {
  return details::convert(hsv);
}

}  // namespace color::conversion
