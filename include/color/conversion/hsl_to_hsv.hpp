/**
 * @file hsl_to_hsv.hpp
 * @brief HSL to HSV color space conversion
 *
 * Provides compile-time conversion from HSL color space to HSV color space.
 * Uses direct mathematical transformation between the two color models.
 *
 * @author Merlot.Qi
 *
 */

#pragma once

#include <cmath>
#include <color/core/hsl.hpp>
#include <color/core/hsv.hpp>
#include <color/utils/maths.hpp>
#include <cstdint>

namespace color::conversion {

namespace details {

template <typename T, intptr_t Scale>
constexpr core::hsva_int_t convert(const core::basic_hsla<T, Scale>& hsl) {
  float h_f{0}, s_hsl{0}, l_f{0}, a_f{0};

  if constexpr (std::is_integral_v<T> && Scale == 1) {
    h_f = static_cast<float>(hsl.h);
    s_hsl = static_cast<float>(hsl.s) / 100.0f;
    l_f = static_cast<float>(hsl.l) / 100.0f;
    a_f = static_cast<float>(hsl.a) / 255.0f;
  } else {
    constexpr float sc = static_cast<float>(Scale);
    h_f = static_cast<float>(hsl.h);
    s_hsl = static_cast<float>(hsl.s) / sc;
    l_f = static_cast<float>(hsl.l) / sc;
    a_f = static_cast<float>(hsl.a) / sc;
  }

  float min_l_1l = (l_f < (1.0f - l_f)) ? l_f : (1.0f - l_f);
  float v_f = l_f + s_hsl * min_l_1l;

  float s_hsv = 0.0f;
  if (v_f > 1e-7f) {
    s_hsv = 2.0f * (1.0f - l_f / v_f);
  }

  return core::hsva_int_t(maths::round<int>(h_f), maths::round<int>(s_hsv * 100.0f), maths::round<int>(v_f * 100.0f),
                          maths::round<int>(a_f * 255.0f));
}

}  // namespace details

template <typename HSLType>
inline constexpr core::hsva_int_t hsl_to_hsv_v = details::convert(HSLType{});

template <typename T, intptr_t Scale>
constexpr core::hsva_int_t convert(const core::basic_hsla<T, Scale>& hsla) {
  return details::convert(hsla);
}

}  // namespace color::conversion
