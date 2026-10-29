/**
 * @file jzazbz.hpp
 * @brief Jzazbz (Safdar et al. 2017) conversion functions.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/jzazbz.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

namespace jzazbz_constants {

// Pre-adaptation factors (Safdar 2017).
inline constexpr float b = 1.15f;
inline constexpr float g = 0.66f;
// Jz tone-mapping constants.
inline constexpr float d = -0.56f;
inline constexpr float d0 = 1.6295499532821566e-11f;
// Modified SMPTE ST 2084 PQ: m2 re-optimized for Jzazbz (m2 = 1.7 * 2523 / 32).
inline constexpr float m1 = 2610.0f / 16384.0f;
inline constexpr float m2 = 1.7f * 2523.0f / 32.0f;
inline constexpr float c1 = 3424.0f / 4096.0f;
inline constexpr float c2 = 2413.0f / 128.0f;
inline constexpr float c3 = 2392.0f / 128.0f;
inline constexpr float peak = 10000.0f;

// XYZ' -> LMS cone matrix.
inline constexpr float M1[9] = {0.41478972f, 0.579999f,   0.0146480f, -0.2015100f, 1.120649f,
                                0.0531008f,  -0.0166008f, 0.264800f,  0.6684799f};
// Inverse of M1 (adjugate, validated by round-trip).
inline constexpr float M1_inv[9] = {1.92422644f,  -1.00479231f, 0.03765140f,  0.35031676f, 0.72648119f,
                                    -0.06538442f, -0.09098281f, -0.31272829f, 1.52276656f};
// LMS' -> Izazbz matrix (Safdar 2017).
inline constexpr float M2[9] = {0.5f, 0.5f, 0.0f, 3.524f, -4.066708f, 0.542708f, 0.199076f, 1.096799f, -1.295875f};
// Inverse of M2 (adjugate, validated by round-trip).
inline constexpr float M2_inv[9] = {1.0f,         0.13860504f, 0.05804732f,  1.0f,        -0.13860504f,
                                    -0.05804732f, 1.0f,        -0.09601924f, -0.81189190f};

// PQ inverse-EOTF (linear luminance -> code value); negative input clamps to 0.
inline float pq_encode(float y) {
  y = std::pow(std::max(y / peak, 0.0f), m1);
  return std::pow((c1 + c2 * y) / (1.0f + c3 * y), m2);
}

// PQ EOTF (code value -> linear luminance).
inline float pq_decode(float p) {
  p = std::pow(std::max(p, 0.0f), 1.0f / m2);
  float num = std::max(p - c1, 0.0f);
  float den = c2 - c3 * p;
  return (den > 0.0f) ? peak * std::pow(num / den, 1.0f / m1) : 0.0f;
}

}  // namespace jzazbz_constants

/**
 * @brief Convert XYZ (D65) tristimulus values to Jzazbz.
 *
 * Implements Safdar et al. 2017: pre-adaptation (b = 1.15, g = 0.66), HPE-like
 * cone matrix, modified PQ encoding (m2 = 1.7 * 2523 / 32), Izazbz matrix, and
 * the Jz tone curve. Black yields (0, 0, 0).
 *
 * @tparam To Target Jzazbz color type (jzazbz_t)
 * @tparam From Source XYZ color type (xyz_t)
 * @param src Input XYZ color (D65)
 * @return Converted Jzazbz color
 */
template <typename To, typename From>
constexpr To xyz_to_jzazbz(const From& src) {
  using namespace jzazbz_constants;

  float X = static_cast<float>(src.template get_index<0>());
  float Y = static_cast<float>(src.template get_index<1>());
  float Z = static_cast<float>(src.template get_index<2>());

  float Xp = b * X - (b - 1.0f) * Z;
  float Yp = g * Y - (g - 1.0f) * X;
  float Zp = Z;

  float lms[3] = {0.0f, 0.0f, 0.0f};
  for (int i = 0; i < 3; ++i) {
    lms[i] = pq_encode(M1[i * 3 + 0] * Xp + M1[i * 3 + 1] * Yp + M1[i * 3 + 2] * Zp);
  }

  float Iz = M2[0] * lms[0] + M2[1] * lms[1] + M2[2] * lms[2];
  float az = M2[3] * lms[0] + M2[4] * lms[1] + M2[5] * lms[2];
  float bz = M2[6] * lms[0] + M2[7] * lms[1] + M2[8] * lms[2];

  float Jz = (1.0f + d) * Iz / (1.0f + d * Iz) - d0;

  return pack_to<To>(from_value<To, 0>(Jz), from_value<To, 1>(az), from_value<To, 2>(bz));
}

/**
 * @brief Convert Jzazbz to XYZ (D65) tristimulus values.
 *
 * Inverse of @ref xyz_to_jzazbz (Safdar et al. 2017).
 *
 * @tparam To Target XYZ color type (xyz_t)
 * @tparam From Source Jzazbz color type (jzazbz_t)
 * @param src Input Jzazbz color
 * @return Converted XYZ color (D65)
 */
template <typename To, typename From>
constexpr To jzazbz_to_xyz(const From& src) {
  using namespace jzazbz_constants;

  float Jz = static_cast<float>(src.template get_index<0>());
  float az = static_cast<float>(src.template get_index<1>());
  float bz = static_cast<float>(src.template get_index<2>());

  float Jz0 = Jz + d0;
  float Iz = Jz0 / (1.0f + d - d * Jz0);

  float lms_p[3] = {0.0f, 0.0f, 0.0f};
  for (int i = 0; i < 3; ++i) {
    lms_p[i] = pq_decode(M2_inv[i * 3 + 0] * Iz + M2_inv[i * 3 + 1] * az + M2_inv[i * 3 + 2] * bz);
  }

  float Xp = M1_inv[0] * lms_p[0] + M1_inv[1] * lms_p[1] + M1_inv[2] * lms_p[2];
  float Yp = M1_inv[3] * lms_p[0] + M1_inv[4] * lms_p[1] + M1_inv[5] * lms_p[2];
  float Zp = M1_inv[6] * lms_p[0] + M1_inv[7] * lms_p[1] + M1_inv[8] * lms_p[2];

  float X = (Xp + (b - 1.0f) * Zp) / b;
  float Y = (Yp + (g - 1.0f) * X) / g;

  return pack_to<To>(from_value<To, 0>(X), from_value<To, 1>(Y), from_value<To, 2>(Zp));
}

}  // namespace colorcpp::operations::conversion::details
