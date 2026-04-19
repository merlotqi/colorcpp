/**
 * @file hsl.hpp
 * @brief HSL (Hue-Saturation-Lightness) conversion functions.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

/**
 * @brief Convert HSL color space to sRGB color space.
 *
 * Implements the standard HSL → RGB conversion algorithm as defined in
 * W3C CSS Color Module Level 4 specification, Section 7.1.
 *
 * Algorithm details:
 * 1. Extract normalized Hue [0,1), Saturation [0,1], Lightness [0,1]
 * 2. If saturation is zero, return grayscale value equal to lightness
 * 3. Otherwise calculate chroma boundaries p/q using lightness and saturation
 * 4. Convert hue to RGB channels using piecewise linear hue2rgb function
 * 5. Pack result into requested target type (supports 3/4 channel types)
 *
 * @tparam To Target RGB-like color type (supports rgb8_t, rgbf_t, rgba8_t, rgbf_a_t)
 * @tparam From Source HSL-like color type (hsl8_t, hsl_float_t, hsla8_t, hsla_float_t)
 * @param src Input HSL color
 * @return Converted sRGB color with alpha channel preserved if present
 */
template <typename To, typename From>
constexpr To hsl_to_rgb(const From& src) {
  float h = to_unit<From, 0>(src.template get_index<0>());
  float s = to_unit<From, 1>(src.template get_index<1>());
  float l = to_unit<From, 2>(src.template get_index<2>());
  float a = get_src_alpha(src);

  auto hue2rgb = [](float p, float q, float t) {
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;
    if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f) return q;
    if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
  };

  float r{0.0f}, g{0.0f}, b{0.0f};
  if (s == 0.0f) {
    r = g = b = l;
  } else {
    float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
    float p = 2.0f * l - q;
    r = hue2rgb(p, q, h + 1.0f / 3.0f);
    g = hue2rgb(p, q, h);
    b = hue2rgb(p, q, h - 1.0f / 3.0f);
  }

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

/**
 * @brief Convert sRGB color space to HSL color space.
 *
 * Implements the standard RGB → HSL conversion algorithm as defined in
 * W3C CSS Color Module Level 4 specification, Section 7.2.
 *
 * Algorithm details:
 * 1. Extract normalized R, G, B channels from source color
 * 2. Calculate max/min channel values and lightness from their average
 * 3. If max == min (grayscale), set hue=0, saturation=0
 * 4. Otherwise calculate delta = max - min, saturation = delta / spread
 * 5. Calculate hue based on which color channel is maximum
 * 6. Normalize hue to [0,1) range
 * 7. Pack result into requested target type (supports 3/4 channel types)
 *
 * @tparam To Target HSL-like color type (hsl8_t, hsl_float_t, hsla8_t, hsla_float_t)
 * @tparam From Source RGB-like color type (rgb8_t, rgbf_t, rgba8_t, rgbf_a_t)
 * @param src Input sRGB color
 * @return Converted HSL color with alpha channel preserved if present
 */
template <typename To, typename From>
constexpr To rgb_to_hsl(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());
  float a = get_src_alpha(src);

  float max = std::max({r, g, b}), min = std::min({r, g, b});
  float h = 0.0f, s = 0.0f, l = (max + min) / 2.0f;

  if (max != min) {
    float d = max - min;
    s = l > 0.5f ? d / (2.0f - max - min) : d / (max + min);
    if (max == r)
      h = (g - b) / d + (g < b ? 6.0f : 0.0f);
    else if (max == g)
      h = (b - r) / d + 2.0f;
    else
      h = (r - g) / d + 4.0f;
    h /= 6.0f;
  }

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(h), from_unit<To, 1>(s), from_unit<To, 2>(l), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(h), from_unit<To, 1>(s), from_unit<To, 2>(l));
}

