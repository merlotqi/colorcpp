/**
 * @file adobe_rgb.hpp
 * @brief Adobe RGB (A98-RGB) conversion functions.
 *
 * Adobe RGB uses pure Gamma 2.2 transfer function (no linear segment like sRGB).
 * White point: D65 (same as sRGB/XYZ hub, no chromatic adaptation needed).
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
 * Applies pure Gamma 2.2 decompression (inverse EOTF).
 * Adobe RGB uses the same D65 white point as sRGB.
 *
 * @tparam To Target Linear Adobe RGB type
 * @tparam From Source Adobe RGB type
 */
template <typename To, typename From>
constexpr To adobe_rgb_to_linear_adobe_rgb(const From& src) {
  // Pure Gamma 2.2 linearization (no sRGB-style linear segment)
  auto linearize = [](float v) noexcept { return std::pow(v, 2.2f); };
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
 * Applies pure Gamma 2.2 compression (EOTF).
 *
 * @tparam To Target Adobe RGB type
 * @tparam From Source Linear Adobe RGB type
 */
template <typename To, typename From>
constexpr To linear_adobe_rgb_to_adobe_rgb(const From& src) {
  // Pure Gamma 2.2 encoding (inverse of linearization)
  auto gamma_encode = [](float v) noexcept {
    v = std::clamp(v, 0.0f, 1.0f);
    return std::pow(v, 1.0f / 2.2f);
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
 * Reference matrix from Bruce Lindbloom (inverse of above).
 */
template <typename To, typename From>
constexpr To xyz_to_linear_adobe_rgb(const From& src) {
  // Read XYZ directly (XYZ channels have [0,2] range, so to_unit would incorrectly rescale)
  float x = static_cast<float>(src.template get_index<0>());
  float y = static_cast<float>(src.template get_index<1>());
  float z = static_cast<float>(src.template get_index<2>());

  float r = 2.0413690f * x - 0.5649654f * y - 0.3446945f * z;
  float g = -0.9692660f * x + 1.8760108f * y + 0.0415560f * z;
  float b = 0.0134474f * x - 0.1183897f * y + 1.0154096f * z;

  if constexpr (To::channels >= 4) {
    float a = get_src_alpha(src);
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  } else {
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
  }
}

/**
 * @brief Convert Adobe RGB directly to sRGB.
 *
 * Optimized direct conversion: Linearize (Gamma 2.2) → matrix → sRGB gamma encode.
 * Avoids intermediate XYZ object creation.
 *
 * @tparam To Target sRGB type
 * @tparam From Source Adobe RGB type
 */
template <typename To, typename From>
constexpr To adobe_rgb_to_srgb(const From& src) {
  // Step 1: Linearize (Gamma 2.2)
  auto linearize = [](float v) noexcept { return std::pow(v, 2.2f); };
  float r_lin = linearize(to_unit<From, 0>(src.template get_index<0>()));
  float g_lin = linearize(to_unit<From, 1>(src.template get_index<1>()));
  float b_lin = linearize(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  // Step 2: Matrix: Adobe RGB linear → sRGB linear
  // Computed as: M_xyz_to_srgb_linear × M_adobe_to_xyz
  float r_srgb_lin = 1.398f * r_lin - 0.399f * g_lin + 0.001f * b_lin;
  float g_srgb_lin = 0.000f * r_lin + 1.000f * g_lin + 0.000f * b_lin;
  float b_srgb_lin = 0.000f * r_lin - 0.043f * g_lin + 1.043f * b_lin;

  // Step 3: Gamma encode for sRGB
  auto gamma_encode = [](float v) noexcept {
    v = std::clamp(v, 0.0f, 1.0f);
    return (v <= 0.0031308f) ? (v * 12.92f) : (1.055f * std::pow(v, 1.0f / 2.4f) - 0.055f);
  };
  float r = gamma_encode(r_srgb_lin);
  float g = gamma_encode(g_srgb_lin);
  float b = gamma_encode(b_srgb_lin);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

/**
 * @brief Convert sRGB directly to Adobe RGB.
 *
 * Optimized direct conversion: sRGB linearize → inverse matrix → Gamma 2.2 encode.
 *
 * @tparam To Target Adobe RGB type
 * @tparam From Source sRGB type
 */
template <typename To, typename From>
constexpr To srgb_to_adobe_rgb(const From& src) {
  // Step 1: Linearize sRGB
  auto linearize = [](float v) noexcept {
    return (v <= 0.04045f) ? (v / 12.92f) : std::pow((v + 0.055f) / 1.055f, 2.4f);
  };
  float r_lin = linearize(to_unit<From, 0>(src.template get_index<0>()));
  float g_lin = linearize(to_unit<From, 1>(src.template get_index<1>()));
  float b_lin = linearize(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  // Step 2: Matrix: sRGB linear → Adobe RGB linear (inverse of above)
  float r_adbe_lin = 0.715f * r_lin + 0.285f * g_lin + 0.000f * b_lin;
  float g_adbe_lin = 0.000f * r_lin + 1.000f * g_lin + 0.000f * b_lin;
  float b_adbe_lin = 0.000f * r_lin + 0.041f * g_lin + 0.959f * b_lin;

  // Step 3: Gamma encode (Adobe RGB Gamma 2.2)
  auto gamma_encode = [](float v) noexcept {
    v = std::clamp(v, 0.0f, 1.0f);
    return std::pow(v, 1.0f / 2.2f);
  };
  float r = gamma_encode(r_adbe_lin);
  float g = gamma_encode(g_adbe_lin);
  float b = gamma_encode(b_adbe_lin);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

}  // namespace colorcpp::operations::conversion::details
