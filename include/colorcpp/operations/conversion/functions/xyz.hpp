/**
 * @file xyz.hpp
 * @brief CIE XYZ conversion functions (XYZ ↔ Display P3).
 */

#pragma once

#include <cmath>
#include <colorcpp/core/display_p3.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion/helpers.hpp>

namespace colorcpp::operations::conversion::details {

template <typename To, typename From>
constexpr To linear_display_p3_to_xyz(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());

  float X = 0.4865709f * r + 0.2656677f * g + 0.1982173f * b;
  float Y = 0.2289746f * r + 0.6917385f * g + 0.0792869f * b;
  float Z = 0.0000000f * r + 0.0451134f * g + 1.0439444f * b;

  return pack_to<To>(from_value<To, 0>(X), from_value<To, 1>(Y), from_value<To, 2>(Z));
}

template <typename To, typename From>
constexpr To xyz_to_linear_display_p3(const From& src) {
  float X = static_cast<float>(src.template get_index<0>());
  float Y = static_cast<float>(src.template get_index<1>());
  float Z = static_cast<float>(src.template get_index<2>());

  float r = 2.4934969f * X - 0.9313836f * Y - 0.4027108f * Z;
  float g = -0.8294890f * X + 1.7626641f * Y + 0.0236247f * Z;
  float b = 0.0358458f * X - 0.0761724f * Y + 0.9568845f * Z;

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(1.0f));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

}  // namespace colorcpp::operations::conversion::details
