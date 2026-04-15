/**
 * @file display_p3.hpp
 * @brief Display P3 conversion functions.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/display_p3.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

template <typename To, typename From>
constexpr To display_p3_to_linear_display_p3(const From& src) {
  auto linearize = [](float v) noexcept {
    return (v <= 0.04045f) ? (v / 12.92f) : std::pow((v + 0.055f) / 1.055f, 2.4f);
  };
  float r = linearize(to_unit<From, 0>(src.template get_index<0>()));
  float g = linearize(to_unit<From, 1>(src.template get_index<1>()));
  float b = linearize(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

template <typename To, typename From>
constexpr To linear_display_p3_to_display_p3(const From& src) {
  auto gamma_encode = [](float v) noexcept {
    v = std::clamp(v, 0.0f, 1.0f);
    return (v <= 0.0031308f) ? (v * 12.92f) : (1.055f * std::pow(v, 1.0f / 2.4f) - 0.055f);
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
 * @brief Display P3 to sRGB (direct conversion).
 *
 * Display P3 uses DCI-P3 primaries with D65 white point and sRGB transfer function.
 * The conversion goes through linear space using a 3x3 matrix, then re-applies sRGB gamma.
 */
template <typename To, typename From>
constexpr To display_p3_to_srgb(const From& src) {
  // First linearize (same transfer function as sRGB)
  auto linearize = [](float v) noexcept {
    return (v <= 0.04045f) ? (v / 12.92f) : std::pow((v + 0.055f) / 1.055f, 2.4f);
  };
  float r_lin = linearize(to_unit<From, 0>(src.template get_index<0>()));
  float g_lin = linearize(to_unit<From, 1>(src.template get_index<1>()));
  float b_lin = linearize(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  // Matrix: Display P3 linear → sRGB linear
  // Reference: http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
  float r_srgb_lin = 1.2249401f * r_lin - 0.2249402f * g_lin + 0.0000000f * b_lin;
  float g_srgb_lin = -0.0420569f * r_lin + 1.0420571f * g_lin + 0.0000000f * b_lin;
  float b_srgb_lin = -0.0196376f * r_lin - 0.0786361f * g_lin + 1.0982735f * b_lin;

  // Gamma encode for sRGB
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
 * @brief sRGB to Display P3 (direct conversion).
 */
template <typename To, typename From>
constexpr To srgb_to_display_p3(const From& src) {
  // Linearize sRGB
  auto linearize = [](float v) noexcept {
    return (v <= 0.04045f) ? (v / 12.92f) : std::pow((v + 0.055f) / 1.055f, 2.4f);
  };
  float r_lin = linearize(to_unit<From, 0>(src.template get_index<0>()));
  float g_lin = linearize(to_unit<From, 1>(src.template get_index<1>()));
  float b_lin = linearize(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  // Matrix: sRGB linear → Display P3 linear
  float r_dp3_lin = 0.8224617f * r_lin + 0.1775383f * g_lin + 0.0000000f * b_lin;
  float g_dp3_lin = 0.0331942f * r_lin + 0.9668058f * g_lin + 0.0000000f * b_lin;
  float b_dp3_lin = 0.0170831f * r_lin + 0.0723974f * g_lin + 0.9105195f * b_lin;

  // Gamma encode (Display P3 uses sRGB transfer function)
  auto gamma_encode = [](float v) noexcept {
    v = std::clamp(v, 0.0f, 1.0f);
    return (v <= 0.0031308f) ? (v * 12.92f) : (1.055f * std::pow(v, 1.0f / 2.4f) - 0.055f);
  };
  float r = gamma_encode(r_dp3_lin);
  float g = gamma_encode(g_dp3_lin);
  float b = gamma_encode(b_dp3_lin);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

}  // namespace colorcpp::operations::conversion::details
