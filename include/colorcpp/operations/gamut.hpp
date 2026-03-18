#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/operations/conversion.hpp>

// Gamut checking and mapping for the sRGB destination space.
//
// is_in_srgb_gamut — test whether a color lies inside the sRGB unit cube.
// gamut_clip       — map an out-of-gamut color back into sRGB using one of
//                    three strategies:
//   clip          — channel-clamp (fast; may shift hue and lightness)
//   chroma_reduce — OKLCh chroma bisect (preserves hue and lightness)
//   css_color4    — CSS Color Level 4 §14.2 algorithm: chroma bisect with
//                   ΔE_OK ≤ JND acceptance (best perceptual quality)
//
// All functions accept any color type registered with color_cast.
// Internally colors are mapped through OKLab so that the bisection operates
// in a perceptually uniform space.  Very out-of-gamut colors whose OKLab
// a/b components exceed ±0.5 are evaluated at their clamped coordinates;
// this is only an issue for imaginary / spectral colors far outside sRGB.

namespace colorcpp::operations::gamut {

enum class clip_method {
  clip,           // Channel clamp in sRGB — O(1)
  chroma_reduce,  // OKLCh chroma bisect until in-gamut — O(log 1/ε)
  css_color4,     // CSS Color Level 4 chroma bisect with ΔE_OK JND — O(log 1/ε)
};

namespace details {

// OKLab → linear sRGB, without clamping (needed for gamut testing).
// Coefficients from conversion.hpp (Ottosson 2020, M1 and M2 matrices).
inline void oklab_to_linrgb(float L, float a, float b, float& r, float& g, float& bl) noexcept {
  float l_ = L + 0.3963377774f * a + 0.2158037573f * b;
  float m_ = L - 0.1055613458f * a - 0.0638541728f * b;
  float s_ = L - 0.0894841775f * a - 1.2914855480f * b;

  const float lms_l = l_ * l_ * l_;
  const float lms_m = m_ * m_ * m_;
  const float lms_s = s_ * s_ * s_;

  r = 4.0767416621f * lms_l - 3.3077115913f * lms_m + 0.2309699292f * lms_s;
  g = -1.2684380046f * lms_l + 2.6097574011f * lms_m - 0.3413193965f * lms_s;
  bl = -0.0041960863f * lms_l - 0.7034186147f * lms_m + 1.7076147010f * lms_s;
}

// linear sRGB → OKLab, without range-checking the result.
// Coefficients match conversion.hpp (same M1/M2 matrices).
inline void linrgb_to_oklab(float r, float g, float b, float& L, float& a, float& bl) noexcept {
  float lms_l = 0.4122214708f * r + 0.5363325363f * g + 0.0514459929f * b;
  float lms_m = 0.2119034982f * r + 0.6806995451f * g + 0.1073969566f * b;
  float lms_s = 0.0883024619f * r + 0.2817188376f * g + 0.6299787005f * b;

  // Sign-preserving cube root (matches conversion.hpp's safe_cbrt).
  auto scbrt = [](float x) noexcept { return x >= 0.0f ? std::cbrt(x) : -std::cbrt(-x); };
  float l_ = scbrt(lms_l), m_ = scbrt(lms_m), s_ = scbrt(lms_s);

  L = 0.2104542553f * l_ + 0.7936177850f * m_ - 0.0040720468f * s_;
  a = 1.9779984951f * l_ - 2.4285922050f * m_ + 0.4505937099f * s_;
  bl = 0.0259040371f * l_ + 0.4072456426f * m_ - 0.4331497194f * s_;
}

inline bool linrgb_in_gamut(float r, float g, float b, float eps = 1e-4f) noexcept {
  return r >= -eps && r <= 1.0f + eps && g >= -eps && g <= 1.0f + eps && b >= -eps && b <= 1.0f + eps;
}

}  // namespace details

// ── is_in_srgb_gamut ─────────────────────────────────────────────────────────

template <typename Color>
bool is_in_srgb_gamut(const Color& c) {
  using namespace conversion;
  auto lab = color_cast<core::oklab_t>(c);
  float r, g, b;
  details::oklab_to_linrgb(lab.template get_index<0>(), lab.template get_index<1>(), lab.template get_index<2>(), r, g,
                           b);
  return details::linrgb_in_gamut(r, g, b);
}

// ── gamut_clip ────────────────────────────────────────────────────────────────

template <typename Color>
Color gamut_clip(const Color& c, clip_method method = clip_method::css_color4) {
  using namespace conversion;

  // ── Method: clip ─────────────────────────────────────────────────────────
  if (method == clip_method::clip) {
    auto rgb = color_cast<core::rgbaf_t>(c);
    // color_cast already clamps via from_unit; make the intent explicit.
    return color_cast<Color>(core::rgbaf_t{
        std::clamp(rgb.template get_index<0>(), 0.0f, 1.0f), std::clamp(rgb.template get_index<1>(), 0.0f, 1.0f),
        std::clamp(rgb.template get_index<2>(), 0.0f, 1.0f), rgb.template get_index<3>()});
  }

  // ── OKLab working representation ─────────────────────────────────────────
  auto lab = color_cast<core::oklab_t>(c);
  const float L = lab.template get_index<0>();
  const float a_ok = lab.template get_index<1>();
  const float b_ok = lab.template get_index<2>();
  const float alpha = color_cast<core::rgbaf_t>(c).template get_index<3>();

  // Achromatic lightness extremes.
  if (L >= 1.0f) return color_cast<Color>(core::rgbaf_t{1.0f, 1.0f, 1.0f, alpha});
  if (L <= 0.0f) return color_cast<Color>(core::rgbaf_t{0.0f, 0.0f, 0.0f, alpha});

  // Already in gamut — return unchanged.
  if (is_in_srgb_gamut(c)) return c;

  // Unit hue vector in OKLab (cos H, sin H).
  const float C_orig = std::sqrt(a_ok * a_ok + b_ok * b_ok);
  const float cos_h = (C_orig > 0.0f) ? a_ok / C_orig : 1.0f;
  const float sin_h = (C_orig > 0.0f) ? b_ok / C_orig : 0.0f;

  constexpr float kEps = 1e-4f;
  float min_C = 0.0f, max_C = C_orig;

  // ── Method: chroma_reduce ─────────────────────────────────────────────────
  if (method == clip_method::chroma_reduce) {
    while (max_C - min_C > kEps) {
      const float mid_C = (min_C + max_C) * 0.5f;
      float r, g, b;
      details::oklab_to_linrgb(L, mid_C * cos_h, mid_C * sin_h, r, g, b);
      if (details::linrgb_in_gamut(r, g, b))
        min_C = mid_C;
      else
        max_C = mid_C;
    }

    // Construct via OKLab with reduced chroma.
    // min_C * |cos_h| ≤ |a_ok| ≤ 0.5, so the channel range is always valid.
    return color_cast<Color>(color_cast<core::rgbaf_t>(core::oklab_t{L, min_C * cos_h, min_C * sin_h}));
  }

  // ── Method: css_color4 ────────────────────────────────────────────────────
  // CSS Color Level 4 §14.2: find the largest C for which channel-clipping
  // the candidate produces a ΔE_OK ≤ JND perceptual error.
  {
    constexpr float kJND = 0.02f;
    bool min_in_gamut = true;

    while (max_C - min_C > kEps) {
      const float mid_C = (min_C + max_C) * 0.5f;
      float r, g, b;
      details::oklab_to_linrgb(L, mid_C * cos_h, mid_C * sin_h, r, g, b);

      if (min_in_gamut && details::linrgb_in_gamut(r, g, b)) {
        min_C = mid_C;
        continue;
      }

      // Clip to sRGB and measure ΔE_OK between clipped and candidate.
      const float r_c = std::clamp(r, 0.0f, 1.0f);
      const float g_c = std::clamp(g, 0.0f, 1.0f);
      const float b_c = std::clamp(b, 0.0f, 1.0f);

      float L_c, a_c, b_clip;
      details::linrgb_to_oklab(r_c, g_c, b_c, L_c, a_c, b_clip);

      const float dL = L - L_c, da = mid_C * cos_h - a_c, db = mid_C * sin_h - b_clip;
      const float dE = std::sqrt(dL * dL + da * da + db * db);

      if (dE < kJND - kEps) {
        // Clipped color is still within JND — allow higher chroma.
        min_in_gamut = false;
        min_C = mid_C;
      } else if (dE > kJND + kEps) {
        // Clipped color is too different — reduce chroma.
        max_C = mid_C;
      } else {
        // ΔE ≈ JND: the clipped version is the answer.
        return color_cast<Color>(core::rgbaf_t{r_c, g_c, b_c, alpha});
      }
    }

    // Bisection converged: clip the final candidate.
    float r, g, b;
    details::oklab_to_linrgb(L, min_C * cos_h, min_C * sin_h, r, g, b);
    return color_cast<Color>(
        core::rgbaf_t{std::clamp(r, 0.0f, 1.0f), std::clamp(g, 0.0f, 1.0f), std::clamp(b, 0.0f, 1.0f), alpha});
  }
}

}  // namespace colorcpp::operations::gamut
