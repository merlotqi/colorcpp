/**
 * @file rec2100.hpp
 * @brief Linear Rec.2020 <-> BT.2100 signal conversions (PQ and HLG).
 *
 * Same BT.2020 primaries as @ref linear_rec2020_rgbf_t; only the transfer
 * function differs:
 *   - PQ: absolute ST 2084. Relative linear light is scaled by
 *     @ref details::kRelativeWhiteNits (Y = 1 -> 100 cd/m^2) before the
 *     encode, so white lands on PQ(100) = 0.5080784. The PQ pair itself is
 *     shared with the ICtCp path (functions/ictcp.hpp) so both encodings
 *     can never drift apart.
 *   - HLG: the scene-referred BT.2100 OETF (ARIB B-67) between scene
 *     linear [0, 1] and signal [0, 1]; the display-dependent OOTF is out
 *     of scope (see core/rec2100.hpp).
 *
 * Both TFs are sign-preserving for out-of-range values, matching the
 * rec2020 transfer-function style; the destination channel range clamps
 * at the type boundary like every non-extended display encoding.
 */

#pragma once

#include <cmath>
#include <colorcpp/core/rec2100.hpp>
#include <colorcpp/operations/conversion/detail.hpp>
#include <colorcpp/operations/conversion/functions/ictcp.hpp>
#include <colorcpp/operations/conversion/functions/rec2020.hpp>

namespace colorcpp::operations::conversion::details {

// BT.2100 HLG OETF constants (ARIB B-67).
namespace rec2100_hlg_tf {

inline constexpr float a = 0.17883277f;
inline constexpr float b = 0.28466892f;
inline constexpr float c = 0.55991073f;

/** @brief Scene linear -> HLG signal (sign-preserving for OOR input). */
inline float oetf(float e) {
  if (e == 0.0f) return 0.0f;
  const float sign = e < 0.0f ? -1.0f : 1.0f;
  const float magnitude = std::fabs(e);
  float r;
  if (magnitude <= 1.0f / 12.0f) {
    r = std::sqrt(3.0f * magnitude);
  } else {
    r = a * std::log(12.0f * magnitude - b) + c;  // 12*|e| - b > 0.71 for |e| > 1/12
  }
  return sign * r;
}

/** @brief HLG signal -> scene linear (sign-preserving for OOR input). */
inline float oetf_inv(float r) {
  if (r == 0.0f) return 0.0f;
  const float sign = r < 0.0f ? -1.0f : 1.0f;
  const float magnitude = std::fabs(r);
  float e;
  if (magnitude <= 0.5f) {
    e = magnitude * magnitude / 3.0f;
  } else {
    e = (std::exp((magnitude - c) / a) + b) / 12.0f;
  }
  return sign * e;
}

}  // namespace rec2100_hlg_tf

/**
 * @brief Linear Rec.2020 (relative) -> BT.2100 PQ signal codes.
 *
 * Scales to absolute nits (@ref kRelativeWhiteNits) before the shared PQ
 * encode; see functions/ictcp.hpp for the ST 2084 formulas.
 */
template <typename To, typename From>
constexpr To linear_rec2020_to_rec2100_pq(const From& src) {
  const float scale = kRelativeWhiteNits;
  float r = itp_pq_encode(to_unit<From, 0>(src.template get_index<0>()) * scale);
  float g = itp_pq_encode(to_unit<From, 1>(src.template get_index<1>()) * scale);
  float b = itp_pq_encode(to_unit<From, 2>(src.template get_index<2>()) * scale);
  float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

/** @brief BT.2100 PQ signal codes -> linear Rec.2020 (relative). */
template <typename To, typename From>
constexpr To rec2100_pq_to_linear_rec2020(const From& src) {
  const float scale = kRelativeWhiteNits;
  float r = itp_pq_decode(to_unit<From, 0>(src.template get_index<0>())) / scale;
  float g = itp_pq_decode(to_unit<From, 1>(src.template get_index<1>())) / scale;
  float b = itp_pq_decode(to_unit<From, 2>(src.template get_index<2>())) / scale;
  float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

/** @brief Linear Rec.2020 (scene-referred) -> BT.2100 HLG signal codes. */
template <typename To, typename From>
constexpr To linear_rec2020_to_rec2100_hlg(const From& src) {
  float r = rec2100_hlg_tf::oetf(to_unit<From, 0>(src.template get_index<0>()));
  float g = rec2100_hlg_tf::oetf(to_unit<From, 1>(src.template get_index<1>()));
  float b = rec2100_hlg_tf::oetf(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

/** @brief BT.2100 HLG signal codes -> linear Rec.2020 (scene-referred). */
template <typename To, typename From>
constexpr To rec2100_hlg_to_linear_rec2020(const From& src) {
  float r = rec2100_hlg_tf::oetf_inv(to_unit<From, 0>(src.template get_index<0>()));
  float g = rec2100_hlg_tf::oetf_inv(to_unit<From, 1>(src.template get_index<1>()));
  float b = rec2100_hlg_tf::oetf_inv(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

}  // namespace colorcpp::operations::conversion::details
