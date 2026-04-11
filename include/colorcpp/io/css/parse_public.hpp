/**
 * @file parse_public.hpp
 * @brief Public CSS color string parsers, including context-aware overloads for ambient CSS values.
 */

#pragma once

#include <colorcpp/io/css/parse_detail.hpp>
#include <optional>
#include <string_view>

namespace colorcpp::io::css {

inline std::optional<core::rgba8_t> parse_css_color_rgba8(std::string_view str, const parse_css_color_context& context) {
  details::trim(str);
  if (str.empty()) return std::nullopt;
  details::Cursor c{str, 0};
  if (auto r = css_parse_detail::parse_light_dark_rgba8(c, context)) {
    c.skip_ws();
    if (!c.eof()) return std::nullopt;
    return r;
  }
  c.i = 0;
  if (auto r = css_parse_detail::parse_color_mix_in_srgb_rgba8(c, context)) {
    c.skip_ws();
    if (!c.eof()) return std::nullopt;
    return r;
  }
  c.i = 0;
  return css_parse_detail::parse_css_color_rgba8_atomic_eof(str, context);
}

inline std::optional<core::rgba8_t> parse_css_color_rgba8(std::string_view str) {
  static const parse_css_color_context empty_context{};
  return parse_css_color_rgba8(str, empty_context);
}

inline std::optional<core::rgbaf_t> parse_css_color_rgbaf(std::string_view str, const parse_css_color_context& context) {
  details::trim(str);
  if (str.empty()) return std::nullopt;
  details::Cursor c{str, 0};
  if (auto r = css_parse_detail::parse_light_dark_rgbaf(c, context)) {
    c.skip_ws();
    if (!c.eof()) return std::nullopt;
    return r;
  }
  c.i = 0;
  if (auto r = css_parse_detail::parse_color_mix_in_srgb_rgbaf(c, context)) {
    c.skip_ws();
    if (!c.eof()) return std::nullopt;
    return r;
  }
  c.i = 0;
  return css_parse_detail::parse_css_color_rgbaf_atomic_eof(str, context);
}

inline std::optional<core::rgbaf_t> parse_css_color_rgbaf(std::string_view str) {
  static const parse_css_color_context empty_context{};
  return parse_css_color_rgbaf(str, empty_context);
}

inline std::optional<core::rgba8_t> parse_css_color_light_dark_rgba8(
    std::string_view str,
    const parse_css_color_context& context) {
  details::trim(str);
  details::Cursor c{str, 0};
  auto resolved = css_parse_detail::parse_light_dark_rgba8(c, context);
  if (!resolved) return std::nullopt;
  c.skip_ws();
  if (!c.eof()) return std::nullopt;
  return resolved;
}

inline std::optional<core::rgba8_t> parse_css_color_light_dark_rgba8(std::string_view str, bool dark_theme) {
  parse_css_color_context context;
  context.dark_theme = dark_theme;
  return parse_css_color_light_dark_rgba8(str, context);
}

}  // namespace colorcpp::io::css
