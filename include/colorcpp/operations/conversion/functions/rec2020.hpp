/**
 * @file rec2020.hpp
 * @brief Rec.2020 (BT.2020) conversion functions.
 *
 * Rec.2020 uses BT.709-style piecewise transfer function.
 * White point: D65 (same as sRGB/XYZ hub, no chromatic adaptation needed).
 *
 * Transfer function parameters:
 *   α = 1.09929682680944
 *   β = 0.018053968510807
 *   Linear segment threshold (encoded) = β × 4.5 ≈ 0.081242858
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/rec2020.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

/**
 * @brief Rec.2020 transfer function constants (BT.709-style piecewise).
 */
namespace rec2020_tf {

constexpr float alpha = 1.09929682680944f;
constexpr float beta = 0.018053968510807f;
constexpr float beta_times_4_5 = beta * 4.5f;  // ≈ 0.081242858
constexpr float exponent = 0.45f;              // 1/2.222...
constexpr float inv_exponent = 1.0f / 0.45f;   // ≈ 2.222...

/**
 * @brief Linearize a Rec.2020 encoded channel value.
 */
constexpr float to_linear(float v) noexcept {
  if (v < beta_times_4_5)
    return v / 4.5f;
  else
    return std::pow((v + alpha - 1.0f) / alpha, inv_exponent);
}

/**
 * @brief Encode a linear channel value to Rec.2020 gamma.
 */
constexpr float from_linear(float v) noexcept {
  if (v < beta)
    return v * 4.5f;
  else
    return alpha * std::pow(v, exponent) - (alpha - 1.0f);
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

/**
 * @brief Convert Rec.2020 directly to sRGB (4-hop shortcut).
 *
 * Optimized direct conversion: Rec.2020 linearize -> matrix -> sRGB gamma encode.
 * Avoids intermediate XYZ object allocation.
 */
template <typename To, typename From>
constexpr To rec2020_to_srgb(const From& src) {
  // Step 1: Linearize Rec.2020 gamma
  float r_lin = rec2020_tf::to_linear(to_unit<From, 0>(src.template get_index<0>()));
  float g_lin = rec2020_tf::to_linear(to_unit<From, 1>(src.template get_index<1>()));
  float b_lin = rec2020_tf::to_linear(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  // Step 2: Matrix: Linear Rec.2020 -> Linear sRGB (computed as M_xyz_to_srgb * M_rec2020_to_xyz)
  float r_srgb_lin = 1.6605f * r_lin - 0.5876f * g_lin - 0.0728f * b_lin;
  float g_srgb_lin = -0.1246f * r_lin + 1.1329f * g_lin - 0.0083f * b_lin;
  float b_srgb_lin = -0.0182f * r_lin - 0.1006f * g_lin + 1.1187f * b_lin;

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
 * @brief Convert sRGB directly to Rec.2020 (4-hop shortcut).
 *
 * Optimized direct conversion: sRGB linearize -> inverse matrix -> Rec.2020 gamma encode.
 */
template <typename To, typename From>
constexpr To srgb_to_rec2020(const From& src) {
  // Step 1: Linearize sRGB
  auto linearize = [](float v) noexcept {
    return (v <= 0.04045f) ? (v / 12.92f) : std::pow((v + 0.055f) / 1.055f, 2.4f);
  };
  float r_lin = linearize(to_unit<From, 0>(src.template get_index<0>()));
  float g_lin = linearize(to_unit<From, 1>(src.template get_index<1>()));
  float b_lin = linearize(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  // Step 2: Matrix: Linear sRGB -> Linear Rec.2020 (inverse of above)
  float r_r2020_lin = 0.6274f * r_lin + 0.3293f * g_lin + 0.0433f * b_lin;
  float g_r2020_lin = 0.0691f * r_lin + 0.9195f * g_lin + 0.0114f * b_lin;
  float b_r2020_lin = 0.0164f * r_lin + 0.0880f * g_lin + 0.8956f * b_lin;

  // Step 3: Gamma encode Rec.2020
  auto gamma_encode = [](float v) noexcept {
    v = std::clamp(v, 0.0f, 1.0f);
    return rec2020_tf::from_linear(v);
  };
  float r = gamma_encode(r_r2020_lin);
  float g = gamma_encode(g_r2020_lin);
  float b = gamma_encode(b_r2020_lin);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

}  // namespace colorcpp::operations::conversion::details
