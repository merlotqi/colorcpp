/**
 * @file index.hpp
 * @brief Includes all conversion function files and registers conversions.
 */

#pragma once

// Include all conversion function files
#include <colorcpp/operations/conversion/functions/adobe_rgb.hpp>
#include <colorcpp/operations/conversion/functions/cmyk.hpp>
#include <colorcpp/operations/conversion/functions/cmyk8_reg.hpp>
#include <colorcpp/operations/conversion/functions/display_p3.hpp>
#include <colorcpp/operations/conversion/functions/hsl.hpp>
#include <colorcpp/operations/conversion/functions/hsv.hpp>
#include <colorcpp/operations/conversion/functions/hwb.hpp>
#include <colorcpp/operations/conversion/functions/lab.hpp>
#include <colorcpp/operations/conversion/functions/linear_rgb.hpp>
#include <colorcpp/operations/conversion/functions/oklab.hpp>
#include <colorcpp/operations/conversion/functions/prophoto_rgb.hpp>
#include <colorcpp/operations/conversion/functions/rec2020.hpp>
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

// 8-bit sRGB ↔ float sRGB (hub alignment for rgb8 / rgba8; wrappers avoid macro comma issues)
COLORCPP_REGISTER_CONVERSION_BIDIR(colorcpp::core::rgb8_t, colorcpp::core::rgbf_t,
                                   colorcpp::operations::conversion::details::rgb8_to_rgbf_reg,
                                   colorcpp::operations::conversion::details::rgbf_to_rgb8_reg)

COLORCPP_REGISTER_CONVERSION_BIDIR(colorcpp::core::rgba8_t, colorcpp::core::rgbaf_t,
                                   colorcpp::operations::conversion::details::rgba8_to_rgbaf_reg,
                                   colorcpp::operations::conversion::details::rgbaf_to_rgba8_reg)

