/**
 * @file xyy.hpp
 * @brief CIE xyY conversion functions.
 */

#pragma once

#include <colorcpp/core/xyy.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

/**
 * @brief Convert XYZ tristimulus values to CIE xyY chromaticity.
 *
 * x = X / (X + Y + Z), y = Y / (X + Y + Z), Y unchanged. A zero sum
 * (black) yields x = y = 0 rather than NaN, matching colour-science's
 * safe-division convention.
 *
 * @tparam To Target xyY color type (xyy_t)
 * @tparam From Source XYZ color type (xyz_t)
 * @param src Input XYZ color
 * @return Converted xyY color
 */
template <typename To, typename From>
constexpr To xyz_to_xyy(const From& src) {
  float X = static_cast<float>(src.template get_index<0>());
  float Y = static_cast<float>(src.template get_index<1>());
  float Z = static_cast<float>(src.template get_index<2>());

  float sum = X + Y + Z;
  float x = (sum != 0.0f) ? X / sum : 0.0f;
  float y = (sum != 0.0f) ? Y / sum : 0.0f;

  return pack_to<To>(from_value<To, 0>(x), from_value<To, 1>(y), from_value<To, 2>(Y));
}

/**
 * @brief Convert CIE xyY chromaticity to XYZ tristimulus values.
 *
 * X = x * Y / y, Y unchanged, Z = (1 - x - y) * Y / y. y = 0 yields
 * X = Z = 0 (safe-division convention) instead of NaN.
 *
 * @tparam To Target XYZ color type (xyz_t)
 * @tparam From Source xyY color type (xyy_t)
 * @param src Input xyY color
 * @return Converted XYZ color
 */
template <typename To, typename From>
constexpr To xyy_to_xyz(const From& src) {
  float x = static_cast<float>(src.template get_index<0>());
  float y = static_cast<float>(src.template get_index<1>());
  float Y = static_cast<float>(src.template get_index<2>());

  if (y != 0.0f) {
    float t = Y / y;
    float X = x * t;
    float Z = (1.0f - x - y) * t;
    return pack_to<To>(from_value<To, 0>(X), from_value<To, 1>(Y), from_value<To, 2>(Z));
  }
  return pack_to<To>(from_value<To, 0>(0.0f), from_value<To, 1>(Y), from_value<To, 2>(0.0f));
}

}  // namespace colorcpp::operations::conversion::details
