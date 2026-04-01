/**
 * @file srgb8_cast.hpp
 * @brief Wrappers for 8-bit ↔ float sRGB registration (macro-safe, no commas in function names).
 */

#pragma once

#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion/helpers.hpp>

namespace colorcpp::operations::conversion::details {

constexpr core::rgbf_t rgb8_to_rgbf_reg(const core::rgb8_t& s) {
  return integral_srgb_to_float_srgb<core::rgbf_t, core::rgb8_t>(s);
}
constexpr core::rgb8_t rgbf_to_rgb8_reg(const core::rgbf_t& s) {
  return float_srgb_to_integral_srgb<core::rgb8_t, core::rgbf_t>(s);
}

constexpr core::rgbaf_t rgba8_to_rgbaf_reg(const core::rgba8_t& s) {
  return integral_srgba_to_float_srgba<core::rgbaf_t, core::rgba8_t>(s);
}
constexpr core::rgba8_t rgbaf_to_rgba8_reg(const core::rgbaf_t& s) {
  return float_srgba_to_integral_srgba<core::rgba8_t, core::rgbaf_t>(s);
}

}  // namespace colorcpp::operations::conversion::details
