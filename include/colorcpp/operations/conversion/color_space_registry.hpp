/**
 * @file color_space_registry.hpp
 * @brief Color space traits registration supporting graph-driven public routing.
 *
 * Public conversion dispatch is graph-driven via @c color_cast and @c graph.hpp.
 * This file defines compatibility metadata for built-in color spaces so registered
 * edges and legacy hub relationships remain available to the routing layer.
 *
 * Compatibility Hub Metadata:
 * ==========================
 *
 *                        XYZ (root hub)
 *                       / | \ \
 *                      /  |  \  \-----------\
 *              Linear RGB  OkLab  CIELAB     \
 *                 |        |        |         \
 *                sRGB     OkLCH   CIELCH       \
 *               / | \                           \
 *         HSL HSV HWB CMYK                       \
 *                                                 \
 *   Display P3  → Linear Display P3  → XYZ         \
 *   Adobe RGB   → Linear Adobe RGB   → XYZ          \
 *   ProPhoto    → Linear ProPhoto    → XYZ (Bradford D50↔D65)
 *
 * These relationships provide metadata and compatibility support for the
 * registered conversion graph rather than defining the public dispatch contract.
 */

#pragma once

#include <colorcpp/core/adobe_rgb.hpp>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/core/cmyk.hpp>
#include <colorcpp/core/display_p3.hpp>
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/hsv.hpp>
#include <colorcpp/core/hwb.hpp>
#include <colorcpp/core/linear_rgb.hpp>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/prophoto_rgb.hpp>
#include <colorcpp/core/rec2020.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion/traits.hpp>

namespace colorcpp::operations::conversion {

// sRGB 8-bit hubs match float sRGB (same path as HSL/HSV/HWB); use registered rgb8↔rgbf edges.
template <>
struct color_traits<core::rgb::model::rgb8> {
  using hub_type = core::rgbf_t;
};

template <>
struct color_traits<core::rgb::model::rgba8> {
  using hub_type = core::rgbaf_t;
};

// Float sRGB uses linear sRGB as the next routing step
template <>
struct color_traits<core::rgb::model::rgb_float> {
  using hub_type = core::linear_rgbf_t;
};

template <>
struct color_traits<core::rgb::model::rgba_float> {
  using hub_type = core::linear_rgbaf_t;
};

// Linear RGB uses XYZ as hub
template <>
struct color_traits<core::linear_rgb::model::linear_rgbf> {
  using hub_type = core::xyz_t;
};

template <>
struct color_traits<core::linear_rgb::model::linear_rgbaf> {
  using hub_type = core::xyz_t;
};

// HSL uses sRGB as hub
template <>
struct color_traits<core::hsl::model::hsl> {
  using hub_type = core::rgbf_t;
};

template <>
struct color_traits<core::hsl::model::hsla> {
  using hub_type = core::rgbaf_t;
};

// HSV uses sRGB as hub
template <>
struct color_traits<core::hsv::model::hsv> {
  using hub_type = core::rgbf_t;
};

template <>
struct color_traits<core::hsv::model::hsva> {
  using hub_type = core::rgbaf_t;
};

// HWB uses sRGB as hub
template <>
struct color_traits<core::hwb::model::hwb> {
  using hub_type = core::rgbf_t;
};

template <>
struct color_traits<core::hwb::model::hwba> {
  using hub_type = core::rgbaf_t;
};

// CMYK uses sRGB as hub
template <>
struct color_traits<core::cmyk::model::cmyk_u8> {
  using hub_type = core::rgbf_t;
};

template <>
struct color_traits<core::cmyk::model::cmyk_float> {
  using hub_type = core::rgbf_t;
};

// OkLab uses XYZ as hub
template <>
struct color_traits<core::oklab::model::oklab> {
  using hub_type = core::xyz_t;
};

// OkLCH routes through OkLab (registered oklch ↔ oklab)
template <>
struct color_traits<core::oklab::model::oklch> {
  using hub_type = core::oklab_t;
};

// CIELAB uses XYZ as hub
template <>
struct color_traits<core::lab::model::cielab> {
  using hub_type = core::xyz_t;
};

// CIELCH routes through CIELAB
template <>
struct color_traits<core::lab::model::cielch> {
  using hub_type = core::cielab_t;
};

// XYZ is a hub itself (no parent)
template <>
struct color_traits<core::xyz::model::xyze> {
  using hub_type = void;
};

// Display P3 uses Linear Display P3 as hub
template <>
struct color_traits<core::display_p3::model::display_p3> {
  using hub_type = core::linear_display_p3f_t;
};

template <>
struct color_traits<core::display_p3::model::display_p3a> {
  using hub_type = core::linear_display_p3af_t;
};

// Linear Display P3 uses XYZ as hub
template <>
struct color_traits<core::display_p3::model::linear_display_p3f> {
  using hub_type = core::xyz_t;
};

template <>
struct color_traits<core::display_p3::model::linear_display_p3af> {
  using hub_type = core::xyz_t;
};

// Adobe RGB uses Linear Adobe RGB as hub
template <>
struct color_traits<core::adobe_rgb::model::adobe_rgb> {
  using hub_type = core::linear_adobe_rgbf_t;
};

template <>
struct color_traits<core::adobe_rgb::model::adobe_rgba> {
  using hub_type = core::linear_adobe_rgbaf_t;
};

// Linear Adobe RGB uses XYZ as hub
template <>
struct color_traits<core::adobe_rgb::model::linear_adobe_rgbf> {
  using hub_type = core::xyz_t;
};

template <>
struct color_traits<core::adobe_rgb::model::linear_adobe_rgbaf> {
  using hub_type = core::xyz_t;
};

// ProPhoto RGB uses Linear ProPhoto RGB as hub
template <>
struct color_traits<core::prophoto_rgb::model::prophoto_rgb> {
  using hub_type = core::linear_prophoto_rgbf_t;
};

template <>
struct color_traits<core::prophoto_rgb::model::prophoto_rgba> {
  using hub_type = core::linear_prophoto_rgbaf_t;
};

// Linear ProPhoto RGB uses XYZ as hub
template <>
struct color_traits<core::prophoto_rgb::model::linear_prophoto_rgbf> {
  using hub_type = core::xyz_t;
};

template <>
struct color_traits<core::prophoto_rgb::model::linear_prophoto_rgbaf> {
  using hub_type = core::xyz_t;
};

// Rec.2020 uses Linear Rec.2020 as hub
template <>
struct color_traits<core::rec2020::model::rec2020> {
  using hub_type = core::linear_rec2020_rgbf_t;
};

template <>
struct color_traits<core::rec2020::model::rec2020a> {
  using hub_type = core::linear_rec2020_rgbaf_t;
};

// Linear Rec.2020 uses XYZ as hub
template <>
struct color_traits<core::rec2020::model::linear_rec2020f> {
  using hub_type = core::xyz_t;
};

template <>
struct color_traits<core::rec2020::model::linear_rec2020af> {
  using hub_type = core::xyz_t;
};

}  // namespace colorcpp::operations::conversion
