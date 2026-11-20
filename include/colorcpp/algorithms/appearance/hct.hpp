/**
 * @file hct.hpp
 * @brief HCT: Material You's colour space — CAM16 hue and chroma with CIE L* tone.
 *
 * HCT separates what the eye separates: hue and chroma come from CAM16
 * (under Material's canonical viewing conditions), tone is the CIE L*
 * lightness of the luminance. The inverse solver is an independent Newton
 * iteration on J — the Y(J) relation is locally quadratic, so
 * J <- J - (Y_cur - Y) * J / (2 * Y_cur) — with a chroma bisection when the
 * requested chroma leaves the sRGB gamut at that hue and tone (hue and tone
 * are then preserved and chroma is maximized, matching Material's
 * contract). The viewing-condition constants match material-color-utilities'
 * kDefaultViewingConditions (Apache-2.0, numeric values only).
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/algorithms/appearance/cam16.hpp>
#include <colorcpp/core/linear_rgb.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::appearance {

/** @brief HCT coordinate triple: hue in degrees, CAM16 chroma, CIE L* tone. */
struct hct {
  float hue;
  float chroma;
  float tone;
};

namespace details {

/** @brief CIE Y (0-100) from L* — exact k = 24389/27 breakpoint at L* = 8. */
inline double y100_from_lstar(double lstar) {
  if (lstar > 8.0) {
    const double t = (lstar + 16.0) / 116.0;
    return t * t * t * 100.0;
  }
  return lstar / (24389.0 / 27.0) * 100.0;
}

/** @brief CIE L* from Y (0-100) — inverse of @ref y100_from_lstar. */
inline double lstar_from_y100(double y100) {
  const double yn = y100 / 100.0;
  if (yn <= 216.0 / 24389.0) return (24389.0 / 27.0) * yn;
  return 116.0 * std::cbrt(yn) - 16.0;
}

/**
 * @brief Newton-solve XYZ for (hue, chroma) at luminance Y.
 *
 * Returns false when J does not converge or the converged colour leaves the
 * sRGB gamut (checked on the extended-range linear cast, which does not
 * clamp). Tolerance matches Material's |dY| < 0.002 on the Y = 100 scale.
 */
inline bool hct_solve(double hue_deg, double chroma, double Y, const viewing_conditions& vc, core::xyz_t& out) {
  if (!(Y > 0.0)) return false;
  double J = 110.0 * std::sqrt(Y);  // Material seed: 11 * sqrt(Y100)
  if (!(J > 0.0)) return false;
  for (int i = 0; i < 16; ++i) {
    const auto xyz = cam16_to_xyz(static_cast<float>(J), static_cast<float>(chroma), static_cast<float>(hue_deg), vc);
    const double Yc = static_cast<double>(xyz.y());
    if (!std::isfinite(Yc) || !(Yc > 0.0)) return false;
    if (std::abs(Yc - Y) < 2e-5) {
      const auto lin = operations::conversion::color_cast<core::linear_rgbf_t>(xyz);
      constexpr float kEps = 1e-5f;
      if (lin.r() < -kEps || lin.r() > 1.0f + kEps || lin.g() < -kEps || lin.g() > 1.0f + kEps || lin.b() < -kEps ||
          lin.b() > 1.0f + kEps) {
        return false;  // converged, but outside sRGB
      }
      out = xyz;
      return true;
    }
    J -= (Yc - Y) * J / (2.0 * Yc);
    if (!(J > 0.0)) return false;
  }
  return false;
}

}  // namespace details

/** @brief CIE L* from a relative (0-1) luminance Y. */
inline float lstar_from_y(float y) {
  return static_cast<float>(details::lstar_from_y100(static_cast<double>(y) * 100.0));
}

/** @brief Relative (0-1) luminance Y from CIE L*. */
inline float y_from_lstar(float lstar) {
  return static_cast<float>(details::y100_from_lstar(static_cast<double>(lstar)) / 100.0);
}

/**
 * @brief Material's canonical HCT viewing conditions.
 *
 * D65 white (0.95047, 1, 1.08883), L_A = 200/pi * Y(L*=50) / 100 =
 * 11.725676537 cd/m^2, background L* = 50 (Y_b = 0.184186503), average
 * surround — material-color-utilities kDefaultViewingConditions.
 */
inline const viewing_conditions& hct_viewing_conditions() {
  static const viewing_conditions vc = [] {
    const core::xyz_t d65{0.95047f, 1.0f, 1.08883f};
    return viewing_conditions::create(d65, 11.725676537f, 0.184186503f, surround::average);
  }();
  return vc;
}

/** @brief Forward HCT: relative XYZ to hue/chroma/tone. */
inline hct xyz_to_hct(const core::xyz_t& xyz) {
  const auto spec = xyz_to_cam16(xyz, hct_viewing_conditions());
  return hct{spec.h, spec.C, lstar_from_y(static_cast<float>(xyz.y()))};
}

/**
 * @brief Inverse HCT: hue/chroma/tone to relative XYZ, limited to sRGB.
 *
 * Tone 0 and 100 are black and white; otherwise Newton solves J for the
 * target luminance, bisecting chroma downward when the full chroma leaves
 * the gamut (hue and tone preserved, chroma maximized).
 */
inline core::xyz_t hct_to_xyz(const hct& c) {
  const auto& vc = hct_viewing_conditions();
  const double Y = details::y100_from_lstar(std::clamp(static_cast<double>(c.tone), 0.0, 100.0)) / 100.0;

  // Chroma 0 (and the L* endpoints, whose chroma is 0 too) is the exact
  // achromatic axis at this luminance — Material shortcuts it to the sRGB
  // gray instead of running the solver (whose CAM16-neutral carries the
  // white point's small residual chroma).
  if (c.chroma < 1e-4f) {
    return core::xyz_t{static_cast<float>(Y) * 0.95047f, static_cast<float>(Y), static_cast<float>(Y) * 1.08883f};
  }

  double hue = std::fmod(static_cast<double>(c.hue), 360.0);
  if (hue < 0.0) hue += 360.0;
  const double chroma = std::max(static_cast<double>(c.chroma), 0.0);

  core::xyz_t best{0.0f, 0.0f, 0.0f};
  if (details::hct_solve(hue, chroma, Y, vc, best)) return best;

  double lo = 0.0;
  double hi = chroma;
  for (int i = 0; i < 20; ++i) {
    const double mid = 0.5 * (lo + hi);
    if (details::hct_solve(hue, mid, Y, vc, best)) {
      lo = mid;
    } else {
      hi = mid;
    }
  }
  if (details::hct_solve(hue, lo, Y, vc, best)) return best;

  // Unreachable for valid input: achromatic fallback at the target luminance.
  return core::xyz_t{static_cast<float>(Y) * 0.95047f, static_cast<float>(Y), static_cast<float>(Y) * 1.08883f};
}

}  // namespace colorcpp::algorithms::appearance
