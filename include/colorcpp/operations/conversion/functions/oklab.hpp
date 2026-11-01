/**
 * @file oklab.hpp
 * @brief OkLab and OkLCH conversion functions.
 */

#pragma once

#include <array>
#include <cmath>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

// The RGB and XYZ OkLab edges must agree bit-for-bit: graph routing may pick
// either one for what users think is the same color (issue #49 — DI <-> DI
// traffic now always goes through the XYZ edge). Both are therefore built on
// the two unclamped cores below. The cores never apply [0,1] unit clamping:
// hub conversions must stay lossless for out-of-gamut colors.

/** @brief linear sRGB (raw, unclamped) -> OkLab core. */
constexpr std::array<float, 3> linear_srgb_to_oklab_raw(float r, float g, float b) {
  float lms_l = 0.4122214708f * r + 0.5363325363f * g + 0.0514459929f * b;
  float lms_m = 0.2119034982f * r + 0.6806995451f * g + 0.1073969566f * b;
  float lms_s = 0.0883024619f * r + 0.2817188376f * g + 0.6299787005f * b;

  auto safe_cbrt = [](float x) noexcept { return x >= 0.0f ? std::cbrt(x) : -std::cbrt(-x); };
  float l_ = safe_cbrt(lms_l);
  float m_ = safe_cbrt(lms_m);
  float s_ = safe_cbrt(lms_s);

  float L = 0.2104542553f * l_ + 0.7936177850f * m_ - 0.0040720468f * s_;
  float A = 1.9779984951f * l_ - 2.4285922050f * m_ + 0.4505937099f * s_;
  float B = 0.0259040371f * l_ + 0.7827717662f * m_ - 0.8086757660f * s_;
  return {L, A, B};
}

/** @brief OkLab -> linear sRGB (raw, unclamped) core. */
constexpr std::array<float, 3> oklab_to_linear_srgb_raw(float L, float A, float B) {
  float l_ = L + 0.3963377774f * A + 0.2158037573f * B;
  float m_ = L - 0.1055613458f * A - 0.0638541728f * B;
  float s_ = L - 0.0894841775f * A - 1.2914855480f * B;

  float lms_l = l_ * l_ * l_;
  float lms_m = m_ * m_ * m_;
  float lms_s = s_ * s_ * s_;

  float r = 4.0767416621f * lms_l - 3.3077115913f * lms_m + 0.2309699292f * lms_s;
  float g = -1.2684380046f * lms_l + 2.6097574011f * lms_m - 0.3413193965f * lms_s;
  float b = -0.0041960863f * lms_l - 0.7034186147f * lms_m + 1.7076147010f * lms_s;
  return {r, g, b};
}

template <typename To, typename From>
constexpr To linear_rgb_to_oklab(const From& src) {
  const float r = to_unit<From, 0>(src.template get_index<0>());
  const float g = to_unit<From, 1>(src.template get_index<1>());
  const float b = to_unit<From, 2>(src.template get_index<2>());

  const auto lab = linear_srgb_to_oklab_raw(r, g, b);
  return pack_to<To>(from_value<To, 0>(lab[0]), from_value<To, 1>(lab[1]), from_value<To, 2>(lab[2]));
}

template <typename To, typename From>
constexpr To oklab_to_linear_rgb(const From& src) {
  const auto rgb = oklab_to_linear_srgb_raw(static_cast<float>(src.template get_index<0>()),
                                            static_cast<float>(src.template get_index<1>()),
                                            static_cast<float>(src.template get_index<2>()));

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(rgb[0]), from_unit<To, 1>(rgb[1]), from_unit<To, 2>(rgb[2]),
                       from_unit<To, 3>(1.0f));
  else
    return pack_to<To>(from_unit<To, 0>(rgb[0]), from_unit<To, 1>(rgb[1]), from_unit<To, 2>(rgb[2]));
}

template <typename To, typename From>
constexpr To oklab_to_oklch(const From& src) {
  float L = static_cast<float>(src.template get_index<0>());
  float a = static_cast<float>(src.template get_index<1>());
  float b = static_cast<float>(src.template get_index<2>());

  float C = std::sqrt(a * a + b * b);
  float H = std::atan2(b, a) * (180.0f / 3.14159265358979323846f);
  if (H < 0.0f) H += 360.0f;

  return pack_to<To>(from_value<To, 0>(L), from_value<To, 1>(C), from_value<To, 2>(H));
}

template <typename To, typename From>
constexpr To oklch_to_oklab(const From& src) {
  float L = static_cast<float>(src.template get_index<0>());
  float C = static_cast<float>(src.template get_index<1>());
  float H = static_cast<float>(src.template get_index<2>());

  float H_rad = H * (3.14159265358979323846f / 180.0f);
  float a = C * std::cos(H_rad);
  float b = C * std::sin(H_rad);

  return pack_to<To>(from_value<To, 0>(L), from_value<To, 1>(a), from_value<To, 2>(b));
}

template <typename To, typename From>
constexpr To xyz_to_oklab(const From& src) {
  const float X = static_cast<float>(src.template get_index<0>());
  const float Y = static_cast<float>(src.template get_index<1>());
  const float Z = static_cast<float>(src.template get_index<2>());

  // XYZ -> linear sRGB (same matrix as xyz_to_linear_rgb in linear_rgb.hpp,
  // but raw — no [0,1] clamp), then the canonical OkLab core above. The
  // previously published XYZ -> LMS matrix disagreed with the RGB-side pair
  // at ~3e-5, which the Lab cube-root amplifies to ~0.02 on dark colors.
  const float r = 3.2404542f * X - 1.5371385f * Y - 0.4985314f * Z;
  const float g = -0.9692660f * X + 1.8760108f * Y + 0.0415560f * Z;
  const float b = 0.0556434f * X - 0.2040259f * Y + 1.0572252f * Z;

  const auto lab = linear_srgb_to_oklab_raw(r, g, b);
  return pack_to<To>(from_value<To, 0>(lab[0]), from_value<To, 1>(lab[1]), from_value<To, 2>(lab[2]));
}

template <typename To, typename From>
constexpr To oklab_to_xyz(const From& src) {
  // Inverse direction of xyz_to_oklab: canonical OkLab -> linear sRGB core,
  // then linear sRGB -> XYZ (same matrix as linear_rgb_to_xyz).
  const auto rgb = oklab_to_linear_srgb_raw(static_cast<float>(src.template get_index<0>()),
                                            static_cast<float>(src.template get_index<1>()),
                                            static_cast<float>(src.template get_index<2>()));
  const float r = rgb[0];
  const float g = rgb[1];
  const float b = rgb[2];

  const float X = 0.4124564f * r + 0.3575761f * g + 0.1804375f * b;
  const float Y = 0.2126729f * r + 0.7151522f * g + 0.0721750f * b;
  const float Z = 0.0193339f * r + 0.1191920f * g + 0.9503041f * b;

  return pack_to<To>(from_value<To, 0>(X), from_value<To, 1>(Y), from_value<To, 2>(Z));
}

}  // namespace colorcpp::operations::conversion::details
