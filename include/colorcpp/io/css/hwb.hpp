/**
 * @file hwb.hpp
 * @brief CSS hwb() color parsing.
 */

#pragma once

#include <colorcpp/core/hwb.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/io/css/details.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <optional>
#include <string_view>

namespace colorcpp::io::css {

inline std::optional<core::rgba8_t> parse_hwb_function(details::Cursor& c) {
  if (!c.consume_ci("hwb")) return std::nullopt;
  c.skip_ws();
  if (!c.consume_char('(')) return std::nullopt;
  c.skip_ws();

  size_t checkpoint = c.i;

  auto try_comma = [&]() -> std::optional<core::rgba8_t> {
    c.i = checkpoint;
    details::Cursor d{c.s, c.i};
    float hue = 0.f;
    if (!d.try_consume_none()) {
      auto h = d.parse_hwb_hue_angle();
      if (!h) return std::nullopt;
      hue = static_cast<float>(*h);
    }
    d.skip_ws();
    if (!d.consume_char(',')) return std::nullopt;
    double wv = 0.0;
    if (!d.try_consume_none()) {
      auto w = d.parse_hsl_sl();
      if (!w) return std::nullopt;
      wv = *w;
    }
    d.skip_ws();
    if (!d.consume_char(',')) return std::nullopt;
    double bv = 0.0;
    if (!d.try_consume_none()) {
      auto b = d.parse_hsl_sl();
      if (!b) return std::nullopt;
      bv = *b;
    }
    details::normalize_hwb_whiteness_blackness(wv, bv);
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
    core::hwba_float_t hf{hue, static_cast<float>(wv), static_cast<float>(bv), alpha_f};
    core::rgba8_t out = operations::conversion::color_cast<core::rgba8_t>(hf);
    c.i = d.i;
    return out;
  };

  auto try_space = [&]() -> std::optional<core::rgba8_t> {
    c.i = checkpoint;
    details::Cursor d{c.s, c.i};
    float hue = 0.f;
    if (!d.try_consume_none()) {
      auto h = d.parse_hwb_hue_angle();
      if (!h) return std::nullopt;
      hue = static_cast<float>(*h);
    }
    d.skip_ws();
    double wv = 0.0;
    if (!d.try_consume_none()) {
      auto w = d.parse_hsl_sl();
      if (!w) return std::nullopt;
      wv = *w;
    }
    d.skip_ws();
    double bv = 0.0;
    if (!d.try_consume_none()) {
      auto b = d.parse_hsl_sl();
      if (!b) return std::nullopt;
      bv = *b;
    }
    details::normalize_hwb_whiteness_blackness(wv, bv);
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
    core::hwba_float_t hf{hue, static_cast<float>(wv), static_cast<float>(bv), alpha_f};
    core::rgba8_t out = operations::conversion::color_cast<core::rgba8_t>(hf);
    c.i = d.i;
    return out;
  };

  if (auto r = try_comma()) return r;
  if (auto r = try_space()) return r;
  return std::nullopt;
}

inline std::optional<core::rgbaf_t> parse_hwb_function_rgbaf(details::Cursor& c) {
  if (!c.consume_ci("hwb")) return std::nullopt;
  c.skip_ws();
  if (!c.consume_char('(')) return std::nullopt;
  c.skip_ws();

  size_t checkpoint = c.i;

  auto try_comma = [&]() -> std::optional<core::rgbaf_t> {
    c.i = checkpoint;
    details::Cursor d{c.s, c.i};
    float hue = 0.f;
    if (!d.try_consume_none()) {
      auto h = d.parse_hwb_hue_angle();
      if (!h) return std::nullopt;
      hue = static_cast<float>(*h);
    }
    d.skip_ws();
    if (!d.consume_char(',')) return std::nullopt;
    double wv = 0.0;
    if (!d.try_consume_none()) {
      auto w = d.parse_hsl_sl();
      if (!w) return std::nullopt;
      wv = *w;
    }
    d.skip_ws();
    if (!d.consume_char(',')) return std::nullopt;
    double bv = 0.0;
    if (!d.try_consume_none()) {
      auto b = d.parse_hsl_sl();
      if (!b) return std::nullopt;
      bv = *b;
    }
    details::normalize_hwb_whiteness_blackness(wv, bv);
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
    core::hwba_float_t hf{hue, static_cast<float>(wv), static_cast<float>(bv), alpha_f};
    c.i = d.i;
    return operations::conversion::color_cast<core::rgbaf_t>(hf);
  };

  auto try_space = [&]() -> std::optional<core::rgbaf_t> {
    c.i = checkpoint;
    details::Cursor d{c.s, c.i};
    float hue = 0.f;
    if (!d.try_consume_none()) {
      auto h = d.parse_hwb_hue_angle();
      if (!h) return std::nullopt;
      hue = static_cast<float>(*h);
    }
    d.skip_ws();
    double wv = 0.0;
    if (!d.try_consume_none()) {
      auto w = d.parse_hsl_sl();
      if (!w) return std::nullopt;
      wv = *w;
    }
    d.skip_ws();
    double bv = 0.0;
    if (!d.try_consume_none()) {
      auto b = d.parse_hsl_sl();
      if (!b) return std::nullopt;
      bv = *b;
    }
    details::normalize_hwb_whiteness_blackness(wv, bv);
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
    core::hwba_float_t hf{hue, static_cast<float>(wv), static_cast<float>(bv), alpha_f};
    c.i = d.i;
    return operations::conversion::color_cast<core::rgbaf_t>(hf);
  };

  if (auto r = try_comma()) return r;
  if (auto r = try_space()) return r;
  return std::nullopt;
}

}  // namespace colorcpp::io::css
