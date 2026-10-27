/**
 * @file rec2020.hpp
 * @brief Rec.2020 (BT.2020) conversion functions.
 *
 * Rec.2020 uses a pure gamma 2.4 transfer function (BT.1886), as defined by
 * CSS Color 4 section 10.8 — NOT the BT.2020 broadcast OETF (alpha/beta piecewise).
 * White point: D65 (same as sRGB/XYZ hub, no chromatic adaptation needed).
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/rec2020.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

/**
 * @brief Rec.2020 transfer function (pure gamma 2.4, BT.1886).
 *
 * CSS Color 4 section 10.8 defines the rec2020 transfer function as
 * "gamma 2.40, from [BT.1886]". Sign-preserving for extended-range values,
 * matching the CSS reference implementation (color.js).
 */
namespace rec2020_tf {

constexpr float exponent = 2.4f;
constexpr float inv_exponent = 1.0f / 2.4f;

/**
 * @brief Linearize a Rec.2020 encoded channel value (sign-preserving).
 */
constexpr float to_linear(float v) noexcept {
  if (v == 0.0f) return v;  // preserves signed zero; skips pow(0)
  const float av = std::fabs(v);
  return std::copysign(std::pow(av, exponent), v);
}

/**
 * @brief Encode a linear channel value to Rec.2020 gamma (sign-preserving).
 */
constexpr float from_linear(float v) noexcept {
  if (v == 0.0f) return v;  // preserves signed zero; skips pow(0)
  const float av = std::fabs(v);
  return std::copysign(std::pow(av, inv_exponent), v);
}

}  // namespace rec2020_tf

/**
 * @brief Convert Rec.2020 gamma-encoded to Linear Rec.2020.
 */
template <typename To, typename From>
constexpr To rec2020_to_linear_rec2020(const From& src) {
  float r = rec2020_tf::to_linear(to_unit<From, 0>(src.template get_index<0>()));
  float g = rec2020_tf::to_linear(to_unit<From, 1>(src.template get_index<1>()));
  float b = rec2020_tf::to_linear(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

/**
 * @brief Convert Linear Rec.2020 to gamma-encoded Rec.2020.
 */
template <typename To, typename From>
constexpr To linear_rec2020_to_rec2020(const From& src) {
  auto encode = [](float v) noexcept {
    v = std::clamp(v, 0.0f, 1.0f);
    return rec2020_tf::from_linear(v);
  };
  float r = encode(to_unit<From, 0>(src.template get_index<0>()));
  float g = encode(to_unit<From, 1>(src.template get_index<1>()));
  float b = encode(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

/**
 * @brief Convert Linear Rec.2020 to CIE XYZ (D65).
 *
 * Standard BT.2020 primaries matrix (ITU-R BT.2020-2, D65 white point).
 */
template <typename To, typename From>
constexpr To linear_rec2020_to_xyz(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());

  float x = 0.6369580483012914f * r + 0.14461690358620838f * g + 0.16888097516417213f * b;
  float y = 0.2627002120112671f * r + 0.6779980715188708f * g + 0.05930171646986196f * b;
  float z = 0.0000000000000000f * r + 0.028072693049087428f * g + 1.0609850577107909f * b;

  // Use from_value for XYZ output (XYZ channels have [0,2] range, not [0,1])
  if constexpr (To::channels >= 4) {
    float a = get_src_alpha(src);
    return pack_to<To>(from_value<To, 0>(x), from_value<To, 1>(y), from_value<To, 2>(z), from_value<To, 3>(a));
  } else {
    return pack_to<To>(from_value<To, 0>(x), from_value<To, 1>(y), from_value<To, 2>(z));
  }
}

/**
 * @brief Convert CIE XYZ (D65) to Linear Rec.2020.
 *
 * Inverse BT.2020 matrix (ITU-R BT.2020-2).
 */
template <typename To, typename From>
constexpr To xyz_to_linear_rec2020(const From& src) {
  // Read XYZ directly (XYZ channels have [0,2] range, so to_unit would incorrectly rescale)
  float x = static_cast<float>(src.template get_index<0>());
  float y = static_cast<float>(src.template get_index<1>());
  float z = static_cast<float>(src.template get_index<2>());

  float r = 1.7166511879712679f * x - 0.35567078377639240f * y - 0.25336628137365992f * z;
  float g = -0.6666843518324889f * x + 1.6164812366349388f * y + 0.015768545813911114f * z;
  float b = 0.01763985744531078f * x - 0.04277061325780865f * y + 0.942103121235474f * z;

  if constexpr (To::channels >= 4) {
    float a = get_src_alpha(src);
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  } else {
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
  }
}

}  // namespace colorcpp::operations::conversion::details