// sRGB ↔ HSL
COLORCPP_REGISTER_CONVERSION_BIDIR(colorcpp::core::rgbf_t, colorcpp::core::hsl_float_t,
                                   colorcpp::operations::conversion::details::rgb_to_hsl<colorcpp::core::hsl_float_t>,
                                   colorcpp::operations::conversion::details::hsl_to_rgb<colorcpp::core::rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(colorcpp::core::rgbaf_t, colorcpp::core::hsla_float_t,
                                   colorcpp::operations::conversion::details::rgb_to_hsl<colorcpp::core::hsla_float_t>,
                                   colorcpp::operations::conversion::details::hsl_to_rgb<colorcpp::core::rgbaf_t>)

// sRGB ↔ HSV
COLORCPP_REGISTER_CONVERSION_BIDIR(colorcpp::core::rgbf_t, colorcpp::core::hsv_float_t,
                                   colorcpp::operations::conversion::details::rgb_to_hsv<colorcpp::core::hsv_float_t>,
                                   colorcpp::operations::conversion::details::hsv_to_rgb<colorcpp::core::rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(colorcpp::core::rgbaf_t, colorcpp::core::hsva_float_t,
                                   colorcpp::operations::conversion::details::rgb_to_hsv<colorcpp::core::hsva_float_t>,
                                   colorcpp::operations::conversion::details::hsv_to_rgb<colorcpp::core::rgbaf_t>)

// sRGB ↔ HWB
COLORCPP_REGISTER_CONVERSION_BIDIR(colorcpp::core::rgbf_t, colorcpp::core::hwb_float_t,
                                   colorcpp::operations::conversion::details::rgb_to_hwb<colorcpp::core::hwb_float_t>,
                                   colorcpp::operations::conversion::details::hwb_to_rgb<colorcpp::core::rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(colorcpp::core::rgbaf_t, colorcpp::core::hwba_float_t,
                                   colorcpp::operations::conversion::details::rgb_to_hwb<colorcpp::core::hwba_float_t>,
                                   colorcpp::operations::conversion::details::hwb_to_rgb<colorcpp::core::rgbaf_t>)

// sRGB ↔ CMYK
COLORCPP_REGISTER_CONVERSION_BIDIR(colorcpp::core::rgbf_t, colorcpp::core::cmyk_float_t,
                                   colorcpp::operations::conversion::details::rgb_to_cmyk<colorcpp::core::cmyk_float_t>,
                                   colorcpp::operations::conversion::details::cmyk_to_rgb<colorcpp::core::rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(colorcpp::core::rgbf_t, colorcpp::core::cmyk8_t,
                                   colorcpp::operations::conversion::details::rgbf_to_cmyk8_reg,
                                   colorcpp::operations::conversion::details::cmyk8_to_rgbf_reg)

// sRGB ↔ Linear RGB
COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::rgbf_t, colorcpp::core::linear_rgbf_t,
    colorcpp::operations::conversion::details::srgb_to_linear_rgb<colorcpp::core::linear_rgbf_t>,
    colorcpp::operations::conversion::details::linear_rgb_to_srgb<colorcpp::core::rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::rgbaf_t, colorcpp::core::linear_rgbaf_t,
    colorcpp::operations::conversion::details::srgb_to_linear_rgb<colorcpp::core::linear_rgbaf_t>,
    colorcpp::operations::conversion::details::linear_rgb_to_srgb<colorcpp::core::rgbaf_t>)

// Linear RGB ↔ XYZ
COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::linear_rgbf_t, colorcpp::core::xyz_t,
    colorcpp::operations::conversion::details::linear_rgb_to_xyz<colorcpp::core::xyz_t>,
    colorcpp::operations::conversion::details::xyz_to_linear_rgb<colorcpp::core::linear_rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::linear_rgbaf_t, colorcpp::core::xyz_t,
    colorcpp::operations::conversion::details::linear_rgb_to_xyz<colorcpp::core::xyz_t>,
    colorcpp::operations::conversion::details::xyz_to_linear_rgb<colorcpp::core::linear_rgbaf_t>)

// Linear RGB ↔ OkLab
COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::linear_rgbf_t, colorcpp::core::oklab_t,
    colorcpp::operations::conversion::details::linear_rgb_to_oklab<colorcpp::core::oklab_t>,
    colorcpp::operations::conversion::details::oklab_to_linear_rgb<colorcpp::core::linear_rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::linear_rgbaf_t, colorcpp::core::oklab_t,
    colorcpp::operations::conversion::details::linear_rgb_to_oklab<colorcpp::core::oklab_t>,
    colorcpp::operations::conversion::details::oklab_to_linear_rgb<colorcpp::core::linear_rgbaf_t>)

// sRGB float ↔ OkLab (canonical path; avoids oklab↔rgbf routing via XYZ)
COLORCPP_REGISTER_CONVERSION_BIDIR(colorcpp::core::rgbf_t, colorcpp::core::oklab_t,
                                   colorcpp::operations::conversion::details::rgbf_to_oklab_reg,
                                   colorcpp::operations::conversion::details::oklab_to_rgbf_reg)

COLORCPP_REGISTER_CONVERSION_BIDIR(colorcpp::core::rgbaf_t, colorcpp::core::oklab_t,
                                   colorcpp::operations::conversion::details::rgbaf_to_oklab_reg,
                                   colorcpp::operations::conversion::details::oklab_to_rgbaf_reg)

// Linear RGB ↔ CIELAB
COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::linear_rgbf_t, colorcpp::core::cielab_t,
    colorcpp::operations::conversion::details::linear_rgb_to_lab<colorcpp::core::cielab_t>,
    colorcpp::operations::conversion::details::lab_to_linear_rgb<colorcpp::core::linear_rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::linear_rgbaf_t, colorcpp::core::cielab_t,
    colorcpp::operations::conversion::details::linear_rgb_to_lab<colorcpp::core::cielab_t>,
    colorcpp::operations::conversion::details::lab_to_linear_rgb<colorcpp::core::linear_rgbaf_t>)

