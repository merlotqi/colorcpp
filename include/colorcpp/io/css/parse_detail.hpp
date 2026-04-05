/**
 * @file parse_detail.hpp
 * @brief Internal CSS color parsing: atomic single-color dispatch, @c color-mix() helpers.
 */

#pragma once

#include <colorcpp/core/cielab.hpp>
#include <colorcpp/core/display_p3.hpp>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/io/css/color_function.hpp>
#include <colorcpp/io/css/details.hpp>
#include <colorcpp/io/css/device_cmyk.hpp>
#include <colorcpp/io/css/hex.hpp>
#include <colorcpp/io/css/hsl.hpp>
#include <colorcpp/io/css/hwb.hpp>
#include <colorcpp/io/css/lab.hpp>
#include <colorcpp/io/css/lch.hpp>
#include <colorcpp/io/css/named_colors.hpp>
#include <colorcpp/io/css/oklab.hpp>
#include <colorcpp/io/css/oklch.hpp>
#include <colorcpp/io/css/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/interpolate/lerp.hpp>
#include <cctype>
#include <optional>
#include <string_view>
#include <utility>

namespace colorcpp::io::css {

std::optional<core::rgba8_t> parse_css_color_rgba8(std::string_view str);
std::optional<core::rgbaf_t> parse_css_color_rgbaf(std::string_view str);

namespace css_parse_detail {

inline std::optional<std::pair<std::string_view, float>> split_color_and_optional_percent(std::string_view s) {
  details::trim(s);
  if (s.empty()) return std::nullopt;
  if (s.back() != '%') return std::make_optional(std::pair<std::string_view, float>(s, 0.5f));
  size_t i = s.size() - 1;
  if (i == 0) return std::nullopt;
  size_t num_tail = i;
  --i;
  while (i < s.size() && details::is_space(s[i])) --i;
  if (i >= s.size()) return std::nullopt;
  const size_t num_end = i + 1;
  size_t num_start = num_end;
  while (num_start > 0) {
    const char ch = s[num_start - 1];
    if (std::isdigit(static_cast<unsigned char>(ch)) || ch == '.' || ch == '-' || ch == '+') {
      --num_start;
    } else {
      break;
    }
  }
  if (num_start == num_end) return std::nullopt;
  const std::string_view num_sv = s.substr(num_start, num_end - num_start);
  details::Cursor nc{num_sv, 0};
  const auto num = nc.parse_number();
  if (!num) return std::nullopt;
  size_t color_end = num_start;
  while (color_end > 0 && details::is_space(s[color_end - 1])) --color_end;
  std::string_view color = s.substr(0, color_end);
  details::trim(color);
  if (color.empty()) return std::nullopt;
  const float w2 = static_cast<float>(std::clamp(*num, 0.0, 100.0) / 100.0);
  return std::pair<std::string_view, float>{color, w2};
}

inline std::optional<core::rgba8_t> try_atomic_rgba8_cursor(details::Cursor& c) {
  size_t a = c.i;
  while (a < c.s.size() && details::is_space(c.s[a])) ++a;
  size_t b = c.s.size();
  while (b > a && details::is_space(c.s[b - 1])) --b;
  if (a >= b) return std::nullopt;
  const std::string_view t = c.s.substr(a, b - a);

  if (t.front() == '#') {
    auto r = parse_hex(t);
    if (!r) return std::nullopt;
    c.i = b;
    return r;
  }
  if (auto named = get_named_color(t)) {
    c.i = b;
    return named;
  }

  details::Cursor sub{t, 0};
  if (auto r = parse_rgb_function(sub)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    return r;
  }
  sub.i = 0;
  if (auto r = parse_hsl_function(sub)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    return r;
  }
  sub.i = 0;
  if (auto r = parse_hwb_function(sub)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    return r;
  }
  sub.i = 0;
  if (auto r = parse_lab_function(sub)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    return operations::conversion::color_cast<core::rgba8_t>(*r);
  }
  sub.i = 0;
  if (auto r = parse_lch_function(sub)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    return operations::conversion::color_cast<core::rgba8_t>(*r);
  }
  sub.i = 0;
  if (auto r = parse_oklab_function(sub)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    return operations::conversion::color_cast<core::rgba8_t>(*r);
  }
  sub.i = 0;
  if (auto r = parse_oklch_function(sub)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    return operations::conversion::color_cast<core::rgba8_t>(*r);
  }
  sub.i = 0;
  if (auto r = parse_color_function_rgba8(sub)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    return r;
  }
  sub.i = 0;
  if (auto r = parse_device_cmyk_function(sub)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    return r;
  }
  return std::nullopt;
}

inline std::optional<core::rgba8_t> parse_css_color_rgba8_atomic_eof(std::string_view str) {
  details::Cursor c{str, 0};
  auto r = try_atomic_rgba8_cursor(c);
  if (!r) return std::nullopt;
  c.skip_ws();
  if (!c.eof()) return std::nullopt;
  return r;
}

inline std::optional<core::rgbaf_t> try_atomic_rgbaf_cursor(details::Cursor& c) {
  const size_t a0 = c.i;
  size_t a = a0;
  while (a < c.s.size() && details::is_space(c.s[a])) ++a;
  size_t b = c.s.size();
  while (b > a && details::is_space(c.s[b - 1])) --b;
  if (a >= b) return std::nullopt;
  const std::string_view t = c.s.substr(a, b - a);

  if (t.front() == '#') {
    auto r = parse_hex_rgbaf(t);
    if (!r) return std::nullopt;
    c.i = b;
    return r;
  }
  if (auto named = get_named_color(t)) {
    c.i = b;
    return operations::conversion::color_cast<core::rgbaf_t>(*named);
  }

  details::Cursor sub{t, 0};
  if (auto r = parse_rgb_function_rgbaf(sub)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    return r;
  }
  sub.i = 0;
  if (auto r = parse_hsl_function_rgbaf(sub)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    return r;
  }
  sub.i = 0;
  if (auto r = parse_hwb_function_rgbaf(sub)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    return r;
  }
  sub.i = 0;
  if (auto r = parse_lab_function(sub)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    return operations::conversion::color_cast<core::rgbaf_t>(*r);
  }
  sub.i = 0;
  if (auto r = parse_lch_function(sub)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    return operations::conversion::color_cast<core::rgbaf_t>(*r);
  }
  sub.i = 0;
  if (auto r = parse_oklab_function(sub)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    return operations::conversion::color_cast<core::rgbaf_t>(*r);
  }
  sub.i = 0;
  if (auto r = parse_oklch_function(sub)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    return operations::conversion::color_cast<core::rgbaf_t>(*r);
  }
  sub.i = 0;
  if (auto r = parse_color_function_rgbaf(sub)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    return r;
  }
  sub.i = 0;
  if (auto r = parse_device_cmyk_function(sub)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    return operations::conversion::color_cast<core::rgbaf_t>(*r);
  }
  return std::nullopt;
}

inline std::optional<core::rgbaf_t> parse_css_color_rgbaf_atomic_eof(std::string_view str) {
  details::Cursor c{str, 0};
  auto r = try_atomic_rgbaf_cursor(c);
  if (!r) return std::nullopt;
  c.skip_ws();
  if (!c.eof()) return std::nullopt;
  return r;
}

inline std::optional<core::rgba8_t> parse_color_mix_in_srgb_rgba8(details::Cursor& c) {
  const size_t save = c.i;
  if (!c.consume_ci("color-mix")) {
    c.i = save;
    return std::nullopt;
  }
  c.skip_ws();
  auto inner_opt = details::consume_parenthesized_contents(c);
  if (!inner_opt) {
    c.i = save;
    return std::nullopt;
  }
  std::string_view inner = *inner_opt;
  details::trim(inner);
  details::Cursor ic{inner, 0};
  if (!ic.consume_ci("in")) {
    c.i = save;
    return std::nullopt;
  }
  ic.skip_ws();
  if (!ic.consume_ci("srgb")) {
    c.i = save;
    return std::nullopt;
  }
  ic.skip_ws();
  if (!ic.consume_char(',')) {
    c.i = save;
    return std::nullopt;
  }
  const size_t p = ic.i;
  const size_t comma = details::find_top_level_comma(inner, p);
  if (comma == std::string_view::npos) {
    c.i = save;
    return std::nullopt;
  }
  std::string_view t1 = inner.substr(p, comma - p);
  std::string_view t2 = inner.substr(comma + 1);
  details::trim(t1);
  details::trim(t2);
  auto c1 = parse_css_color_rgba8(t1);
  if (!c1) {
    c.i = save;
    return std::nullopt;
  }
  const auto t2split = split_color_and_optional_percent(t2);
  if (!t2split) {
    c.i = save;
    return std::nullopt;
  }
  auto col2 = parse_css_color_rgba8_atomic_eof(t2split->first);
  if (!col2) {
    c.i = save;
    return std::nullopt;
  }
  const float w2 = t2split->second;
  auto mixed = operations::interpolate::lerp(*c1, *col2, w2);
  return operations::conversion::color_cast<core::rgba8_t>(mixed);
}

inline std::optional<core::rgbaf_t> parse_color_mix_in_srgb_rgbaf(details::Cursor& c) {
  const size_t save = c.i;
  if (!c.consume_ci("color-mix")) {
    c.i = save;
    return std::nullopt;
  }
  c.skip_ws();
  auto inner_opt = details::consume_parenthesized_contents(c);
  if (!inner_opt) {
    c.i = save;
    return std::nullopt;
  }
  std::string_view inner = *inner_opt;
  details::trim(inner);
  details::Cursor ic{inner, 0};
  if (!ic.consume_ci("in")) {
    c.i = save;
    return std::nullopt;
  }
  ic.skip_ws();
  if (!ic.consume_ci("srgb")) {
    c.i = save;
    return std::nullopt;
  }
  ic.skip_ws();
  if (!ic.consume_char(',')) {
    c.i = save;
    return std::nullopt;
  }
  const size_t p = ic.i;
  const size_t comma = details::find_top_level_comma(inner, p);
  if (comma == std::string_view::npos) {
    c.i = save;
    return std::nullopt;
  }
  std::string_view t1 = inner.substr(p, comma - p);
  std::string_view t2 = inner.substr(comma + 1);
  details::trim(t1);
  details::trim(t2);
  auto c1 = parse_css_color_rgbaf(t1);
  if (!c1) {
    c.i = save;
    return std::nullopt;
  }
  const auto t2split = split_color_and_optional_percent(t2);
  if (!t2split) {
    c.i = save;
    return std::nullopt;
  }
  auto col2 = parse_css_color_rgbaf_atomic_eof(t2split->first);
  if (!col2) {
    c.i = save;
    return std::nullopt;
  }
  return operations::interpolate::lerp(*c1, *col2, t2split->second);
}

}  // namespace css_parse_detail

}  // namespace colorcpp::io::css
