/**
 * @file cam16.hpp
 * @brief CAM16 colour appearance model: viewing conditions and the forward transform.
 *
 * Reference: Li, Li, Wang, Zu, Luo, Melgosa, Brill, Pointer (2017),
 * "Comprehensive colour solutions: CAM16, CAT16 and CAM16-UCS",
 * Color Research & Application 42(6). The mathematics is cross-checked
 * against the colour-science implementation (BSD-3-Clause) and its
 * reference vectors, which the unit tests pin.
 *
 * @par Scale convention
 * This library's XYZ is relative with Y = 1 at the white; CAM16's published
 * equations use the Y = 100 scale, so XYZ and Y_b are scaled x100 at this
 * module's boundary. L_A is absolute (cd/m^2) and passes through unchanged.
 */

#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <colorcpp/algorithms/chromatic_adaptation.hpp>
#include <colorcpp/core/xyz.hpp>

namespace colorcpp::algorithms::appearance {

/** @brief Surround induction category (CIECAM02/CAM16 induction factors F, c, N_c). */
enum class surround {
  average,
  dim,
  dark
};

namespace details {

// CIE 159:2004 induction factors, shared by CAM16 (colour-science:
// VIEWING_CONDITIONS_CAM16 aliases VIEWING_CONDITIONS_CIECAM02).
inline constexpr std::array<float, 3> kInduction[] = {
    {1.0f, 0.69f, 1.0f},   // average
    {0.9f, 0.59f, 0.9f},   // dim
    {0.8f, 0.525f, 0.8f},  // dark
};

constexpr double kPi = 3.14159265358979323846;

/** @brief Signed power: sign(x) * |x|^p (colour-science spow). */
inline double spow(double x, double p) { return std::copysign(std::pow(std::abs(x), p), x); }

/** @brief 3x3 row-major matrix times a 3-vector. */
inline std::array<double, 3> mul3(const float* m, const std::array<double, 3>& v) {
  return {m[0] * v[0] + m[1] * v[1] + m[2] * v[2], m[3] * v[0] + m[4] * v[1] + m[5] * v[2],
          m[6] * v[0] + m[7] * v[1] + m[8] * v[2]};
}

/**
 * @brief CAM16 nonlinear compression of adapted cone responses.
 *
 * RGB_a = 400 * sign(RGB_c) * (F_L |RGB_c| / 100)^0.42 / (27.13 + (F_L |RGB_c| / 100)^0.42) + 0.1
 * (sign(0) = 0, so a zero cone response maps to exactly 0.1).
 */
inline std::array<double, 3> compress(const std::array<double, 3>& RGB_c, double F_L) {
  std::array<double, 3> out{};
  for (std::size_t i = 0; i < 3; ++i) {
    const double m = std::pow(F_L * std::abs(RGB_c[i]) / 100.0, 0.42);
    const double signed_m = RGB_c[i] == 0.0 ? 0.0 : (RGB_c[i] < 0.0 ? -m : m);
    out[i] = 400.0 * signed_m / (27.13 + m) + 0.1;
  }
  return out;
}

/** @brief Achromatic response A = (2 Ra + Ga + Ba/20 - 0.305) * N_bb. */
inline double achromatic(const std::array<double, 3>& RGB_a, double N_bb) {
  return (2.0 * RGB_a[0] + RGB_a[1] + RGB_a[2] / 20.0 - 0.305) * N_bb;
}

/**
 * @brief CAM16 hue quadrature H(h) on the 0-400 scale.
 *
 * Colour-science HUE_DATA_FOR_HUE_QUADRATURE (shared with CIECAM02):
 * nodes h_i = {20.14, 90, 164.25, 237.53, 380.14} with eccentricities
 * e_i = {0.8, 0.7, 1.0, 1.2, 0.8} and H_i = {0, 100, 200, 300, 400};
 * the last bin and the wrap below 20.14 use the rational form anchored at
 * H(360) = 385.9. Piecewise: H = H_k + 100 * t1 / (t1 + t2) with
 * t1 = (h - h_k) / e_k, t2 = (h_{k+1} - h) / e_{k+1}.
 */
inline double hue_quadrature_d(double h) {
  if (h < 20.14) {  // wrap: anchored at H = 385.9 (h = 0) and H = 400 (h -> 20.14-)
    const double t1 = h / 0.856;
    const double t2 = (20.14 - h) / 0.8;
    return 385.9 + (14.1 * t1) / (t1 + t2);
  }
  if (h < 90.0) {
    const double t1 = (h - 20.14) / 0.8;
    const double t2 = (90.0 - h) / 0.7;
    return 100.0 * t1 / (t1 + t2);
  }
  if (h < 164.25) {
    const double t1 = (h - 90.0) / 0.7;
    const double t2 = (164.25 - h) / 1.0;
    return 100.0 + 100.0 * t1 / (t1 + t2);
  }
  if (h < 237.53) {
    const double t1 = (h - 164.25) / 1.0;
    const double t2 = (237.53 - h) / 1.2;
    return 200.0 + 100.0 * t1 / (t1 + t2);
  }
  const double t1 = (h - 237.53) / 1.2;
  return 300.0 + (85.9 * t1) / (t1 + (360.0 - h) / 0.856);
}

}  // namespace details

/**
 * @brief CAM16 viewing conditions: every precomputed term of the model.
 *
 * Built once per scene via @ref create and reused across colours; the
 * derived terms (F_L, D, A_w, ...) depend only on the white, L_A, Y_b
 * and the surround.
 */
struct viewing_conditions {
  float F;                      ///< Induction factor F.
  float c;                      ///< Exponent c.
  float N_c;                    ///< Chromatic induction N_c.
  double D;                     ///< Degree of adaptation, clamped to [0, 1].
  double F_L;                   ///< Luminance-level adaptation factor.
  double n;                     ///< Background-to-white luminance ratio (Y = 100 scale).
  double z;                     ///< 1.48 + sqrt(n).
  double N_bb;                  ///< Brightness induction (== N_cb).
  double N_cb;                  ///< Chromatic induction of the background.
  double Y_w;                   ///< White luminance on the Y = 100 scale.
  std::array<double, 3> RGB_w;  ///< M_CAT16 * XYZ_w (Y = 100 scale).
  std::array<double, 3> D_RGB;  ///< Per-cone adaptation factor D * Y_w / RGB_w + 1 - D.
  double A_w;                   ///< Achromatic response of the adapted white.

