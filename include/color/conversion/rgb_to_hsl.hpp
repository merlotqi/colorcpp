/**
 * @file rgb_to_hsl.hpp
 * @brief RGB to HSL color space conversion
 *
 * Provides compile-time conversion from RGB color space to HSL color space.
 * Uses the standard RGB to HSL transformation algorithm.
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
constexpr core::hsla_int_t convert(const core::basic_rgba<T, Scale>& rgb) {
  float r_f{0}, g_f{0}, b_f{0}, a_f{0};

  if constexpr (std::is_integral_v<T> && Scale == 1) {
    r_f = static_cast<float>(rgb.r) / 255.0f;
    g_f = static_cast<float>(rgb.g) / 255.0f;
    b_f = static_cast<float>(rgb.b) / 255.0f;
    a_f = static_cast<float>(rgb.a) / 255.0f;
  } else {
    constexpr float sc = static_cast<float>(Scale);
    r_f = static_cast<float>(rgb.r) / sc;
    g_f = static_cast<float>(rgb.g) / sc;
    b_f = static_cast<float>(rgb.b) / sc;
    a_f = static_cast<float>(rgb.a) / sc;
  }

  //  Max, Min, Delta
  float max_v = (r_f > g_f) ? ((r_f > b_f) ? r_f : b_f) : ((g_f > b_f) ? g_f : b_f);
  float min_v = (r_f < g_f) ? ((r_f < b_f) ? r_f : b_f) : ((g_f < b_f) ? g_f : b_f);
  float delta = max_v - min_v;

  // Hue
  float h = 0.0f;
  if (delta > 1e-7f) {
    if (max_v == r_f) {
      h = 60.0f * maths::fmod((g_f - b_f) / delta, 6.0f);
    } else if (max_v == g_f) {
      h = 60.0f * ((b_f - r_f) / delta + 2.0f);
    } else {
      h = 60.0f * ((r_f - g_f) / delta + 4.0f);
    }
    if (h < 0.0f) h += 360.0f;
  }

  // Lightness
  float l = (max_v + min_v) / 2.0f;

  // Saturation
  float s = 0.0f;
  float denom = 1.0f - maths::abs(2.0f * l - 1.0f);
  if (denom > 1e-7f) {
    s = delta / denom;
  }

  return core::hsla_int_t(maths::round<int>(h), maths::round<int>(s * 100.0f), maths::round<int>(l * 100.0f),
                          maths::round<int>(a_f * 255.0f));
}

}  // namespace details

template <typename RGBType>
inline constexpr core::hsla_int_t rgb_to_hsl_v = details::convert(RGBType{});

template <typename T, intptr_t Scale>
constexpr core::hsla_int_t convert(const core::basic_rgba<T, Scale>& rgb) {
  return details::convert(rgb);
}

}  // namespace color::conversion
