/**
 * @file multi_colorspace.hpp
 * @brief Multi-colorspace gamut support (sRGB, Display P3, Adobe RGB).
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/algorithms/gamut/details.hpp>
#include <colorcpp/core/adobe_rgb.hpp>
#include <colorcpp/core/cmyk.hpp>
#include <colorcpp/core/display_p3.hpp>
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/hsv.hpp>
#include <colorcpp/core/hwb.hpp>
#include <colorcpp/core/prophoto_rgb.hpp>
#include <colorcpp/core/rec2020.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::gamut {

/**
 * @brief Gamut types supported for checking and clipping.
 */
enum class gamut_type {
  srgb,          ///< Standard RGB (sRGB)
  display_p3,    ///< Display P3 (wide gamut)
  adobe_rgb,     ///< Adobe RGB (wide gamut)
  prophoto_rgb,  ///< ProPhoto RGB (widest gamut)
  rec2020,       ///< Rec. 2020 / BT.2020 (wide gamut)
};

namespace details {

// Default tolerance for wide-gamut membership. The shared 7-digit XYZ matrix
// coefficients leave up to ~1.3e-4 of rounding noise on near-boundary colors
// (D65 white lands at 1.00012 in linear P3 / ProPhoto), so the sRGB check's
// 1e-4 would exclude white itself from every wide gamut.
constexpr float kWideGamutEps = 2e-4f;

// Check if linear RGB is in Display P3 gamut
inline bool linrgb_in_displayp3_gamut(float r, float g, float b, float eps = kWideGamutEps) noexcept {
  // Display P3 shares sRGB's transfer function but not its primaries:
  // convert linear sRGB through XYZ to linear P3 and range-test unclamped.
  float dp3_r, dp3_g, dp3_b;
  linrgb_to_linear_displayp3(r, g, b, dp3_r, dp3_g, dp3_b);
  return linrgb_in_gamut(dp3_r, dp3_g, dp3_b, eps);
}

// Check if linear RGB is in Adobe RGB gamut
inline bool linrgb_in_adobergb_gamut(float r, float g, float b, float eps = kWideGamutEps) noexcept {
  float argb_r, argb_g, argb_b;
  linrgb_to_linear_adobergb(r, g, b, argb_r, argb_g, argb_b);
  return linrgb_in_gamut(argb_r, argb_g, argb_b, eps);
}

// Check if linear RGB is in ProPhoto RGB gamut
inline bool linrgb_in_prophoto_gamut(float r, float g, float b, float eps = kWideGamutEps) noexcept {
  float pprgb_r, pprgb_g, pprgb_b;
  linrgb_to_linear_prophoto(r, g, b, pprgb_r, pprgb_g, pprgb_b);
  return linrgb_in_gamut(pprgb_r, pprgb_g, pprgb_b, eps);
}

// Check if linear RGB is in Rec.2020 gamut
inline bool linrgb_in_rec2020_gamut(float r, float g, float b, float eps = kWideGamutEps) noexcept {
  float r20_r, r20_g, r20_b;
  linrgb_to_linear_rec2020(r, g, b, r20_r, r20_g, r20_b);
  return linrgb_in_gamut(r20_r, r20_g, r20_b, eps);
}

// Generic gamut check based on gamut type
inline bool linrgb_in_gamut(float r, float g, float b, gamut_type type, float eps = -1.0f) noexcept {
  if (eps < 0.0f) {
    // Not specified: per-type default (sRGB keeps its tighter 1e-4).
    eps = (type == gamut_type::srgb) ? 1e-4f : kWideGamutEps;
  }
  switch (type) {
    case gamut_type::srgb:
      return details::linrgb_in_gamut(r, g, b, eps);
    case gamut_type::display_p3:
      return linrgb_in_displayp3_gamut(r, g, b, eps);
    case gamut_type::adobe_rgb:
      return linrgb_in_adobergb_gamut(r, g, b, eps);
    case gamut_type::prophoto_rgb:
      return linrgb_in_prophoto_gamut(r, g, b, eps);
    case gamut_type::rec2020:
      return linrgb_in_rec2020_gamut(r, g, b, eps);
    default:
      return details::linrgb_in_gamut(r, g, b, eps);
  }
}

}  // namespace details

/**
 * @brief Check if a color is in the specified gamut.
 * @tparam Color Input color type.
 * @param c The color to check.
 * @param type The gamut type to check against.
 * @return True if the color is in the specified gamut.
 */
