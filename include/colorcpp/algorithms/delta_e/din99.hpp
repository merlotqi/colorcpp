/**
 * @file din99.hpp
 * @brief DIN99 color difference metric.
 *
 * DIN99 (DIN 6176) is a German standard for color difference measurement.
 * It applies a series of non-linear transforms to CIELAB to produce a more
 * perceptually uniform color space, then computes Euclidean distance in that space.
 *
 * The transformation includes:
 * 1. LAB to LCh conversion
 * 2. Logarithmic chroma compression
 * 3. Hue rotation to reduce blue-region non-uniformity
 *
 * @see https://en.wikipedia.org/wiki/DIN99
 */

#pragma once

#include <cmath>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::delta_e {

namespace details {
/**
 * @brief Transform CIELAB to DIN99 coordinates.
 *
 * Implements DIN 6176:2001 standard transformation.
 * Reference: Witt (1999) "Colour metrics in technology and business"
 *
 * @param L CIELAB L* (0-100).
 * @param a CIELAB a*.
 * @param b CIELAB b*.
 * @param k_L Lightness scaling factor (default: 1.0).
 * @param k_E Overall scaling factor (default: 1.0).
 * @param L99 Output DIN99 L99.
 * @param a99 Output DIN99 a99.
 * @param b99 Output DIN99 b99.
 */
inline void lab_to_din99(double L, double a, double b, double k_L, double k_E, double& L99, double& a99, double& b99) {
  constexpr double kPi = 3.14159265358979323846;
  constexpr double Deg16 = 16.0 * kPi / 180.0;

  // Step 1: Compute L99 with lightness scaling
  L99 = k_E * 105.51 * std::log(1.0 + 0.0158 * L) / k_L;

  // Step 2: Compute CIELAB chroma
  double C = std::sqrt(a * a + b * b);

  // Step 3: Compute hue
  double h = 0.0;
  if (b != 0.0 || a != 0.0) {
    h = std::atan2(b, a);
    if (h < 0.0) h += 2.0 * kPi;
  }

  // Step 4: Apply logarithmic chroma compression
  double C99 = std::log(1.0 + 0.045 * C) / 0.045;

  // Step 5: Compute DIN99 hue with rotation
  double h99 = h - Deg16;

  // Step 6: Convert to DIN99 a99, b99 with overall scaling
  a99 = k_E * C99 * std::cos(h99);
  b99 = k_E * C99 * std::sin(h99);
}

}  // namespace details

/**
 * @brief DIN99 color difference (Euclidean distance in DIN99 space).
 *
 * @param a First color.
 * @param b Second color.
 * @param k_L Lightness scaling factor (default: 1.0).
 * @param k_E Overall scaling factor (default: 1.0).
 * @return ΔE_DIN99 value.
 */
template <typename ColorA, typename ColorB>
float delta_e_din99(const ColorA& a, const ColorB& b, float k_L = 1.0f, float k_E = 1.0f) {
  using namespace operations::conversion;

  auto la = color_cast<core::cielab_t>(a);
  auto lb = color_cast<core::cielab_t>(b);

  const double L1 = la.template get_index<0>();
  const double a1 = la.template get_index<1>();
  const double b1 = la.template get_index<2>();
  const double L2 = lb.template get_index<0>();
  const double a2 = lb.template get_index<1>();
  const double b2 = lb.template get_index<2>();

  // Transform both colors to DIN99
  double L99_1, a99_1, b99_1;
  double L99_2, a99_2, b99_2;

  details::lab_to_din99(L1, a1, b1, k_L, k_E, L99_1, a99_1, b99_1);
  details::lab_to_din99(L2, a2, b2, k_L, k_E, L99_2, a99_2, b99_2);

  // Compute Euclidean distance in DIN99 space
  const double dL99 = L99_2 - L99_1;
  const double da99 = a99_2 - a99_1;
  const double db99 = b99_2 - b99_1;

  return static_cast<float>(k_E * std::sqrt(dL99 * dL99 + da99 * da99 + db99 * db99));
}

}  // namespace colorcpp::algorithms::delta_e
