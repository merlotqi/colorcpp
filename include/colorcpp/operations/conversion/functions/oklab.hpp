/**
 * @file oklab.hpp
 * @brief OkLab and OkLCH conversion functions.
 */

#pragma once

#include <cmath>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

template <typename To, typename From>
constexpr To linear_rgb_to_oklab(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());

  float lms_l = 0.4122214708f * r + 0.5363325363f * g + 0.0514459929f * b;
  float lms_m = 0.2119034982f * r + 0.6806995451f * g + 0.1073969566f * b;
  float lms_s = 0.0883024619f * r + 0.2817188376f * g + 0.6299787005f * b;

  auto safe_cbrt = [](float x) noexcept { return x >= 0.0f ? std::cbrt(x) : -std::cbrt(-x); };
  float l_ = safe_cbrt(lms_l);
  float m_ = safe_cbrt(lms_m);
  float s_ = safe_cbrt(lms_s);

  float L = 0.2104542553f * l_ + 0.7936177850f * m_ - 0.0040720468f * s_;
  float A = 1.9779984951f * l_ - 2.4285922050f * m_ + 0.4505937099f * s_;
  float B = 0.0259040371f * l_ + 0.4072456426f * m_ - 0.4331497194f * s_;

  return pack_to<To>(from_value<To, 0>(L), from_value<To, 1>(A), from_value<To, 2>(B));
}

template <typename To, typename From>
constexpr To oklab_to_linear_rgb(const From& src) {
  float L = static_cast<float>(src.template get_index<0>());
  float A = static_cast<float>(src.template get_index<1>());
  float B = static_cast<float>(src.template get_index<2>());

  float l_ = L + 0.3939205158f * A + 0.4003836363f * B;
  float m_ = L - 0.1048460944f * A - 0.1184695156f * B;
  float s_ = L - 0.0750179025f * A - 2.3961106171f * B;

  float lms_l = l_ * l_ * l_;
  float lms_m = m_ * m_ * m_;
  float lms_s = s_ * s_ * s_;

  float r = 4.0767416621f * lms_l - 3.3077115913f * lms_m + 0.2309699292f * lms_s;
  float g = -1.2684380046f * lms_l + 2.6097574011f * lms_m - 0.3413193965f * lms_s;
  float b = -0.0041960863f * lms_l - 0.7034186147f * lms_m + 1.7076147010f * lms_s;

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(1.0f));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
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
  float X = static_cast<float>(src.template get_index<0>());
  float Y = static_cast<float>(src.template get_index<1>());
  float Z = static_cast<float>(src.template get_index<2>());

  float lms_l = 0.8189330101f * X + 0.3618667424f * Y - 0.1288597137f * Z;
  float lms_m = 0.0329845436f * X + 0.9293118715f * Y + 0.0361456387f * Z;
  float lms_s = 0.0482003018f * X + 0.2643662691f * Y + 0.6338517070f * Z;

  auto safe_cbrt = [](float x) noexcept { return x >= 0.0f ? std::cbrt(x) : -std::cbrt(-x); };
  float l_ = safe_cbrt(lms_l);
  float m_ = safe_cbrt(lms_m);
  float s_ = safe_cbrt(lms_s);

  float L = 0.2104542553f * l_ + 0.7936177850f * m_ - 0.0040720468f * s_;
  float A = 1.9779984951f * l_ - 2.4285922050f * m_ + 0.4505937099f * s_;
  float B = 0.0259040371f * l_ + 0.4072456426f * m_ - 0.4331497194f * s_;

  return pack_to<To>(from_value<To, 0>(L), from_value<To, 1>(A), from_value<To, 2>(B));
}

template <typename To, typename From>
constexpr To oklab_to_xyz(const From& src) {
  float L = static_cast<float>(src.template get_index<0>());
  float A = static_cast<float>(src.template get_index<1>());
  float B = static_cast<float>(src.template get_index<2>());

  float l_ = L + 0.3939205158f * A + 0.4003836363f * B;
  float m_ = L - 0.1048460944f * A - 0.1184695156f * B;
  float s_ = L - 0.0750179025f * A - 2.3961106171f * B;

  float lms_l = l_ * l_ * l_;
  float lms_m = m_ * m_ * m_;
  float lms_s = s_ * s_ * s_;

  float X = 1.2270138511f * lms_l - 0.5577999807f * lms_m + 0.2812561490f * lms_s;
  float Y = -0.0405801784f * lms_l + 1.1122568696f * lms_m - 0.0716766787f * lms_s;
  float Z = -0.0763812845f * lms_l - 0.4214819784f * lms_m + 1.5861632204f * lms_s;

  return pack_to<To>(from_value<To, 0>(X), from_value<To, 1>(Y), from_value<To, 2>(Z));
}