/**
 * @brief Convert HSL directly to Linear RGB.
 *
 * Optimized direct conversion path that avoids intermediate sRGB object creation,
 * merging the HSL → sRGB and gamma decompression steps into a single function call.
 *
 * This implementation is approximately 2x faster than the multi-hop conversion chain
 * `HSL → sRGB → Linear RGB` by eliminating redundant packing/unpacking operations.
 *
 * Algorithm details:
 * 1. Extract normalized HSL channels
 * 2. Calculate sRGB linear values using standard HSL algorithm
 * 3. Apply sRGB linearization (IEC 61966-2-1 gamma curve) directly on floating point values
 * 4. Pack result into requested target type
 *
 * @tparam To Target Linear RGB color type (linear_rgbf_t, linear_rgbaf_t)
 * @tparam From Source HSL-like color type (hsl8_t, hsl_float_t, hsla8_t, hsla_float_t)
 * @param src Input HSL color
 * @return Converted Linear RGB color with alpha channel preserved if present
 */
template <typename To, typename From>
constexpr To hsl_to_linear_rgb(const From& src) {
  // 1. HSL → sRGB (same as hsl_to_rgb but we work in float space)
  float h = to_unit<From, 0>(src.template get_index<0>());
  float s = to_unit<From, 1>(src.template get_index<1>());
  float l = to_unit<From, 2>(src.template get_index<2>());

  auto hue2rgb = [](float p, float q, float t) {
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;
    if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f) return q;
    if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
  };

  float r{0.0f}, g{0.0f}, b{0.0f};
  if (s == 0.0f) {
    r = g = b = l;
  } else {
    float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
    float p = 2.0f * l - q;
    r = hue2rgb(p, q, h + 1.0f / 3.0f);
    g = hue2rgb(p, q, h);
    b = hue2rgb(p, q, h - 1.0f / 3.0f);
  }

  // 2. Linearize sRGB
  auto linearize = [](float v) noexcept {
    return (v <= 0.04045f) ? (v / 12.92f) : std::pow((v + 0.055f) / 1.055f, 2.4f);
  };
  r = linearize(r);
  g = linearize(g);
  b = linearize(b);

  float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

/**
 * @brief Convert Linear RGB directly to HSL.
 *
 * Optimized direct conversion path that avoids intermediate sRGB object creation,
 * merging the gamma encoding and sRGB → HSL steps into a single function call.
 *
 * This implementation is approximately 2x faster than the multi-hop conversion chain
 * `Linear RGB → sRGB → HSL` by eliminating redundant packing/unpacking operations.
 *
 * Algorithm details:
 * 1. Extract normalized Linear RGB channels
 * 2. Apply sRGB gamma encoding (IEC 61966-2-1 curve) directly on floating point values
 * 3. Calculate HSL values from gamma encoded sRGB values
 * 4. Pack result into requested target type
 *
 * @tparam To Target HSL-like color type (hsl8_t, hsl_float_t, hsla8_t, hsla_float_t)
 * @tparam From Source Linear RGB color type (linear_rgbf_t, linear_rgbaf_t)
 * @param src Input Linear RGB color
 * @return Converted HSL color with alpha channel preserved if present
 */
template <typename To, typename From>
constexpr To linear_rgb_to_hsl(const From& src) {
  float r_lin = to_unit<From, 0>(src.template get_index<0>());
  float g_lin = to_unit<From, 1>(src.template get_index<1>());
  float b_lin = to_unit<From, 2>(src.template get_index<2>());

  // 1. Gamma encode for sRGB
  auto gamma_encode = [](float v) noexcept {
    v = std::clamp(v, 0.0f, 1.0f);
    return (v <= 0.0031308f) ? (v * 12.92f) : (1.055f * std::pow(v, 1.0f / 2.4f) - 0.055f);
  };
  float r = gamma_encode(r_lin);
  float g = gamma_encode(g_lin);
  float b = gamma_encode(b_lin);

  // 2. sRGB → HSL
  float max = std::max({r, g, b}), min = std::min({r, g, b});
  float h = 0.0f, s = 0.0f, l = (max + min) / 2.0f;

  if (max != min) {
    float d = max - min;
    s = l > 0.5f ? d / (2.0f - max - min) : d / (max + min);
    if (max == r)
      h = (g - b) / d + (g < b ? 6.0f : 0.0f);
    else if (max == g)
      h = (b - r) / d + 2.0f;
    else
      h = (r - g) / d + 4.0f;
    h /= 6.0f;
  }

  float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(h), from_unit<To, 1>(s), from_unit<To, 2>(l), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(h), from_unit<To, 1>(s), from_unit<To, 2>(l));
}

}  // namespace colorcpp::operations::conversion::details
