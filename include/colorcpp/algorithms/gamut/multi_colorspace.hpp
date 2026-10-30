/**
 * @file multi_colorspace.hpp
 * @brief Multi-colorspace gamut support (sRGB, Display P3, Adobe RGB).
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/algorithms/gamut/details.hpp>
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

  auto lab = color_cast<core::oklab_t>(c);
  float r, g, b;
  details::oklab_to_linrgb(lab.template get_index<0>(), lab.template get_index<1>(), lab.template get_index<2>(), r, g,
                           b);
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
    // OkLab L is not an sRGB value: rebuild through OkLab instead of treating
    // L as a channel, and carry the source alpha through.
    auto gray = color_cast<core::rgbaf_t>(core::oklab_t{L_clamped, 0.0f, 0.0f});
    gray.a() = alpha;
    return color_cast<Color>(gray);
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

  // Construct the clipped color (OkLab has no alpha channel: re-apply it)
  float final_a = min_C * cos_h;
  float final_b = min_C * sin_h;
  auto out = color_cast<core::rgbaf_t>(core::oklab_t{L, final_a, final_b});
  out.a() = alpha;
  return color_cast<Color>(out);
}

}  // namespace colorcpp::algorithms::gamut
