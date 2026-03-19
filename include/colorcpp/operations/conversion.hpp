#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/core/cmyk.hpp>
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/hsv.hpp>
#include <colorcpp/core/linear_rgb.hpp>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/core/xyz.hpp>
#include <tuple>
#include <type_traits>
#include <utility>

namespace colorcpp::operations::conversion {

// Forward declaration required for two-hop dispatch inside color_cast_impl.
template <typename To, typename From>
constexpr To color_cast(const From& src);

namespace details {

template <typename T>
struct extract_model;

template <template <typename...> class Color, typename Model, typename... Args>
struct extract_model<Color<Model, Args...>> {
  using type = Model;
};

template <typename T>
using extract_model_t = typename extract_model<T>::type;

// V is deduced from the call argument (e.g. uint8_t for rgb8, float for rgbf).
// Using a deduced V instead of Color::value_type makes to_unit work correctly
// with heterogeneous-channel models where each channel has its own scalar type.
template <typename Color, std::size_t I, typename V>
constexpr float to_unit(V v) {
  using channel_t = std::tuple_element_t<I, typename Color::channels_tuple>;
  return static_cast<float>(v - channel_t::min) / static_cast<float>(channel_t::max - channel_t::min);
}

// Maps a unit-normalized value u ∈ [0,1] back to channel I's [min, max] range.
// The clamp is applied in unit space to guard against floating-point arithmetic
// producing values like 1.0000001; it does NOT clip out-of-gamut channel values.
// Returns channel I's exact value_type (supports heterogeneous-channel models).
template <typename Color, std::size_t I>
constexpr typename Color::template channel_value_t<I> from_unit(float v) {
  using channel_t = std::tuple_element_t<I, typename Color::channels_tuple>;
  using value_t = typename Color::template channel_value_t<I>;
  v = std::clamp(v, 0.0f, 1.0f);
  float val = v * static_cast<float>(channel_t::max - channel_t::min) + static_cast<float>(channel_t::min);
  if constexpr (std::is_integral_v<value_t>)
    return static_cast<value_t>(std::round(val));
  else
    return static_cast<value_t>(val);
}

// Maps a raw channel value (already in the channel's natural units, possibly signed)
// directly to the target type, clamping to the channel's [min, max].
// Use this instead of from_unit when the value is NOT in [0,1] unit space —
// e.g., when writing Oklab a/b channels that live in [-0.5, 0.5].
template <typename Color, std::size_t I>
constexpr typename Color::template channel_value_t<I> from_value(float v) {
  using channel_t = std::tuple_element_t<I, typename Color::channels_tuple>;
  using value_t = typename Color::template channel_value_t<I>;
  float clamped = std::clamp(v, static_cast<float>(channel_t::min), static_cast<float>(channel_t::max));
  if constexpr (std::is_integral_v<value_t>)
    return static_cast<value_t>(std::round(clamped));
  else
    return static_cast<value_t>(clamped);
}

// Returns the alpha channel as a unit [0,1] value.
// Only called on types where channel 3 is known to be alpha (RGB, HSL, HSV).
// CMYK must NOT use this helper — its channel 3 is K (black), not alpha.
template <typename From>
constexpr float get_src_alpha(const From& src) {
  if constexpr (From::channels >= 4)
    return to_unit<From, 3>(src.template get_index<3>());
  else
    return 1.0f;
}

template <typename To, std::size_t... Is, typename... Args>
constexpr To pack_to_impl(std::index_sequence<Is...>, Args&&... args) {
  auto tp = std::forward_as_tuple(std::forward<Args>(args)...);
  return To{std::get<Is>(std::move(tp))...};
}

template <typename To, typename... Args>
constexpr To pack_to(Args&&... args) {
  if constexpr (sizeof...(args) > To::channels) {
    return pack_to_impl<To>(std::make_index_sequence<To::channels>{}, std::forward<Args>(args)...);
  } else {
    return To{std::forward<Args>(args)...};
  }
}

// model_category maps a model tag to a boolean category flag.
// NOTE: This struct will be redesigned in T-04 (open/closed routing via specializable traits).
// For now, new categories are added here as the library grows.
template <typename ModelTag>
struct model_category {
  // Gamma-encoded sRGB (uint8 or float)
  static constexpr bool is_rgb =
      std::is_same_v<ModelTag, core::rgb::model::rgb8> || std::is_same_v<ModelTag, core::rgb::model::rgba8> ||
      std::is_same_v<ModelTag, core::rgb::model::rgb_float> || std::is_same_v<ModelTag, core::rgb::model::rgba_float>;

