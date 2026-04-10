/**
 * @file parse_detail.hpp
 * @brief Internal CSS color parsing: atomic single-color dispatch, @c color-mix() helpers.
 */

#pragma once

#include <cctype>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/core/display_p3.hpp>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/io/css/color_function.hpp>
#include <colorcpp/io/css/context.hpp>
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
#include <optional>
#include <string_view>
#include <utility>

namespace colorcpp::io::css {

std::optional<core::rgba8_t> parse_css_color_rgba8(std::string_view str);
std::optional<core::rgba8_t> parse_css_color_rgba8(std::string_view str, const parse_css_color_context& context);
std::optional<core::rgbaf_t> parse_css_color_rgbaf(std::string_view str);
std::optional<core::rgbaf_t> parse_css_color_rgbaf(std::string_view str, const parse_css_color_context& context);

namespace css_parse_detail {

struct color_mix_operand {
  std::string_view color;
  std::optional<float> weight;
};

enum class color_mix_space {
  srgb,
  srgb_linear,
  lab,
  lch,
  oklab,
  oklch,
  xyz,
  display_p3,
  display_p3_linear,
};

inline std::optional<color_mix_operand> split_color_and_optional_percent(std::string_view s) {
  details::trim(s);
  if (s.empty()) return std::nullopt;
  if (s.back() != '%') return color_mix_operand{s, std::nullopt};
  size_t i = s.size() - 1;
  if (i == 0) return std::nullopt;
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
  const float weight = static_cast<float>(std::clamp(*num, 0.0, 100.0) / 100.0);
  return color_mix_operand{color, weight};
}

inline std::optional<std::pair<float, float>> resolve_color_mix_weights(const color_mix_operand& first,
                                                                        const color_mix_operand& second) {
  std::optional<float> w1 = first.weight;
  std::optional<float> w2 = second.weight;

  if (!w1 && !w2) {
    w1 = 0.5f;
    w2 = 0.5f;
  } else if (w1 && !w2) {
    w2 = 1.0f - *w1;
  } else if (!w1 && w2) {
    w1 = 1.0f - *w2;
  }

  const float a = std::clamp(*w1, 0.0f, 1.0f);
  const float b = std::clamp(*w2, 0.0f, 1.0f);
  const float sum = a + b;
  if (sum <= 0.0f) return std::nullopt;
  return std::pair<float, float>{a / sum, b / sum};
}

inline std::optional<color_mix_space> parse_color_mix_space(details::Cursor& c) {
  if (c.consume_ci("srgb-linear")) return color_mix_space::srgb_linear;
  if (c.consume_ci("srgb")) return color_mix_space::srgb;
  if (c.consume_ci("display-p3-linear")) return color_mix_space::display_p3_linear;
  if (c.consume_ci("display-p3")) return color_mix_space::display_p3;
  if (c.consume_ci("lab")) return color_mix_space::lab;
  if (c.consume_ci("lch")) return color_mix_space::lch;
  if (c.consume_ci("oklab")) return color_mix_space::oklab;
  if (c.consume_ci("oklch")) return color_mix_space::oklch;
  if (c.consume_ci("xyz-d50")) return color_mix_space::xyz;
  if (c.consume_ci("xyz-d65")) return color_mix_space::xyz;
  if (c.consume_ci("xyz")) return color_mix_space::xyz;
  return std::nullopt;
}

inline core::rgbaf_t mix_alpha(const core::rgbaf_t& out, const core::rgbaf_t& a, const core::rgbaf_t& b, float t) {
  auto with_alpha = out;
  with_alpha.a() = a.a() + (b.a() - a.a()) * t;
  return with_alpha;
}

template <typename Color3>
inline core::rgbaf_t mix_in_three_channel_space(const core::rgbaf_t& a, const core::rgbaf_t& b, float t) {
  auto ca = operations::conversion::color_cast<Color3>(a);
  auto cb = operations::conversion::color_cast<Color3>(b);
  Color3 mid{
      ca.template get_index<0>() + (cb.template get_index<0>() - ca.template get_index<0>()) * t,
      ca.template get_index<1>() + (cb.template get_index<1>() - ca.template get_index<1>()) * t,
      ca.template get_index<2>() + (cb.template get_index<2>() - ca.template get_index<2>()) * t,
  };
  return mix_alpha(operations::conversion::color_cast<core::rgbaf_t>(mid), a, b, t);
}

inline core::rgbaf_t mix_colors_in_space(color_mix_space space, const core::rgbaf_t& a, const core::rgbaf_t& b,
                                         float t) {
  switch (space) {
    case color_mix_space::srgb:
      return operations::interpolate::lerp(a, b, t);
    case color_mix_space::srgb_linear:
      return mix_in_three_channel_space<core::linear_rgbf_t>(a, b, t);
    case color_mix_space::lab:
      return operations::interpolate::lerp_lab<core::rgbaf_t>(a, b, t);
    case color_mix_space::lch:
      return operations::interpolate::lerp_lch<core::rgbaf_t>(a, b, t);
    case color_mix_space::oklab:
      return operations::interpolate::lerp_oklab<core::rgbaf_t>(a, b, t);
    case color_mix_space::oklch:
      return operations::interpolate::lerp_oklch<core::rgbaf_t>(a, b, t);
    case color_mix_space::xyz:
      return mix_in_three_channel_space<core::xyz_t>(a, b, t);
    case color_mix_space::display_p3:
      return mix_in_three_channel_space<core::display_p3f_t>(a, b, t);
    case color_mix_space::display_p3_linear:
      return mix_in_three_channel_space<core::linear_display_p3f_t>(a, b, t);
  }

  return operations::interpolate::lerp(a, b, t);
}

inline std::optional<core::rgbaf_t> parse_color_mix_rgbaf(details::Cursor& c, const parse_css_color_context& context) {
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
  auto space = parse_color_mix_space(ic);
  if (!space) {
    c.i = save;
    return std::nullopt;
  }
  ic.skip_ws();
  if (!ic.consume_char(',')) {
    c.i = save;
    return std::nullopt;
  }

  const size_t first_start = ic.i;
  const size_t comma = details::find_top_level_comma(inner, first_start);
  if (comma == std::string_view::npos) {
    c.i = save;
    return std::nullopt;
  }

  std::string_view raw_first = inner.substr(first_start, comma - first_start);
  std::string_view raw_second = inner.substr(comma + 1);
  details::trim(raw_first);
  details::trim(raw_second);

  const auto first = split_color_and_optional_percent(raw_first);
  const auto second = split_color_and_optional_percent(raw_second);
  if (!first || !second) {
    c.i = save;
    return std::nullopt;
  }

  const auto weights = resolve_color_mix_weights(*first, *second);
  if (!weights) {
    c.i = save;
    return std::nullopt;
  }

  auto c1 = parse_css_color_rgbaf(first->color, context);
  auto c2 = parse_css_color_rgbaf(second->color, context);
  if (!c1 || !c2) {
    c.i = save;
    return std::nullopt;
  }

  return mix_colors_in_space(*space, *c1, *c2, weights->second);
}

inline std::optional<core::rgbaf_t> resolve_context_color_rgbaf(std::string_view t,
                                                                const parse_css_color_context& context) {
  if (details::equals_ci(t, "currentcolor")) return context.current_color;
  if (details::equals_ci(t, "accentcolor")) return context.accent_color;
  if (details::equals_ci(t, "accentcolortext")) return context.accent_color_text;
  if (details::equals_ci(t, "activetext")) return context.active_text;
  if (details::equals_ci(t, "buttonborder")) return context.button_border;
  if (details::equals_ci(t, "buttonface")) return context.button_face;
  if (details::equals_ci(t, "buttontext")) return context.button_text;
  if (details::equals_ci(t, "canvas")) return context.canvas;
  if (details::equals_ci(t, "canvastext")) return context.canvas_text;
  if (details::equals_ci(t, "field")) return context.field;
  if (details::equals_ci(t, "fieldtext")) return context.field_text;
  if (details::equals_ci(t, "graytext")) return context.gray_text;
  if (details::equals_ci(t, "highlight")) return context.highlight;
  if (details::equals_ci(t, "highlighttext")) return context.highlight_text;
  if (details::equals_ci(t, "linktext")) return context.link_text;
  if (details::equals_ci(t, "mark")) return context.mark;
  if (details::equals_ci(t, "marktext")) return context.mark_text;
  if (details::equals_ci(t, "selecteditem")) return context.selected_item;
  if (details::equals_ci(t, "selecteditemtext")) return context.selected_item_text;
  if (details::equals_ci(t, "visitedtext")) return context.visited_text;
  return std::nullopt;
}

inline std::optional<core::rgba8_t> resolve_context_color_rgba8(std::string_view t,
                                                                const parse_css_color_context& context) {
  auto color = resolve_context_color_rgbaf(t, context);
  if (!color) return std::nullopt;
  return operations::conversion::color_cast<core::rgba8_t>(*color);
}

inline std::optional<core::rgba8_t> try_atomic_rgba8_cursor(details::Cursor& c,
                                                            const parse_css_color_context& context) {
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
  if (auto contextual = resolve_context_color_rgba8(t, context)) {
    c.i = b;
    return contextual;
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

inline std::optional<core::rgba8_t> try_atomic_rgba8_cursor(details::Cursor& c) {
  static const parse_css_color_context empty_context{};
  return try_atomic_rgba8_cursor(c, empty_context);
}

inline std::optional<core::rgba8_t> parse_css_color_rgba8_atomic_eof(std::string_view str,
                                                                     const parse_css_color_context& context) {
  details::Cursor c{str, 0};
  auto r = try_atomic_rgba8_cursor(c, context);
  if (!r) return std::nullopt;
  c.skip_ws();
  if (!c.eof()) return std::nullopt;
  return r;
}

inline std::optional<core::rgba8_t> parse_css_color_rgba8_atomic_eof(std::string_view str) {
  static const parse_css_color_context empty_context{};
  return parse_css_color_rgba8_atomic_eof(str, empty_context);
}

inline std::optional<core::rgbaf_t> try_atomic_rgbaf_cursor(details::Cursor& c,
                                                            const parse_css_color_context& context) {
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
  if (auto contextual = resolve_context_color_rgbaf(t, context)) {
    c.i = b;
    return contextual;
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
  if (auto r = parse_device_cmyk_function_rgbaf(sub)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    return r;
  }
  return std::nullopt;
}

inline std::optional<core::rgbaf_t> try_atomic_rgbaf_cursor(details::Cursor& c) {
  static const parse_css_color_context empty_context{};
  return try_atomic_rgbaf_cursor(c, empty_context);
}

inline std::optional<core::rgbaf_t> parse_css_color_rgbaf_atomic_eof(std::string_view str,
                                                                     const parse_css_color_context& context) {
  details::Cursor c{str, 0};
  auto r = try_atomic_rgbaf_cursor(c, context);
  if (!r) return std::nullopt;
  c.skip_ws();
  if (!c.eof()) return std::nullopt;
  return r;
}

inline std::optional<core::rgbaf_t> parse_css_color_rgbaf_atomic_eof(std::string_view str) {
  static const parse_css_color_context empty_context{};
  return parse_css_color_rgbaf_atomic_eof(str, empty_context);
}

inline std::optional<core::rgba8_t> parse_light_dark_rgba8(details::Cursor& c, const parse_css_color_context& context) {
  const size_t save = c.i;
  if (!c.consume_ci("light-dark")) {
    c.i = save;
    return std::nullopt;
  }
  auto inner_opt = details::consume_parenthesized_contents(c);
  if (!inner_opt) {
    c.i = save;
    return std::nullopt;
  }
  std::string_view inner = *inner_opt;
  details::trim(inner);
  const size_t comma = details::find_top_level_comma(inner, 0);
  if (comma == std::string_view::npos) {
    c.i = save;
    return std::nullopt;
  }
  std::string_view t1 = inner.substr(0, comma);
  std::string_view t2 = inner.substr(comma + 1);
  details::trim(t1);
  details::trim(t2);
  auto light = parse_css_color_rgba8(t1, context);
  auto dark = parse_css_color_rgba8(t2, context);
  if (!light || !dark) {
    c.i = save;
    return std::nullopt;
  }
  return context.dark_theme ? dark : light;
}

inline std::optional<core::rgbaf_t> parse_light_dark_rgbaf(details::Cursor& c, const parse_css_color_context& context) {
  const size_t save = c.i;
  if (!c.consume_ci("light-dark")) {
    c.i = save;
    return std::nullopt;
  }
  auto inner_opt = details::consume_parenthesized_contents(c);
  if (!inner_opt) {
    c.i = save;
    return std::nullopt;
  }
  std::string_view inner = *inner_opt;
  details::trim(inner);
  const size_t comma = details::find_top_level_comma(inner, 0);
  if (comma == std::string_view::npos) {
    c.i = save;
    return std::nullopt;
  }
  std::string_view t1 = inner.substr(0, comma);
  std::string_view t2 = inner.substr(comma + 1);
  details::trim(t1);
  details::trim(t2);
  auto light = parse_css_color_rgbaf(t1, context);
  auto dark = parse_css_color_rgbaf(t2, context);
  if (!light || !dark) {
    c.i = save;
    return std::nullopt;
  }
  return context.dark_theme ? dark : light;
}

inline std::optional<core::rgba8_t> parse_color_mix_in_srgb_rgba8(details::Cursor& c,
                                                                  const parse_css_color_context& context) {
  auto mixed = parse_color_mix_rgbaf(c, context);
  if (!mixed) return std::nullopt;
  return operations::conversion::color_cast<core::rgba8_t>(*mixed);
}

inline std::optional<core::rgba8_t> parse_color_mix_in_srgb_rgba8(details::Cursor& c) {
  static const parse_css_color_context empty_context{};
  return parse_color_mix_in_srgb_rgba8(c, empty_context);
}

inline std::optional<core::rgbaf_t> parse_color_mix_in_srgb_rgbaf(details::Cursor& c,
                                                                  const parse_css_color_context& context) {
  return parse_color_mix_rgbaf(c, context);
}

inline std::optional<core::rgbaf_t> parse_color_mix_in_srgb_rgbaf(details::Cursor& c) {
  static const parse_css_color_context empty_context{};
  return parse_color_mix_in_srgb_rgbaf(c, empty_context);
}

}  // namespace css_parse_detail

}  // namespace colorcpp::io::css
