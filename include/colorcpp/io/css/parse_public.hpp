/**
 * @file parse_public.hpp
 * @brief Public CSS color string parsers: @c parse_css_color_rgba8, @c parse_css_color_rgbaf, @c light-dark.
 */

#pragma once

#include <colorcpp/io/css/parse_detail.hpp>
#include <optional>
#include <string_view>

namespace colorcpp::io::css {

inline std::optional<core::rgba8_t> parse_css_color_rgba8(std::string_view str) {
  details::trim(str);
  if (str.empty()) return std::nullopt;
  details::Cursor c{str, 0};
  if (auto r = css_parse_detail::parse_color_mix_in_srgb_rgba8(c)) {
    c.skip_ws();
    if (!c.eof()) return std::nullopt;
    return r;
  }
  c.i = 0;
  return css_parse_detail::parse_css_color_rgba8_atomic_eof(str);
}

inline std::optional<core::rgbaf_t> parse_css_color_rgbaf(std::string_view str) {
  details::trim(str);
  if (str.empty()) return std::nullopt;
  details::Cursor c{str, 0};
  if (auto r = css_parse_detail::parse_color_mix_in_srgb_rgbaf(c)) {
    c.skip_ws();
    if (!c.eof()) return std::nullopt;
    return r;
  }
  c.i = 0;
  return css_parse_detail::parse_css_color_rgbaf_atomic_eof(str);
}

inline std::optional<core::rgba8_t> parse_css_color_light_dark_rgba8(std::string_view str, bool dark_theme) {
  details::trim(str);
  details::Cursor c{str, 0};
  if (!c.consume_ci("light-dark")) return std::nullopt;
  auto inner_opt = details::consume_parenthesized_contents(c);
  if (!inner_opt) return std::nullopt;
  std::string_view inner = *inner_opt;
  details::trim(inner);
  const size_t comma = details::find_top_level_comma(inner, 0);
  if (comma == std::string_view::npos) return std::nullopt;
  std::string_view t1 = inner.substr(0, comma);
  std::string_view t2 = inner.substr(comma + 1);
  details::trim(t1);
  details::trim(t2);
  auto light = parse_css_color_rgba8(t1);
  auto dk = parse_css_color_rgba8(t2);
  if (!light || !dk) return std::nullopt;
  c.skip_ws();
  if (!c.eof()) return std::nullopt;
  return dark_theme ? dk : light;
}

}  // namespace colorcpp::io::css
