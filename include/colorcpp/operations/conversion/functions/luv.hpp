/**
 * @file luv.hpp
 * @brief CIE L*u*v* conversion functions (D65 reference white).
 */

#pragma once

#include <cmath>
#include <colorcpp/core/luv.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

/**
 * @brief Convert XYZ tristimulus values to CIE L*u*v*.
 *
 * Implements CIE 15:2004 / ISO 11664-5 using the CIE 1976 u'v' diagram
 * (u' = 4X/(X+15Y+3Z), v' = 9Y/(X+15Y+3Z)) and the D65 reference white
 * (0.95047, 1.0, 1.08883). Black yields L* = u* = v* = 0 without NaN.
 *
 * @tparam To Target Luv color type (luv_t)
 * @tparam From Source XYZ color type (xyz_t)
 * @param src Input XYZ color
 * @return Converted Luv color
 */
template <typename To, typename From>
constexpr To xyz_to_luv(const From& src) {
  float X = static_cast<float>(src.template get_index<0>());
  float Y = static_cast<float>(src.template get_index<1>());
  float Z = static_cast<float>(src.template get_index<2>());

  constexpr float Xn = 0.95047f, Yn = 1.00000f, Zn = 1.08883f;
  constexpr float eps = 216.0f / 24389.0f;   // (6/29)^3
  constexpr float kappa = 24389.0f / 27.0f;  // (29/3)^3

  float L = (Y > eps) ? 116.0f * std::cbrt(Y) - 16.0f : kappa * Y;

  float u_prime = 0.0f, v_prime = 0.0f;
  float sum = X + 15.0f * Y + 3.0f * Z;
  if (sum != 0.0f) {
    u_prime = 4.0f * X / sum;
    v_prime = 9.0f * Y / sum;
  }

  float sum_n = Xn + 15.0f * Yn + 3.0f * Zn;
  float un_prime = 4.0f * Xn / sum_n;
  float vn_prime = 9.0f * Yn / sum_n;

  float u = 13.0f * L * (u_prime - un_prime);
  float v = 13.0f * L * (v_prime - vn_prime);

  return pack_to<To>(from_value<To, 0>(L), from_value<To, 1>(u), from_value<To, 2>(v));
}

/**
 * @brief Convert CIE L*u*v* to XYZ tristimulus values.
 *
 * Implements the inverse CIE 15:2004 / ISO 11664-5 conversion using the D65
 * reference white. L* = 0 yields XYZ = (0, 0, 0).
 *
 * @tparam To Target XYZ color type (xyz_t)
 * @tparam From Source Luv color type (luv_t)
 * @param src Input Luv color
 * @return Converted XYZ color
 */
template <typename To, typename From>
constexpr To luv_to_xyz(const From& src) {
  float L = static_cast<float>(src.template get_index<0>());
  float u = static_cast<float>(src.template get_index<1>());
  float v = static_cast<float>(src.template get_index<2>());

  if (L == 0.0f) {
    return pack_to<To>(from_value<To, 0>(0.0f), from_value<To, 1>(0.0f), from_value<To, 2>(0.0f));
  }

  constexpr float Xn = 0.95047f, Yn = 1.00000f, Zn = 1.08883f;
  constexpr float kappa = 24389.0f / 27.0f;  // (29/3)^3

  float Y = (L > 8.0f) ? ((L + 16.0f) / 116.0f) * ((L + 16.0f) / 116.0f) * ((L + 16.0f) / 116.0f) : L / kappa;

  float sum_n = Xn + 15.0f * Yn + 3.0f * Zn;
  float un_prime = 4.0f * Xn / sum_n;
  float vn_prime = 9.0f * Yn / sum_n;

  float u_prime = u / (13.0f * L) + un_prime;
  float v_prime = v / (13.0f * L) + vn_prime;

  if (v_prime == 0.0f) {
    return pack_to<To>(from_value<To, 0>(0.0f), from_value<To, 1>(Y), from_value<To, 2>(0.0f));
  }

  float X = Y * 9.0f * u_prime / (4.0f * v_prime);
  float Z = Y * (12.0f - 3.0f * u_prime - 20.0f * v_prime) / (4.0f * v_prime);

  return pack_to<To>(from_value<To, 0>(X), from_value<To, 1>(Y), from_value<To, 2>(Z));
}

}  // namespace colorcpp::operations::conversion::details
