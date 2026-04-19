/**
 * @file cmyk.hpp
 * @brief CMYK (Cyan-Magenta-Yellow-Key) conversion functions.
 * @see CSS Color Level 4 §6.2
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/cmyk.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

/**
 * @brief Convert CMYK color space to RGB color space.
 *
 * Implements standard subtractive to additive color model conversion.
 * CMYK is the standard color model used in color printing and graphic design.
 *
 * Conversion algorithm:
 *  Each RGB channel is calculated as (1 - CMY_component) * (1 - Key_black)
 *  This follows the standard undercolor removal formula for CMYK conversion.
 *
 * @tparam To Target color type, must satisfy RGB concept
 * @tparam From Source color type, must satisfy CMYK concept
 * @param src Input CMYK color value
 * @return Converted RGB color value
 *
 * @note CMYK channels are C, M, Y, K — channel 3 is K (black), NOT alpha.
 * @note This function is constexpr and can be used at compile time.
 * @note When converting to RGBA, alpha channel is set to 1.0 (fully opaque).
 * @note Implementation uses floating point arithmetic for maximum accuracy.
 *
 * @see rgb_to_cmyk() inverse conversion function
 * @see CSS Color Level 4 https://www.w3.org/TR/css-color-4/#cmyk-rgb
 */
template <typename To, typename From>
constexpr To cmyk_to_rgb(const From& src) {
  float c = to_unit<From, 0>(src.template get_index<0>());
  float m = to_unit<From, 1>(src.template get_index<1>());
  float y = to_unit<From, 2>(src.template get_index<2>());
  float k = to_unit<From, 3>(src.template get_index<3>());

  float r = (1.0f - c) * (1.0f - k);
  float g = (1.0f - m) * (1.0f - k);
  float b = (1.0f - y) * (1.0f - k);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(1.0f));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

/**
 * @brief Convert RGB color space to CMYK color space.
 *
 * Implements conversion from additive RGB model to subtractive CMYK printing model.
 * Uses standard undercolor removal algorithm with black generation.
 *
 * Conversion algorithm steps:
 * 1. Calculate Key (K) as 1 minus maximum RGB component value
 * 2. Compute denominator as (1 - K)
 * 3. Calculate C, M, Y components using division by denominator
 * 4. Handle pure black case safely by avoiding division by zero
 * 5. Pack all four CMYK channels into target type
 *
 * @tparam To Target color type, must satisfy CMYK concept
 * @tparam From Source color type, must satisfy RGB concept
 * @param src Input RGB color value
 * @return Converted CMYK color value
 *
 * @note This function is constexpr and can be used at compile time.
 * @note Division by zero is safely handled for pure black (R=G=B=0).
 * @note Alpha channel from source RGB is not preserved (CMYK has no alpha).
 * @note This is the standard conversion used in most graphics software.
 *
 * @see cmyk_to_rgb() inverse conversion function
 * @see CSS Color Level 4 https://www.w3.org/TR/css-color-4/#rgb-cmyk
 */
template <typename To, typename From>
constexpr To rgb_to_cmyk(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());

  float k = 1.0f - std::max({r, g, b});
  float denom = 1.0f - k;
  float c = (denom > 0.0f) ? (1.0f - r - k) / denom : 0.0f;
  float m = (denom > 0.0f) ? (1.0f - g - k) / denom : 0.0f;
  float y = (denom > 0.0f) ? (1.0f - b - k) / denom : 0.0f;

  return pack_to<To>(from_unit<To, 0>(c), from_unit<To, 1>(m), from_unit<To, 2>(y), from_unit<To, 3>(k));
}

}  // namespace colorcpp::operations::conversion::details
