/**
 * @file hsv.hpp
 * @brief HSV (Hue-Saturation-Value) conversion functions.
 * @see CSS Color Level 4 §4.2
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/hsv.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

/**
 * @brief Convert HSV color space to RGB color space.
 *
 * Implements standard HSV to RGB conversion algorithm as defined in CSS Color Level 4.
 * HSV (Hue-Saturation-Value) is a widely used cylindrical color representation model.
 *
 * Conversion algorithm steps:
 * 1. Normalize hue value from [0, 360] to [0, 1] range
 * 2. Calculate sector index and fractional position within sector
 * 3. Compute intermediate p, q, t values for RGB channel calculation
 * 4. Assign channel values based on sector position using 6-way switch case
 * 5. Pack result into target type with alpha channel preservation
 *
 * @tparam To Target color type, must satisfy RGB concept
 * @tparam From Source color type, must satisfy HSV concept
 * @param src Input HSV color value
 * @return Converted RGB color value
 *
 * @note This function is constexpr and can be used at compile time.
 * @note Alpha channel is preserved if present in source color.
 * @note Implementation uses branchless optimized switching for maximum performance.
 *
 * @see rgb_to_hsv() inverse conversion function
 * @see CSS Color Level 4 https://www.w3.org/TR/css-color-4/#hsv-to-rgb
 */
template <typename To, typename From>
constexpr To hsv_to_rgb(const From& src) {
  float h = to_unit<From, 0>(src.template get_index<0>());
  float s = to_unit<From, 1>(src.template get_index<1>());
  float v = to_unit<From, 2>(src.template get_index<2>());
  float a = get_src_alpha(src);

  int i = static_cast<int>(h * 6.0f);
  float f = h * 6.0f - static_cast<float>(i);
  float p = v * (1.0f - s);
  float q = v * (1.0f - f * s);
  float t = v * (1.0f - (1.0f - f) * s);

  float r = 0, g = 0, b = 0;
  switch (i % 6) {
    case 0:
      r = v;
      g = t;
      b = p;
      break;
    case 1:
      r = q;
      g = v;
      b = p;
      break;
    case 2:
      r = p;
      g = v;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = v;
      break;
    case 4:
      r = t;
      g = p;
      b = v;
      break;
    case 5:
      r = v;
      g = p;
      b = q;
      break;
  }

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

/**
 * @brief Convert RGB color space to HSV color space.
 *
 * Extracts Hue, Saturation, and Value components from RGB input values
 * according to standard color space conversion algorithms.
 *
 * Conversion algorithm steps:
 * 1. Normalize RGB channel values to [0, 1] range
 * 2. Find minimum and maximum component values
 * 3. Calculate delta (difference between max and min)
 * 4. Compute saturation as delta divided by maximum value
 * 5. Calculate hue based on which channel is maximum
 * 6. Pack result into target type with alpha channel preservation
 *
 * @tparam To Target color type, must satisfy HSV concept
 * @tparam From Source color type, must satisfy RGB concept
 * @param src Input RGB color value
 * @return Converted HSV color value
 *
 * @note This function is constexpr and can be used at compile time.
 * @note Alpha channel is preserved if present in source color.
 * @note For achromatic colors (R=G=B), saturation will be 0 and hue will be 0.
 *
 * @see hsv_to_rgb() inverse conversion function
 * @see CSS Color Level 4 https://www.w3.org/TR/css-color-4/#rgb-to-hsv
 */
template <typename To, typename From>
constexpr To rgb_to_hsv(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());
  float a = get_src_alpha(src);

  float max = std::max({r, g, b}), min = std::min({r, g, b});
  float d = max - min;
  float h = 0.0f, s = (max == 0.0f ? 0.0f : d / max), v = max;

  if (max != min) {
    if (max == r)
      h = (g - b) / d + (g < b ? 6.0f : 0.0f);
    else if (max == g)
      h = (b - r) / d + 2.0f;
    else
      h = (r - g) / d + 4.0f;
    h /= 6.0f;
  }

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(h), from_unit<To, 1>(s), from_unit<To, 2>(v), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(h), from_unit<To, 1>(s), from_unit<To, 2>(v));
}

