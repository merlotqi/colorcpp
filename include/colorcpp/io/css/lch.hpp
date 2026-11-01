/**
 * @file lch.hpp
 * @brief CSS lch() color parsing (D50) with adaptation to the D65 core type.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/io/css/details.hpp>
#include <colorcpp/io/css/lab.hpp>
#include <optional>
#include <string_view>

namespace colorcpp::io::css {

/**
 * @brief Parse lch(L C H [/ alpha]) — CSS Color Level 4.
 *
 * CSS lch() uses the D50 white point (like lab()); the result is
 * Bradford-adapted to the D65-based core::cielch_t.
 *
 * L: 0-100% or 0-100 (lightness)
 * C: 0-150 or 0-150% (chroma)
 * H: hue angle with units (deg, grad, rad, turn)
 * Optional alpha with /
 *
 * @param alpha_out Optional sink for the parsed alpha in [0,1]; written only on
 *        success (see @ref parse_lab_function).
 */
inline std::optional<core::cielch_t> parse_lch_function(details::Cursor& c, float* alpha_out = nullptr) {
  if (!c.consume_ci("lch")) return std::nullopt;
  c.skip_ws();
  if (!c.consume_char('(')) return std::nullopt;
  c.skip_ws();

  size_t checkpoint = c.i;

  auto try_space = [&]() -> std::optional<core::cielch_t> {
    c.i = checkpoint;
    details::Cursor d{c.s, c.i};

    float L = 0.f;
    if (!d.try_consume_none()) {
      auto l_cv = d.parse_component_value();
      if (!l_cv) return std::nullopt;
      L = static_cast<float>(l_cv->first);
      L = std::clamp(L, 0.0f, 100.0f);
    }

    d.skip_ws();

    float C = 0.f;
    if (!d.try_consume_none()) {
      auto c_cv = d.parse_component_value();
      if (!c_cv) return std::nullopt;
      C = static_cast<float>(c_cv->first);
      if (c_cv->second) {
        C = std::clamp(C, 0.0f, 100.0f) * 1.5f;
      } else {
        C = std::clamp(C, 0.0f, 150.0f);
      }
    }

    d.skip_ws();

    float H = 0.f;
    if (!d.try_consume_none()) {
      auto h = d.parse_hue_angle();
      if (!h) return std::nullopt;
      H = static_cast<float>(*h);
    }

    d.skip_ws();

    // Parse optional alpha; cielch_t has no alpha channel, so hand it to the
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

    // D50 lch → D50 lab → Bradford → D65 lab → D65 lch
    const float h_rad = H * (3.14159265358979323846f / 180.0f);
    const auto lab_d65 = css_lab_detail::lab_d50_to_d65(L, C * std::cos(h_rad), C * std::sin(h_rad));

    const float c_out = std::sqrt(lab_d65.a() * lab_d65.a() + lab_d65.b() * lab_d65.b());
    float h_out = std::atan2(lab_d65.b(), lab_d65.a()) * (180.0f / 3.14159265358979323846f);
    if (h_out < 0.0f) h_out += 360.0f;
    return core::cielch_t{lab_d65.l(), std::clamp(c_out, 0.0f, 200.0f), h_out};
  };

  if (auto r = try_space()) return r;
  return std::nullopt;
}

}  // namespace colorcpp::io::css