  static constexpr bool is_hsl =
      std::is_same_v<ModelTag, core::hsl::model::hsl> || std::is_same_v<ModelTag, core::hsl::model::hsla>;

  static constexpr bool is_hsv =
      std::is_same_v<ModelTag, core::hsv::model::hsv> || std::is_same_v<ModelTag, core::hsv::model::hsva>;

  static constexpr bool is_cmyk =
      std::is_same_v<ModelTag, core::cmyk::model::cmyk_u8> || std::is_same_v<ModelTag, core::cmyk::model::cmyk_float>;

  // Scene-linear sRGB (no gamma encoding)
  static constexpr bool is_linear_rgb = std::is_same_v<ModelTag, core::linear_rgb::model::linear_rgbf> ||
                                        std::is_same_v<ModelTag, core::linear_rgb::model::linear_rgbaf>;

  // Oklab perceptually-uniform space (Björn Ottosson 2020)
  static constexpr bool is_oklab = std::is_same_v<ModelTag, core::oklab::model::oklab>;

  // OkLCH — cylindrical form of Oklab
  static constexpr bool is_oklch = std::is_same_v<ModelTag, core::oklab::model::oklch>;

  // CIELAB (CIE L*a*b*, ISO 11664-4, D65 reference white)
  static constexpr bool is_lab = std::is_same_v<ModelTag, core::lab::model::cielab>;

  // CIELCH — cylindrical form of CIELAB
  static constexpr bool is_lch = std::is_same_v<ModelTag, core::lab::model::cielch>;

  // CIE XYZ (D65) — device-independent hub space
  static constexpr bool is_xyz = std::is_same_v<ModelTag, core::xyz::model::xyze>;
};

// --- Conversion functions ---
// Each function works in normalised unit [0,1] space internally.
// Alpha is handled conditionally: only emitted when To has >= 4 channels.

template <typename To, typename From>
constexpr To hsl_to_rgb(const From& src) {
  float h = to_unit<From, 0>(src.template get_index<0>());
  float s = to_unit<From, 1>(src.template get_index<1>());
  float l = to_unit<From, 2>(src.template get_index<2>());
  float a = get_src_alpha(src);

  auto hue2rgb = [](float p, float q, float t) {
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;
    if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f) return q;
    if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
  };

  float r{0.0f}, g{0.0f}, b{0.0f};
  if (s == 0.0f) {
    r = g = b = l;
  } else {
    float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
    float p = 2.0f * l - q;
    r = hue2rgb(p, q, h + 1.0f / 3.0f);
    g = hue2rgb(p, q, h);
    b = hue2rgb(p, q, h - 1.0f / 3.0f);
  }
  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

template <typename To, typename From>
constexpr To rgb_to_hsl(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());
  float a = get_src_alpha(src);

  float max = std::max({r, g, b}), min = std::min({r, g, b});
  float h = 0.0f, s = 0.0f, l = (max + min) / 2.0f;

  if (max != min) {
    float d = max - min;
    s = l > 0.5f ? d / (2.0f - max - min) : d / (max + min);
    if (max == r)
      h = (g - b) / d + (g < b ? 6.0f : 0.0f);
    else if (max == g)
      h = (b - r) / d + 2.0f;
    else
      h = (r - g) / d + 4.0f;
    h /= 6.0f;
  }
  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(h), from_unit<To, 1>(s), from_unit<To, 2>(l), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(h), from_unit<To, 1>(s), from_unit<To, 2>(l));
}

