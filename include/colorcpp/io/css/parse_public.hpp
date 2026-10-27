/**
 * @file parse_public.hpp
 * @brief Public CSS color string parsers, including context-aware overloads for ambient CSS values.
 */

#pragma once

#include <cmath>
#include <colorcpp/algorithms/accessibility/wcag.hpp>
#include <colorcpp/io/css/parse_detail.hpp>
#include <colorcpp/io/css/relative_color.hpp>
#include <optional>
#include <string_view>

namespace colorcpp::io::css {

inline std::optional<core::rgbaf_t> parse_css_color_rgbaf(std::string_view str, const parse_css_color_context& context);

// contrast-color(<color>) — CSS Color 5 §8. Resolves to white or black,
// whichever contrasts more as text on the argument used as a solid
// background; equal contrast resolves to white. The spec leaves the precise
// algorithm UA-defined; this subset uses WCAG 2.x relative luminance.
inline std::optional<core::rgbaf_t> parse_contrast_color_rgbaf(std::string_view str,
                                                               const parse_css_color_context& context) {
  details::trim(str);
  if (str.empty()) return std::nullopt;
  details::Cursor c{str, 0};
  if (!c.consume_ci("contrast-color")) return std::nullopt;
  c.skip_ws();
  if (!c.consume_char('(')) return std::nullopt;

  const size_t arg_start = c.i;
  int depth = 1;
  size_t close = std::string_view::npos;
  for (size_t i = c.i; i < str.size(); ++i) {
    if (str[i] == '(') {
      ++depth;
    } else if (str[i] == ')') {
      if (--depth == 0) {
        close = i;
        break;
      }
    }
  }
  if (close == std::string_view::npos) return std::nullopt;

  std::string_view arg = str.substr(arg_start, close - arg_start);
  details::trim(arg);
  if (arg.empty()) return std::nullopt;

  // The argument is a full <color> (light-dark/color-mix/relative/atomic);
  // contrast-color() itself is not absolute, so it is not accepted here.
  auto bg = parse_css_color_rgbaf(arg, context);
  if (!bg) return std::nullopt;

  c.i = close + 1;
  c.skip_ws();
  if (!c.eof()) return std::nullopt;

  // White contrast 1.05/(L+0.05) vs black (L+0.05)/0.05; equal at
  // L = sqrt(0.0525) - 0.05. Tie resolves to white per the spec.
  const float lum = algorithms::accessibility::relative_luminance(*bg);
  const double tie = std::sqrt(0.0525) - 0.05;
  if (static_cast<double>(lum) <= tie) return core::rgbaf_t{1.0f, 1.0f, 1.0f, 1.0f};
  return core::rgbaf_t{0.0f, 0.0f, 0.0f, 1.0f};
}

inline std::optional<core::rgba8_t> parse_css_color_rgba8(std::string_view str,
                                                          const parse_css_color_context& context) {
  details::trim(str);
  if (str.empty()) return std::nullopt;
  details::Cursor c{str, 0};
  if (auto r = css_parse_detail::parse_light_dark_rgba8(c, context)) {
    c.skip_ws();
    if (!c.eof()) return std::nullopt;
    return r;
  }
  c.i = 0;
  if (auto r = parse_contrast_color_rgbaf(str, context)) {
    return core::rgba8_t{static_cast<uint8_t>(r->r() * 255.0f + 0.5f), static_cast<uint8_t>(r->g() * 255.0f + 0.5f),
                         static_cast<uint8_t>(r->b() * 255.0f + 0.5f), static_cast<uint8_t>(r->a() * 255.0f + 0.5f)};
  }
  if (auto r = css_parse_detail::parse_color_mix_in_srgb_rgba8(c, context)) {
    c.skip_ws();
    if (!c.eof()) return std::nullopt;
    return r;
  }
  if (auto ast = relative_css_detail::parse_relative_css_color(str)) {
    auto evaluated = evaluate<core::rgba8_t>(parsed_css_color{std::move(*ast)}, context);
    if (!evaluated) return std::nullopt;
    return evaluated;
  }
  c.i = 0;
  return css_parse_detail::parse_css_color_rgba8_atomic_eof(str, context);
}

inline std::optional<core::rgba8_t> parse_css_color_rgba8(std::string_view str) {
  static const parse_css_color_context empty_context{};
  return parse_css_color_rgba8(str, empty_context);
}

inline std::optional<core::rgbaf_t> parse_css_color_rgbaf(std::string_view str,
                                                          const parse_css_color_context& context) {
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
  if (auto r = parse_contrast_color_rgbaf(str, context)) return r;
  if (auto ast = relative_css_detail::parse_relative_css_color(str)) {
    auto evaluated = evaluate<core::rgbaf_t>(parsed_css_color{std::move(*ast)}, context);
    if (!evaluated) return std::nullopt;
    return evaluated;
  }
  c.i = 0;
  return css_parse_detail::parse_css_color_rgbaf_atomic_eof(str, context);
}

inline std::optional<core::rgbaf_t> parse_css_color_rgbaf(std::string_view str) {
  static const parse_css_color_context empty_context{};
  return parse_css_color_rgbaf(str, empty_context);
}

inline std::optional<core::rgbaf_t> parse_color_mix_item_rgbaf(std::string_view str,
                                                               const parse_css_color_context& context) {
  // CSS Color 5: non-absolute colors are not permitted inside color-mix().
  // Relative colors and nested color-mix() items remain legal.
  details::trim(str);
  if (str.empty()) return std::nullopt;
  {
    details::Cursor c{str, 0};
    c.skip_ws();
    const size_t save = c.i;
    bool non_absolute_call = c.consume_ci("light-dark");
    if (!non_absolute_call) {
      c.i = save;
      non_absolute_call = c.consume_ci("contrast-color");
    }
    if (non_absolute_call) {
      c.skip_ws();
      if (c.consume_char('(')) return std::nullopt;
    }
  }
  return parse_css_color_rgbaf(str, context);
}

inline std::optional<core::rgba8_t> parse_css_color_light_dark_rgba8(std::string_view str,
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
