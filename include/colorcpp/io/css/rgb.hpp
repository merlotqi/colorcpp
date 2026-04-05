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

namespace details_rgb {

inline std::optional<uint8_t> parse_rgb8_channel(details::Cursor& d) {
  if (d.try_consume_none()) return 0;
  auto a = d.parse_component_value();
  if (!a) return std::nullopt;
  return static_cast<uint8_t>(d.channel_from_component(*a));
}

inline std::optional<float> parse_rgbf_channel(details::Cursor& d) {
  if (d.try_consume_none()) return 0.f;
  auto a = d.parse_component_value();
  if (!a) return std::nullopt;
  return d.channel_float_01_from_rgb_component(*a);
}

}  // namespace details_rgb

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
    auto rch = details_rgb::parse_rgb8_channel(d);
    if (!rch) return std::nullopt;
    d.skip_ws();
    if (!d.consume_char(',')) return std::nullopt;
    auto gch = details_rgb::parse_rgb8_channel(d);
    if (!gch) return std::nullopt;
    d.skip_ws();
    if (!d.consume_char(',')) return std::nullopt;
    auto bch = details_rgb::parse_rgb8_channel(d);
    if (!bch) return std::nullopt;
    uint8_t r = *rch;
    uint8_t g = *gch;
    uint8_t b = *bch;
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
    return core::rgba8_t{r, g, b, alpha};
  };

  auto try_space = [&]() -> std::optional<core::rgba8_t> {
    c.i = checkpoint;
    details::Cursor d{c.s, c.i};
    auto rch = details_rgb::parse_rgb8_channel(d);
    if (!rch) return std::nullopt;
    d.skip_ws();
    auto gch = details_rgb::parse_rgb8_channel(d);
    if (!gch) return std::nullopt;
    d.skip_ws();
    auto bch = details_rgb::parse_rgb8_channel(d);
    if (!bch) return std::nullopt;
    uint8_t r = *rch;
    uint8_t g = *gch;
    uint8_t b = *bch;
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
    return core::rgba8_t{r, g, b, alpha};
  };

  if (auto r = try_comma()) return r;
  if (auto r = try_space()) return r;
  return std::nullopt;
}

/** @brief Parse @c rgb() / @c rgba() into float sRGB (supports @c none as 0). */
inline std::optional<core::rgbaf_t> parse_rgb_function_rgbaf(details::Cursor& c) {
  bool rgba = c.consume_ci("rgba");
  if (!rgba && !c.consume_ci("rgb")) return std::nullopt;
  c.skip_ws();
  if (!c.consume_char('(')) return std::nullopt;
  c.skip_ws();

  size_t checkpoint = c.i;
  auto try_comma = [&]() -> std::optional<core::rgbaf_t> {
    c.i = checkpoint;
    details::Cursor d{c.s, c.i};
    auto rch = details_rgb::parse_rgbf_channel(d);
    if (!rch) return std::nullopt;
    d.skip_ws();
    if (!d.consume_char(',')) return std::nullopt;
    auto gch = details_rgb::parse_rgbf_channel(d);
    if (!gch) return std::nullopt;
    d.skip_ws();
    if (!d.consume_char(',')) return std::nullopt;
    auto bch = details_rgb::parse_rgbf_channel(d);
    if (!bch) return std::nullopt;
    float alpha = 1.f;
    d.skip_ws();
    if (d.consume_char(',')) {
      auto av = d.parse_alpha_value();
      if (!av) return std::nullopt;
      double a = std::clamp(*av, 0.0, 1.0);
      alpha = static_cast<float>(a);
    }
    d.skip_ws();
    if (!d.consume_char(')')) return std::nullopt;
    d.skip_ws();
    if (!d.eof()) return std::nullopt;
    c.i = d.i;
    return core::rgbaf_t{*rch, *gch, *bch, alpha};
  };

  auto try_space = [&]() -> std::optional<core::rgbaf_t> {
    c.i = checkpoint;
    details::Cursor d{c.s, c.i};
    auto rch = details_rgb::parse_rgbf_channel(d);
    if (!rch) return std::nullopt;
    d.skip_ws();
    auto gch = details_rgb::parse_rgbf_channel(d);
    if (!gch) return std::nullopt;
    d.skip_ws();
    auto bch = details_rgb::parse_rgbf_channel(d);
    if (!bch) return std::nullopt;
    float alpha = 1.f;
    d.skip_ws();
    if (d.consume_char('/')) {
      auto av = d.parse_alpha_value();
      if (!av) return std::nullopt;
      alpha = static_cast<float>(std::clamp(*av, 0.0, 1.0));
    }
    d.skip_ws();
    if (!d.consume_char(')')) return std::nullopt;
    d.skip_ws();
    if (!d.eof()) return std::nullopt;
    c.i = d.i;
    return core::rgbaf_t{*rch, *gch, *bch, alpha};
  };

  if (auto r = try_comma()) return r;
  if (auto r = try_space()) return r;
  return std::nullopt;
}

}  // namespace colorcpp::io::css