template <typename To, typename From>
constexpr To hsv_to_rgb(const From& src) {
  float h = to_unit<From, 0>(src.template get_index<0>());
  float s = to_unit<From, 1>(src.template get_index<1>());
  float v = to_unit<From, 2>(src.template get_index<2>());
  float a = get_src_alpha(src);

  int i = static_cast<int>(h * 6.0f);
  float f = h * 6.0f - static_cast<float>(i);
  float p = v * (1.0f - s);
  float q = v * (1.0f - f * s);
  float t = v * (1.0f - (1.0f - f) * s);

  float r = 0, g = 0, b = 0;
  switch (i % 6) {
    case 0:
      r = v;
      g = t;
      b = p;
      break;
    case 1:
      r = q;
      g = v;
      b = p;
      break;
    case 2:
      r = p;
      g = v;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = v;
      break;
    case 4:
      r = t;
      g = p;
      b = v;
      break;
    case 5:
      r = v;
      g = p;
      b = q;
      break;
  }
  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

template <typename To, typename From>
constexpr To rgb_to_hsv(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());
  float a = get_src_alpha(src);

  float max = std::max({r, g, b}), min = std::min({r, g, b});
  float d = max - min;
  float h = 0.0f, s = (max == 0.0f ? 0.0f : d / max), v = max;

  if (max != min) {
    if (max == r)
      h = (g - b) / d + (g < b ? 6.0f : 0.0f);
    else if (max == g)
      h = (b - r) / d + 2.0f;
    else
      h = (r - g) / d + 4.0f;
    h /= 6.0f;
  }
  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(h), from_unit<To, 1>(s), from_unit<To, 2>(v), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(h), from_unit<To, 1>(s), from_unit<To, 2>(v));
}

// CMYK channels are C, M, Y, K — channel 3 is K (black), NOT alpha.
// Alpha is not a CMYK concept; conversions to/from CMYK always use opaque alpha.
template <typename To, typename From>
constexpr To cmyk_to_rgb(const From& src) {
  float c = to_unit<From, 0>(src.template get_index<0>());
  float m = to_unit<From, 1>(src.template get_index<1>());
  float y = to_unit<From, 2>(src.template get_index<2>());
  float k = to_unit<From, 3>(src.template get_index<3>());

  float r = (1.0f - c) * (1.0f - k);
  float g = (1.0f - m) * (1.0f - k);
  float b = (1.0f - y) * (1.0f - k);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(1.0f));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

template <typename To, typename From>
constexpr To rgb_to_cmyk(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());

  float k = 1.0f - std::max({r, g, b});
  float denom = 1.0f - k;
  float c = (denom > 0.0f) ? (1.0f - r - k) / denom : 0.0f;
  float m = (denom > 0.0f) ? (1.0f - g - k) / denom : 0.0f;
  float y = (denom > 0.0f) ? (1.0f - b - k) / denom : 0.0f;

  return pack_to<To>(from_unit<To, 0>(c), from_unit<To, 1>(m), from_unit<To, 2>(y), from_unit<To, 3>(k));
}

// ── sRGB ↔ Linear sRGB ────────────────────────────────────────────────────
// Applies the IEC 61966-2-1 (sRGB) piecewise transfer functions.

