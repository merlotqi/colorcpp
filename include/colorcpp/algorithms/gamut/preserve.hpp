/**
 * @file preserve.hpp
 * @brief Perceptual preservation strategies for gamut mapping.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/algorithms/gamut/details.hpp>
#include <colorcpp/algorithms/gamut/distance.hpp>
#include <colorcpp/algorithms/gamut/mapping_result.hpp>
#include <colorcpp/algorithms/gamut/srgb.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::gamut {

/**
 * @brief Preservation modes for gamut mapping.
 */
enum class preserve_mode {
  lightness,   ///< Preserve lightness (L in OKLab)
  hue,         ///< Preserve hue (angle in OKLCh)
  saturation,  ///< Preserve chroma/saturation
  perceptual,  ///< Minimize perceptual difference (ΔE_OK)
};

namespace details {

// Calculate OKLab chroma
inline float oklab_chroma(float a, float b) noexcept { return std::sqrt(a * a + b * b); }

// Calculate OKLab hue (in radians)
inline float oklab_hue(float a, float b) noexcept { return std::atan2(b, a); }

}  // namespace details

/**
 * @brief Gamut mapping with lightness preservation.
 *
 * Attempts to preserve lightness while reducing chroma to fit in gamut.
 * If lightness is out of gamut, it will be clipped to [0, 1].
 */
template <typename Color>
Color gamut_clip_preserve_lightness(const Color& c) {
  using namespace operations::conversion;

  auto lab = color_cast<core::oklab_t>(c);
  float L = lab.template get_index<0>();
  float a = lab.template get_index<1>();
  float b = lab.template get_index<2>();
  float alpha = color_cast<core::rgbaf_t>(c).template get_index<3>();

  // Clamp lightness to valid range
  L = std::clamp(L, 0.0f, 1.0f);

  // Calculate chroma
  const float C = details::oklab_chroma(a, b);
  if (C < 1e-6f) {
    // Achromatic - just return with clamped lightness
    return color_cast<Color>(core::rgbaf_t{L, L, L, alpha});
  }

  // Calculate hue
  const float H = details::oklab_hue(a, b);
  const float cos_h = std::cos(H);
  const float sin_h = std::sin(H);

  // Binary search for maximum chroma that stays in gamut
  float min_C = 0.0f;
  float max_C = C;
  constexpr float kEps = 1e-4f;

  while (max_C - min_C > kEps) {
    const float mid_C = (min_C + max_C) * 0.5f;
    float r, g, bl;
    details::oklab_to_linrgb(L, mid_C * cos_h, mid_C * sin_h, r, g, bl);

    if (details::linrgb_in_gamut(r, g, bl)) {
      min_C = mid_C;
    } else {
      max_C = mid_C;
    }
  }

  // Construct color with preserved lightness and reduced chroma
  return color_cast<Color>(color_cast<core::rgbaf_t>(core::oklab_t{L, min_C * cos_h, min_C * sin_h}));
}

/**
 * @brief Gamut mapping with hue preservation.
 *
 * Attempts to preserve hue while reducing chroma and adjusting lightness to fit in gamut.
 */
template <typename Color>
Color gamut_clip_preserve_hue(const Color& c) {
  using namespace operations::conversion;

  auto lab = color_cast<core::oklab_t>(c);
  float L = lab.template get_index<0>();
  float a = lab.template get_index<1>();
  float b = lab.template get_index<2>();
  float alpha = color_cast<core::rgbaf_t>(c).template get_index<3>();

  // Calculate chroma and hue
  const float C = details::oklab_chroma(a, b);
  if (C < 1e-6f) {
    // Achromatic - use simple clipping
    L = std::clamp(L, 0.0f, 1.0f);
    return color_cast<Color>(core::rgbaf_t{L, L, L, alpha});
  }

  const float H = details::oklab_hue(a, b);
  const float cos_h = std::cos(H);
  const float sin_h = std::sin(H);

  // Try to preserve chroma first
  float r, g, bl;
  details::oklab_to_linrgb(L, C * cos_h, C * sin_h, r, g, bl);

  if (details::linrgb_in_gamut(r, g, bl)) {
    // Already in gamut
    return c;
  }

  // Binary search for maximum chroma that stays in gamut
  float min_C = 0.0f;
  float max_C = C;
  constexpr float kEps = 1e-4f;

  while (max_C - min_C > kEps) {
    const float mid_C = (min_C + max_C) * 0.5f;
    details::oklab_to_linrgb(L, mid_C * cos_h, mid_C * sin_h, r, g, bl);

    if (details::linrgb_in_gamut(r, g, bl)) {
      min_C = mid_C;
    } else {
      max_C = mid_C;
    }
  }

  return color_cast<Color>(color_cast<core::rgbaf_t>(core::oklab_t{L, min_C * cos_h, min_C * sin_h}));
}

