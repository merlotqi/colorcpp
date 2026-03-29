/**
 * @file lab.hpp
 * @brief CIELAB and CIELCH conversion functions.
 */

#pragma once

#include <cmath>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion/helpers.hpp>

namespace colorcpp::operations::conversion::details {

template <typename To, typename From>
constexpr To linear_rgb_to_lab(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());

  float X = 0.4124564f * r + 0.3575761f * g + 0.1804375f * b;
  float Y = 0.2126729f * r + 0.7151522f * g + 0.0721750f * b;
  float Z = 0.0193339f * r + 0.1191920f * g + 0.9503041f * b;

  constexpr float Xn = 0.95047f, Yn = 1.00000f, Zn = 1.08883f;
  float fx = X / Xn, fy = Y / Yn, fz = Z / Zn;

  constexpr float delta = 6.0f / 29.0f;
  constexpr float delta3 = delta * delta * delta;
  constexpr float k = 1.0f / (3.0f * delta * delta);
  constexpr float b0 = 4.0f / 29.0f;

  auto f = [=](float t) noexcept { return t > delta3 ? std::cbrt(t) : k * t + b0; };

  float fxv = f(fx), fyv = f(fy), fzv = f(fz);

  float L = 116.0f * fyv - 16.0f;
  float A = 500.0f * (fxv - fyv);
  float B = 200.0f * (fyv - fzv);

  return pack_to<To>(from_value<To, 0>(L), from_value<To, 1>(A), from_value<To, 2>(B));
}

template <typename To, typename From>
constexpr To lab_to_linear_rgb(const From& src) {
  float L = static_cast<float>(src.template get_index<0>());
  float A = static_cast<float>(src.template get_index<1>());
  float B = static_cast<float>(src.template get_index<2>());

  float fy = (L + 16.0f) / 116.0f;
  float fx = A / 500.0f + fy;
  float fz = fy - B / 200.0f;

  constexpr float delta = 6.0f / 29.0f;
  constexpr float delta2 = delta * delta;
  auto g = [=](float f) noexcept { return f > delta ? f * f * f : 3.0f * delta2 * (f - 4.0f / 29.0f); };

  constexpr float Xn = 0.95047f, Yn = 1.00000f, Zn = 1.08883f;
  float X = Xn * g(fx);
  float Y = Yn * g(fy);
  float Z = Zn * g(fz);

  float r = 3.2404542f * X - 1.5371385f * Y - 0.4985314f * Z;
  float gv = -0.9692660f * X + 1.8760108f * Y + 0.0415560f * Z;
  float bv = 0.0556434f * X - 0.2040259f * Y + 1.0572252f * Z;

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(gv), from_unit<To, 2>(bv), from_unit<To, 3>(1.0f));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(gv), from_unit<To, 2>(bv));
}

template <typename To, typename From>
constexpr To lab_to_lch(const From& src) {
  float L = static_cast<float>(src.template get_index<0>());
  float a = static_cast<float>(src.template get_index<1>());
  float b = static_cast<float>(src.template get_index<2>());

  float C = std::sqrt(a * a + b * b);
  float H = std::atan2(b, a) * (180.0f / 3.14159265358979323846f);
  if (H < 0.0f) H += 360.0f;

  return pack_to<To>(from_value<To, 0>(L), from_value<To, 1>(C), from_value<To, 2>(H));
}

template <typename To, typename From>
constexpr To lch_to_lab(const From& src) {
  float L = static_cast<float>(src.template get_index<0>());
  float C = static_cast<float>(src.template get_index<1>());
  float H = static_cast<float>(src.template get_index<2>());

  float H_rad = H * (3.14159265358979323846f / 180.0f);
  float a = C * std::cos(H_rad);
  float b = C * std::sin(H_rad);

  return pack_to<To>(from_value<To, 0>(L), from_value<To, 1>(a), from_value<To, 2>(b));
}

template <typename To, typename From>
constexpr To xyz_to_lab(const From& src) {
  float X = static_cast<float>(src.template get_index<0>());
  float Y = static_cast<float>(src.template get_index<1>());
  float Z = static_cast<float>(src.template get_index<2>());

  constexpr float Xn = 0.95047f, Yn = 1.00000f, Zn = 1.08883f;
  constexpr float delta = 6.0f / 29.0f;
  constexpr float delta3 = delta * delta * delta;
  constexpr float k = 1.0f / (3.0f * delta * delta);
  constexpr float b0 = 4.0f / 29.0f;

  auto f = [=](float t) noexcept { return t > delta3 ? std::cbrt(t) : k * t + b0; };

  float fxv = f(X / Xn), fyv = f(Y / Yn), fzv = f(Z / Zn);

  float L = 116.0f * fyv - 16.0f;
  float A = 500.0f * (fxv - fyv);
  float B = 200.0f * (fyv - fzv);

  return pack_to<To>(from_value<To, 0>(L), from_value<To, 1>(A), from_value<To, 2>(B));
}

template <typename To, typename From>
constexpr To lab_to_xyz(const From& src) {
  float L = static_cast<float>(src.template get_index<0>());
  float A = static_cast<float>(src.template get_index<1>());
  float B = static_cast<float>(src.template get_index<2>());

  float fy = (L + 16.0f) / 116.0f;
  float fx = A / 500.0f + fy;
  float fz = fy - B / 200.0f;

  constexpr float delta = 6.0f / 29.0f;
  constexpr float delta2 = delta * delta;
  auto g = [=](float f) noexcept { return f > delta ? f * f * f : 3.0f * delta2 * (f - 4.0f / 29.0f); };

  constexpr float Xn = 0.95047f, Yn = 1.00000f, Zn = 1.08883f;
  float X = Xn * g(fx);
  float Y = Yn * g(fy);
  float Z = Zn * g(fz);

  return pack_to<To>(from_value<To, 0>(X), from_value<To, 1>(Y), from_value<To, 2>(Z));
}

}  // namespace colorcpp::operations::conversion::details
