/**
 * @file aces.hpp
 * @brief ACES <-> XYZ(D65) conversions (AP0/AP1 with Bradford D60->D65)
 *        and the ACEScct grading transfer function.
 *
 * The AP0 -> XYZ matrix is the published SMPTE ST 2065-1 matrix; AP1 ->
 * XYZ is the standard normalised-primary construction with the ACES
 * white (0.32168, 0.33767) — it reproduces the published AP1 matrix
 * digit for digit (tests pin that). Both are then Bradford-adapted
 * D60 -> D65 into the library hub with the destination white = the
 * library's D65 anchor, so (1, 1, 1) maps exactly onto the reference
 * white; the composed matrices were baked from an independent
 * double-precision evaluation (anchors in tests/core/test_aces.cpp).
 *
 * ACEScct (Academy S-2016-003): toe A*x + B below X_BRK = 0.0078125
 * (A = 10.5402377416545, B = 0.0729055341958355), else
 * (log2(x) + 9.72) / 17.52 — continuous at Y_BRK = 0.155251141552511.
 */

#pragma once

#include <cmath>
#include <colorcpp/core/aces.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

/** @brief ACEScct transfer function constants (Academy S-2016-003). */
namespace acescct_tf {

constexpr float X_BRK = 0.0078125f;
constexpr float Y_BRK = 0.155251141552511f;
constexpr float A = 10.5402377416545f;
constexpr float B = 0.0729055341958355f;

/** @brief Linear AP1 -> ACEScct signal (toe below the breakpoint, log above). */
inline float encode(float x) {
  if (x <= X_BRK) return A * x + B;
  return (std::log2(x) + 9.72f) / 17.52f;
}

/** @brief ACEScct signal -> linear AP1 (inverse of @ref encode). */
inline float decode(float y) {
  if (y <= Y_BRK) return (y - B) / A;
  return std::exp2(y * 17.52f - 9.72f);
}

}  // namespace acescct_tf

// AP0 -> XYZ(D60) as published, Bradford-adapted to the library's D65 hub.
inline constexpr float M_ACES2065_TO_XYZ[9] = {0.938327114f,  -0.004436419f, 0.016579307f,  0.337391014f, 0.729514698f,
                                               -0.066905685f, 0.001170562f,  -0.003700518f, 1.091359905f};
inline constexpr float M_XYZ_TO_ACES2065[9] = {1.063449458f, 0.006387223f,  -0.015763740f, -0.492089113f, 1.368245153f,
                                               0.091355633f, -0.002809172f, 0.004632514f,  0.916614685f};
// AP1 -> XYZ(D60) via normalised primaries, Bradford-adapted to the D65 hub.
inline constexpr float M_ACESCG_TO_XYZ[9] = {0.652271352f, 0.128255509f,  0.169943141f, 0.267687342f, 0.674337135f,
                                             0.057975550f, -0.005382420f, 0.001376398f, 1.092835971f};
inline constexpr float M_XYZ_TO_ACESCG[9] = {1.660520489f, -0.315329676f, -0.241493383f, -0.659940892f, 1.608419870f,
                                             0.017297565f, 0.009009550f,  -0.003578819f, 0.913839221f};

/** @brief ACES2065-1 (AP0, linear) -> XYZ(D65 hub). */
template <typename To, typename From>
constexpr To aces2065_to_xyz(const From& src) {
  // XYZ channels have a [-1, 2] nominal range — read/pack directly (see rec2020).
  const float r = static_cast<float>(src.template get_index<0>());
  const float g = static_cast<float>(src.template get_index<1>());
  const float b = static_cast<float>(src.template get_index<2>());

  const float x = M_ACES2065_TO_XYZ[0] * r + M_ACES2065_TO_XYZ[1] * g + M_ACES2065_TO_XYZ[2] * b;
  const float y = M_ACES2065_TO_XYZ[3] * r + M_ACES2065_TO_XYZ[4] * g + M_ACES2065_TO_XYZ[5] * b;
  const float z = M_ACES2065_TO_XYZ[6] * r + M_ACES2065_TO_XYZ[7] * g + M_ACES2065_TO_XYZ[8] * b;
  const float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_value<To, 0>(x), from_value<To, 1>(y), from_value<To, 2>(z), from_value<To, 3>(a));
  else
    return pack_to<To>(from_value<To, 0>(x), from_value<To, 1>(y), from_value<To, 2>(z));
}

