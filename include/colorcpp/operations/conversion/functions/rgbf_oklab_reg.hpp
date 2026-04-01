/**
 * @file rgbf_oklab_reg.hpp
 * @brief One-step sRGB float ↔ OkLab wrappers so routing does not prefer XYZ over linear sRGB.
 */

#pragma once

#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion/functions/linear_rgb.hpp>
#include <colorcpp/operations/conversion/functions/oklab.hpp>

namespace colorcpp::operations::conversion::details {

constexpr core::oklab_t rgbf_to_oklab_reg(const core::rgbf_t& s) {
  return linear_rgb_to_oklab<core::oklab_t>(srgb_to_linear_rgb<core::linear_rgbf_t>(s));
}
constexpr core::rgbf_t oklab_to_rgbf_reg(const core::oklab_t& lab) {
  return linear_rgb_to_srgb<core::rgbf_t>(oklab_to_linear_rgb<core::linear_rgbf_t>(lab));
}

constexpr core::oklab_t rgbaf_to_oklab_reg(const core::rgbaf_t& s) {
  return linear_rgb_to_oklab<core::oklab_t>(srgb_to_linear_rgb<core::linear_rgbaf_t>(s));
}
constexpr core::rgbaf_t oklab_to_rgbaf_reg(const core::oklab_t& lab) {
  return linear_rgb_to_srgb<core::rgbaf_t>(oklab_to_linear_rgb<core::linear_rgbaf_t>(lab));
}

}  // namespace colorcpp::operations::conversion::details
