/**
 * @file hwb.hpp
 * @brief HWB (Hue-Whiteness-Blackness) conversion functions.
 * @see CSS Color Level 4 §4.5
 */

#pragma once

#include <algorithm>
#include <colorcpp/core/hwb.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

/**
 * @brief Convert HWB color space to RGB color space.
 *
 * Implements conversion according to CSS Color Level 4 specification §4.5.
 * HWB (Hue-Whiteness-Blackness) is a perceptually intuitive cylindrical color model.
 *
 * Conversion algorithm steps:
 * 1. Normalize input values to [0, 1] range
 * 2. Handle achromatic case when w + b >= 1 (return uniform gray)
 * 3. Convert HWB to HSV intermediate representation
 * 4. Perform standard HSV to RGB conversion
 * 5. Pack result into target type with alpha channel preservation
 *
 * @tparam To Target color type, must satisfy RGB concept
 * @tparam From Source color type, must satisfy HWB concept
 * @param src Input HWB color value
 * @return Converted RGB color value
 *
 * @note This function is constexpr and can be used at compile time.
 * @note Alpha channel is preserved if present in source color.
 * @note Achromatic case (grayscale) is optimized for performance and numerical stability.
 *
 * @see rgb_to_hwb() inverse conversion function
 * @see CSS Color Level 4 https://www.w3.org/TR/css-color-4/#hwb-to-rgb
 */
template <typename To, typename From>
constexpr To hwb_to_rgb(const From& src) {
  float h = to_unit<From, 0>(src.template get_index<0>());  // [0,1] from [0,360]
  float w = to_unit<From, 1>(src.template get_index<1>());  // [0,1]
  float b = to_unit<From, 2>(src.template get_index<2>());  // [0,1]
  float a = get_src_alpha(src);

  // Non-finite hue would make the sector cast UB, and negative hue picked a
  // wrong (negative) sector — wrap onto the ring first (issue #66).
  h = wrap_unit_hue(h);

  // If whiteness + blackness >= 1, the color is achromatic
  if (w + b >= 1.0f) {
    float grayf = w / (w + b);
    if constexpr (To::channels >= 4)
      return pack_to<To>(from_unit<To, 0>(grayf), from_unit<To, 1>(grayf), from_unit<To, 2>(grayf),
                         from_unit<To, 3>(a));
    else
      return pack_to<To>(from_unit<To, 0>(grayf), from_unit<To, 1>(grayf), from_unit<To, 2>(grayf));
  }

  // Convert HWB to HSV first
  float s = 1.0f - w / (1.0f - b);
  float v = 1.0f - b;

  // Now convert HSV to RGB
  int i = static_cast<int>(h * 6.0f);
  float f = h * 6.0f - static_cast<float>(i);
  float p = v * (1.0f - s);
  float q = v * (1.0f - f * s);
  float t = v * (1.0f - (1.0f - f) * s);

  float r = 0, g = 0, bl = 0;
  switch (i % 6) {
    case 0:
      r = v;
      g = t;
      bl = p;
      break;
    case 1:
      r = q;
      g = v;
      bl = p;
      break;
    case 2:
      r = p;
      g = v;
      bl = t;
      break;
    case 3:
      r = p;
      g = q;
      bl = v;
      break;
    case 4:
      r = t;
      g = p;
      bl = v;
      break;
    case 5:
      r = v;
      g = p;
      bl = q;
      break;
  }

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(bl), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(bl));
}

/**
 * @brief Convert RGB color space to HWB color space.
 *
 * Implements conversion according to CSS Color Level 4 specification §4.5.
 * Extracts hue, whiteness, and blackness components from RGB inputs.
 *
 * Conversion algorithm steps:
 * 1. Normalize RGB channel values to [0, 1] range
 * 2. Calculate whiteness as minimum of RGB components
 * 3. Calculate blackness as 1 minus maximum of RGB components
 * 4. Calculate hue using standard HSV hue decomposition algorithm
 * 5. Convert hue from [0,1] normalized range to [0, 360] degrees
 * 6. Pack result into target type with alpha channel preservation
 *
 * @tparam To Target color type, must satisfy HWB concept
 * @tparam From Source color type, must satisfy RGB concept
 * @param src Input RGB color value
 * @return Converted HWB color value
 *
 * @note This function is constexpr and can be used at compile time.
 * @note Alpha channel is preserved if present in source color.
 * @note For achromatic colors (R=G=B), hue value will be 0.
 *
 * @see hwb_to_rgb() inverse conversion function
 * @see CSS Color Level 4 https://www.w3.org/TR/css-color-4/#rgb-to-hwb
 */
template <typename To, typename From>
constexpr To rgb_to_hwb(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());
  float a = get_src_alpha(src);

  float w = std::min({r, g, b});
  float bk = 1.0f - std::max({r, g, b});

  // Hue: same as HSV hue calculation
  float max = std::max({r, g, b}), min = std::min({r, g, b});
  float d = max - min;
  float h = 0.0f;

  if (max != min) {
    if (max == r)
      h = (g - b) / d + (g < b ? 6.0f : 0.0f);
    else if (max == g)
      h = (b - r) / d + 2.0f;
    else
      h = (r - g) / d + 4.0f;
    h /= 6.0f;
  }

  // Convert h from [0,1] to [0,360]
  h = h * 360.0f;

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_value<To, 0>(h), from_unit<To, 1>(w), from_unit<To, 2>(bk), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_value<To, 0>(h), from_unit<To, 1>(w), from_unit<To, 2>(bk));
}

}  // namespace colorcpp::operations::conversion::details
