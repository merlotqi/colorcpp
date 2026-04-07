/**
 * @file color_temperature.hpp
 * @brief Kelvin color temperature to RGB conversion.
 *
 * Implements Tanner Helland's approximation algorithm for converting
 * correlated color temperature (CCT) in Kelvin to sRGB values.
 *
 * Reference: https://tannerhelland.com/2012/09/18/convert-temperature-rgb-algorithm-code.html
 * Based on approximation of CIE 1931 xy chromaticity coordinates for Planckian locus.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/rgb.hpp>

namespace colorcpp::algorithms {

/**
 * @brief Color temperature conversion utilities.
 *
 * Converts correlated color temperature (CCT) in Kelvin to various RGB formats.
 * Valid range: 1000K – 40000K.
 */
namespace color_temperature {

namespace details {

/**
 * @brief Clamp a float to [0, 1].
 */
inline float clamp01(float v) {
  if (v < 0.0f) return 0.0f;
  if (v > 1.0f) return 1.0f;
  return v;
}

/**
 * @brief Convert sRGB gamma-encoded value to linear.
 */
inline float srgb_to_linear(float v) { return v <= 0.04045f ? v / 12.92f : std::pow((v + 0.055f) / 1.055f, 2.4f); }

/**
 * @brief Convert linear value to sRGB gamma-encoded.
 */
inline float linear_to_srgb(float v) {
  return v <= 0.0031308f ? 12.92f * v : 1.055f * std::pow(v, 1.0f / 2.4f) - 0.055f;
}

}  // namespace details

/**
 * @brief Convert Kelvin temperature to linear RGB (no gamma correction).
 *
 * Uses Tanner Helland's algorithm. Output channels are in [0, 1].
 *
 * @param kelvin Color temperature in Kelvin (1000–40000).
 * @return Linear RGB with channels clamped to [0, 1].
 */
inline core::rgbf_t kelvin_to_linear_rgb(float kelvin) {
  kelvin = std::clamp(kelvin, 1000.0f, 40000.0f);
  float temp = kelvin / 100.0f;

  float r, g, b;

  // Red
  if (temp <= 66.0f) {
    r = 1.0f;
  } else {
    r = temp - 60.0f;
    r = 329.698727446f * std::pow(r, -0.1332047592f);
    r = details::clamp01(r / 255.0f);
  }

  // Green
  if (temp <= 66.0f) {
    g = 99.4708025861f * std::log(temp) - 161.1195681661f;
  } else {
    g = temp - 60.0f;
    g = 288.1221695283f * std::pow(g, -0.0755148492f);
  }
  g = details::clamp01(g / 255.0f);

  // Blue
  if (temp >= 66.0f) {
    b = 1.0f;
  } else if (temp <= 19.0f) {
    b = 0.0f;
  } else {
    b = temp - 10.0f;
    b = 138.5177312231f * std::log(b) - 305.0447927307f;
    b = details::clamp01(b / 255.0f);
  }

  return core::rgbf_t{r, g, b};
}

/**
 * @brief Convert Kelvin temperature to gamma-corrected sRGB.
 *
 * @param kelvin Color temperature in Kelvin (1000–40000).
 * @return sRGB with channels in [0, 1].
 */
inline core::rgbf_t kelvin_to_rgb(float kelvin) {
  auto linear = kelvin_to_linear_rgb(kelvin);
  return core::rgbf_t{
      details::linear_to_srgb(linear.r()),
      details::linear_to_srgb(linear.g()),
      details::linear_to_srgb(linear.b()),
  };
}

/**
 * @brief Convert Kelvin temperature to 8-bit sRGB.
 *
 * @param kelvin Color temperature in Kelvin (1000–40000).
 * @param alpha Alpha channel value (default 255).
 * @return rgba8_t color.
 */
inline core::rgba8_t kelvin_to_rgba8(float kelvin, uint8_t alpha = 255) {
  auto srgb = kelvin_to_rgb(kelvin);
  return core::rgba8_t{
      static_cast<uint8_t>(std::round(details::clamp01(srgb.r()) * 255.0f)),
      static_cast<uint8_t>(std::round(details::clamp01(srgb.g()) * 255.0f)),
      static_cast<uint8_t>(std::round(details::clamp01(srgb.b()) * 255.0f)),
      alpha,
  };
}

namespace details {

inline float linear_rgb_distance_sq(const core::rgbf_t& a, const core::rgbf_t& b) {
  const float dr = a.r() - b.r();
  const float dg = a.g() - b.g();
  const float db = a.b() - b.b();
  return dr * dr + dg * dg + db * db;
}

}  // namespace details

/**
 * @brief Estimate correlated color temperature (K) from **linear** sRGB in [0, 1].
 *
 * Minimizes Euclidean distance to @ref kelvin_to_linear_rgb over [1000, 40000] K. Saturated colors not on the
 * Planckian locus still get a best-fit K (useful for warm/cool sliders, not spectrally accurate CCT).
 */
inline float linear_rgb_to_kelvin_estimate(const core::rgbf_t& linear_srgb) {
  float best_k = 6500.0f;
  float best_e = 1e30f;
  for (float k = 1000.0f; k <= 40000.0f; k += 250.0f) {
    const float e = details::linear_rgb_distance_sq(kelvin_to_linear_rgb(k), linear_srgb);
    if (e < best_e) {
      best_e = e;
      best_k = k;
    }
  }
  float lo = std::max(1000.0f, best_k - 400.0f);
  float hi = std::min(40000.0f, best_k + 400.0f);
  for (int i = 0; i < 28; ++i) {
    const float m1 = lo + (hi - lo) / 3.0f;
    const float m2 = hi - (hi - lo) / 3.0f;
    const float e1 = details::linear_rgb_distance_sq(kelvin_to_linear_rgb(m1), linear_srgb);
    const float e2 = details::linear_rgb_distance_sq(kelvin_to_linear_rgb(m2), linear_srgb);
    if (e1 < e2)
      hi = m2;
    else
      lo = m1;
  }
  return (lo + hi) * 0.5f;
}

/** @brief Estimate K from gamma-encoded sRGB in [0, 1]. */
inline float rgb_to_kelvin_estimate(const core::rgbf_t& srgb) {
  const core::rgbf_t lin{details::srgb_to_linear(srgb.r()), details::srgb_to_linear(srgb.g()),
                         details::srgb_to_linear(srgb.b())};
  return linear_rgb_to_kelvin_estimate(lin);
}

}  // namespace color_temperature

}  // namespace colorcpp::algorithms