template <typename Color>
bool is_in_gamut(const Color& c, gamut_type type = gamut_type::srgb) {
  using namespace operations::conversion;

  // Through XYZ (nominal [-1, 2], wide enough for every supported gamut —
  // issue #56) and raw float math, never through oklab_t: membership must not
  // depend on any color storage range clipping the value first (issue #67).
  // The xyz↔oklab edge is raw, so this costs nothing in accuracy; XYZ's pack
  // clamp only touches values already outside every RGB gamut (Y > 1 alone
  // rules all of them out).
  const auto xyz = color_cast<core::xyz_t>(c);
  float r, g, b;
  details::xyz_to_linrgb(xyz.x(), xyz.y(), xyz.z(), r, g, b);
  return details::linrgb_in_gamut(r, g, b, type);
}

/**
 * @brief Check if a color is in Display P3 gamut.
 */
template <typename Color>
bool is_in_displayp3_gamut(const Color& c) {
  return is_in_gamut(c, gamut_type::display_p3);
}

/**
 * @brief Check if a color is in Adobe RGB gamut.
 */
template <typename Color>
bool is_in_adobergb_gamut(const Color& c) {
  return is_in_gamut(c, gamut_type::adobe_rgb);
}

/**
 * @brief Check if a color is in ProPhoto RGB gamut.
 */
template <typename Color>
bool is_in_prophoto_gamut(const Color& c) {
  return is_in_gamut(c, gamut_type::prophoto_rgb);
}

/**
 * @brief Check if a color is in Rec.2020 gamut.
 */
template <typename Color>
bool is_in_rec2020_gamut(const Color& c) {
  return is_in_gamut(c, gamut_type::rec2020);
}

/**
 * @brief Destination gamut of a color model (issue #79).
 *
 * Display-encoded models clamp to their own RGB gamut when a conversion
 * lands on them, so an out-of-gamut working-space midpoint has to be
 * gamut-mapped — not per-channel hard-clipped — before the cast. Unbounded
 * working spaces (OkLab/LCh, CIELAB/LCh, Luv, Jzazbz, xyy) and
 * extended-range intermediates (linear RGB families, XYZ — issue #76) have
 * no target gamut and pass values through untouched.
 */
template <gamut_type G>
struct bounded_gamut {
  static constexpr bool bounded = true;
  static constexpr gamut_type gamut = G;
};

/// Primary template: no bounded target gamut.
template <typename Model>
struct model_target_gamut {
  static constexpr bool bounded = false;
  static constexpr gamut_type gamut = gamut_type::srgb;
};

// sRGB cube — rgb8/rgba8/rgbf/rgbaf and the parametric aliases that all
// land inside it (HSL, HSV, HWB, naive CMYK).
template <>
struct model_target_gamut<core::rgb::model::rgb8> : bounded_gamut<gamut_type::srgb> {};
template <>
struct model_target_gamut<core::rgb::model::rgba8> : bounded_gamut<gamut_type::srgb> {};
template <>
struct model_target_gamut<core::rgb::model::rgb_float> : bounded_gamut<gamut_type::srgb> {};
template <>
struct model_target_gamut<core::rgb::model::rgba_float> : bounded_gamut<gamut_type::srgb> {};
template <>
struct model_target_gamut<core::hsl::model::hsl> : bounded_gamut<gamut_type::srgb> {};
template <>
struct model_target_gamut<core::hsl::model::hsla> : bounded_gamut<gamut_type::srgb> {};
template <>
struct model_target_gamut<core::hsv::model::hsv> : bounded_gamut<gamut_type::srgb> {};
template <>
struct model_target_gamut<core::hsv::model::hsva> : bounded_gamut<gamut_type::srgb> {};
template <>
struct model_target_gamut<core::hwb::model::hwb> : bounded_gamut<gamut_type::srgb> {};
template <>
struct model_target_gamut<core::hwb::model::hwba> : bounded_gamut<gamut_type::srgb> {};
template <>
struct model_target_gamut<core::cmyk::model::cmyk_u8> : bounded_gamut<gamut_type::srgb> {};
template <>
struct model_target_gamut<core::cmyk::model::cmyk_float> : bounded_gamut<gamut_type::srgb> {};

// Wide-gamut display encodings map into their own primaries, never sRGB.
template <>
struct model_target_gamut<core::display_p3::model::display_p3> : bounded_gamut<gamut_type::display_p3> {};
template <>
struct model_target_gamut<core::display_p3::model::display_p3a> : bounded_gamut<gamut_type::display_p3> {};
template <>
struct model_target_gamut<core::adobe_rgb::model::adobe_rgb> : bounded_gamut<gamut_type::adobe_rgb> {};
template <>
struct model_target_gamut<core::adobe_rgb::model::adobe_rgba> : bounded_gamut<gamut_type::adobe_rgb> {};
template <>
struct model_target_gamut<core::prophoto_rgb::model::prophoto_rgb> : bounded_gamut<gamut_type::prophoto_rgb> {};
template <>
struct model_target_gamut<core::prophoto_rgb::model::prophoto_rgba> : bounded_gamut<gamut_type::prophoto_rgb> {};
template <>
struct model_target_gamut<core::rec2020::model::rec2020> : bounded_gamut<gamut_type::rec2020> {};
template <>
struct model_target_gamut<core::rec2020::model::rec2020a> : bounded_gamut<gamut_type::rec2020> {};

