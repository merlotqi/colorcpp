/**
 * @file details.hpp
 * @brief Internal helper functions for gamut operations.
 */

#pragma once

#include <algorithm>
#include <cmath>

namespace colorcpp::algorithms::gamut::details {

// OKLab → linear sRGB, without clamping (needed for gamut testing).
// Coefficients from conversion.hpp (Ottosson 2020, M1 and M2 matrices).
inline void oklab_to_linrgb(float L, float a, float b, float& r, float& g, float& bl) noexcept {
  float l_ = L + 0.3939205158f * a + 0.4003836363f * b;
  float m_ = L - 0.1048460944f * a - 0.1184695156f * b;
  float s_ = L - 0.0750179025f * a - 2.3961106171f * b;

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

// Display P3 → linear RGB (D65)
inline void displayp3_to_linrgb(float r, float g, float b, float& lr, float& lg, float& lb) noexcept {
  // Display P3 uses the same primaries as DCI-P3 but with D65 white point
  // and sRGB transfer function
  auto gamma_decode = [](float v) noexcept {
    return (v <= 0.04045f) ? (v / 12.92f) : std::pow((v + 0.055f) / 1.055f, 2.4f);
  };
  lr = gamma_decode(r);
  lg = gamma_decode(g);
  lb = gamma_decode(b);
}

// linear RGB → Display P3
inline void linrgb_to_displayp3(float lr, float lg, float lb, float& r, float& g, float& b) noexcept {
  auto gamma_encode = [](float v) noexcept {
    v = std::clamp(v, 0.0f, 1.0f);
    return (v <= 0.0031308f) ? (12.92f * v) : (1.055f * std::pow(v, 1.0f / 2.4f) - 0.055f);
  };
  r = gamma_encode(lr);
  g = gamma_encode(lg);
  b = gamma_encode(lb);
}

// Adobe RGB → linear RGB (D65)
inline void adobergb_to_linrgb(float r, float g, float b, float& lr, float& lg, float& lb) noexcept {
  // Adobe RGB uses gamma 2.19921875 (563/256)
  constexpr float gamma = 2.19921875f;
  lr = std::pow(r, gamma);
  lg = std::pow(g, gamma);
  lb = std::pow(b, gamma);
}

// linear RGB → Adobe RGB
inline void linrgb_to_adobergb(float lr, float lg, float lb, float& r, float& g, float& b) noexcept {
  constexpr float gamma = 2.19921875f;
  constexpr float inv_gamma = 1.0f / gamma;
  r = std::pow(std::clamp(lr, 0.0f, 1.0f), inv_gamma);
  g = std::pow(std::clamp(lg, 0.0f, 1.0f), inv_gamma);
  b = std::pow(std::clamp(lb, 0.0f, 1.0f), inv_gamma);
}

}  // namespace colorcpp::algorithms::gamut::details