template <typename To, typename From>
constexpr To srgb_to_linear_rgb(const From& src) {
  auto linearize = [](float v) noexcept {
    return (v <= 0.04045f) ? (v / 12.92f) : std::pow((v + 0.055f) / 1.055f, 2.4f);
  };
  float r = linearize(to_unit<From, 0>(src.template get_index<0>()));
  float g = linearize(to_unit<From, 1>(src.template get_index<1>()));
  float b = linearize(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);
  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

template <typename To, typename From>
constexpr To linear_rgb_to_srgb(const From& src) {
  auto gamma_encode = [](float v) noexcept {
    v = std::clamp(v, 0.0f, 1.0f);
    return (v <= 0.0031308f) ? (v * 12.92f) : (1.055f * std::pow(v, 1.0f / 2.4f) - 0.055f);
  };
  float r = gamma_encode(to_unit<From, 0>(src.template get_index<0>()));
  float g = gamma_encode(to_unit<From, 1>(src.template get_index<1>()));
  float b = gamma_encode(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);
  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

// ── Linear sRGB ↔ Oklab ───────────────────────────────────────────────────
// Reference: https://bottosson.github.io/posts/oklab/
// Matrices and cube-root linearisation per the original specification.

template <typename To, typename From>
constexpr To linear_rgb_to_oklab(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());

  float lms_l = 0.4122214708f * r + 0.5363325363f * g + 0.0514459929f * b;
  float lms_m = 0.2119034982f * r + 0.6806995451f * g + 0.1073969566f * b;
  float lms_s = 0.0883024619f * r + 0.2817188376f * g + 0.6299787005f * b;

  // Safe cube root: preserve sign for near-zero negative values from FP arithmetic.
  auto safe_cbrt = [](float x) noexcept { return x >= 0.0f ? std::cbrt(x) : -std::cbrt(-x); };
  float l_ = safe_cbrt(lms_l);
  float m_ = safe_cbrt(lms_m);
  float s_ = safe_cbrt(lms_s);

  float L = 0.2104542553f * l_ + 0.7936177850f * m_ - 0.0040720468f * s_;
  float A = 1.9779984951f * l_ - 2.4285922050f * m_ + 0.4505937099f * s_;
  float B = 0.0259040371f * l_ + 0.4072456426f * m_ - 0.4331497194f * s_;

  // A and B are in [-0.5, 0.5]: use from_value (not from_unit) to avoid
  // incorrect clamping to [0,1] unit space.
  return pack_to<To>(from_value<To, 0>(L), from_value<To, 1>(A), from_value<To, 2>(B));
}

template <typename To, typename From>
constexpr To oklab_to_linear_rgb(const From& src) {
  // Read raw channel values (Oklab L ∈ [0,1], a/b ∈ [-0.5, 0.5]).
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

  // from_unit clamps to [0,1]: out-of-gamut colors are gamut-clipped here.
  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(1.0f));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

// ── Oklab ↔ OkLCH ─────────────────────────────────────────────────────────

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

// ── Linear sRGB ↔ CIELAB (via XYZ, D65 reference white) ──────────────────
// Forward matrix (sRGB IEC 61966-2-1, D65):  sRGB linear → XYZ
// Inverse matrix:                             XYZ → sRGB linear
// D65 reference white: Xn=0.95047, Yn=1.00000, Zn=1.08883
// CIELAB f() function: cube-root with linear extension below (6/29)^3

template <typename To, typename From>
constexpr To linear_rgb_to_lab(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());

  // Linear sRGB → XYZ (D65)
  float X = 0.4124564f * r + 0.3575761f * g + 0.1804375f * b;
  float Y = 0.2126729f * r + 0.7151522f * g + 0.0721750f * b;
  float Z = 0.0193339f * r + 0.1191920f * g + 0.9503041f * b;

  // Normalize by D65 reference white
  constexpr float Xn = 0.95047f, Yn = 1.00000f, Zn = 1.08883f;
  float fx = X / Xn, fy = Y / Yn, fz = Z / Zn;

  // CIE f() piecewise function
  constexpr float delta = 6.0f / 29.0f;               // 0.20689...
  constexpr float delta3 = delta * delta * delta;     // (6/29)^3
  constexpr float k = 1.0f / (3.0f * delta * delta);  // 29^2 / (3 * 6^2)
  constexpr float b0 = 4.0f / 29.0f;

  auto f = [=](float t) noexcept { return t > delta3 ? std::cbrt(t) : k * t + b0; };

  float fxv = f(fx), fyv = f(fy), fzv = f(fz);

  float L = 116.0f * fyv - 16.0f;
  float A = 500.0f * (fxv - fyv);
  float B = 200.0f * (fyv - fzv);

  // L* ∈ [0,100], a*/b* ∈ [-128,128]: use from_value (not from_unit) since
  // these are not in [0,1] unit space.
  return pack_to<To>(from_value<To, 0>(L), from_value<To, 1>(A), from_value<To, 2>(B));
}

