/**
 * @file adobe_rgb.hpp
 * @brief Adobe RGB (A98-RGB) conversion functions.
 *
 * Adobe RGB uses a pure power transfer function with the spec exponent
 * 563/256 (= 2.19921875, often rounded to 2.2; issue #68) and no linear
 * segment like sRGB. White point: D65 (same as sRGB/XYZ hub, no chromatic
 * adaptation needed).
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/adobe_rgb.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

/**
 * @brief Convert Adobe RGB gamma-encoded color to Linear Adobe RGB.
 *
 * Applies pure power decompression with the spec exponent 563/256
 * (inverse EOTF). Adobe RGB uses the same D65 white point as sRGB.
 *
 * @tparam To Target Linear Adobe RGB type
 * @tparam From Source Adobe RGB type
 */
template <typename To, typename From>
constexpr To adobe_rgb_to_linear_adobe_rgb(const From& src) {
  // Spec exponent 563/256, not a rounded 2.2 (issue #68); no sRGB-style
  // linear segment.
  auto linearize = [](float v) noexcept { return std::pow(v, 563.0f / 256.0f); };
  float r = linearize(to_unit<From, 0>(src.template get_index<0>()));
  float g = linearize(to_unit<From, 1>(src.template get_index<1>()));
  float b = linearize(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

/**
 * @brief Convert Linear Adobe RGB to gamma-encoded Adobe RGB.
 *
 * Applies pure power compression with exponent 256/563 (EOTF), the exact
 * inverse of the 563/256 linearization (issue #68).
 *
 * @tparam To Target Adobe RGB type
 * @tparam From Source Linear Adobe RGB type
 */
template <typename To, typename From>
constexpr To linear_adobe_rgb_to_adobe_rgb(const From& src) {
  // Spec inverse exponent 256/563 (exact inverse of 563/256, issue #68)
  auto gamma_encode = [](float v) noexcept {
    v = std::clamp(v, 0.0f, 1.0f);
    return std::pow(v, 256.0f / 563.0f);
  };
  float r = gamma_encode(to_unit<From, 0>(src.template get_index<0>()));
  float g = gamma_encode(to_unit<From, 1>(src.template get_index<1>()));
  float b = gamma_encode(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

/**
 * @brief Convert Linear Adobe RGB to CIE XYZ (D65).
 *
 * Reference matrix from Bruce Lindbloom:
 * http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
 *
 * Adobe RGB (1998) primaries with D65 white point.
 */
template <typename To, typename From>
constexpr To linear_adobe_rgb_to_xyz(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());

  float x = 0.5766690f * r + 0.1855580f * g + 0.1882290f * b;
  float y = 0.2973450f * r + 0.6273640f * g + 0.0752910f * b;
  float z = 0.0270310f * r + 0.0706890f * g + 0.9913380f * b;

  if constexpr (To::channels >= 4) {
    float a = get_src_alpha(src);
    return pack_to<To>(from_value<To, 0>(x), from_value<To, 1>(y), from_value<To, 2>(z), from_value<To, 3>(a));
  } else {
    return pack_to<To>(from_value<To, 0>(x), from_value<To, 1>(y), from_value<To, 2>(z));
  }
}

/**
 * @brief Convert CIE XYZ (D65) to Linear Adobe RGB.
 *
 * Exact inverse of the forward matrix above (Gauss-Jordan elimination on the
 * 7-digit coefficients), so the pair is mutually inverse: linear (1,1,1)
 * round-trips back to (1,1,1). The previously used published inverse came
 * from a different source than the forward matrix and even carried a
 * transposed digit, leaving white's blue at 1.00023 — outside Adobe's own
 * gamut (issue #68).
 */
template <typename To, typename From>
constexpr To xyz_to_linear_adobe_rgb(const From& src) {
  // Read XYZ directly (XYZ channels have [0,2] range, so to_unit would incorrectly rescale)
  float x = static_cast<float>(src.template get_index<0>());
  float y = static_cast<float>(src.template get_index<1>());
  float z = static_cast<float>(src.template get_index<2>());

  float r = 2.0415871f * x - 0.5650054f * y - 0.3447321f * z;
  float g = -0.9692428f * x + 1.8759654f * y + 0.0415562f * z;
  float b = 0.0134451f * x - 0.1183627f * y + 1.0151743f * z;

  if constexpr (To::channels >= 4) {
    float a = get_src_alpha(src);
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  } else {
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
  }
}

}  // namespace colorcpp::operations::conversion::details
