/**
 * @file cmyk8_reg.hpp
 * @brief CMYK 8-bit ↔ float sRGB registration (avoids oscillation with hub routing).
 */

#pragma once

#include <colorcpp/core/cmyk.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion/functions/cmyk.hpp>

namespace colorcpp::operations::conversion::details {

constexpr core::rgbf_t cmyk8_to_rgbf_reg(const core::cmyk8_t& s) { return cmyk_to_rgb<core::rgbf_t>(s); }
constexpr core::cmyk8_t rgbf_to_cmyk8_reg(const core::rgbf_t& s) { return rgb_to_cmyk<core::cmyk8_t>(s); }

}  // namespace colorcpp::operations::conversion::details