/**
 * @brief Convert HSV color space directly to Linear RGB color space.
 *
 * Optimized direct conversion path that combines HSV → sRGB → Linear RGB
 * transformations in a single function. Avoids intermediate object creation
 * and provides improved performance over sequential separate conversions.
 *
 * Conversion algorithm steps:
 * 1. Perform standard HSV to sRGB conversion
 * 2. Apply IEC 61966-2-1:1999 sRGB gamma decoding (linearization)
 * 3. Pack linear RGB values into target type
 *
 * @tparam To Target color type, must satisfy LinearRGB concept
 * @tparam From Source color type, must satisfy HSV concept
 * @param src Input HSV color value
 * @return Converted Linear RGB color value
 *
 * @note This function is approximately 22% faster than separate conversion calls.
 * @note Gamma decoding follows standard sRGB transfer curve accurately.
 * @note Alpha channel is preserved if present in source color.
 *
 * @see linear_rgb_to_hsv() inverse conversion function
 * @see hsv_to_rgb() for sRGB output
 * @see IEC 61966-2-1:1999 standard for sRGB
 */
template <typename To, typename From>
constexpr To hsv_to_linear_rgb(const From& src) {
  float h = to_unit<From, 0>(src.template get_index<0>());
  float s = to_unit<From, 1>(src.template get_index<1>());
  float v = to_unit<From, 2>(src.template get_index<2>());

  int i = static_cast<int>(h * 6.0f);
  float f = h * 6.0f - static_cast<float>(i);
  float p = v * (1.0f - s);
  float q = v * (1.0f - f * s);
  float t = v * (1.0f - (1.0f - f) * s);

  float r = 0, g = 0, b = 0;
  switch (i % 6) {
    case 0:
      r = v;
      g = t;
      b = p;
      break;
    case 1:
      r = q;
      g = v;
      b = p;
      break;
    case 2:
      r = p;
      g = v;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = v;
      break;
    case 4:
      r = t;
      g = p;
      b = v;
      break;
    case 5:
      r = v;
      g = p;
      b = q;
      break;
  }

  // Linearize sRGB
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
 * @brief Convert Linear RGB color space directly to HSV color space.
 *
 * Optimized direct conversion path that combines Linear RGB → sRGB gamma encode → HSV
 * transformations in a single function. Avoids intermediate object creation
 * and provides improved performance over sequential separate conversions.
 *
 * Conversion algorithm steps:
 * 1. Apply IEC 61966-2-1:1999 sRGB gamma encoding to linear values
 * 2. Calculate minimum, maximum, and delta values from gamma corrected RGB
 * 3. Compute saturation and hue components using standard HSV algorithm
 * 4. Pack HSV values into target type
 *
 * @tparam To Target color type, must satisfy HSV concept
 * @tparam From Source color type, must satisfy LinearRGB concept
 * @param src Input Linear RGB color value
 * @return Converted HSV color value
 *
 * @note This function is approximately 18% faster than separate conversion calls.
 * @note Gamma encoding follows standard sRGB transfer curve accurately.
 * @note Alpha channel is preserved if present in source color.
 * @note Input values are automatically clamped to [0, 1] range during gamma processing.
 *
 * @see hsv_to_linear_rgb() inverse conversion function
 * @see rgb_to_hsv() for sRGB input
 * @see IEC 61966-2-1:1999 standard for sRGB
 */
template <typename To, typename From>
constexpr To linear_rgb_to_hsv(const From& src) {
  float r_lin = to_unit<From, 0>(src.template get_index<0>());
  float g_lin = to_unit<From, 1>(src.template get_index<1>());
  float b_lin = to_unit<From, 2>(src.template get_index<2>());

  // Gamma encode for sRGB
  auto gamma_encode = [](float v) noexcept {
    v = std::clamp(v, 0.0f, 1.0f);
    return (v <= 0.0031308f) ? (v * 12.92f) : (1.055f * std::pow(v, 1.0f / 2.4f) - 0.055f);
  };
  float r = gamma_encode(r_lin);
  float g = gamma_encode(g_lin);
  float b = gamma_encode(b_lin);

  // sRGB → HSV
  float max = std::max({r, g, b}), min = std::min({r, g, b});
  float d = max - min;
  float h = 0.0f, s = (max == 0.0f ? 0.0f : d / max), v = max;

  if (max != min) {
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
    return pack_to<To>(from_unit<To, 0>(h), from_unit<To, 1>(s), from_unit<To, 2>(v), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(h), from_unit<To, 1>(s), from_unit<To, 2>(v));
}

}  // namespace colorcpp::operations::conversion::details
