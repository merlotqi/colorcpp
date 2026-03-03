/**
 * @file hsv_to_hsl.hpp
 * @brief HSV to HSL color space conversion
 *
 * Provides compile-time conversion from HSV color space to HSL color space.
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
constexpr core::hsla_int_t convert(const core::basic_hsva<T, Scale>& hsv) {
  float h_f{0}, s_hsv{0}, v_f{0}, a_f{0};

  if constexpr (std::is_integral_v<T> && Scale == 1) {
    h_f = static_cast<float>(hsv.h);
    s_hsv = static_cast<float>(hsv.s) / 100.0f;
    v_f = static_cast<float>(hsv.v) / 100.0f;
    a_f = static_cast<float>(hsv.a) / 255.0f;
  } else {
    constexpr float sc = static_cast<float>(Scale);
    h_f = static_cast<float>(hsv.h);
    s_hsv = static_cast<float>(hsv.s) / sc;
    v_f = static_cast<float>(hsv.v) / sc;
    a_f = static_cast<float>(hsv.a) / sc;
  }

  float l_f = v_f * (1.0f - s_hsv / 2.0f);

  float s_hsl = 0.0f;
  float min_l = (l_f < (1.0f - l_f)) ? l_f : (1.0f - l_f);

  if (min_l > 1e-7f) {
    s_hsl = (v_f - l_f) / min_l;
  }

  return core::hsla_int_t(maths::round<int>(h_f), maths::round<int>(s_hsl * 100.0f), maths::round<int>(l_f * 100.0f),
                          maths::round<int>(a_f * 255.0f));
}

}  // namespace details

template <typename HSVType>
inline constexpr core::hsla_int_t hsv_to_hsl_v = details::convert(HSVType{});

template <typename T, intptr_t Scale>
constexpr core::hsla_int_t convert(const core::basic_hsva<T, Scale>& hsva) {
  return details::convert(hsva);
}

}  // namespace color::conversion