/**
 * @brief Gamut mapping with saturation/chroma preservation.
 *
 * Attempts to preserve chroma while adjusting lightness to fit in gamut.
 */
template <typename Color>
Color gamut_clip_preserve_saturation(const Color& c) {
  using namespace operations::conversion;

  auto lab = color_cast<core::oklab_t>(c);
  float L = lab.template get_index<0>();
  float a = lab.template get_index<1>();
  float b = lab.template get_index<2>();
  float alpha = color_cast<core::rgbaf_t>(c).template get_index<3>();

  // Calculate chroma
  const float C = details::oklab_chroma(a, b);
  if (C < 1e-6f) {
    // Achromatic - use simple clipping
    L = std::clamp(L, 0.0f, 1.0f);
    return color_cast<Color>(core::rgbaf_t{L, L, L, alpha});
  }

  // Calculate hue
  const float H = details::oklab_hue(a, b);
  const float cos_h = std::cos(H);
  const float sin_h = std::sin(H);

  // Binary search for lightness that keeps chroma in gamut
  float min_L = 0.0f;
  float max_L = 1.0f;
  constexpr float kEps = 1e-4f;

  // Find the lightness that allows the full chroma
  while (max_L - min_L > kEps) {
    const float mid_L = (min_L + max_L) * 0.5f;
    float r, g, bl;
    details::oklab_to_linrgb(mid_L, C * cos_h, C * sin_h, r, g, bl);

    if (details::linrgb_in_gamut(r, g, bl)) {
      // This lightness works - try to get closer to original
      if (mid_L < L) {
        min_L = mid_L;
      } else {
        max_L = mid_L;
      }
    } else {
      // This lightness doesn't work
      if (mid_L < L) {
        min_L = mid_L;
      } else {
        max_L = mid_L;
      }
    }
  }

  // Use the found lightness
  const float final_L = (min_L + max_L) * 0.5f;
  return color_cast<Color>(color_cast<core::rgbaf_t>(core::oklab_t{final_L, C * cos_h, C * sin_h}));
}

/**
 * @brief Perceptual gamut mapping (minimize ΔE_OK).
 *
 * Uses CSS Color 4 style chroma reduction with perceptual error threshold.
 */
