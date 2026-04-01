/**
 * @file rgb.hpp
 * @brief CSS rgb()/rgba() color parsing.
 */

#pragma once

#include <colorcpp/core/rgb.hpp>
#include <colorcpp/io/css/details.hpp>
#include <optional>
#include <string_view>

namespace colorcpp::io::css {

inline std::optional<core::rgba8_t> parse_rgb_function(details::Cursor& c) {
  bool rgba = c.consume_ci("rgba");
  if (!rgba && !c.consume_ci("rgb")) return std::nullopt;
  c.skip_ws();
  if (!c.consume_char('(')) return std::nullopt;
  c.skip_ws();

  size_t checkpoint = c.i;
  auto try_comma = [&]() -> std::optional<core::rgba8_t> {
    c.i = checkpoint;
    details::Cursor d{c.s, c.i};
    auto a = d.parse_component_value();
    if (!a) return std::nullopt;
    d.skip_ws();
    if (!d.consume_char(',')) return std::nullopt;
    auto b = d.parse_component_value();
    if (!b) return std::nullopt;
    d.skip_ws();
    if (!d.consume_char(',')) return std::nullopt;
    auto g = d.parse_component_value();
    if (!g) return std::nullopt;
    uint8_t r = static_cast<uint8_t>(d.channel_from_component(*a));
    uint8_t gch = static_cast<uint8_t>(d.channel_from_component(*b));
    uint8_t bch = static_cast<uint8_t>(d.channel_from_component(*g));
    d.skip_ws();
    uint8_t alpha = 255;
    if (d.consume_char(',')) {
      auto av = d.parse_alpha_value();
      if (!av) return std::nullopt;
      alpha = d.alpha_byte_from_double(*av);
    }
    d.skip_ws();
    if (!d.consume_char(')')) return std::nullopt;
    d.skip_ws();
    if (!d.eof()) return std::nullopt;
    c.i = d.i;
    return core::rgba8_t{r, gch, bch, alpha};
  };

  auto try_space = [&]() -> std::optional<core::rgba8_t> {
    c.i = checkpoint;
    details::Cursor d{c.s, c.i};
    auto a = d.parse_component_value();
    if (!a) return std::nullopt;
    d.skip_ws();
    auto b = d.parse_component_value();
    if (!b) return std::nullopt;
    d.skip_ws();
    auto g = d.parse_component_value();
    if (!g) return std::nullopt;
    uint8_t r = static_cast<uint8_t>(d.channel_from_component(*a));
    uint8_t gch = static_cast<uint8_t>(d.channel_from_component(*b));
    uint8_t bch = static_cast<uint8_t>(d.channel_from_component(*g));
    d.skip_ws();
    uint8_t alpha = 255;
    if (d.consume_char('/')) {
      auto av = d.parse_alpha_value();
      if (!av) return std::nullopt;
      alpha = d.alpha_byte_from_double(*av);
    }
    d.skip_ws();
    if (!d.consume_char(')')) return std::nullopt;
    d.skip_ws();
    if (!d.eof()) return std::nullopt;
    c.i = d.i;
    return core::rgba8_t{r, gch, bch, alpha};
  };

  if (auto r = try_comma()) return r;
  if (auto r = try_space()) return r;
  return std::nullopt;
}

}  // namespace colorcpp::io::css