template <typename To, typename From>
constexpr To lab_to_linear_rgb(const From& src) {
  // Read raw CIE values: L* ∈ [0,100], a*/b* ∈ [-128,128]
  float L = static_cast<float>(src.template get_index<0>());
  float A = static_cast<float>(src.template get_index<1>());
  float B = static_cast<float>(src.template get_index<2>());

  // CIELAB → f values
  float fy = (L + 16.0f) / 116.0f;
  float fx = A / 500.0f + fy;
  float fz = fy - B / 200.0f;

  // CIE inverse g() function
  constexpr float delta = 6.0f / 29.0f;
  constexpr float delta2 = delta * delta;

  auto g = [=](float f) noexcept { return f > delta ? f * f * f : 3.0f * delta2 * (f - 4.0f / 29.0f); };

  // XYZ (D65)
  constexpr float Xn = 0.95047f, Yn = 1.00000f, Zn = 1.08883f;
  float X = Xn * g(fx);
  float Y = Yn * g(fy);
  float Z = Zn * g(fz);

  // XYZ → Linear sRGB (D65 inverse matrix)
  float r = 3.2404542f * X - 1.5371385f * Y - 0.4985314f * Z;
  float gv = -0.9692660f * X + 1.8760108f * Y + 0.0415560f * Z;
  float bv = 0.0556434f * X - 0.2040259f * Y + 1.0572252f * Z;

  // from_unit clamps to [0,1]: out-of-gamut colors are gamut-clipped here.
  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(gv), from_unit<To, 2>(bv), from_unit<To, 3>(1.0f));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(gv), from_unit<To, 2>(bv));
}

// ── CIELAB ↔ CIELCH (polar ↔ cartesian) ──────────────────────────────────

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

// ── Linear sRGB ↔ CIE XYZ (D65) ──────────────────────────────────────────
// Matrix: IEC 61966-2-1 (sRGB specification), D65 reference white.
// XYZ channels are stored in natural CIE units ([0, 2]), not unit [0,1] space,
// so from_value / static_cast are used instead of from_unit / to_unit.

template <typename To, typename From>
constexpr To linear_rgb_to_xyz(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());

  float X = 0.4124564f * r + 0.3575761f * g + 0.1804375f * b;
  float Y = 0.2126729f * r + 0.7151522f * g + 0.0721750f * b;
  float Z = 0.0193339f * r + 0.1191920f * g + 0.9503041f * b;

  return pack_to<To>(from_value<To, 0>(X), from_value<To, 1>(Y), from_value<To, 2>(Z));
}

template <typename To, typename From>
constexpr To xyz_to_linear_rgb(const From& src) {
  float X = static_cast<float>(src.template get_index<0>());
  float Y = static_cast<float>(src.template get_index<1>());
  float Z = static_cast<float>(src.template get_index<2>());

  float r = 3.2404542f * X - 1.5371385f * Y - 0.4985314f * Z;
  float g = -0.9692660f * X + 1.8760108f * Y + 0.0415560f * Z;
  float b = 0.0556434f * X - 0.2040259f * Y + 1.0572252f * Z;

  // from_unit clamps to [0,1]: out-of-sRGB-gamut XYZ values are gamut-clipped here.
  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(1.0f));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

// ── CIE XYZ (D65) ↔ CIELAB ───────────────────────────────────────────────
// XYZ values are read as raw channel values (not unit space).
// CIELAB output uses from_value (L* ∈ [0,100], a*/b* ∈ [-128,128]).

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

// ── CIE XYZ (D65) ↔ OkLab ────────────────────────────────────────────────
// Direct conversion without routing through linear sRGB, so no gamut clipping
// occurs for colors outside the sRGB gamut.
//
// M1_xyz (XYZ D65 → OkLab LMS) — from Ottosson 2020 / CSS Color Level 4:
//   [0.8189330101  0.3618667424 -0.1288597137]
//   [0.0329845436  0.9293118715  0.0361456387]
//   [0.0482003018  0.2643662691  0.6338517070]
//
// M2 (cbrt(LMS) → OkLab): same as in linear_rgb_to_oklab.
//
// Inverse: M2_inv (OkLab → cbrt(LMS)) then cube then M1_xyz_inv (LMS → XYZ D65).

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

