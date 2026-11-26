/**
 * @file rec709.hpp
 * @brief Rec.709 <-> linear sRGB conversion (pure gamma 2.4, BT.1886).
 *
 * Rec.709 shares the sRGB primaries and D65 white, so the linear side is
 * @ref linear_rgbf_t itself — only the transfer function differs. The
 * gamma 2.4 curve is the same one Rec.2020 uses (rec2020_tf), so this is
 * a pure TF swap with no matrix work.
 */

#pragma once

#include <colorcpp/core/rec709.hpp>
#include <colorcpp/operations/conversion/detail.hpp>
#include <colorcpp/operations/conversion/functions/rec2020.hpp>

namespace colorcpp::operations::conversion::details {

/** @brief Rec.709 signal (gamma 2.4) -> linear sRGB. */
template <typename To, typename From>
constexpr To rec709_to_linear_rgb(const From& src) {
  float r = rec2020_tf::to_linear(to_unit<From, 0>(src.template get_index<0>()));
  float g = rec2020_tf::to_linear(to_unit<From, 1>(src.template get_index<1>()));
  float b = rec2020_tf::to_linear(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

/** @brief Linear sRGB -> Rec.709 signal (gamma 2.4, BT.1886). */
template <typename To, typename From>
constexpr To linear_rgb_to_rec709(const From& src) {
  float r = rec2020_tf::from_linear(to_unit<From, 0>(src.template get_index<0>()));
  float g = rec2020_tf::from_linear(to_unit<From, 1>(src.template get_index<1>()));
  float b = rec2020_tf::from_linear(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

}  // namespace colorcpp::operations::conversion::details
