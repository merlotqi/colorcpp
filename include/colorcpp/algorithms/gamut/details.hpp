/**
 * @file details.hpp
 * @brief Internal helper functions for gamut operations.
 */

#pragma once

#include <array>
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

// CIE XYZ (D65) → linear Adobe RGB (A98). Exact inverse of the forward
// matrix registered in conversion/functions/adobe_rgb.hpp (Gauss-Jordan on
// the 7-digit coefficients): the pair must be mutually inverse or colors on
// Adobe's own cube — including white — round-trip outside it (issue #68).
inline void xyz_to_linear_adobergb(float x, float y, float z, float& r, float& g, float& b) noexcept {
  r = 2.0415871f * x - 0.5650054f * y - 0.3447321f * z;
  g = -0.9692428f * x + 1.8759654f * y + 0.0415562f * z;
  b = 0.0134451f * x - 0.1183627f * y + 1.0151743f * z;
}

// CIE XYZ (D65) → XYZ (D50) Bradford adaptation, folded once at compile time:
// M_BFD_inv · diag(cone_D50 / cone_D65) · M_BFD, with the cone responses of
// the two white points. Derived from algorithms::chromatic_adaptation's
// matrices and white points (they are constexpr), so the two paths cannot
// drift; per-call recomputation used to cost ~8 ns instead of ~3 ns
// (issue #68).
constexpr std::array<float, 9> MakeBradfordD65ToD50() {
  namespace ca = colorcpp::algorithms::chromatic_adaptation;
  const float* m = ca::details::M_BFD;
  const float* mi = ca::details::M_BFD_inv;
  const auto& sw = ca::WHITEPOINT_D65;
  const auto& dw = ca::WHITEPOINT_D50;

  const float cone_src[3] = {m[0] * sw.x() + m[1] * sw.y() + m[2] * sw.z(),
                             m[3] * sw.x() + m[4] * sw.y() + m[5] * sw.z(),
                             m[6] * sw.x() + m[7] * sw.y() + m[8] * sw.z()};
  const float cone_dst[3] = {m[0] * dw.x() + m[1] * dw.y() + m[2] * dw.z(),
                             m[3] * dw.x() + m[4] * dw.y() + m[5] * dw.z(),
                             m[6] * dw.x() + m[7] * dw.y() + m[8] * dw.z()};

  // adapted = M_BFD_inv · S · M_BFD where S = diag(cone_dst / cone_src)
  std::array<float, 9> out{};
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      float sum = 0.0f;
      for (int k = 0; k < 3; ++k) {
        sum += mi[3 * i + k] * (cone_dst[k] / cone_src[k]) * m[3 * k + j];
      }
      out[3 * i + j] = sum;
    }
  }
  return out;
}
inline constexpr std::array<float, 9> kBradfordD65ToD50 = MakeBradfordD65ToD50();

// CIE XYZ (D65) → linear ProPhoto RGB (D50): precomputed Bradford adaptation
// (kBradfordD65ToD50), then the ProPhoto primaries matrix. Plain-float
// arithmetic (no color_cast) so out-of-range values survive.
inline void xyz_to_linear_prophoto(float x, float y, float z, float& r, float& g, float& b) noexcept {
  const auto& a = kBradfordD65ToD50;

  const float x50 = a[0] * x + a[1] * y + a[2] * z;
  const float y50 = a[3] * x + a[4] * y + a[5] * z;
  const float z50 = a[6] * x + a[7] * y + a[8] * z;

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