// Helper: read channel I from src if it exists, else return a default (for alpha expansion).
// Uses if constexpr so get_index<I> is never instantiated on a type with fewer channels.
template <typename To, typename From, std::size_t I>
constexpr typename To::template channel_value_t<I> channel_or_default(const From& src, float default_val) {
  if constexpr (I < From::channels)
    return from_unit<To, I>(to_unit<From, I>(src.template get_index<I>()));
  else
    return from_unit<To, I>(default_val);
}

template <typename To, typename From, std::size_t... Is>
constexpr To same_model_cast_impl(const From& src, std::index_sequence<Is...>) {
  return pack_to<To>(channel_or_default<To, From, Is>(src, 1.0f)...);
}

}  // namespace details

template <typename To, typename From, typename = void>
struct color_cast_impl {
  static constexpr To convert(const From& src) {
    using FromTag = details::extract_model_t<From>;
    using ToTag = details::extract_model_t<To>;
    using FromCat = details::model_category<FromTag>;
    using ToCat = details::model_category<ToTag>;

    // ── Identity cast: return a copy without any arithmetic ────────────────
    if constexpr (std::is_same_v<From, To>) {
      return src;

      // ── Same space (variant or precision cast) ────────────────────────────
    } else if constexpr (std::is_same_v<FromTag, ToTag> || (FromCat::is_rgb && ToCat::is_rgb) ||
                         (FromCat::is_hsl && ToCat::is_hsl) || (FromCat::is_hsv && ToCat::is_hsv) ||
                         (FromCat::is_cmyk && ToCat::is_cmyk) || (FromCat::is_linear_rgb && ToCat::is_linear_rgb) ||
                         (FromCat::is_oklab && ToCat::is_oklab) || (FromCat::is_oklch && ToCat::is_oklch) ||
                         (FromCat::is_lab && ToCat::is_lab) || (FromCat::is_lch && ToCat::is_lch)) {
      return details::same_model_cast_impl<To>(src, std::make_index_sequence<To::channels>{});

      // ── Direct: sRGB ↔ HSL / HSV / CMYK ─────────────────────────────────
    } else if constexpr (FromCat::is_hsl && ToCat::is_rgb) {
      return details::hsl_to_rgb<To>(src);
    } else if constexpr (FromCat::is_rgb && ToCat::is_hsl) {
      return details::rgb_to_hsl<To>(src);
    } else if constexpr (FromCat::is_hsv && ToCat::is_rgb) {
      return details::hsv_to_rgb<To>(src);
    } else if constexpr (FromCat::is_rgb && ToCat::is_hsv) {
      return details::rgb_to_hsv<To>(src);
    } else if constexpr (FromCat::is_cmyk && ToCat::is_rgb) {
      return details::cmyk_to_rgb<To>(src);
    } else if constexpr (FromCat::is_rgb && ToCat::is_cmyk) {
      return details::rgb_to_cmyk<To>(src);

      // ── Direct: sRGB ↔ Linear sRGB (gamma encode/decode) ─────────────────
    } else if constexpr (FromCat::is_rgb && ToCat::is_linear_rgb) {
      return details::srgb_to_linear_rgb<To>(src);
    } else if constexpr (FromCat::is_linear_rgb && ToCat::is_rgb) {
      return details::linear_rgb_to_srgb<To>(src);

      // ── Direct: Linear sRGB ↔ Oklab (matrix + cbrt) ──────────────────────
    } else if constexpr (FromCat::is_linear_rgb && ToCat::is_oklab) {
      return details::linear_rgb_to_oklab<To>(src);
    } else if constexpr (FromCat::is_oklab && ToCat::is_linear_rgb) {
      return details::oklab_to_linear_rgb<To>(src);

      // ── Direct: Oklab ↔ OkLCH (polar ↔ cartesian) ───────────────────────
    } else if constexpr (FromCat::is_oklab && ToCat::is_oklch) {
      return details::oklab_to_oklch<To>(src);
    } else if constexpr (FromCat::is_oklch && ToCat::is_oklab) {
      return details::oklch_to_oklab<To>(src);

      // ── Multi-hop: sRGB ↔ Oklab  (via Linear sRGB) ───────────────────────
    } else if constexpr (FromCat::is_rgb && ToCat::is_oklab) {
      return color_cast<To>(color_cast<core::linear_rgbaf_t>(src));
    } else if constexpr (FromCat::is_oklab && ToCat::is_rgb) {
      return color_cast<To>(color_cast<core::linear_rgbaf_t>(src));

      // ── Multi-hop: sRGB ↔ OkLCH (via Oklab) ─────────────────────────────
    } else if constexpr (FromCat::is_rgb && ToCat::is_oklch) {
      return color_cast<To>(color_cast<core::oklab_t>(src));
    } else if constexpr (FromCat::is_oklch && ToCat::is_rgb) {
      return color_cast<To>(color_cast<core::oklab_t>(src));

      // ── Direct: Linear sRGB ↔ CIELAB (via XYZ, D65) ──────────────────────
    } else if constexpr (FromCat::is_linear_rgb && ToCat::is_lab) {
      return details::linear_rgb_to_lab<To>(src);
    } else if constexpr (FromCat::is_lab && ToCat::is_linear_rgb) {
      return details::lab_to_linear_rgb<To>(src);

      // ── Direct: CIELAB ↔ CIELCH (polar ↔ cartesian) ──────────────────────
    } else if constexpr (FromCat::is_lab && ToCat::is_lch) {
      return details::lab_to_lch<To>(src);
    } else if constexpr (FromCat::is_lch && ToCat::is_lab) {
      return details::lch_to_lab<To>(src);

      // ── Multi-hop: sRGB ↔ CIELAB (via Linear sRGB) ───────────────────────
    } else if constexpr (FromCat::is_rgb && ToCat::is_lab) {
      return color_cast<To>(color_cast<core::linear_rgbf_t>(src));
    } else if constexpr (FromCat::is_lab && ToCat::is_rgb) {
      return color_cast<To>(color_cast<core::linear_rgbf_t>(src));

      // ── Multi-hop: sRGB ↔ CIELCH (via CIELAB) ────────────────────────────
    } else if constexpr (FromCat::is_rgb && ToCat::is_lch) {
      return color_cast<To>(color_cast<core::cielab_t>(src));
    } else if constexpr (FromCat::is_lch && ToCat::is_rgb) {
      return color_cast<To>(color_cast<core::cielab_t>(src));

      // ── Direct: Linear sRGB ↔ XYZ D65 (matrix) ───────────────────────────
    } else if constexpr (FromCat::is_linear_rgb && ToCat::is_xyz) {
      return details::linear_rgb_to_xyz<To>(src);
    } else if constexpr (FromCat::is_xyz && ToCat::is_linear_rgb) {
      return details::xyz_to_linear_rgb<To>(src);

      // ── Direct: XYZ D65 ↔ CIELAB (D65 reference white) ───────────────────
    } else if constexpr (FromCat::is_xyz && ToCat::is_lab) {
      return details::xyz_to_lab<To>(src);
    } else if constexpr (FromCat::is_lab && ToCat::is_xyz) {
      return details::lab_to_xyz<To>(src);

      // ── Direct: XYZ D65 ↔ OkLab (no gamut clip) ──────────────────────────
    } else if constexpr (FromCat::is_xyz && ToCat::is_oklab) {
      return details::xyz_to_oklab<To>(src);
    } else if constexpr (FromCat::is_oklab && ToCat::is_xyz) {
      return details::oklab_to_xyz<To>(src);

      // ── Multi-hop: sRGB ↔ XYZ (via Linear sRGB) ──────────────────────────
    } else if constexpr (FromCat::is_rgb && ToCat::is_xyz) {
      return color_cast<To>(color_cast<core::linear_rgbf_t>(src));
    } else if constexpr (FromCat::is_xyz && ToCat::is_rgb) {
      return color_cast<To>(color_cast<core::linear_rgbf_t>(src));

      // ── Multi-hop: XYZ ↔ CIELCH (via CIELAB) ─────────────────────────────
    } else if constexpr (FromCat::is_xyz && ToCat::is_lch) {
      return color_cast<To>(color_cast<core::cielab_t>(src));
    } else if constexpr (FromCat::is_lch && ToCat::is_xyz) {
      return color_cast<To>(color_cast<core::cielab_t>(src));

      // ── Multi-hop: XYZ ↔ OkLCH (via OkLab) ──────────────────────────────
    } else if constexpr (FromCat::is_xyz && ToCat::is_oklch) {
      return color_cast<To>(color_cast<core::oklab_t>(src));
    } else if constexpr (FromCat::is_oklch && ToCat::is_xyz) {
      return color_cast<To>(color_cast<core::oklab_t>(src));

      // ── Gamut-safe cross-space: CIELAB ↔ OkLab (via XYZ, no sRGB clip) ───
    } else if constexpr (FromCat::is_lab && ToCat::is_oklab) {
      return color_cast<To>(color_cast<core::xyz_t>(src));
    } else if constexpr (FromCat::is_oklab && ToCat::is_lab) {
      return color_cast<To>(color_cast<core::xyz_t>(src));

      // ── Gamut-safe cross-space: CIELAB ↔ OkLCH (via XYZ → OkLab) ─────────
    } else if constexpr (FromCat::is_lab && ToCat::is_oklch) {
      return color_cast<To>(color_cast<core::oklab_t>(color_cast<core::xyz_t>(src)));
    } else if constexpr (FromCat::is_oklch && ToCat::is_lab) {
      return color_cast<To>(color_cast<core::xyz_t>(color_cast<core::oklab_t>(src)));

      // ── Gamut-safe cross-space: CIELCH ↔ OkLab (via CIELAB → XYZ) ────────
    } else if constexpr (FromCat::is_lch && ToCat::is_oklab) {
      return color_cast<To>(color_cast<core::xyz_t>(color_cast<core::cielab_t>(src)));
    } else if constexpr (FromCat::is_oklab && ToCat::is_lch) {
      return color_cast<To>(color_cast<core::cielab_t>(color_cast<core::xyz_t>(src)));

      // ── Gamut-safe cross-space: CIELCH ↔ OkLCH (via CIELAB → XYZ → OkLab)
    } else if constexpr (FromCat::is_lch && ToCat::is_oklch) {
      return color_cast<To>(color_cast<core::oklab_t>(color_cast<core::xyz_t>(color_cast<core::cielab_t>(src))));
    } else if constexpr (FromCat::is_oklch && ToCat::is_lch) {
      return color_cast<To>(color_cast<core::cielab_t>(color_cast<core::xyz_t>(color_cast<core::oklab_t>(src))));

      // ── Generic two-hop via sRGBaf ────────────────────────────────────────
      // Handles all remaining cross-category pairs (HSL↔CMYK, linear_rgb↔HSL, etc.)
      // by routing through the sRGB float hub.  Neither side may be sRGB to prevent
      // infinite recursion; the explicit cases above handle sRGB combinations first.
    } else if constexpr (!FromCat::is_rgb && !ToCat::is_rgb) {
      auto mid = color_cast<core::rgbaf_t>(src);
      return color_cast<To>(mid);

    } else {
      static_assert(sizeof(To) == 0,
                    "colorcpp: unsupported conversion path. "
                    "Specialize color_cast_impl<To,From> to add support.");
      return To{};
    }
  }
};

template <typename To, typename From>
constexpr To color_cast(const From& src) {
  return color_cast_impl<To, From>::convert(src);
}

}  // namespace colorcpp::operations::conversion