/** @brief XYZ(D65 hub) -> ACES2065-1 (AP0, linear). */
template <typename To, typename From>
constexpr To xyz_to_aces2065(const From& src) {
  const float x = static_cast<float>(src.template get_index<0>());
  const float y = static_cast<float>(src.template get_index<1>());
  const float z = static_cast<float>(src.template get_index<2>());

  const float r = M_XYZ_TO_ACES2065[0] * x + M_XYZ_TO_ACES2065[1] * y + M_XYZ_TO_ACES2065[2] * z;
  const float g = M_XYZ_TO_ACES2065[3] * x + M_XYZ_TO_ACES2065[4] * y + M_XYZ_TO_ACES2065[5] * z;
  const float b = M_XYZ_TO_ACES2065[6] * x + M_XYZ_TO_ACES2065[7] * y + M_XYZ_TO_ACES2065[8] * z;
  const float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

/** @brief ACEScg (AP1, linear) -> XYZ(D65 hub). */
template <typename To, typename From>
constexpr To acescg_to_xyz(const From& src) {
  const float r = static_cast<float>(src.template get_index<0>());
  const float g = static_cast<float>(src.template get_index<1>());
  const float b = static_cast<float>(src.template get_index<2>());

  const float x = M_ACESCG_TO_XYZ[0] * r + M_ACESCG_TO_XYZ[1] * g + M_ACESCG_TO_XYZ[2] * b;
  const float y = M_ACESCG_TO_XYZ[3] * r + M_ACESCG_TO_XYZ[4] * g + M_ACESCG_TO_XYZ[5] * b;
  const float z = M_ACESCG_TO_XYZ[6] * r + M_ACESCG_TO_XYZ[7] * g + M_ACESCG_TO_XYZ[8] * b;
  const float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_value<To, 0>(x), from_value<To, 1>(y), from_value<To, 2>(z), from_value<To, 3>(a));
  else
    return pack_to<To>(from_value<To, 0>(x), from_value<To, 1>(y), from_value<To, 2>(z));
}

/** @brief XYZ(D65 hub) -> ACEScg (AP1, linear). */
template <typename To, typename From>
constexpr To xyz_to_acescg(const From& src) {
  const float x = static_cast<float>(src.template get_index<0>());
  const float y = static_cast<float>(src.template get_index<1>());
  const float z = static_cast<float>(src.template get_index<2>());

  const float r = M_XYZ_TO_ACESCG[0] * x + M_XYZ_TO_ACESCG[1] * y + M_XYZ_TO_ACESCG[2] * z;
  const float g = M_XYZ_TO_ACESCG[3] * x + M_XYZ_TO_ACESCG[4] * y + M_XYZ_TO_ACESCG[5] * z;
  const float b = M_XYZ_TO_ACESCG[6] * x + M_XYZ_TO_ACESCG[7] * y + M_XYZ_TO_ACESCG[8] * z;
  const float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

/** @brief ACEScg (AP1 linear) -> ACEScct signal (S-2016-003 log). */
template <typename To, typename From>
constexpr To acescg_to_acescct(const From& src) {
  float r = acescct_tf::encode(to_unit<From, 0>(src.template get_index<0>()));
  float g = acescct_tf::encode(to_unit<From, 1>(src.template get_index<1>()));
  float b = acescct_tf::encode(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

/** @brief ACEScct signal (S-2016-003 log) -> ACEScg (AP1 linear). */
template <typename To, typename From>
constexpr To acescct_to_acescg(const From& src) {
  float r = acescct_tf::decode(to_unit<From, 0>(src.template get_index<0>()));
  float g = acescct_tf::decode(to_unit<From, 1>(src.template get_index<1>()));
  float b = acescct_tf::decode(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

}  // namespace colorcpp::operations::conversion::details
