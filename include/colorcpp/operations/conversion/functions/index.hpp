/**
 * @file index.hpp
 * @brief Includes all conversion function files and registers conversions.
 */

#pragma once

// Include all conversion function files
#include <colorcpp/operations/conversion/functions/cmyk.hpp>
#include <colorcpp/operations/conversion/functions/cmyk8_reg.hpp>
#include <colorcpp/operations/conversion/functions/display_p3.hpp>
#include <colorcpp/operations/conversion/functions/hsl.hpp>
#include <colorcpp/operations/conversion/functions/hsv.hpp>
#include <colorcpp/operations/conversion/functions/hwb.hpp>
#include <colorcpp/operations/conversion/functions/lab.hpp>
#include <colorcpp/operations/conversion/functions/linear_rgb.hpp>
#include <colorcpp/operations/conversion/functions/oklab.hpp>
#include <colorcpp/operations/conversion/functions/rgbf_oklab_reg.hpp>
#include <colorcpp/operations/conversion/functions/srgb8_cast.hpp>
#include <colorcpp/operations/conversion/functions/xyz.hpp>

// Register conversions
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/core/cmyk.hpp>
#include <colorcpp/core/display_p3.hpp>
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/hsv.hpp>
#include <colorcpp/core/hwb.hpp>
#include <colorcpp/core/linear_rgb.hpp>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion/registry.hpp>

namespace colorcpp::operations::conversion {

// 8-bit sRGB ↔ float sRGB (hub alignment for rgb8 / rgba8; wrappers avoid macro comma issues)
COLORCPP_REGISTER_CONVERSION_BIDIR(core::rgb8_t, core::rgbf_t, details::rgb8_to_rgbf_reg, details::rgbf_to_rgb8_reg)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::rgba8_t, core::rgbaf_t, details::rgba8_to_rgbaf_reg,
                                   details::rgbaf_to_rgba8_reg)

// sRGB ↔ HSL
COLORCPP_REGISTER_CONVERSION_BIDIR(core::rgbf_t, core::hsl_float_t, details::rgb_to_hsl<core::hsl_float_t>,
                                   details::hsl_to_rgb<core::rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::rgbaf_t, core::hsla_float_t, details::rgb_to_hsl<core::hsla_float_t>,
                                   details::hsl_to_rgb<core::rgbaf_t>)

// sRGB ↔ HSV
COLORCPP_REGISTER_CONVERSION_BIDIR(core::rgbf_t, core::hsv_float_t, details::rgb_to_hsv<core::hsv_float_t>,
                                   details::hsv_to_rgb<core::rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::rgbaf_t, core::hsva_float_t, details::rgb_to_hsv<core::hsva_float_t>,
                                   details::hsv_to_rgb<core::rgbaf_t>)

// sRGB ↔ HWB
COLORCPP_REGISTER_CONVERSION_BIDIR(core::rgbf_t, core::hwb_float_t, details::rgb_to_hwb<core::hwb_float_t>,
                                   details::hwb_to_rgb<core::rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::rgbaf_t, core::hwba_float_t, details::rgb_to_hwb<core::hwba_float_t>,
                                   details::hwb_to_rgb<core::rgbaf_t>)

// sRGB ↔ CMYK
COLORCPP_REGISTER_CONVERSION_BIDIR(core::rgbf_t, core::cmyk_float_t, details::rgb_to_cmyk<core::cmyk_float_t>,
                                   details::cmyk_to_rgb<core::rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::rgbf_t, core::cmyk8_t, details::rgbf_to_cmyk8_reg, details::cmyk8_to_rgbf_reg)

// sRGB ↔ Linear RGB
COLORCPP_REGISTER_CONVERSION_BIDIR(core::rgbf_t, core::linear_rgbf_t, details::srgb_to_linear_rgb<core::linear_rgbf_t>,
                                   details::linear_rgb_to_srgb<core::rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::rgbaf_t, core::linear_rgbaf_t,
                                   details::srgb_to_linear_rgb<core::linear_rgbaf_t>,
                                   details::linear_rgb_to_srgb<core::rgbaf_t>)

// Linear RGB ↔ XYZ
COLORCPP_REGISTER_CONVERSION_BIDIR(core::linear_rgbf_t, core::xyz_t, details::linear_rgb_to_xyz<core::xyz_t>,
                                   details::xyz_to_linear_rgb<core::linear_rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::linear_rgbaf_t, core::xyz_t, details::linear_rgb_to_xyz<core::xyz_t>,
                                   details::xyz_to_linear_rgb<core::linear_rgbaf_t>)

// Linear RGB ↔ OkLab
COLORCPP_REGISTER_CONVERSION_BIDIR(core::linear_rgbf_t, core::oklab_t, details::linear_rgb_to_oklab<core::oklab_t>,
                                   details::oklab_to_linear_rgb<core::linear_rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::linear_rgbaf_t, core::oklab_t, details::linear_rgb_to_oklab<core::oklab_t>,
                                   details::oklab_to_linear_rgb<core::linear_rgbaf_t>)

// sRGB float ↔ OkLab (canonical path; avoids oklab↔rgbf routing via XYZ)
COLORCPP_REGISTER_CONVERSION_BIDIR(core::rgbf_t, core::oklab_t, details::rgbf_to_oklab_reg, details::oklab_to_rgbf_reg)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::rgbaf_t, core::oklab_t, details::rgbaf_to_oklab_reg,
                                   details::oklab_to_rgbaf_reg)

// Linear RGB ↔ CIELAB
COLORCPP_REGISTER_CONVERSION_BIDIR(core::linear_rgbf_t, core::cielab_t, details::linear_rgb_to_lab<core::cielab_t>,
                                   details::lab_to_linear_rgb<core::linear_rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::linear_rgbaf_t, core::cielab_t, details::linear_rgb_to_lab<core::cielab_t>,
                                   details::lab_to_linear_rgb<core::linear_rgbaf_t>)