/// Target gamut of a color type (delegates to its model).
template <typename Color, typename = void>
struct target_gamut {
  static constexpr bool bounded = false;
  static constexpr gamut_type gamut = gamut_type::srgb;
};
template <typename Color>
struct target_gamut<Color, std::void_t<::colorcpp::operations::conversion::details::extract_model_t<Color>>>
    : model_target_gamut<::colorcpp::operations::conversion::details::extract_model_t<Color>> {};

/// True when conversions into @p Color clamp to a bounded RGB gamut.
template <typename Color>
inline constexpr bool has_target_gamut_v = target_gamut<Color>::bounded;

/// The bounded gamut @p Color clamps to (meaningful only when
/// @ref has_target_gamut_v is true).
template <typename Color>
inline constexpr gamut_type target_gamut_v = target_gamut<Color>::gamut;

namespace details {

// Assemble a gamut-clip result: target-gamut OkLab cast straight to Color
// (never through core::rgbaf_t, whose sRGB clamp would re-clip wide-gamut
// targets — issue #65), with unit-space alpha re-applied.
template <typename Color, typename Mid>
Color clip_result_to(const Mid& mid, float alpha) {
  Color out = ::colorcpp::operations::conversion::color_cast<Color>(mid);
  if constexpr (::colorcpp::operations::conversion::details::has_alpha_v<Color>) {
    constexpr std::size_t ai = ::colorcpp::operations::conversion::details::alpha_index_v<Color>;
    out.template get_index<ai>() = ::colorcpp::operations::conversion::details::from_unit<Color, ai>(alpha);
  }
  return out;
}

}  // namespace details

/**
 * @brief Clip a color to the specified gamut using simple channel clamping.
 * @tparam Color Input color type.
 * @param c The color to clip.
 * @param type The gamut type to clip to.
 * @return The clipped color.
 */
template <typename Color>
Color gamut_clip_to_gamut(const Color& c, gamut_type type = gamut_type::srgb) {
  using namespace operations::conversion;

  // Get OKLab values
  auto lab = color_cast<core::oklab_t>(c);
  float L = lab.template get_index<0>();
  float a = lab.template get_index<1>();
  float b = lab.template get_index<2>();
  float alpha = color_cast<core::rgbaf_t>(c).template get_index<3>();

  // Check if already in gamut
  if (is_in_gamut(c, type)) {
    return c;
  }

  // For achromatic colors, just clamp lightness
  float chroma = std::sqrt(a * a + b * b);
  if (chroma < 1e-6f) {
    float L_clamped = std::clamp(L, 0.0f, 1.0f);
    // Rebuild through OkLab and cast straight to Color (issue #65): routing
    // the result through core::rgbaf_t would clamp it to sRGB even when the
    // target gamut is wider. Carry the source alpha through.
    return details::clip_result_to<Color>(core::oklab_t{L_clamped, 0.0f, 0.0f}, alpha);
  }

  // Calculate hue
  float cos_h = a / chroma;
  float sin_h = b / chroma;

  // Binary search for maximum chroma that stays in gamut
  float min_C = 0.0f;
  float max_C = chroma;
  constexpr float kEps = 1e-4f;

  while (max_C - min_C > kEps) {
    float mid_C = (min_C + max_C) * 0.5f;
    float test_a = mid_C * cos_h;
    float test_b = mid_C * sin_h;

    // Convert test color to linear RGB
    float r_lin, g_lin, b_lin;
    details::oklab_to_linrgb(L, test_a, test_b, r_lin, g_lin, b_lin);

    if (details::linrgb_in_gamut(r_lin, g_lin, b_lin, type)) {
      min_C = mid_C;
    } else {
      max_C = mid_C;
    }
  }

  // Construct the clipped color (OkLab has no alpha channel: re-apply it).
  // Cast straight to Color: the search already landed on the *target* gamut's
  // boundary, and rebuilding through core::rgbaf_t would clamp that wide
  // result back into sRGB (issue #65).
  float final_a = min_C * cos_h;
  float final_b = min_C * sin_h;
  return details::clip_result_to<Color>(core::oklab_t{L, final_a, final_b}, alpha);
}

}  // namespace colorcpp::algorithms::gamut
