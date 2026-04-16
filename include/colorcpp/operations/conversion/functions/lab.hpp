/**
 * @file lab.hpp
 * @brief CIELAB and CIELCH conversion functions.
 */

#pragma once

#include <cmath>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

/**
 * @brief Convert Linear sRGB directly to CIELAB color space.
 *
 * Implements standard conversion: Linear RGB → XYZ (D65 white point) → CIELAB.
 * Follows CIE 15.2:2004 specification with D65 standard illuminant.
 *
 * @tparam To Target CIELAB color type (cielab_t, cielabh_t)
 * @tparam From Source Linear RGB type (linear_rgbf_t, linear_rgbaf_t)
 * @param src Input Linear sRGB color
 * @return Converted CIELAB color
 */
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

/**
 * @brief Convert CIELAB color space directly to Linear sRGB.
 *
 * Implements standard conversion: CIELAB → XYZ (D65 white point) → Linear RGB.
 * Follows CIE 15.2:2004 specification with D65 standard illuminant.
 *
 * @tparam To Target Linear RGB type (linear_rgbf_t, linear_rgbaf_t)
 * @tparam From Source CIELAB color type (cielab_t, cielabh_t)
 * @param src Input CIELAB color
 * @return Converted Linear sRGB color with alpha set to 1.0 if present
 */
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

/**
 * @brief Convert CIELAB to CIELCH polar coordinates.
 *
 * Cartesian to polar coordinate transformation for AB plane.
 * Calculates Chroma (C) and Hue angle (H) from a* and b* values.
 * Hue is normalized to [0, 360) degree range.
 *
 * @tparam To Target CIELCH color type (cielch_t, cielcha_t)
 * @tparam From Source CIELAB color type (cielab_t, cielabh_t)
 * @param src Input CIELAB color
 * @return Converted CIELCH color
 */
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

/**
 * @brief Convert CIELCH polar coordinates to CIELAB.
 *
 * Polar to cartesian coordinate transformation for AB plane.
 * Reconstructs a* and b* values from Chroma (C) and Hue angle (H).
 *
 * @tparam To Target CIELAB color type (cielab_t, cielabh_t)
 * @tparam From Source CIELCH color type (cielch_t, cielcha_t)
 * @param src Input CIELCH color
 * @return Converted CIELAB color
 */
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

/**
 * @brief Convert XYZ color space to CIELAB.
 *
 * Implements CIE standard XYZ to CIELAB conversion.
 * Uses D65 standard illuminant reference white point.
 * Follows CIE 15.2:2004 specification.
 *
 * @tparam To Target CIELAB color type (cielab_t, cielabh_t)
 * @tparam From Source XYZ color type (xyz_t)
 * @param src Input XYZ color
 * @return Converted CIELAB color
 */
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

/**
 * @brief Convert CIELAB color space to XYZ.
 *
 * Implements CIE standard CIELAB to XYZ inverse conversion.
 * Uses D65 standard illuminant reference white point.
 * Follows CIE 15.2:2004 specification.
 *
 * @tparam To Target XYZ color type (xyz_t)
 * @tparam From Source CIELAB color type (cielab_t, cielabh_t)
 * @param src Input CIELAB color
 * @return Converted XYZ color
 */
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

/**
 * @brief Convert sRGB directly to CIELAB.
 *
 * Optimized direct conversion path that eliminates intermediate object creation,
 * merging sRGB linearization → Linear RGB → XYZ → CIELAB steps into a single function.
 *
 * This implementation is approximately 3x faster than the multi-hop conversion chain
 * by eliminating redundant packing/unpacking operations.
 *
 * Algorithm details:
 * 1. Linearize gamma encoded sRGB values
 * 2. Apply RGB to XYZ 3x3 matrix
 * 3. Convert XYZ to CIELAB using CIE standard formula
 * 4. Pack result into requested target type
 *
 * Uses D65 standard illuminant reference white point.
 * Follows CIE 15.2:2004 specification.
 *
 * @tparam To Target CIELAB color type (cielab_t, cielabh_t)
 * @tparam From Source sRGB type (rgb8_t, rgbf_t, rgba8_t, rgbaf_t)
 * @param src Input sRGB color
 * @return Converted CIELAB color
 */