// OkLab ↔ OkLCH
COLORCPP_REGISTER_CONVERSION_BIDIR(colorcpp::core::oklab_t, colorcpp::core::oklch_t,
                                   colorcpp::operations::conversion::details::oklab_to_oklch<colorcpp::core::oklch_t>,
                                   colorcpp::operations::conversion::details::oklch_to_oklab<colorcpp::core::oklab_t>)

// CIELAB ↔ CIELCH
COLORCPP_REGISTER_CONVERSION_BIDIR(colorcpp::core::cielab_t, colorcpp::core::cielch_t,
                                   colorcpp::operations::conversion::details::lab_to_lch<colorcpp::core::cielch_t>,
                                   colorcpp::operations::conversion::details::lch_to_lab<colorcpp::core::cielab_t>)

// XYZ ↔ CIELAB
COLORCPP_REGISTER_CONVERSION_BIDIR(colorcpp::core::xyz_t, colorcpp::core::cielab_t,
                                   colorcpp::operations::conversion::details::xyz_to_lab<colorcpp::core::cielab_t>,
                                   colorcpp::operations::conversion::details::lab_to_xyz<colorcpp::core::xyz_t>)

// XYZ ↔ OkLab
COLORCPP_REGISTER_CONVERSION_BIDIR(colorcpp::core::xyz_t, colorcpp::core::oklab_t,
                                   colorcpp::operations::conversion::details::xyz_to_oklab<colorcpp::core::oklab_t>,
                                   colorcpp::operations::conversion::details::oklab_to_xyz<colorcpp::core::xyz_t>)

// Display P3 ↔ Linear Display P3
COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::display_p3f_t, colorcpp::core::linear_display_p3f_t,
    colorcpp::operations::conversion::details::display_p3_to_linear_display_p3<colorcpp::core::linear_display_p3f_t>,
    colorcpp::operations::conversion::details::linear_display_p3_to_display_p3<colorcpp::core::display_p3f_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::display_p3af_t, colorcpp::core::linear_display_p3af_t,
    colorcpp::operations::conversion::details::display_p3_to_linear_display_p3<colorcpp::core::linear_display_p3af_t>,
    colorcpp::operations::conversion::details::linear_display_p3_to_display_p3<colorcpp::core::display_p3af_t>)

// Linear Display P3 ↔ XYZ
COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::linear_display_p3f_t, colorcpp::core::xyz_t,
    colorcpp::operations::conversion::details::linear_display_p3_to_xyz<colorcpp::core::xyz_t>,
    colorcpp::operations::conversion::details::xyz_to_linear_display_p3<colorcpp::core::linear_display_p3f_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::linear_display_p3af_t, colorcpp::core::xyz_t,
    colorcpp::operations::conversion::details::linear_display_p3_to_xyz<colorcpp::core::xyz_t>,
    colorcpp::operations::conversion::details::xyz_to_linear_display_p3<colorcpp::core::linear_display_p3af_t>)

// RGBAF ↔ RGBF (alpha strip/fill)
COLORCPP_REGISTER_CONVERSION(colorcpp::core::rgbaf_t, colorcpp::core::rgbf_t,
                             colorcpp::operations::conversion::details::rgbaf_to_rgbf)

COLORCPP_REGISTER_CONVERSION(colorcpp::core::rgbf_t, colorcpp::core::rgbaf_t,
                             colorcpp::operations::conversion::details::rgbf_to_rgbaf)

// Adobe RGB ↔ Linear Adobe RGB
COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::adobe_rgbf_t, colorcpp::core::linear_adobe_rgbf_t,
    colorcpp::operations::conversion::details::adobe_rgb_to_linear_adobe_rgb<colorcpp::core::linear_adobe_rgbf_t>,
    colorcpp::operations::conversion::details::linear_adobe_rgb_to_adobe_rgb<colorcpp::core::adobe_rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::adobe_rgbaf_t, colorcpp::core::linear_adobe_rgbaf_t,
    colorcpp::operations::conversion::details::adobe_rgb_to_linear_adobe_rgb<colorcpp::core::linear_adobe_rgbaf_t>,
    colorcpp::operations::conversion::details::linear_adobe_rgb_to_adobe_rgb<colorcpp::core::adobe_rgbaf_t>)

