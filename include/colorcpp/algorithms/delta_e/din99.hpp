/**
 * @file din99.hpp
 * @brief DIN99 color difference metric.
 *
 * DIN99 (DIN 6176) is a German standard for color difference measurement.
 * It applies a published non-linear transform from CIELAB into DIN99 coordinates
 * before computing Euclidean distance in that space.
 *
 * The transformation includes:
 * 1. Lightness compression to L99
 * 2. 16-degree rotation into intermediate e/f axes
 * 3. 0.7 compression on the rotated blue-yellow axis
 * 4. Logarithmic chroma compression in the rotated plane
 *
 * The final distance applies k_L to the lightness delta and k_E as an inverse
 * scale on the Euclidean norm.
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
 * Implements the published DIN99 transform:
 * - lightness compression to L99
 * - 16-degree rotation into intermediate e/f axes
 * - 0.7 compression on the rotated blue-yellow axis
 * - logarithmic chroma compression before Euclidean distance in DIN99 space
 *
 * @param L CIELAB L* (0-100).
 * @param a CIELAB a*.
 * @param b CIELAB b*.
 * @param L99 Output DIN99 L99.
 * @param a99 Output DIN99 a99.
 * @param b99 Output DIN99 b99.
 */
inline void lab_to_din99(double L, double a, double b, double& L99, double& a99, double& b99) {
  constexpr double kCos16 = 0.9612616959383189;
  constexpr double kSin16 = 0.27563735581699916;

  L99 = 105.51 * std::log(1.0 + 0.0158 * L);

  const double e = a * kCos16 + b * kSin16;
  const double f = 0.7 * (b * kCos16 - a * kSin16);
  const double G = std::sqrt(e * e + f * f);

  if (G == 0.0) {
    a99 = 0.0;
    b99 = 0.0;
    return;
  }

  const double C99 = std::log(1.0 + 0.045 * G) / 0.045;
  const double scale = C99 / G;
  a99 = scale * e;
  b99 = scale * f;
}

}  // namespace details

/**
 * @brief DIN99 color difference (Euclidean distance in DIN99 space).
 *
 * @param a First color.
 * @param b Second color.
 * @param k_L Positive lightness scaling factor. Non-positive values fall back to 1.0.
 * @param k_E Positive inverse scaling factor for the final norm. Non-positive values fall back to 1.0.
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

  double L99_1, a99_1, b99_1;
  double L99_2, a99_2, b99_2;

  details::lab_to_din99(L1, a1, b1, L99_1, a99_1, b99_1);
  details::lab_to_din99(L2, a2, b2, L99_2, a99_2, b99_2);

  const double safe_kL = k_L > 0.0f ? static_cast<double>(k_L) : 1.0;
  const double safe_kE = k_E > 0.0f ? static_cast<double>(k_E) : 1.0;

  const double dL99 = (L99_2 - L99_1) / safe_kL;
  const double da99 = a99_2 - a99_1;
  const double db99 = b99_2 - b99_1;

  return static_cast<float>((1.0 / safe_kE) * std::sqrt(dL99 * dL99 + da99 * da99 + db99 * db99));
}

}  // namespace colorcpp::algorithms::delta_e
