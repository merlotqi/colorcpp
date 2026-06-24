/**
 * @file prophoto_rgb.hpp
 * @brief ProPhoto RGB (ROMM RGB) conversion functions.
 *
 * ProPhoto RGB uses Gamma 1.8 with a linear segment below 1/512.
 * White point: D50 (differs from XYZ(D65) hub, requiring Bradford adaptation).
 *
 * Conversion path:
 *   ProPhoto gamma → Linear ProPhoto (D50) → [Bradford D50→D65] → XYZ(D65)
 *   XYZ(D65) → [Bradford D65→D50] → Linear ProPhoto (D50) → ProPhoto gamma
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/prophoto_rgb.hpp>
#include <colorcpp/algorithms/chromatic_adaptation.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

/**
 * @brief ProPhoto RGB transfer function constants.
 */
namespace prophoto_tf {

// Linear segment cutoff on the linear side
constexpr float linear_cutoff = 1.0f / 512.0f;  // ≈ 0.001953125

// Linear segment cutoff on the encoded side (inverse)
constexpr float encoded_cutoff = 1.0f / 32.0f;  // ≈ 0.03125

constexpr float gamma = 1.8f;

/**
 * @brief Linearize a ProPhoto RGB encoded channel value.
 *
 * ProPhoto transfer: below 1/512 use linear segment, otherwise power 1.8.
 */
constexpr float to_linear(float v) noexcept {
  if (v < encoded_cutoff)
    return v / 16.0f;
  else
    return std::pow(v, gamma);
}

/**
 * @brief Encode a linear ProPhoto RGB channel value.
 */
constexpr float from_linear(float v) noexcept {
  if (v < linear_cutoff)
    return v * 16.0f;
  else
    return std::pow(v, 1.0f / gamma);
}

}  // namespace prophoto_tf

/**
 * @brief Convert ProPhoto RGB gamma-encoded color to Linear ProPhoto RGB.
 *
 * Applies ProPhoto transfer (Gamma 1.8 with linear shadow segment).
 *
 * @tparam To Target Linear ProPhoto RGB type
 * @tparam From Source ProPhoto RGB type
 */
template <typename To, typename From>
constexpr To prophoto_rgb_to_linear_prophoto_rgb(const From& src) {
  float r = prophoto_tf::to_linear(to_unit<From, 0>(src.template get_index<0>()));
  float g = prophoto_tf::to_linear(to_unit<From, 1>(src.template get_index<1>()));
  float b = prophoto_tf::to_linear(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

/**
 * @brief Convert Linear ProPhoto RGB to gamma-encoded ProPhoto RGB.
 *
 * Applies inverse ProPhoto transfer (power 1/1.8 with linear shadow segment).
 *
 * @tparam To Target ProPhoto RGB type
 * @tparam From Source Linear ProPhoto RGB type
 */
template <typename To, typename From>
constexpr To linear_prophoto_rgb_to_prophoto_rgb(const From& src) {
  auto encode = [](float v) noexcept {
    v = std::clamp(v, 0.0f, 1.0f);
    return prophoto_tf::from_linear(v);
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
 * @brief Convert Linear ProPhoto RGB (D50) to CIE XYZ (D65).
 *
 * This is a two-step conversion:
 *   1. Linear ProPhoto RGB → XYZ(D50) via ProPhoto primaries matrix
 *   2. XYZ(D50) → XYZ(D65) via Bradford chromatic adaptation
 *
 * Reference:
 *   http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
 */
template <typename To, typename From>
constexpr To linear_prophoto_rgb_to_xyz(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());

  // Step 1: Linear ProPhoto RGB → XYZ(D50) via pure primaries matrix
  // M_prophoto_to_xyz(D50) from Bruce Lindbloom
  float x_d50 = 0.7977605f * r + 0.1351858f * g + 0.0313493f * b;
  float y_d50 = 0.2880711f * r + 0.7118432f * g + 0.0000857f * b;
  float z_d50 = 0.0000000f * r + 0.0000000f * g + 0.8251046f * b;

  // Step 2: Bradford chromatic adaptation D50 → D65
  core::xyz_t xyz_d50{x_d50, y_d50, z_d50};
  auto xyz_d65 = algorithms::chromatic_adaptation::bradford_adapt(
      xyz_d50, algorithms::chromatic_adaptation::WHITEPOINT_D50,
      algorithms::chromatic_adaptation::WHITEPOINT_D65);

  if constexpr (To::channels >= 4) {
    float a = get_src_alpha(src);
    return pack_to<To>(from_value<To, 0>(xyz_d65.x()), from_value<To, 1>(xyz_d65.y()),
                       from_value<To, 2>(xyz_d65.z()), from_value<To, 3>(a));
  } else {
    return pack_to<To>(from_value<To, 0>(xyz_d65.x()), from_value<To, 1>(xyz_d65.y()),
                       from_value<To, 2>(xyz_d65.z()));
  }
}

/**
 * @brief Convert CIE XYZ (D65) to Linear ProPhoto RGB (D50).
 *
 * This is a two-step conversion:
 *   1. XYZ(D65) → XYZ(D50) via inverse Bradford chromatic adaptation
 *   2. XYZ(D50) → Linear ProPhoto RGB via inverse ProPhoto primaries matrix
 */
template <typename To, typename From>
constexpr To xyz_to_linear_prophoto_rgb(const From& src) {
  // Read XYZ directly — XYZ channels use natural [0,2] range, not unit [0,1]
  float x = static_cast<float>(src.template get_index<0>());
  float y = static_cast<float>(src.template get_index<1>());
  float z = static_cast<float>(src.template get_index<2>());

  // Step 1: Bradford chromatic adaptation D65 → D50
  core::xyz_t xyz_d65{x, y, z};
  auto xyz_d50 = algorithms::chromatic_adaptation::bradford_adapt(
      xyz_d65, algorithms::chromatic_adaptation::WHITEPOINT_D65,
      algorithms::chromatic_adaptation::WHITEPOINT_D50);

  // Step 2: XYZ(D50) → Linear ProPhoto RGB via inverse primaries matrix
  // M_xyz_to_prophoto(D50) from Bruce Lindbloom
  float r =  1.3459433f * xyz_d50.x() - 0.2556075f * xyz_d50.y() - 0.0511118f * xyz_d50.z();
  float g = -0.5445989f * xyz_d50.x() + 1.5081673f * xyz_d50.y() + 0.0205351f * xyz_d50.z();
  float b =  0.0000000f * xyz_d50.x() + 0.0000000f * xyz_d50.y() + 1.2119587f * xyz_d50.z();

  if constexpr (To::channels >= 4) {
    float a = get_src_alpha(src);
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  } else {
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
  }
}

}  // namespace colorcpp::operations::conversion::details
