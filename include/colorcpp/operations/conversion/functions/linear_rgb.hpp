/**
 * @file linear_rgb.hpp
 * @brief Linear RGB conversion functions (sRGB ↔ Linear RGB, Linear RGB ↔ XYZ).
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/linear_rgb.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

/**
 * @brief Convert gamma-encoded sRGB to Linear RGB.
 *
 * Applies IEC 61966-2-1:1999 sRGB gamma decompression curve.
 * This is the inverse function of linear_rgb_to_srgb.
 *
 * Algorithm details:
 * - Values ≤ 0.04045: linear scaling by 1/12.92
 * - Values > 0.04045: power curve ((v + 0.055)/1.055)^2.4
 *
 * @tparam To Target Linear RGB type (linear_rgbf_t, linear_rgbaf_t)
 * @tparam From Source sRGB type (rgb8_t, rgbf_t, rgba8_t, rgbaf_t)
 * @param src Input gamma encoded sRGB color
 * @return Linearized RGB color with alpha channel preserved if present
 */
template <typename To, typename From>
constexpr To srgb_to_linear_rgb(const From& src) {
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

/**
 * @brief Convert Linear RGB to gamma-encoded sRGB.
 *
 * Applies IEC 61966-2-1:1999 sRGB gamma compression curve.
 * This is the inverse function of srgb_to_linear_rgb.
 *
 * Algorithm details:
 * - Values ≤ 0.0031308: linear scaling by 12.92
 * - Values > 0.0031308: power curve 1.055 * v^(1/2.4) - 0.055
 *
 * @tparam To Target sRGB type (rgb8_t, rgbf_t, rgba8_t, rgbaf_t)
 * @tparam From Source Linear RGB type (linear_rgbf_t, linear_rgbaf_t)
 * @param src Input linear RGB color
 * @return Gamma encoded sRGB color with alpha channel preserved if present
 */
template <typename To, typename From>
constexpr To linear_rgb_to_srgb(const From& src) {
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
 * @brief Convert Linear sRGB to XYZ tristimulus values.
 *
 * Applies standard sRGB to XYZ conversion matrix.
 * Uses D65 standard illuminant reference white point.
 * Matrix values from IEC 61966-2-1:1999 specification.
 *
 * @tparam To Target XYZ color type (xyz_t)
 * @tparam From Source Linear RGB type (linear_rgbf_t, linear_rgbaf_t)
 * @param src Input Linear sRGB color
 * @return Converted XYZ color
 */
template <typename To, typename From>
constexpr To linear_rgb_to_xyz(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());

  float X = 0.4124564f * r + 0.3575761f * g + 0.1804375f * b;
  float Y = 0.2126729f * r + 0.7151522f * g + 0.0721750f * b;
  float Z = 0.0193339f * r + 0.1191920f * g + 0.9503041f * b;

  return pack_to<To>(from_value<To, 0>(X), from_value<To, 1>(Y), from_value<To, 2>(Z));
}

/**
 * @brief Convert XYZ tristimulus values to Linear sRGB.
 *
 * Applies standard XYZ to sRGB inverse conversion matrix.
 * Uses D65 standard illuminant reference white point.
 * This is the inverse function of linear_rgb_to_xyz.
 *
 * @tparam To Target Linear RGB type (linear_rgbf_t, linear_rgbaf_t)
 * @tparam From Source XYZ color type (xyz_t)
 * @param src Input XYZ color
 * @return Converted Linear sRGB color with alpha set to 1.0 if present
 */
template <typename To, typename From>
constexpr To xyz_to_linear_rgb(const From& src) {
  float X = static_cast<float>(src.template get_index<0>());
  float Y = static_cast<float>(src.template get_index<1>());
  float Z = static_cast<float>(src.template get_index<2>());

  float r = 3.2404542f * X - 1.5371385f * Y - 0.4985314f * Z;
  float g = -0.9692660f * X + 1.8760108f * Y + 0.0415560f * Z;
  float b = 0.0556434f * X - 0.2040259f * Y + 1.0572252f * Z;

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(1.0f));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

// RGBAF ↔ RGBF (alpha strip/fill)
/**
 * @brief Convert RGBAF to RGBF by stripping alpha channel.
 *
 * Simple conversion that drops the fourth (alpha) channel.
 * Used for automatic conversion between 4-channel and 3-channel types.
 *
 * @tparam To Target 3-channel RGB float type (rgbf_t)
 * @tparam From Source 4-channel RGBA float type (rgbaf_t)
 * @param src Input RGBA color
 * @return RGB color without alpha channel
 */
template <typename To, typename From>
constexpr To rgbaf_to_rgbf(const From& src) {
  return details::pack_to<To>(src.template get_index<0>(), src.template get_index<1>(), src.template get_index<2>());
}

/**
 * @brief Convert RGBF to RGBAF by adding full opacity alpha channel.
 *
 * Simple conversion that adds a fourth (alpha) channel with value 1.0.
 * Used for automatic conversion between 3-channel and 4-channel types.
 *
 * @tparam To Target 4-channel RGBA float type (rgbaf_t)
 * @tparam From Source 3-channel RGB float type (rgbf_t)
 * @param src Input RGB color
 * @return RGBA color with full opacity (alpha = 1.0)
 */
template <typename To, typename From>
constexpr To rgbf_to_rgbaf(const From& src) {
  return details::pack_to<To>(src.template get_index<0>(), src.template get_index<1>(), src.template get_index<2>(),
                              1.0f);
}

}  // namespace colorcpp::operations::conversion::details