/**
 * @brief CIELAB to OkLab (direct conversion).
 *
 * Combines Lab → XYZ → OkLab into a single function.
 * The OkLab XYZ→OkLab matrix operates on cbrt(XYZ), so we apply
 * the Lab → XYZ inverse transform, then feed directly into the
 * OkLab LMS pipeline.
 */
template <typename To, typename From>
constexpr To cielab_to_oklab(const From& src) {
  // 1. CIELAB → XYZ
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

  // 2. XYZ → OkLab
  float lms_l = 0.8189330101f * X + 0.3618667424f * Y - 0.1288597137f * Z;
  float lms_m = 0.0329845436f * X + 0.9293118715f * Y + 0.0361456387f * Z;
  float lms_s = 0.0482003018f * X + 0.2643662691f * Y + 0.6338517070f * Z;

  auto safe_cbrt = [](float x) noexcept { return x >= 0.0f ? std::cbrt(x) : -std::cbrt(-x); };
  float l_ = safe_cbrt(lms_l);
  float m_ = safe_cbrt(lms_m);
  float s_ = safe_cbrt(lms_s);

  float OkL = 0.2104542553f * l_ + 0.7936177850f * m_ - 0.0040720468f * s_;
  float OkA = 1.9779984951f * l_ - 2.4285922050f * m_ + 0.4505937099f * s_;
  float OkB = 0.0259040371f * l_ + 0.4072456426f * m_ - 0.4331497194f * s_;

  return pack_to<To>(from_value<To, 0>(OkL), from_value<To, 1>(OkA), from_value<To, 2>(OkB));
}

/**
 * @brief OkLab to CIELAB (direct conversion).
 *
 * Combines OkLab → XYZ → Lab into a single function.
 * The OkLab → XYZ inverse uses LMS = l_^3, then applies the OkLab → XYZ matrix.
 * We then feed XYZ directly into the Lab transform.
 */
template <typename To, typename From>
constexpr To oklab_to_cielab(const From& src) {
  // 1. OkLab → XYZ
  float L = static_cast<float>(src.template get_index<0>());
  float A = static_cast<float>(src.template get_index<1>());
  float B = static_cast<float>(src.template get_index<2>());

  float l_ = L + 0.3939205158f * A + 0.4003836363f * B;
  float m_ = L - 0.1048460944f * A - 0.1184695156f * B;
  float s_ = L - 0.0750179025f * A - 2.3961106171f * B;

  float lms_l = l_ * l_ * l_;
  float lms_m = m_ * m_ * m_;
  float lms_s = s_ * s_ * s_;

  float X = 1.2270138511f * lms_l - 0.5577999807f * lms_m + 0.2812561490f * lms_s;
  float Y = -0.0405801784f * lms_l + 1.1122568696f * lms_m - 0.0716766787f * lms_s;
  float Z = -0.0763812845f * lms_l - 0.4214819784f * lms_m + 1.5861632204f * lms_s;

  // 2. XYZ → Lab
  constexpr float Xn = 0.95047f, Yn = 1.00000f, Zn = 1.08883f;
  float fx = X / Xn, fy = Y / Yn, fz = Z / Zn;

  constexpr float delta = 6.0f / 29.0f;
  constexpr float delta3 = delta * delta * delta;
  constexpr float k = 1.0f / (3.0f * delta * delta);
  constexpr float b0 = 4.0f / 29.0f;

  auto f = [=](float t) noexcept { return t > delta3 ? std::cbrt(t) : k * t + b0; };

  float fxv = f(fx), fyv = f(fy), fzv = f(fz);

  float LabL = 116.0f * fyv - 16.0f;
  float LabA = 500.0f * (fxv - fyv);
  float LabB = 200.0f * (fyv - fzv);

  return pack_to<To>(from_value<To, 0>(LabL), from_value<To, 1>(LabA), from_value<To, 2>(LabB));
}

}  // namespace colorcpp::operations::conversion::details
