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
    auto h = d.parse_hwb_hue_angle();
    if (!h) return std::nullopt;
    d.skip_ws();
    if (!d.consume_char(',')) return std::nullopt;
    auto w = d.parse_hsl_sl();  // HWB whiteness is percentage (0-100%)
    if (!w) return std::nullopt;
    d.skip_ws();
    if (!d.consume_char(',')) return std::nullopt;
    auto b = d.parse_hsl_sl();  // HWB blackness is percentage (0-100%)
    if (!b) return std::nullopt;
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
    core::hwba_float_t hf{static_cast<float>(*h), static_cast<float>(*w), static_cast<float>(*b), alpha_f};
    core::rgba8_t out = operations::conversion::color_cast<core::rgba8_t>(hf);
    c.i = d.i;
    return out;
  };

  auto try_space = [&]() -> std::optional<core::rgba8_t> {
    c.i = checkpoint;
    details::Cursor d{c.s, c.i};
    auto h = d.parse_hwb_hue_angle();
    if (!h) return std::nullopt;
    d.skip_ws();
    auto w = d.parse_hsl_sl();  // HWB whiteness is percentage (0-100%)
    if (!w) return std::nullopt;
    d.skip_ws();
    auto b = d.parse_hsl_sl();  // HWB blackness is percentage (0-100%)
    if (!b) return std::nullopt;
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
    core::hwba_float_t hf{static_cast<float>(*h), static_cast<float>(*w), static_cast<float>(*b), alpha_f};
    core::rgba8_t out = operations::conversion::color_cast<core::rgba8_t>(hf);
    c.i = d.i;
    return out;
  };

  if (auto r = try_comma()) return r;
  if (auto r = try_space()) return r;
  return std::nullopt;
}

}  // namespace colorcpp::io::css
