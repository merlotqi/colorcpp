/**
 * @file ycbcr.hpp
 * @brief RGB <-> YCbCr matrix conversions for BT.601 / BT.709 / BT.2020.
 *
 * The luma weights are the ITU-R Kr/Kg/Kb triples; chroma follows the
 * normalized definitions Cb = (B - Y) / (2 (1 - Kb)),
 * Cr = (R - Y) / (2 (1 - Kr)). Inputs and outputs are float channels in
 * [0, 1]; quantization to limited/full-range codes lives in
 * @ref core::ycbcr::quantize.
 */

#pragma once

#include <colorcpp/core/rgb.hpp>
#include <colorcpp/core/ycbcr.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

/**
 * @brief RGB (normalized [0, 1]) to YCbCr under the given luma weights.
 * @tparam To Target ycbcr601_t / ycbcr709_t / ycbcr2020_t.
 */
template <typename To, typename From>
constexpr To rgb_to_ycbcr(const From& src, float kr, float kg, float kb) {
  const float r = static_cast<float>(src.template get_index<0>());
  const float g = static_cast<float>(src.template get_index<1>());
  const float b = static_cast<float>(src.template get_index<2>());

  const float y = kr * r + kg * g + kb * b;
  const float cb = (b - y) / (2.0f * (1.0f - kb));
  const float cr = (r - y) / (2.0f * (1.0f - kr));
  return To(y, cb, cr);
}

/**
 * @brief YCbCr (normalized) to RGB under the given luma weights.
 * @tparam To Target rgbf_t.
 */
template <typename To, typename From>
constexpr To ycbcr_to_rgb(const From& src, float kr, float kg, float kb) {
  const float y = static_cast<float>(src.template get_index<0>());
  const float cb = static_cast<float>(src.template get_index<1>());
  const float cr = static_cast<float>(src.template get_index<2>());

  const float r = y + 2.0f * (1.0f - kr) * cr;
  const float b = y + 2.0f * (1.0f - kb) * cb;
  const float g = (y - kr * r - kb * b) / kg;
  return To(r, g, b);
}

// Per-standard wrappers (ITU-R BT.601 / BT.709 / BT.2020 luma weights).
template <typename To, typename From>
constexpr To rgb_to_ycbcr601(const From& src) {
  return rgb_to_ycbcr<To>(src, 0.299f, 0.587f, 0.114f);
}
template <typename To, typename From>
constexpr To ycbcr601_to_rgb(const From& src) {
  return ycbcr_to_rgb<To>(src, 0.299f, 0.587f, 0.114f);
}
template <typename To, typename From>
constexpr To rgb_to_ycbcr709(const From& src) {
  return rgb_to_ycbcr<To>(src, 0.2126f, 0.7152f, 0.0722f);
}
template <typename To, typename From>
constexpr To ycbcr709_to_rgb(const From& src) {
  return ycbcr_to_rgb<To>(src, 0.2126f, 0.7152f, 0.0722f);
}
template <typename To, typename From>
constexpr To rgb_to_ycbcr2020(const From& src) {
  return rgb_to_ycbcr<To>(src, 0.2627f, 0.6780f, 0.0593f);
}
template <typename To, typename From>
constexpr To ycbcr2020_to_rgb(const From& src) {
  return ycbcr_to_rgb<To>(src, 0.2627f, 0.6780f, 0.0593f);
}

}  // namespace colorcpp::operations::conversion::details
