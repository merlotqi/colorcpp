/**
 * @file cam16_ucs.hpp
 * @brief CAM16-UCS: uniform perceptual coordinates J'a'b' and their delta E.
 *
 * Li et al. (2017) adopt the Luo (2006) CAM02-UCS coefficients for
 * CAM16-UCS: K_L = 1.00, c1 = 0.007, c2 = 0.0228. The coordinate mapping
 * runs on (J, M, h) — colourfulness, not chroma — exactly as in the
 * colour-science implementation (BSD-3-Clause).
 *
 *   J' = (1 + 100 c1) J / (1 + c1 J)
 *   M' = ln(1 + c2 M) / c2
 *   a' = M' cos h,  b' = M' sin h
 *
 * delta E is the plain Euclidean distance in J'a'b'.
 */

#pragma once

#include <cmath>
#include <colorcpp/algorithms/appearance/cam16.hpp>

namespace colorcpp::algorithms::appearance {

/** @brief CAM16-UCS coordinate triple (Luo 2006 / Li 2017 uniform space). */
struct cam16_ucs {
  float Jp;
  float ap;
  float bp;
};

/** @brief CAM16 correlates that feed the UCS transform (colourfulness, not chroma). */
struct cam16_jmh {
  float J;
  float M;
  float h;
};

/** @brief Luo (2006) CAM02-UCS coefficients, adopted by CAM16-UCS (Li et al. 2017). */
inline constexpr double kCam16UcsC1 = 0.007;
inline constexpr double kCam16UcsC2 = 0.0228;

/**
 * @brief Forward CAM16-UCS transform: (J, C, h) specification to J'a'b'.
 *
 * Uses the M (colourfulness) correlate of @p spec — chroma is NOT the
 * UCS input.
 */
inline cam16_ucs cam16_to_ucs(const cam16_spec& spec) {
  const double J = static_cast<double>(spec.J);
  const double M = static_cast<double>(spec.M);
  const double h_rad = static_cast<double>(spec.h) * details::kPi / 180.0;
  const double Jp = (1.0 + 100.0 * kCam16UcsC1) * J / (1.0 + kCam16UcsC1 * J);
  const double Mp = std::log1p(kCam16UcsC2 * M) / kCam16UcsC2;
  return cam16_ucs{static_cast<float>(Jp), static_cast<float>(Mp * std::cos(h_rad)),
                   static_cast<float>(Mp * std::sin(h_rad))};
}

/**
 * @brief Inverse CAM16-UCS transform: J'a'b' back to (J, M, h).
 *
 * The achromatic origin maps to M = 0 with h = 0 (atan2(0, 0) = 0).
 */
inline cam16_jmh ucs_to_cam16_jmh(const cam16_ucs& c) {
  const double Jp = static_cast<double>(c.Jp);
  const double ap = static_cast<double>(c.ap);
  const double bp = static_cast<double>(c.bp);
  const double J = -Jp / (kCam16UcsC1 * Jp - 1.0 - 100.0 * kCam16UcsC1);
  const double Mp = std::hypot(ap, bp);
  const double M = std::expm1(kCam16UcsC2 * Mp) / kCam16UcsC2;
  double h = std::atan2(bp, ap) * 180.0 / details::kPi;
  if (h < 0.0) h += 360.0;
  return cam16_jmh{static_cast<float>(J), static_cast<float>(M), static_cast<float>(h)};
}

/** @brief Euclidean colour difference in CAM16-UCS (Li et al. 2017). */
inline float delta_e_cam16_ucs(const cam16_ucs& a, const cam16_ucs& b) {
  const double dj = static_cast<double>(a.Jp) - static_cast<double>(b.Jp);
  const double da = static_cast<double>(a.ap) - static_cast<double>(b.ap);
  const double db = static_cast<double>(a.bp) - static_cast<double>(b.bp);
  return static_cast<float>(std::sqrt(dj * dj + da * da + db * db));
}

}  // namespace colorcpp::algorithms::appearance
