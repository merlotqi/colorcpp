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
  srgb,        ///< Standard RGB (sRGB)
  display_p3,  ///< Display P3 (wide gamut)
  adobe_rgb,   ///< Adobe RGB (wide gamut)
};

namespace details {

// Check if linear RGB is in Display P3 gamut
inline bool linrgb_in_displayp3_gamut(float r, float g, float b, float eps = 1e-4f) noexcept {
  // Display P3 has the same transfer function as sRGB but different primaries
  // For simplicity, we check if the color can be represented in Display P3
  // by converting to Display P3 and checking if values are in [0, 1]
  float dp3_r, dp3_g, dp3_b;
  linrgb_to_displayp3(r, g, b, dp3_r, dp3_g, dp3_b);
  return dp3_r >= -eps && dp3_r <= 1.0f + eps && dp3_g >= -eps && dp3_g <= 1.0f + eps && dp3_b >= -eps &&
         dp3_b <= 1.0f + eps;
}

// Check if linear RGB is in Adobe RGB gamut
inline bool linrgb_in_adobergb_gamut(float r, float g, float b, float eps = 1e-4f) noexcept {
  // Adobe RGB uses different primaries and gamma
  float argb_r, argb_g, argb_b;
  linrgb_to_adobergb(r, g, b, argb_r, argb_g, argb_b);
  return argb_r >= -eps && argb_r <= 1.0f + eps && argb_g >= -eps && argb_g <= 1.0f + eps && argb_b >= -eps &&
         argb_b <= 1.0f + eps;
}

// Generic gamut check based on gamut type
inline bool linrgb_in_gamut(float r, float g, float b, gamut_type type, float eps = 1e-4f) noexcept {
  switch (type) {
    case gamut_type::srgb:
      return details::linrgb_in_gamut(r, g, b, eps);
    case gamut_type::display_p3:
      return linrgb_in_displayp3_gamut(r, g, b, eps);
    case gamut_type::adobe_rgb:
      return linrgb_in_adobergb_gamut(r, g, b, eps);
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
    return color_cast<Color>(core::rgbaf_t{L_clamped, L_clamped, L_clamped, alpha});
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

  // Construct the clipped color
  float final_a = min_C * cos_h;
  float final_b = min_C * sin_h;
  return color_cast<Color>(color_cast<core::rgbaf_t>(core::oklab_t{L, final_a, final_b}));
}

}  // namespace colorcpp::algorithms::gamut