template <typename Color>
Color gamut_clip_perceptual(const Color& c) {
  using namespace operations::conversion;

  auto lab = color_cast<core::oklab_t>(c);
  float L = lab.template get_index<0>();
  float a = lab.template get_index<1>();
  float b = lab.template get_index<2>();
  float alpha = color_cast<core::rgbaf_t>(c).template get_index<3>();

  // Achromatic extremes
  if (L >= 1.0f) return color_cast<Color>(core::rgbaf_t{1.0f, 1.0f, 1.0f, alpha});
  if (L <= 0.0f) return color_cast<Color>(core::rgbaf_t{0.0f, 0.0f, 0.0f, alpha});

  // Already in gamut
  if (is_in_srgb_gamut(c)) return c;

  // Calculate chroma and hue
  const float C = details::oklab_chroma(a, b);
  if (C < 1e-6f) {
    // Achromatic - clamp lightness
    float L_clamped = std::clamp(L, 0.0f, 1.0f);
    return color_cast<Color>(core::rgbaf_t{L_clamped, L_clamped, L_clamped, alpha});
  }

  const float H = details::oklab_hue(a, b);
  const float cos_h = std::cos(H);
  const float sin_h = std::sin(H);

  // CSS Color 4 style: find largest chroma where clipping produces ΔE_OK ≤ JND
  constexpr float kJND = 0.02f;
  constexpr float kEps = 1e-4f;

  float min_C = 0.0f;
  float max_C = C;
  bool min_in_gamut = true;

  while (max_C - min_C > kEps) {
    const float mid_C = (min_C + max_C) * 0.5f;
    float r, g, bl;
    details::oklab_to_linrgb(L, mid_C * cos_h, mid_C * sin_h, r, g, bl);

    if (min_in_gamut && details::linrgb_in_gamut(r, g, bl)) {
      min_C = mid_C;
      continue;
    }

    // Clip to sRGB and measure ΔE_OK
    const float r_c = std::clamp(r, 0.0f, 1.0f);
    const float g_c = std::clamp(g, 0.0f, 1.0f);
    const float b_c = std::clamp(bl, 0.0f, 1.0f);

    float L_c, a_c, b_c_oklab;
    details::linrgb_to_oklab(r_c, g_c, b_c, L_c, a_c, b_c_oklab);

    const float dL = L - L_c;
    const float da = mid_C * cos_h - a_c;
    const float db = mid_C * sin_h - b_c_oklab;
    const float dE = std::sqrt(dL * dL + da * da + db * db);

    if (dE < kJND - kEps) {
      min_in_gamut = false;
      min_C = mid_C;
    } else if (dE > kJND + kEps) {
      max_C = mid_C;
    } else {
      // ΔE ≈ JND - return clipped version
      return color_cast<Color>(core::rgbaf_t{r_c, g_c, b_c, alpha});
    }
  }

  // Final candidate - clip it
  float r, g, bl;
  details::oklab_to_linrgb(L, min_C * cos_h, min_C * sin_h, r, g, bl);
  return color_cast<Color>(
      core::rgbaf_t{std::clamp(r, 0.0f, 1.0f), std::clamp(g, 0.0f, 1.0f), std::clamp(bl, 0.0f, 1.0f), alpha});
}

/**
 * @brief Gamut mapping with specified preservation mode.
 * @tparam Color Input color type.
 * @param c The color to map.
 * @param mode The preservation mode to use.
 * @return The mapped color.
 */
template <typename Color>
Color gamut_clip_preserve(const Color& c, preserve_mode mode = preserve_mode::perceptual) {
  switch (mode) {
    case preserve_mode::lightness:
      return gamut_clip_preserve_lightness(c);
    case preserve_mode::hue:
      return gamut_clip_preserve_hue(c);
    case preserve_mode::saturation:
      return gamut_clip_preserve_saturation(c);
    case preserve_mode::perceptual:
      return gamut_clip_perceptual(c);
    default:
      return gamut_clip_perceptual(c);
  }
}

/**
 * @brief Gamut mapping with detailed result information.
 * @tparam Color Input color type.
 * @param c The color to map.
 * @param mode The preservation mode to use.
 * @return Result containing mapped color and mapping information.
 */
template <typename Color>
gamut_mapping_result<Color> gamut_clip_with_info(const Color& c, preserve_mode mode = preserve_mode::perceptual) {
  using namespace operations::conversion;

  // Get original OKLab values
  auto lab_orig = color_cast<core::oklab_t>(c);
  const float L_orig = lab_orig.template get_index<0>();
  const float a_orig = lab_orig.template get_index<1>();
  const float b_orig = lab_orig.template get_index<2>();
  const float C_orig = details::oklab_chroma(a_orig, b_orig);

  // Check if already in gamut
  const bool was_in_gamut = is_in_srgb_gamut(c);

  // Map the color
  Color mapped = gamut_clip_preserve(c, mode);

  // Get mapped OKLab values
  auto lab_mapped = color_cast<core::oklab_t>(mapped);
  const float a_mapped = lab_mapped.template get_index<1>();
  const float b_mapped = lab_mapped.template get_index<2>();
  const float C_mapped = details::oklab_chroma(a_mapped, b_mapped);

  // Calculate ΔE_OK
  const float dL = L_orig - lab_mapped.template get_index<0>();
  const float da = a_orig - a_mapped;
  const float db = b_orig - b_mapped;
  const float delta_e = std::sqrt(dL * dL + da * da + db * db);

  return gamut_mapping_result<Color>{mapped, delta_e, C_orig, C_mapped, was_in_gamut};
}

}  // namespace colorcpp::algorithms::gamut