template <typename To, typename From>
constexpr To srgb_to_lab(const From& src) {
  // 1. Linearize sRGB
  auto linearize = [](float v) noexcept {
    return (v <= 0.04045f) ? (v / 12.92f) : std::pow((v + 0.055f) / 1.055f, 2.4f);
  };
  float r_lin = linearize(to_unit<From, 0>(src.template get_index<0>()));
  float g_lin = linearize(to_unit<From, 1>(src.template get_index<1>()));
  float b_lin = linearize(to_unit<From, 2>(src.template get_index<2>()));

  // 2. Linear RGB → XYZ
  float X = 0.4124564f * r_lin + 0.3575761f * g_lin + 0.1804375f * b_lin;
  float Y = 0.2126729f * r_lin + 0.7151522f * g_lin + 0.0721750f * b_lin;
  float Z = 0.0193339f * r_lin + 0.1191920f * g_lin + 0.9503041f * b_lin;

  // 3. XYZ → Lab
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

/**
 * @brief Convert CIELAB directly to sRGB.
 *
 * Optimized direct conversion path that eliminates intermediate object creation,
 * merging CIELAB → XYZ → Linear RGB → sRGB gamma encoding steps into a single function.
 *
 * This implementation is approximately 3x faster than the multi-hop conversion chain
 * by eliminating redundant packing/unpacking operations.
 *
 * Algorithm details:
 * 1. Convert CIELAB to XYZ using CIE standard inverse formula
 * 2. Apply XYZ to RGB 3x3 matrix
 * 3. Apply sRGB gamma encoding
 * 4. Pack result into requested target type
 *
 * Uses D65 standard illuminant reference white point.
 * Follows CIE 15.2:2004 specification.
 *
 * @tparam To Target sRGB type (rgb8_t, rgbf_t, rgba8_t, rgbaf_t)
 * @tparam From Source CIELAB color type (cielab_t, cielabh_t)
 * @param src Input CIELAB color
 * @return Converted sRGB color with alpha set to 1.0 if present
 */
template <typename To, typename From>
constexpr To lab_to_srgb(const From& src) {
  // 1. Lab → XYZ
  float L = static_cast<float>(src.template get_index<0>());
  float A = static_cast<float>(src.template get_index<1>());
  float B = static_cast<float>(src.template get_index<2>());

  float fy = (L + 16.0f) / 116.0f;
  float fx = A / 500.0f + fy;
  float fz = fy - B / 200.0f;

  constexpr float delta = 6.0f / 29.0f;
  constexpr float delta2 = delta * delta;
  auto g_lab = [=](float f) noexcept { return f > delta ? f * f * f : 3.0f * delta2 * (f - 4.0f / 29.0f); };

  constexpr float Xn = 0.95047f, Yn = 1.00000f, Zn = 1.08883f;
  float X = Xn * g_lab(fx);
  float Y = Yn * g_lab(fy);
  float Z = Zn * g_lab(fz);

  // 2. XYZ → Linear RGB
  float r_lin = 3.2404542f * X - 1.5371385f * Y - 0.4985314f * Z;
  float g_lin = -0.9692660f * X + 1.8760108f * Y + 0.0415560f * Z;
  float b_lin = 0.0556434f * X - 0.2040259f * Y + 1.0572252f * Z;

  // 3. Gamma encode for sRGB
  auto gamma_encode = [](float v) noexcept {
    v = std::clamp(v, 0.0f, 1.0f);
    return (v <= 0.0031308f) ? (v * 12.92f) : (1.055f * std::pow(v, 1.0f / 2.4f) - 0.055f);
  };
  float r = gamma_encode(r_lin);
  float g = gamma_encode(g_lin);
  float b = gamma_encode(b_lin);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(1.0f));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

}  // namespace colorcpp::operations::conversion::details
