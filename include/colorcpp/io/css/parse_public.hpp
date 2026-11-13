/**
 * @file parse_public.hpp
 * @brief Public CSS color string parsers, including context-aware overloads for ambient CSS values.
 */

#pragma once

#include <cmath>
#include <colorcpp/algorithms/accessibility/wcag.hpp>
#include <colorcpp/io/css/display_p3.hpp>
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
  // One step of the shared nesting budget (issue #63): light-dark(),
  // color-mix(), contrast-color() and relative colors re-enter this entry.
  details::css_nesting_guard guard;
  if (details::css_nesting_exceeded()) return std::nullopt;
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
  // One step of the shared nesting budget (issue #63); see rgba8 overload.
  details::css_nesting_guard guard;
  if (details::css_nesting_exceeded()) return std::nullopt;
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

// ===== parse_css_color<Color> template API =====
// Lives here (was parse_templates.hpp) so parse_color_mix_item_space below
// can see it: color-mix() resolves its items into the interpolation space
// (issue #110), and this header is where that helper must be defined for
// every translation unit that reaches resolve_color_mix_items.

template <typename Color>
inline std::optional<Color> parse_css_color(std::string_view str);

template <typename Color>
inline std::optional<Color> parse_css_color(std::string_view str, const parse_css_color_context& context);

namespace parse_templates_detail {

template <typename Color>
inline std::optional<Color> parse_exact_css_color(std::string_view) {
  return std::nullopt;
}

template <>
inline std::optional<core::cielab_t> parse_exact_css_color<core::cielab_t>(std::string_view str) {
  details::trim(str);
  if (str.empty()) return std::nullopt;
  details::Cursor c{str, 0};
  if (auto r = parse_lab_function(c)) return r;
  return std::nullopt;
}

template <>
inline std::optional<core::cielch_t> parse_exact_css_color<core::cielch_t>(std::string_view str) {
  details::trim(str);
  if (str.empty()) return std::nullopt;
  details::Cursor c{str, 0};
  if (auto r = parse_lch_function(c)) return r;
  return std::nullopt;
}

template <>
inline std::optional<core::oklab_t> parse_exact_css_color<core::oklab_t>(std::string_view str) {
  details::trim(str);
  if (str.empty()) return std::nullopt;
  details::Cursor c{str, 0};
  if (auto r = parse_oklab_function(c)) return r;
  return std::nullopt;
}

template <>
inline std::optional<core::oklch_t> parse_exact_css_color<core::oklch_t>(std::string_view str) {
  details::trim(str);
  if (str.empty()) return std::nullopt;
  details::Cursor c{str, 0};
  if (auto r = parse_oklch_function(c)) return r;
  return std::nullopt;
}

template <>
inline std::optional<core::display_p3f_t> parse_exact_css_color<core::display_p3f_t>(std::string_view str) {
  details::trim(str);
  if (str.empty()) return std::nullopt;
  details::Cursor c{str, 0};
  if (auto r = parse_display_p3_function(c)) return r;
  return std::nullopt;
}

template <typename Color>
inline std::optional<Color> parse_css_color_impl(std::string_view str) {
  details::trim(str);
  if (str.empty()) return std::nullopt;
  if (auto exact = parse_exact_css_color<Color>(str)) return exact;
  details::Cursor c{str, 0};
  if (auto typed = parse_color_function_as<Color>(c)) return typed;
  if (auto ast = relative_css_detail::parse_relative_css_color(str)) {
    auto evaluated = evaluate<Color>(parsed_css_color{std::move(*ast)}, parse_css_color_context{});
    if (evaluated) return evaluated;
  }
  auto parsed = parse_css_color_rgbaf(str);
  if (!parsed) return std::nullopt;
  return operations::conversion::color_cast<Color>(*parsed);
}

template <typename Color>
inline std::optional<Color> parse_css_color_impl(std::string_view str, const parse_css_color_context& context) {
  details::trim(str);
  if (str.empty()) return std::nullopt;
  if (auto exact = parse_exact_css_color<Color>(str)) return exact;
  details::Cursor c{str, 0};
  if (auto typed = parse_color_function_as<Color>(c)) return typed;
  if (auto ast = relative_css_detail::parse_relative_css_color(str)) {
    auto evaluated = evaluate<Color>(parsed_css_color{std::move(*ast)}, context);
    if (evaluated) return evaluated;
  }
  auto parsed = parse_css_color_rgbaf(str, context);
  if (!parsed) return std::nullopt;
  return operations::conversion::color_cast<Color>(*parsed);
}

}  // namespace parse_templates_detail

