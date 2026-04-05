/**
 * @file hsl.hpp
 * @brief CSS hsl()/hsla() color parsing.
 */

#pragma once

#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/io/css/details.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <optional>
#include <string_view>

namespace colorcpp::io::css {

inline std::optional<core::rgba8_t> parse_hsl_function(details::Cursor& c) {
  bool hsla = c.consume_ci("hsla");
  if (!hsla && !c.consume_ci("hsl")) return std::nullopt;
  c.skip_ws();
  if (!c.consume_char('(')) return std::nullopt;
  c.skip_ws();

  size_t checkpoint = c.i;

  auto try_comma = [&]() -> std::optional<core::rgba8_t> {
    c.i = checkpoint;
    details::Cursor d{c.s, c.i};
    float hf = 0.f;
    if (!d.try_consume_none()) {
      auto h = d.parse_hue_angle();
      if (!h) return std::nullopt;
      hf = static_cast<float>(*h);
    }
    d.skip_ws();
    if (!d.consume_char(',')) return std::nullopt;
    float sf = 0.f;
    if (!d.try_consume_none()) {
      auto s = d.parse_hsl_sl();
      if (!s) return std::nullopt;
      sf = static_cast<float>(*s);
    }
    d.skip_ws();
    if (!d.consume_char(',')) return std::nullopt;
    float lf = 0.f;
    if (!d.try_consume_none()) {
      auto l = d.parse_hsl_sl();
      if (!l) return std::nullopt;
      lf = static_cast<float>(*l);
    }
    float alpha_f = 1.0f;
    d.skip_ws();
    if (d.consume_char(',')) {
      auto av = d.parse_alpha_value();
      if (!av) return std::nullopt;
      double a = *av;
      if (a < 0.0) a = 0.0;
      if (a > 1.0) a = 1.0;
      alpha_f = static_cast<float>(a);
    }
    d.skip_ws();
    if (!d.consume_char(')')) return std::nullopt;
    d.skip_ws();
    if (!d.eof()) return std::nullopt;
    core::hsla_float_t hslf{hf, sf, lf, alpha_f};
    core::rgba8_t out = operations::conversion::color_cast<core::rgba8_t>(hslf);
    c.i = d.i;
    return out;
  };

  auto try_space = [&]() -> std::optional<core::rgba8_t> {
    c.i = checkpoint;
    details::Cursor d{c.s, c.i};
    float hf = 0.f;
    if (!d.try_consume_none()) {
      auto h = d.parse_hue_angle();
      if (!h) return std::nullopt;
      hf = static_cast<float>(*h);
    }
    d.skip_ws();
    float sf = 0.f;
    if (!d.try_consume_none()) {
      auto s = d.parse_hsl_sl();
      if (!s) return std::nullopt;
      sf = static_cast<float>(*s);
    }
    d.skip_ws();
    float lf = 0.f;
    if (!d.try_consume_none()) {
      auto l = d.parse_hsl_sl();
      if (!l) return std::nullopt;
      lf = static_cast<float>(*l);
    }
    float alpha_f = 1.0f;
    d.skip_ws();
    if (d.consume_char('/')) {
      auto av = d.parse_alpha_value();
      if (!av) return std::nullopt;
      double a = *av;
      if (a < 0.0) a = 0.0;
      if (a > 1.0) a = 1.0;
      alpha_f = static_cast<float>(a);
    }
    d.skip_ws();
    if (!d.consume_char(')')) return std::nullopt;
    d.skip_ws();
    if (!d.eof()) return std::nullopt;
    core::hsla_float_t hslf{hf, sf, lf, alpha_f};
    core::rgba8_t out = operations::conversion::color_cast<core::rgba8_t>(hslf);
    c.i = d.i;
    return out;
  };

  if (auto r = try_comma()) return r;
  if (auto r = try_space()) return r;
  return std::nullopt;
}

inline std::optional<core::rgbaf_t> parse_hsl_function_rgbaf(details::Cursor& c) {
  bool hsla = c.consume_ci("hsla");
  if (!hsla && !c.consume_ci("hsl")) return std::nullopt;
  c.skip_ws();
  if (!c.consume_char('(')) return std::nullopt;
  c.skip_ws();

  size_t checkpoint = c.i;

  auto try_comma = [&]() -> std::optional<core::rgbaf_t> {
    c.i = checkpoint;
    details::Cursor d{c.s, c.i};
    float hf = 0.f;
    if (!d.try_consume_none()) {
      auto h = d.parse_hue_angle();
      if (!h) return std::nullopt;
      hf = static_cast<float>(*h);
    }
    d.skip_ws();
    if (!d.consume_char(',')) return std::nullopt;
    float sf = 0.f;
    if (!d.try_consume_none()) {
      auto s = d.parse_hsl_sl();
      if (!s) return std::nullopt;
      sf = static_cast<float>(*s);
    }
    d.skip_ws();
    if (!d.consume_char(',')) return std::nullopt;
    float lf = 0.f;
    if (!d.try_consume_none()) {
      auto l = d.parse_hsl_sl();
      if (!l) return std::nullopt;
      lf = static_cast<float>(*l);
    }
    float alpha_f = 1.0f;
    d.skip_ws();
    if (d.consume_char(',')) {
      auto av = d.parse_alpha_value();
      if (!av) return std::nullopt;
      alpha_f = static_cast<float>(std::clamp(*av, 0.0, 1.0));
    }
    d.skip_ws();
    if (!d.consume_char(')')) return std::nullopt;
    d.skip_ws();
    if (!d.eof()) return std::nullopt;
    core::hsla_float_t hslf{hf, sf, lf, alpha_f};
    c.i = d.i;
    return operations::conversion::color_cast<core::rgbaf_t>(hslf);
  };

  auto try_space = [&]() -> std::optional<core::rgbaf_t> {
    c.i = checkpoint;
    details::Cursor d{c.s, c.i};
    float hf = 0.f;
    if (!d.try_consume_none()) {
      auto h = d.parse_hue_angle();
      if (!h) return std::nullopt;
      hf = static_cast<float>(*h);
    }
    d.skip_ws();
    float sf = 0.f;
    if (!d.try_consume_none()) {
      auto s = d.parse_hsl_sl();
      if (!s) return std::nullopt;
      sf = static_cast<float>(*s);
    }
    d.skip_ws();
    float lf = 0.f;
    if (!d.try_consume_none()) {
      auto l = d.parse_hsl_sl();
      if (!l) return std::nullopt;
      lf = static_cast<float>(*l);
    }
    float alpha_f = 1.0f;
    d.skip_ws();
    if (d.consume_char('/')) {
      auto av = d.parse_alpha_value();
      if (!av) return std::nullopt;
      alpha_f = static_cast<float>(std::clamp(*av, 0.0, 1.0));
    }
    d.skip_ws();
    if (!d.consume_char(')')) return std::nullopt;
    d.skip_ws();
    if (!d.eof()) return std::nullopt;
    core::hsla_float_t hslf{hf, sf, lf, alpha_f};
    c.i = d.i;
    return operations::conversion::color_cast<core::rgbaf_t>(hslf);
  };

  if (auto r = try_comma()) return r;
  if (auto r = try_space()) return r;
  return std::nullopt;
}

}  // namespace colorcpp::io::css