// Linear Adobe RGB ↔ XYZ
COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::linear_adobe_rgbf_t, colorcpp::core::xyz_t,
    colorcpp::operations::conversion::details::linear_adobe_rgb_to_xyz<colorcpp::core::xyz_t>,
    colorcpp::operations::conversion::details::xyz_to_linear_adobe_rgb<colorcpp::core::linear_adobe_rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::linear_adobe_rgbaf_t, colorcpp::core::xyz_t,
    colorcpp::operations::conversion::details::linear_adobe_rgb_to_xyz<colorcpp::core::xyz_t>,
    colorcpp::operations::conversion::details::xyz_to_linear_adobe_rgb<colorcpp::core::linear_adobe_rgbaf_t>)

// ProPhoto RGB ↔ Linear ProPhoto RGB
COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::prophoto_rgbf_t, colorcpp::core::linear_prophoto_rgbf_t,
    colorcpp::operations::conversion::details::prophoto_rgb_to_linear_prophoto_rgb<
        colorcpp::core::linear_prophoto_rgbf_t>,
    colorcpp::operations::conversion::details::linear_prophoto_rgb_to_prophoto_rgb<colorcpp::core::prophoto_rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::prophoto_rgbaf_t, colorcpp::core::linear_prophoto_rgbaf_t,
    colorcpp::operations::conversion::details::prophoto_rgb_to_linear_prophoto_rgb<
        colorcpp::core::linear_prophoto_rgbaf_t>,
    colorcpp::operations::conversion::details::linear_prophoto_rgb_to_prophoto_rgb<colorcpp::core::prophoto_rgbaf_t>)

// Linear ProPhoto RGB ↔ XYZ
COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::linear_prophoto_rgbf_t, colorcpp::core::xyz_t,
    colorcpp::operations::conversion::details::linear_prophoto_rgb_to_xyz<colorcpp::core::xyz_t>,
    colorcpp::operations::conversion::details::xyz_to_linear_prophoto_rgb<colorcpp::core::linear_prophoto_rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::linear_prophoto_rgbaf_t, colorcpp::core::xyz_t,
    colorcpp::operations::conversion::details::linear_prophoto_rgb_to_xyz<colorcpp::core::xyz_t>,
    colorcpp::operations::conversion::details::xyz_to_linear_prophoto_rgb<colorcpp::core::linear_prophoto_rgbaf_t>)

// Rec.2020 ↔ Linear Rec.2020
COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::rec2020_rgbf_t, colorcpp::core::linear_rec2020_rgbf_t,
    colorcpp::operations::conversion::details::rec2020_to_linear_rec2020<colorcpp::core::linear_rec2020_rgbf_t>,
    colorcpp::operations::conversion::details::linear_rec2020_to_rec2020<colorcpp::core::rec2020_rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::rec2020_rgbaf_t, colorcpp::core::linear_rec2020_rgbaf_t,
    colorcpp::operations::conversion::details::rec2020_to_linear_rec2020<colorcpp::core::linear_rec2020_rgbaf_t>,
    colorcpp::operations::conversion::details::linear_rec2020_to_rec2020<colorcpp::core::rec2020_rgbaf_t>)

// Linear Rec.2020 ↔ XYZ
COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::linear_rec2020_rgbf_t, colorcpp::core::xyz_t,
    colorcpp::operations::conversion::details::linear_rec2020_to_xyz<colorcpp::core::xyz_t>,
    colorcpp::operations::conversion::details::xyz_to_linear_rec2020<colorcpp::core::linear_rec2020_rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(
    colorcpp::core::linear_rec2020_rgbaf_t, colorcpp::core::xyz_t,
    colorcpp::operations::conversion::details::linear_rec2020_to_xyz<colorcpp::core::xyz_t>,
    colorcpp::operations::conversion::details::xyz_to_linear_rec2020<colorcpp::core::linear_rec2020_rgbaf_t>)