template <>
inline std::optional<core::rgbaf_t> parse_css_color<core::rgbaf_t>(std::string_view str) {
  return parse_css_color_rgbaf(str);
}

template <typename Color>
inline std::optional<Color> parse_css_color(std::string_view str) {
  return parse_templates_detail::parse_css_color_impl<Color>(str);
}

template <>
inline std::optional<core::rgbaf_t> parse_css_color<core::rgbaf_t>(std::string_view str,
                                                                   const parse_css_color_context& context) {
  return parse_css_color_rgbaf(str, context);
}

template <typename Color>
inline std::optional<Color> parse_css_color(std::string_view str, const parse_css_color_context& context) {
  return parse_templates_detail::parse_css_color_impl<Color>(str, context);
}

namespace css_parse_detail {

/**
 * @brief Parse a color-mix() item straight into @p space (issue #110).
 *
 * Two passes over the same string: the typed parse carries the channels into
 * the interpolation space without an intermediate sRGB gamut clamp (wide-gamut
 * inputs like display-p3 green keep R<0/G>1 on the way in), and the rgbaf pass
 * supplies alpha — its gamut clamp never touches alpha — and rejects
 * light-dark()/contrast-color() (CSS Color 5), which the typed parse would
 * otherwise resolve. Returns [r, g, b, a] in @p space.
 */
inline std::optional<std::array<float, 4>> parse_color_mix_item_space(std::string_view str, color_mix_space space,
                                                                      const parse_css_color_context& context) {
  auto full = parse_color_mix_item_rgbaf(str, context);
  if (!full) return std::nullopt;

  std::array<float, 4> out{};
  const auto assign = [&](const auto& c) {
    out[0] = c.template get_index<0>();
    out[1] = c.template get_index<1>();
    out[2] = c.template get_index<2>();
    out[3] = full->a();
  };

  switch (space) {
    case color_mix_space::srgb: {
      auto c = parse_css_color<core::rgbf_t>(str, context);
      if (!c) return std::nullopt;
      assign(*c);
      break;
    }
    case color_mix_space::srgb_linear: {
      auto c = parse_css_color<core::linear_rgbf_t>(str, context);
      if (!c) return std::nullopt;
      assign(*c);
      break;
    }
    case color_mix_space::lab: {
      auto c = parse_css_color<core::cielab_t>(str, context);
      if (!c) return std::nullopt;
      assign(*c);
      break;
    }
    case color_mix_space::lch: {
      auto c = parse_css_color<core::cielch_t>(str, context);
      if (!c) return std::nullopt;
      assign(*c);
      break;
    }
    case color_mix_space::oklab: {
      auto c = parse_css_color<core::oklab_t>(str, context);
      if (!c) return std::nullopt;
      assign(*c);
      break;
    }
    case color_mix_space::oklch: {
      auto c = parse_css_color<core::oklch_t>(str, context);
      if (!c) return std::nullopt;
      assign(*c);
      break;
    }
    case color_mix_space::xyz: {
      auto c = parse_css_color<core::xyz_t>(str, context);
      if (!c) return std::nullopt;
      assign(*c);
      break;
    }
    case color_mix_space::display_p3: {
      auto c = parse_css_color<core::display_p3f_t>(str, context);
      if (!c) return std::nullopt;
      assign(*c);
      break;
    }
    case color_mix_space::display_p3_linear: {
      auto c = parse_css_color<core::linear_display_p3f_t>(str, context);
      if (!c) return std::nullopt;
      assign(*c);
      break;
    }
    case color_mix_space::a98_rgb: {
      auto c = parse_css_color<core::adobe_rgbf_t>(str, context);
      if (!c) return std::nullopt;
      assign(*c);
      break;
    }
    case color_mix_space::a98_rgb_linear: {
      auto c = parse_css_color<core::linear_adobe_rgbf_t>(str, context);
      if (!c) return std::nullopt;
      assign(*c);
      break;
    }
    case color_mix_space::prophoto_rgb: {
      auto c = parse_css_color<core::prophoto_rgbf_t>(str, context);
      if (!c) return std::nullopt;
      assign(*c);
      break;
    }
    case color_mix_space::prophoto_rgb_linear: {
      auto c = parse_css_color<core::linear_prophoto_rgbf_t>(str, context);
      if (!c) return std::nullopt;
      assign(*c);
      break;
    }
  }
  return out;
}

}  // namespace css_parse_detail

}  // namespace colorcpp::io::css