// OkLab ↔ OkLCH
COLORCPP_REGISTER_CONVERSION_BIDIR(core::oklab_t, core::oklch_t, details::oklab_to_oklch<core::oklch_t>,
                                   details::oklch_to_oklab<core::oklab_t>)

// CIELAB ↔ CIELCH
COLORCPP_REGISTER_CONVERSION_BIDIR(core::cielab_t, core::cielch_t, details::lab_to_lch<core::cielch_t>,
                                   details::lch_to_lab<core::cielab_t>)

// XYZ ↔ CIELAB
COLORCPP_REGISTER_CONVERSION_BIDIR(core::xyz_t, core::cielab_t, details::xyz_to_lab<core::cielab_t>,
                                   details::lab_to_xyz<core::xyz_t>)

// XYZ ↔ OkLab
COLORCPP_REGISTER_CONVERSION_BIDIR(core::xyz_t, core::oklab_t, details::xyz_to_oklab<core::oklab_t>,
                                   details::oklab_to_xyz<core::xyz_t>)

// Display P3 ↔ Linear Display P3
COLORCPP_REGISTER_CONVERSION_BIDIR(core::display_p3f_t, core::linear_display_p3f_t,
                                   details::display_p3_to_linear_display_p3<core::linear_display_p3f_t>,
                                   details::linear_display_p3_to_display_p3<core::display_p3f_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::display_p3af_t, core::linear_display_p3af_t,
                                   details::display_p3_to_linear_display_p3<core::linear_display_p3af_t>,
                                   details::linear_display_p3_to_display_p3<core::display_p3af_t>)

// Linear Display P3 ↔ XYZ
COLORCPP_REGISTER_CONVERSION_BIDIR(core::linear_display_p3f_t, core::xyz_t,
                                   details::linear_display_p3_to_xyz<core::xyz_t>,
                                   details::xyz_to_linear_display_p3<core::linear_display_p3f_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::linear_display_p3af_t, core::xyz_t,
                                   details::linear_display_p3_to_xyz<core::xyz_t>,
                                   details::xyz_to_linear_display_p3<core::linear_display_p3af_t>)

// RGBAF ↔ RGBF (alpha strip/fill)
COLORCPP_REGISTER_CONVERSION(core::rgbaf_t, core::rgbf_t, details::rgbaf_to_rgbf)

COLORCPP_REGISTER_CONVERSION(core::rgbf_t, core::rgbaf_t, details::rgbf_to_rgbaf)

// Display P3 ↔ sRGB (direct short link: 1 hop instead of 4 via Linear P3 → XYZ → Linear RGB)
COLORCPP_REGISTER_CONVERSION_BIDIR(core::display_p3f_t, core::rgbf_t, details::display_p3_to_srgb<core::rgbf_t>,
                                   details::srgb_to_display_p3<core::display_p3f_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::display_p3af_t, core::rgbaf_t, details::display_p3_to_srgb<core::rgbaf_t>,
                                   details::srgb_to_display_p3<core::display_p3af_t>)

// sRGB ↔ CIELAB (direct short link: 1 hop instead of 2 via Linear RGB)
COLORCPP_REGISTER_CONVERSION_BIDIR(core::rgbf_t, core::cielab_t, details::srgb_to_lab<core::cielab_t>,
                                   details::lab_to_srgb<core::rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::rgbaf_t, core::cielab_t, details::srgb_to_lab<core::cielab_t>,
                                   details::lab_to_srgb<core::rgbaf_t>)

// CIELAB ↔ OkLab (direct short link: 1 hop instead of 2 via XYZ)
COLORCPP_REGISTER_CONVERSION_BIDIR(core::cielab_t, core::oklab_t, details::cielab_to_oklab<core::oklab_t>,
                                   details::oklab_to_cielab<core::cielab_t>)

// HSL ↔ Linear RGB (direct short link: 1 hop instead of 2 via sRGB)
COLORCPP_REGISTER_CONVERSION_BIDIR(core::hsl_float_t, core::linear_rgbf_t,
                                   details::hsl_to_linear_rgb<core::linear_rgbf_t>,
                                   details::linear_rgb_to_hsl<core::hsl_float_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::hsla_float_t, core::linear_rgbaf_t,
                                   details::hsl_to_linear_rgb<core::linear_rgbaf_t>,
                                   details::linear_rgb_to_hsl<core::hsla_float_t>)

// HSV ↔ Linear RGB (direct short link: 1 hop instead of 2 via sRGB)
COLORCPP_REGISTER_CONVERSION_BIDIR(core::hsv_float_t, core::linear_rgbf_t,
                                   details::hsv_to_linear_rgb<core::linear_rgbf_t>,
                                   details::linear_rgb_to_hsv<core::hsv_float_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::hsva_float_t, core::linear_rgbaf_t,
                                   details::hsv_to_linear_rgb<core::linear_rgbaf_t>,
                                   details::linear_rgb_to_hsv<core::hsva_float_t>)

// HWB ↔ Linear RGB (direct short link: 1 hop instead of 2 via sRGB)
COLORCPP_REGISTER_CONVERSION_BIDIR(core::hwb_float_t, core::linear_rgbf_t,
                                   details::hwb_to_linear_rgb<core::linear_rgbf_t>,
                                   details::linear_rgb_to_hwb<core::hwb_float_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::hwba_float_t, core::linear_rgbaf_t,
                                   details::hwb_to_linear_rgb<core::linear_rgbaf_t>,
                                   details::linear_rgb_to_hwb<core::hwba_float_t>)

}  // namespace colorcpp::operations::conversion
