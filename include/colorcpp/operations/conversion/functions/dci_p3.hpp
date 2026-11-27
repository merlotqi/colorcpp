/**
 * @file dci_p3.hpp
 * @brief DCI-P3 <-> XYZ(D65) conversion: gamma 2.6 + Bradford D62->D65.
 *
 * The matrix is the composition of
 *   primaries -> XYZ(D62)  (D62 white xy = 0.314, 0.351)
 *   Bradford adaptation D62 -> D65 (library M_BFD constants, destination
 *   white = the library's D65 anchor 0.95047, 1, 1.08883, so signal
 *   (1, 1, 1) maps exactly onto the reference white),
 * baked at authoring time from an independent double-precision
 * evaluation (see tests/core/test_dci_p3.cpp for the pinned anchors).
 * The theatrical EOTF is pure gamma 2.6, sign-preserving like the other
 * display transfer functions.
 */

#pragma once

#include <cmath>
#include <colorcpp/core/dci_p3.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

/** @brief DCI-P3 theatrical transfer function (pure gamma 2.6). */
namespace dci_p3_tf {

constexpr float exponent = 2.6f;
constexpr float inv_exponent = 1.0f / 2.6f;

/** @brief Signal -> linear (sign-preserving for OOR values). */
constexpr float to_linear(float v) noexcept {
  if (v == 0.0f) return v;
  const float av = std::fabs(v);
  return std::copysign(std::pow(av, exponent), v);
}

/** @brief Linear -> signal (sign-preserving for OOR values). */
constexpr float from_linear(float v) noexcept {
  if (v == 0.0f) return v;
  const float av = std::fabs(v);
  return std::copysign(std::pow(av, inv_exponent), v);
}

}  // namespace dci_p3_tf

// DCI-P3 primaries + D62 white, Bradford-adapted to the library's D65 hub.
inline constexpr float M_DCI_P3_TO_XYZ[9] = {0.459275835f, 0.295817100f, 0.195377067f, 0.215160879f, 0.709134294f,
                                             0.075704855f, 0.000271053f, 0.046936212f, 1.041622684f};
inline constexpr float M_XYZ_TO_DCI_P3[9] = {2.690136320f, -1.094062211f, -0.425072468f, -0.820093881f, 1.750513820f,
                                             0.026598060f, 0.036253944f,  -0.078594620f, 0.958952623f};

/** @brief DCI-P3 signal (gamma 2.6) -> XYZ(D65 hub). */
template <typename To, typename From>
constexpr To dci_p3_to_xyz(const From& src) {
  const float r = dci_p3_tf::to_linear(to_unit<From, 0>(src.template get_index<0>()));
  const float g = dci_p3_tf::to_linear(to_unit<From, 1>(src.template get_index<1>()));
  const float b = dci_p3_tf::to_linear(to_unit<From, 2>(src.template get_index<2>()));
  const float a = get_src_alpha(src);

  const float x = M_DCI_P3_TO_XYZ[0] * r + M_DCI_P3_TO_XYZ[1] * g + M_DCI_P3_TO_XYZ[2] * b;
  const float y = M_DCI_P3_TO_XYZ[3] * r + M_DCI_P3_TO_XYZ[4] * g + M_DCI_P3_TO_XYZ[5] * b;
  const float z = M_DCI_P3_TO_XYZ[6] * r + M_DCI_P3_TO_XYZ[7] * g + M_DCI_P3_TO_XYZ[8] * b;

  // XYZ channels have a [-1, 2] nominal range — from_value clamps without
  // rescaling (from_unit would wrongly affine-map the value; see rec2020).
  if constexpr (To::channels >= 4)
    return pack_to<To>(from_value<To, 0>(x), from_value<To, 1>(y), from_value<To, 2>(z), from_value<To, 3>(a));
  else
    return pack_to<To>(from_value<To, 0>(x), from_value<To, 1>(y), from_value<To, 2>(z));
}

/** @brief XYZ(D65 hub) -> DCI-P3 signal (gamma 2.6). */
template <typename To, typename From>
constexpr To xyz_to_dci_p3(const From& src) {
  const float x = static_cast<float>(src.template get_index<0>());
  const float y = static_cast<float>(src.template get_index<1>());
  const float z = static_cast<float>(src.template get_index<2>());

  const float r = dci_p3_tf::from_linear(M_XYZ_TO_DCI_P3[0] * x + M_XYZ_TO_DCI_P3[1] * y + M_XYZ_TO_DCI_P3[2] * z);
  const float g = dci_p3_tf::from_linear(M_XYZ_TO_DCI_P3[3] * x + M_XYZ_TO_DCI_P3[4] * y + M_XYZ_TO_DCI_P3[5] * z);
  const float b = dci_p3_tf::from_linear(M_XYZ_TO_DCI_P3[6] * x + M_XYZ_TO_DCI_P3[7] * y + M_XYZ_TO_DCI_P3[8] * z);
  const float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

}  // namespace colorcpp::operations::conversion::details
