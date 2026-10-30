/**
 * @file lab.hpp
 * @brief CSS lab() color parsing (D50) with adaptation to the D65 core type.
 */

#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <colorcpp/algorithms/chromatic_adaptation.hpp>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/io/css/details.hpp>
#include <optional>
#include <string_view>

namespace colorcpp::io::css {

namespace css_lab_detail {

namespace ca = colorcpp::algorithms::chromatic_adaptation;

// CIE L*a*b* ↔ XYZ with an arbitrary reference white (CSS lab()/lch() are
// D50-based; core::cielab_t is D65-based).
inline std::array<float, 3> lab_to_xyz(float L, float a, float b, const core::xyz_t& wp) {
  const float fy = (L + 16.0f) / 116.0f;
  const float fx = a / 500.0f + fy;
  const float fz = fy - b / 200.0f;

  constexpr float delta = 6.0f / 29.0f;
  constexpr float delta2 = delta * delta;
  const auto g = [=](float f) noexcept { return f > delta ? f * f * f : 3.0f * delta2 * (f - 4.0f / 29.0f); };

  return {wp.x() * g(fx), wp.y() * g(fy), wp.z() * g(fz)};
}

inline core::cielab_t xyz_to_lab(const std::array<float, 3>& xyz, const core::xyz_t& wp) {
  constexpr float delta = 6.0f / 29.0f;
  constexpr float delta3 = delta * delta * delta;
  constexpr float k = 1.0f / (3.0f * delta * delta);
  constexpr float b0 = 4.0f / 29.0f;
  const auto f = [=](float t) noexcept { return t > delta3 ? std::cbrt(t) : k * t + b0; };

  const float fx = f(xyz[0] / wp.x());
  const float fy = f(xyz[1] / wp.y());
  const float fz = f(xyz[2] / wp.z());

  // Clamp to the core channel ranges so construction never throws — the
  // parser contract is nullopt-on-failure, never exceptions. Bounds come
  // from the channel types so this cannot drift when they are widened.
  using a_ch = core::lab::channel::lab_a;
  using b_ch = core::lab::channel::lab_b;
  const float L = std::clamp(116.0f * fy - 16.0f, 0.0f, 100.0f);
  const float A = std::clamp(500.0f * (fx - fy), static_cast<float>(a_ch::min), static_cast<float>(a_ch::max));
  const float B = std::clamp(200.0f * (fy - fz), static_cast<float>(b_ch::min), static_cast<float>(b_ch::max));
  return core::cielab_t{L, A, B};
}

// Bradford chromatic adaptation on raw XYZ triples (avoids materializing
// intermediate core types whose channel ranges may reject intermediate values).
inline std::array<float, 3> bradford_adapt(const std::array<float, 3>& xyz, const core::xyz_t& src_wp,
                                           const core::xyz_t& dst_wp) {
  const auto mul = [](const float* m, float x, float y, float z) {
    return std::array<float, 3>{
        m[0] * x + m[1] * y + m[2] * z,
        m[3] * x + m[4] * y + m[5] * z,
        m[6] * x + m[7] * y + m[8] * z,
    };
  };

  const auto lms_src = mul(&ca::details::M_BFD[0], src_wp.x(), src_wp.y(), src_wp.z());
  const auto lms_dst = mul(&ca::details::M_BFD[0], dst_wp.x(), dst_wp.y(), dst_wp.z());
  auto lms = mul(&ca::details::M_BFD[0], xyz[0], xyz[1], xyz[2]);
  for (int i = 0; i < 3; ++i) {
    if (lms_src[i] != 0.0f) lms[i] *= lms_dst[i] / lms_src[i];
  }
  return mul(&ca::details::M_BFD_inv[0], lms[0], lms[1], lms[2]);
}

// CSS lab()/lch() D50 channels → D65 core::cielab_t (CSS Color 4 §9.1:
// Bradford adaptation between white points).
inline core::cielab_t lab_d50_to_d65(float L, float a, float b) {
  const auto xyz50 = lab_to_xyz(L, a, b, ca::WHITEPOINT_D50);
  const auto xyz65 = bradford_adapt(xyz50, ca::WHITEPOINT_D50, ca::WHITEPOINT_D65);
  return xyz_to_lab(xyz65, ca::WHITEPOINT_D65);
}

// Inverse direction — used by relative lab(from …)/lch(from …), whose
// channel keywords resolve to D50 values.
inline core::cielab_t lab_d65_to_d50(const core::cielab_t& lab) {
  const auto xyz65 = lab_to_xyz(lab.l(), lab.a(), lab.b(), ca::WHITEPOINT_D65);
  const auto xyz50 = bradford_adapt(xyz65, ca::WHITEPOINT_D65, ca::WHITEPOINT_D50);
  return xyz_to_lab(xyz50, ca::WHITEPOINT_D50);
}

}  // namespace css_lab_detail

/**
 * @brief Parse lab(L a b [/ alpha]) — CSS Color Level 4.
 *
 * CSS lab() uses the D50 white point; the result is Bradford-adapted to the
 * D65-based core::cielab_t.
 *
 * L: 0-100% or 0-100 (lightness)
 * a: -125 to 125 (green-red axis)
 * b: -125 to 125 (blue-yellow axis)
 * Optional alpha with /
 *
 * @param alpha_out Optional sink for the parsed alpha in [0,1]; written only on
 *        success. @c cielab_t has no alpha channel, so callers that cast to an
 *        alpha-carrying type must read it here. Left untouched when the caller
 *        passes @c nullptr (the default).
 */
inline std::optional<core::cielab_t> parse_lab_function(details::Cursor& c, float* alpha_out = nullptr) {
  if (!c.consume_ci("lab")) return std::nullopt;
  c.skip_ws();
  if (!c.consume_char('(')) return std::nullopt;
  c.skip_ws();

  size_t checkpoint = c.i;

  auto try_space = [&]() -> std::optional<core::cielab_t> {
    c.i = checkpoint;
    details::Cursor d{c.s, c.i};

    // Parse L (lightness): 0-100% or 0-100; `none` → 0 (static default subset)
    float L = 0.f;
    if (!d.try_consume_none()) {
      auto l_cv = d.parse_component_value();
      if (!l_cv) return std::nullopt;
      L = static_cast<float>(l_cv->first);
      L = std::clamp(L, 0.0f, 100.0f);
    }

    d.skip_ws();

    float A = 0.f;
    if (!d.try_consume_none()) {
      auto a_cv = d.parse_component_value();
      if (!a_cv) return std::nullopt;
      A = static_cast<float>(a_cv->first);
      if (a_cv->second) {
        A = std::clamp(A, -100.0f, 100.0f) * 1.25f;
      } else {
        A = std::clamp(A, -125.0f, 125.0f);
      }
    }

    d.skip_ws();

    float B = 0.f;
    if (!d.try_consume_none()) {
      auto b_cv = d.parse_component_value();
      if (!b_cv) return std::nullopt;
      B = static_cast<float>(b_cv->first);
      if (b_cv->second) {
        B = std::clamp(B, -100.0f, 100.0f) * 1.25f;
      } else {
        B = std::clamp(B, -125.0f, 125.0f);
      }
    }

    d.skip_ws();

    // Parse optional alpha; cielab_t has no alpha channel, so hand it to the
    // caller through alpha_out (CSS default is opaque when "/" is omitted).
    float alpha = 1.0f;
    if (d.consume_char('/')) {
      auto av = d.parse_alpha_value();
      if (!av) return std::nullopt;
      alpha = std::clamp(static_cast<float>(*av), 0.0f, 1.0f);
    }

    d.skip_ws();
    if (!d.consume_char(')')) return std::nullopt;
    d.skip_ws();
    if (!d.eof()) return std::nullopt;

    c.i = d.i;
    if (alpha_out) *alpha_out = alpha;
    return css_lab_detail::lab_d50_to_d65(L, A, B);
  };

  if (auto r = try_space()) return r;
  return std::nullopt;
}

}  // namespace colorcpp::io::css