  /**
   * @brief Build viewing conditions from a relative white point.
   * @param white Reference white, this library's Y = 1 scale (e.g. D65).
   * @param La Adapted luminance in cd/m^2 (absolute; colour-science uses 318.31).
   * @param Yb Background luminance factor, 0-1 (colour-science uses 20 for 0.2).
   * @param s Surround induction category (default average).
   */
  static viewing_conditions create(const core::xyz_t& white, float La, float Yb, surround s = surround::average) {
    using chromatic_adaptation::details::M_CAT16;
    const auto& ind = details::kInduction[static_cast<std::size_t>(s)];

    viewing_conditions vc{};
    vc.F = ind[0];
    vc.c = ind[1];
    vc.N_c = ind[2];

    const double La_d = static_cast<double>(La);
    vc.Y_w = static_cast<double>(white.y()) * 100.0;
    const double Y_b = static_cast<double>(Yb) * 100.0;
    vc.n = Y_b / vc.Y_w;

    const double k = 1.0 / (5.0 * La_d + 1.0);
    const double k4 = k * k * k * k;
    vc.F_L = 0.2 * k4 * (5.0 * La_d) + 0.1 * (1.0 - k4) * (1.0 - k4) * std::cbrt(5.0 * La_d);
    vc.N_bb = 0.725 * std::pow(1.0 / vc.n, 0.2);
    vc.N_cb = vc.N_bb;
    vc.z = 1.48 + std::sqrt(vc.n);
    vc.D = std::clamp(vc.F * (1.0 - (1.0 / 3.6) * std::exp((-La_d - 42.0) / 92.0)), 0.0, 1.0);

    const std::array<double, 3> XYZ_w{static_cast<double>(white.x()) * 100.0, static_cast<double>(white.y()) * 100.0,
                                      static_cast<double>(white.z()) * 100.0};
    vc.RGB_w = details::mul3(M_CAT16, XYZ_w);
    for (std::size_t i = 0; i < 3; ++i) {
      vc.D_RGB[i] = vc.D * vc.Y_w / vc.RGB_w[i] + 1.0 - vc.D;
    }
    const std::array<double, 3> RGB_wc{vc.D_RGB[0] * vc.RGB_w[0], vc.D_RGB[1] * vc.RGB_w[1], vc.D_RGB[2] * vc.RGB_w[2]};
    vc.A_w = details::achromatic(details::compress(RGB_wc, vc.F_L), vc.N_bb);
    return vc;
  }
};

/**
 * @brief CAM16 specification: the full set of appearance correlates.
 *
 * J lightness, C chroma, h hue angle (deg, 0-360), s saturation,
 * Q brightness, M colourfulness, H hue quadrature (0-400, see
 * @ref cam16_hue_quadrature).
 */
struct cam16_spec {
  float J;
  float C;
  float h;
  float s;
  float Q;
  float M;
  float H;
};

/** @brief CAM16 hue quadrature H(h) on the 0-400 scale (wrap region included). */
inline float cam16_hue_quadrature(float h) { return static_cast<float>(details::hue_quadrature_d(h)); }

/**
 * @brief Forward CAM16 transform: relative XYZ to appearance correlates.
 *
 * @param xyz Colour in this library's Y = 1 relative XYZ scale.
 * @param vc Viewing conditions from @ref viewing_conditions::create.
 * @return Appearance correlates (see @ref cam16_spec).
 */
inline cam16_spec xyz_to_cam16(const core::xyz_t& xyz, const viewing_conditions& vc) {
  using chromatic_adaptation::details::M_CAT16;

  const std::array<double, 3> XYZ{static_cast<double>(xyz.x()) * 100.0, static_cast<double>(xyz.y()) * 100.0,
                                  static_cast<double>(xyz.z()) * 100.0};
  const auto RGB = details::mul3(M_CAT16, XYZ);
  const std::array<double, 3> RGB_c{vc.D_RGB[0] * RGB[0], vc.D_RGB[1] * RGB[1], vc.D_RGB[2] * RGB[2]};
  const auto RGB_a = details::compress(RGB_c, vc.F_L);
  const double Ra = RGB_a[0];
  const double Ga = RGB_a[1];
  const double Ba = RGB_a[2];

  const double a = Ra - (12.0 / 11.0) * Ga + Ba / 11.0;
  const double b = (Ra + Ga - 2.0 * Ba) / 9.0;
  double h = std::atan2(b, a) * 180.0 / details::kPi;
  if (h < 0.0) h += 360.0;

  const double e_t = (std::cos(h * details::kPi / 180.0 + 2.0) + 3.8) / 4.0;  // +2 radians
  const double A = details::achromatic(RGB_a, vc.N_bb);
  const double J = 100.0 * std::pow(A / vc.A_w, vc.c * vc.z);
  const double Q = (4.0 / vc.c) * std::sqrt(J / 100.0) * (vc.A_w + 4.0) * std::pow(vc.F_L, 0.25);
  const double t =
      (50000.0 / 13.0) * vc.N_c * vc.N_cb * (e_t * std::sqrt(a * a + b * b) / (Ra + Ga + (21.0 / 20.0) * Ba));
  const double C = std::pow(t, 0.9) * std::pow(J / 100.0, 0.5) * std::pow(1.64 - std::pow(0.29, vc.n), 0.73);
  const double M = C * std::pow(vc.F_L, 0.25);
  const double s = 100.0 * std::pow(M / Q, 0.5);

  float h_f = static_cast<float>(h);
  // atan2 can return a hair below 0 (e.g. b = -1e-20); after +360 the
  // double is 360-eps, which float32 rounds up to exactly 360.
  if (h_f >= 360.0f) h_f = 0.0f;
  return cam16_spec{static_cast<float>(J),
                    static_cast<float>(C),
                    h_f,
                    static_cast<float>(s),
                    static_cast<float>(Q),
                    static_cast<float>(M),
                    static_cast<float>(details::hue_quadrature_d(h))};
}

/**
 * @brief Inverse CAM16 transform: appearance correlates back to relative XYZ.
 *
 * Recovers the opponent a/b pair from (J, C, h) with the standard
 * singularity guards (t = 0 collapses to the achromatic axis; the
 * |sin h| vs |cos h| branch avoids dividing by a near-zero component),
 * inverts the nonlinear compression, undoes the cone adaptation and
 * applies the inverse CAT16 matrix.
 *
 * @param J Lightness (0-100).
 * @param C Chroma.
 * @param h Hue angle in degrees (0-360).
 * @param vc Viewing conditions from @ref viewing_conditions::create.
 * @return Colour in this library's Y = 1 relative XYZ scale.
 */
inline core::xyz_t cam16_to_xyz(float J, float C, float h, const viewing_conditions& vc) {
  using chromatic_adaptation::details::M_CAT16_inv;

  const double Jd = static_cast<double>(J);
  const double Cd = static_cast<double>(C);
  const double hd = static_cast<double>(h);

  const double J_prime = std::max(Jd, 1e-6);
  const double chroma_factor = std::pow(1.64 - std::pow(0.29, vc.n), 0.73);
  const double t = std::pow(Cd / (std::sqrt(J_prime / 100.0) * chroma_factor), 1.0 / 0.9);
  const double A = vc.A_w * std::pow(Jd / 100.0, 1.0 / (vc.c * vc.z));

  const double h_rad = hd * details::kPi / 180.0;
  const double e_t = (std::cos(h_rad + 2.0) + 3.8) / 4.0;
  const double sin_h = std::sin(h_rad);
  const double cos_h = std::cos(h_rad);

  double a = 0.0;
  double b = 0.0;
  const double P2 = A / vc.N_bb + 0.305;
  if (t > 0.0) {
    const double P1 = (50000.0 / 13.0) * vc.N_c * vc.N_cb * e_t / t;
    constexpr double P3 = 21.0 / 20.0;
    const double n_ab = P2 * (2.0 + P3) * (460.0 / 1403.0);
    if (std::abs(sin_h) >= std::abs(cos_h)) {
      const double P4 = P1 / sin_h;
      const double cot_h = cos_h / sin_h;
      b = n_ab / (P4 + (2.0 + P3) * (220.0 / 1403.0) * cot_h - 27.0 / 1403.0 + P3 * (6300.0 / 1403.0));
      a = b * cot_h;
    } else {
      const double P5 = P1 / cos_h;
      const double tan_h = sin_h / cos_h;
      a = n_ab / (P5 + (2.0 + P3) * (220.0 / 1403.0) - (27.0 / 1403.0 - P3 * (6300.0 / 1403.0)) * tan_h);
      b = a * tan_h;
    }
  }

  const std::array<double, 3> RGB_a{(460.0 * P2 + 451.0 * a + 288.0 * b) / 1403.0,
                                    (460.0 * P2 - 891.0 * a - 261.0 * b) / 1403.0,
                                    (460.0 * P2 - 220.0 * a - 6300.0 * b) / 1403.0};

  std::array<double, 3> RGB_c{};
  for (std::size_t i = 0; i < 3; ++i) {
    const double d = RGB_a[i] - 0.1;
    const double m = std::abs(d);
    const double sign = d == 0.0 ? 0.0 : (d < 0.0 ? -1.0 : 1.0);
    // Inverse of y = 400 x / (27.13 + x): x = 27.13 y / (400 - y) with
    // x = (F_L |RGB_c| / 100)^0.42 — the sign scales the result, not |y|.
    RGB_c[i] = sign * (100.0 / vc.F_L) * std::pow((27.13 * m) / (400.0 - m), 1.0 / 0.42);
  }

  const std::array<double, 3> RGB{RGB_c[0] / vc.D_RGB[0], RGB_c[1] / vc.D_RGB[1], RGB_c[2] / vc.D_RGB[2]};
  const auto XYZ100 = details::mul3(M_CAT16_inv, RGB);
  return core::xyz_t(static_cast<float>(XYZ100[0] / 100.0), static_cast<float>(XYZ100[1] / 100.0),
                     static_cast<float>(XYZ100[2] / 100.0));
}

}  // namespace colorcpp::algorithms::appearance
