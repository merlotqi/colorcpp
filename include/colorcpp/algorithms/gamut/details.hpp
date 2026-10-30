/**
 * @file details.hpp
 * @brief Internal helper functions for gamut operations.
 */

#pragma once

#include <cmath>
#include <colorcpp/algorithms/chromatic_adaptation.hpp>
#include <colorcpp/core/xyz.hpp>

namespace colorcpp::algorithms::gamut::details {

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
  bl = 0.0259040371f * l_ + 0.7827717662f * m_ - 0.8086757660f * s_;
}

inline bool linrgb_in_gamut(float r, float g, float b, float eps = 1e-4f) noexcept {
  return r >= -eps && r <= 1.0f + eps && g >= -eps && g <= 1.0f + eps && b >= -eps && b <= 1.0f + eps;
}

// ---- Wide-gamut membership helpers ----
// linear sRGB → target linear RGB via XYZ. Coefficients match the matrices
// registered in operations/conversion/functions/{linear_rgb,xyz,adobe_rgb,
// prophoto_rgb}.hpp. Everything stays unclamped: gamut testing needs the
// out-of-range values (transfer curves and [0,1] clamps hide them).

// linear sRGB → CIE XYZ (D65)
inline void linrgb_to_xyz(float r, float g, float b, float& x, float& y, float& z) noexcept {
  x = 0.4124564f * r + 0.3575761f * g + 0.1804375f * b;
  y = 0.2126729f * r + 0.7151522f * g + 0.0721750f * b;
  z = 0.0193339f * r + 0.1191920f * g + 0.9503041f * b;
}

// CIE XYZ (D65) → linear sRGB, without clamping. Exact inverse of the matrix
// above (matches conversion::details::xyz_to_oklab's raw first stage; D65
// white maps to (1, 1, 1)). Gamut membership computes through this raw helper
// so no color storage range can influence the result (issue #67).
inline void xyz_to_linrgb(float x, float y, float z, float& r, float& g, float& b) noexcept {
  r = 3.2404542f * x - 1.5371385f * y - 0.4985314f * z;
  g = -0.9692660f * x + 1.8760108f * y + 0.0415560f * z;
  b = 0.0556434f * x - 0.2040259f * y + 1.0572252f * z;
}

// CIE XYZ (D65) → linear Display P3
inline void xyz_to_linear_displayp3(float x, float y, float z, float& r, float& g, float& b) noexcept {
  r = 2.4934969f * x - 0.9313836f * y - 0.4027108f * z;
  g = -0.8294890f * x + 1.7626641f * y + 0.0236247f * z;
  b = 0.0358458f * x - 0.0761724f * y + 0.9568845f * z;
}

// CIE XYZ (D65) → linear Adobe RGB (A98)
inline void xyz_to_linear_adobergb(float x, float y, float z, float& r, float& g, float& b) noexcept {
  r = 2.0413690f * x - 0.5649654f * y - 0.3446945f * z;
  g = -0.9692660f * x + 1.8760108f * y + 0.0415560f * z;
  b = 0.0134474f * x - 0.1183897f * y + 1.0154096f * z;
}

// CIE XYZ (D65) → linear ProPhoto RGB (D50): Bradford adaptation, then the
// ProPhoto primaries matrix. Reuses the cone matrices and white points of
// algorithms::chromatic_adaptation so the two paths cannot drift; plain-float
// arithmetic (no color_cast) so out-of-range values survive.
inline void xyz_to_linear_prophoto(float x, float y, float z, float& r, float& g, float& b) noexcept {
  namespace ca = colorcpp::algorithms::chromatic_adaptation;
  const auto& m = ca::details::M_BFD;
  const auto& mi = ca::details::M_BFD_inv;
  const auto& sw = ca::WHITEPOINT_D65;
  const auto& dw = ca::WHITEPOINT_D50;

  const float cone_src[3] = {m[0] * sw.x() + m[1] * sw.y() + m[2] * sw.z(),
                             m[3] * sw.x() + m[4] * sw.y() + m[5] * sw.z(),
                             m[6] * sw.x() + m[7] * sw.y() + m[8] * sw.z()};
  const float cone_dst[3] = {m[0] * dw.x() + m[1] * dw.y() + m[2] * dw.z(),
                             m[3] * dw.x() + m[4] * dw.y() + m[5] * dw.z(),
                             m[6] * dw.x() + m[7] * dw.y() + m[8] * dw.z()};
  const float cone[3] = {m[0] * x + m[1] * y + m[2] * z, m[3] * x + m[4] * y + m[5] * z,
                         m[6] * x + m[7] * y + m[8] * z};

  float scaled[3];
  for (int i = 0; i < 3; ++i) {
    scaled[i] = (cone_src[i] != 0.0f) ? cone[i] * cone_dst[i] / cone_src[i] : cone[i];
  }

  const float x50 = mi[0] * scaled[0] + mi[1] * scaled[1] + mi[2] * scaled[2];
  const float y50 = mi[3] * scaled[0] + mi[4] * scaled[1] + mi[5] * scaled[2];
  const float z50 = mi[6] * scaled[0] + mi[7] * scaled[1] + mi[8] * scaled[2];

  // XYZ(D50) → linear ProPhoto (matches conversion::details::xyz_to_linear_prophoto_rgb)
  r = 1.3459433f * x50 - 0.2556075f * y50 - 0.0511118f * z50;
  g = -0.5445989f * x50 + 1.5081673f * y50 + 0.0205351f * z50;
  b = 1.2119587f * z50;
}

// linear sRGB → linear Display P3 (D65)
inline void linrgb_to_linear_displayp3(float lr, float lg, float lb, float& r, float& g, float& b) noexcept {
  float x, y, z;
  linrgb_to_xyz(lr, lg, lb, x, y, z);
  xyz_to_linear_displayp3(x, y, z, r, g, b);
}

// linear sRGB → linear Adobe RGB (D65)
inline void linrgb_to_linear_adobergb(float lr, float lg, float lb, float& r, float& g, float& b) noexcept {
  float x, y, z;
  linrgb_to_xyz(lr, lg, lb, x, y, z);
  xyz_to_linear_adobergb(x, y, z, r, g, b);
}

// linear sRGB → linear ProPhoto RGB (D50)
inline void linrgb_to_linear_prophoto(float lr, float lg, float lb, float& r, float& g, float& b) noexcept {
  float x, y, z;
  linrgb_to_xyz(lr, lg, lb, x, y, z);
  xyz_to_linear_prophoto(x, y, z, r, g, b);
}

}  // namespace colorcpp::algorithms::gamut::details
