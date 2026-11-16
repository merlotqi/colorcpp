/**
 * @file parse_detail.hpp
 * @brief Internal CSS color parsing: atomic single-color dispatch, @c color-mix() helpers.
 */

#pragma once

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <colorcpp/core/adobe_rgb.hpp>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/core/display_p3.hpp>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/prophoto_rgb.hpp>
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
#include <vector>

namespace colorcpp::io::css {

std::optional<core::rgba8_t> parse_css_color_rgba8(std::string_view str);
std::optional<core::rgba8_t> parse_css_color_rgba8(std::string_view str, const parse_css_color_context& context);
std::optional<core::rgbaf_t> parse_css_color_rgbaf(std::string_view str);
std::optional<core::rgbaf_t> parse_css_color_rgbaf(std::string_view str, const parse_css_color_context& context);

// color-mix() item colors: full <color> minus the non-absolute forms
// (light-dark(), contrast-color()) that CSS Color 5 forbids inside color-mix().
std::optional<core::rgbaf_t> parse_color_mix_item_rgbaf(std::string_view str, const parse_css_color_context& context);

namespace css_parse_detail {

struct color_mix_item {
  std::string_view color;
  std::optional<float> weight;
};

enum class color_mix_hue_method {
  shorter,
  longer,
  increasing,
  decreasing,
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
  a98_rgb,
  a98_rgb_linear,
  prophoto_rgb,
  prophoto_rgb_linear,
};

// Item parsed straight into the interpolation space (issue #110): the typed
// parse keeps wide-gamut inputs unclamped on the way in. Declared here,
// defined in parse_public.hpp next to parse_css_color<Color> — that template
// lives above this header in the include graph. Returns [r, g, b, a] with the
// channels already in @p space.
std::optional<std::array<float, 4>> parse_color_mix_item_space(std::string_view str, color_mix_space space,
                                                               const parse_css_color_context& context);

struct color_mix_interpolation_method {
  color_mix_space space;
  color_mix_hue_method hue_method = color_mix_hue_method::shorter;
};

struct color_mix_weights {
  float first = 0.5f;
  float second = 0.5f;
  float alpha_multiplier = 1.0f;
};

struct parsed_color_mix {
  color_mix_interpolation_method method{color_mix_space::oklab};
  std::vector<color_mix_item> items;
};

struct color_mix_progress_header {
  color_mix_interpolation_method method{color_mix_space::oklab};
  float progress = 0.5f;
};

struct resolved_color_mix_item {
  // [r, g, b, a] already in resolved_color_mix::method.space — not sRGB.
  // Keeping items in the interpolation space is what stops a wide-gamut
  // input from being gamut-clamped to sRGB before the mix (issue #110).
  std::array<float, 4> color{};
  float weight = 0.0f;
};

struct resolved_color_mix {
  color_mix_interpolation_method method{color_mix_space::oklab};
  std::vector<resolved_color_mix_item> items;
  float alpha_multiplier = 1.0f;
};

inline std::optional<std::vector<std::string_view>> split_top_level_comma_list(std::string_view s) {
  std::vector<std::string_view> parts;
  size_t start = 0;
  while (start <= s.size()) {
    const size_t comma = details::find_top_level_comma(s, start);
    std::string_view part = comma == std::string_view::npos ? s.substr(start) : s.substr(start, comma - start);
    details::trim(part);
    if (part.empty()) return std::nullopt;
    parts.push_back(part);
    if (comma == std::string_view::npos) break;
    start = comma + 1;
  }
  return parts;
}

inline std::optional<std::pair<float, std::string_view>> split_optional_leading_percent(std::string_view s) {
  details::Cursor c{s, 0};
  auto cv = c.parse_component_value();
  if (!cv || !cv->second) return std::nullopt;
  std::string_view color = s.substr(c.i);
  details::trim(color);
  if (color.empty()) return std::nullopt;
  const float weight = static_cast<float>(std::clamp(cv->first, 0.0, 100.0) / 100.0);
  return std::pair<float, std::string_view>{weight, color};
}

inline std::optional<std::pair<std::string_view, float>> split_optional_trailing_percent(std::string_view s) {
  details::trim(s);
  if (s.empty() || s.back() != '%') return std::nullopt;
  size_t i = s.size() - 1;
  if (i == 0) return std::nullopt;
  --i;
  while (i < s.size() && details::is_space(s[i])) --i;
  if (i >= s.size()) return std::nullopt;
  const size_t number_end = i + 1;
  size_t number_start = number_end;
  while (number_start > 0) {
    const char ch = s[number_start - 1];
    if (std::isdigit(static_cast<unsigned char>(ch)) || ch == '.' || ch == '-' || ch == '+') {
      --number_start;
    } else {
      break;
    }
  }

  if (number_start == number_end) return std::nullopt;

  details::Cursor nc{s.substr(number_start, number_end - number_start), 0};
  auto number = nc.parse_number();
  if (!number) return std::nullopt;

  size_t color_end = number_start;
  while (color_end > 0 && details::is_space(s[color_end - 1])) --color_end;
  std::string_view color = s.substr(0, color_end);
  details::trim(color);
  if (color.empty()) return std::nullopt;

  const float weight = static_cast<float>(std::clamp(*number, 0.0, 100.0) / 100.0);
  return std::pair<std::string_view, float>{color, weight};
}

inline bool is_bare_percentage_token(std::string_view s) {
  details::Cursor c{s, 0};
  auto cv = c.parse_component_value();
  if (!cv || !cv->second) return false;
  c.skip_ws();
  return c.eof();
}

inline std::optional<color_mix_item> parse_color_mix_item(std::string_view s) {
  details::trim(s);
  if (s.empty()) return std::nullopt;
  if (is_bare_percentage_token(s)) return std::nullopt;

  if (auto leading = split_optional_leading_percent(s)) {
    return color_mix_item{leading->second, leading->first};
  }
  if (auto trailing = split_optional_trailing_percent(s)) {
    return color_mix_item{trailing->first, trailing->second};
  }
  return color_mix_item{s, std::nullopt};
}

inline std::optional<color_mix_hue_method> parse_color_mix_hue_method(details::Cursor& c) {
  if (c.consume_ci("shorter")) return color_mix_hue_method::shorter;
  if (c.consume_ci("longer")) return color_mix_hue_method::longer;
  if (c.consume_ci("increasing")) return color_mix_hue_method::increasing;
  if (c.consume_ci("decreasing")) return color_mix_hue_method::decreasing;
  return std::nullopt;
}

inline bool is_color_mix_polar_space(color_mix_space space) {
  return space == color_mix_space::lch || space == color_mix_space::oklch;
}

inline std::optional<color_mix_interpolation_method> parse_color_mix_space(details::Cursor& c) {
  std::optional<color_mix_space> space;
  if (c.consume_ci("srgb-linear"))
    space = color_mix_space::srgb_linear;
  else if (c.consume_ci("srgb"))
    space = color_mix_space::srgb;
  else if (c.consume_ci("display-p3-linear"))
    space = color_mix_space::display_p3_linear;
  else if (c.consume_ci("display-p3"))
    space = color_mix_space::display_p3;
  else if (c.consume_ci("a98-rgb-linear"))
    space = color_mix_space::a98_rgb_linear;
  else if (c.consume_ci("a98-rgb"))
    space = color_mix_space::a98_rgb;
  else if (c.consume_ci("prophoto-rgb-linear"))
    space = color_mix_space::prophoto_rgb_linear;
  else if (c.consume_ci("prophoto-rgb"))
    space = color_mix_space::prophoto_rgb;
  else if (c.consume_ci("lab"))
    space = color_mix_space::lab;
  else if (c.consume_ci("lch"))
    space = color_mix_space::lch;
  else if (c.consume_ci("oklab"))
    space = color_mix_space::oklab;
  else if (c.consume_ci("oklch"))
    space = color_mix_space::oklch;
  else if (c.consume_ci("xyz-d50"))
    space = color_mix_space::xyz;
  else if (c.consume_ci("xyz-d65"))
    space = color_mix_space::xyz;
  else if (c.consume_ci("xyz"))
    space = color_mix_space::xyz;

  if (!space) return std::nullopt;

  color_mix_interpolation_method method{*space};
  const size_t save = c.i;
  if (auto hue_method = parse_color_mix_hue_method(c)) {
    if (!c.consume_ci("hue")) return std::nullopt;
    if (!is_color_mix_polar_space(*space)) return std::nullopt;
    method.hue_method = *hue_method;
    return method;
  }

  c.i = save;
  return method;
}

inline std::optional<parsed_color_mix> parse_color_mix_item_list(const std::vector<std::string_view>& parts) {
  if (parts.empty()) return std::nullopt;

  parsed_color_mix parsed;
  size_t first_item_index = 0;

  details::Cursor header{parts.front(), 0};
  if (header.consume_ci("in")) {
    header.skip_ws();
    auto method = parse_color_mix_space(header);
    if (!method) return std::nullopt;
    header.skip_ws();
    if (!header.eof()) return std::nullopt;
    parsed.method = *method;
    first_item_index = 1;
  }

  if (first_item_index >= parts.size()) return std::nullopt;
  for (size_t index = first_item_index; index < parts.size(); ++index) {
    auto item = parse_color_mix_item(parts[index]);
    if (!item) return std::nullopt;
    parsed.items.push_back(*item);
  }
  return parsed;
}

inline std::optional<float> parse_color_mix_progress_value(details::Cursor& c) {
  auto value = c.parse_alpha_value();
  if (!value) return std::nullopt;
  return static_cast<float>(std::clamp(*value, 0.0, 1.0));
}

inline std::optional<color_mix_progress_header> parse_color_mix_progress_header(std::string_view s) {
  details::Cursor c{s, 0};
  color_mix_progress_header header;
  bool saw_method = false;
  bool saw_progress = false;

  while (!c.eof()) {
    const size_t before = c.i;
    if (!saw_method && c.consume_ci("in")) {
      c.skip_ws();
      auto method = parse_color_mix_space(c);
      if (!method) return std::nullopt;
      header.method = *method;
      saw_method = true;
    } else if (!saw_progress) {
      auto value = parse_color_mix_progress_value(c);
      if (!value) return std::nullopt;
      header.progress = *value;
      saw_progress = true;
    } else {
      return std::nullopt;
    }

    c.skip_ws();
    if (c.i == before) return std::nullopt;
  }

  if (!saw_progress) return std::nullopt;
  return header;
}

inline std::optional<parsed_color_mix> parse_color_mix_progress_form(const std::vector<std::string_view>& parts) {
  if (parts.size() != 3) return std::nullopt;

  auto header = parse_color_mix_progress_header(parts[0]);
  if (!header) return std::nullopt;

  auto first = parse_color_mix_item(parts[1]);
  auto second = parse_color_mix_item(parts[2]);
  if (!first || !second) return std::nullopt;
  if (first->weight || second->weight) return std::nullopt;

  parsed_color_mix parsed;
  parsed.method = header->method;
  parsed.items.push_back(color_mix_item{first->color, 1.0f - header->progress});
  parsed.items.push_back(color_mix_item{second->color, header->progress});
  return parsed;
}

inline std::optional<parsed_color_mix> parse_color_mix_arguments(std::string_view inner) {
  auto parts = split_top_level_comma_list(inner);
  if (!parts) return std::nullopt;
  if (parts->size() == 3) {
    if (auto progress = parse_color_mix_progress_form(*parts)) {
      return progress;
    }
  }
  return parse_color_mix_item_list(*parts);
}

inline float normalize_hue_degrees(float hue) {
  hue = std::fmod(hue, 360.0f);
  if (hue < 0.0f) hue += 360.0f;
  return hue;
}

inline void adjust_hues_for_interpolation(float& first, float& second, color_mix_hue_method method) {
  constexpr float hue_epsilon = 1e-3f;
  first = normalize_hue_degrees(first);
  second = normalize_hue_degrees(second);
  const float delta = second - first;

  switch (method) {
    case color_mix_hue_method::shorter:
      if (delta > 180.0f + hue_epsilon) {
        first += 360.0f;
      } else if (delta < -180.0f - hue_epsilon) {
        second += 360.0f;
      }
      break;
    case color_mix_hue_method::longer:
      if (delta > hue_epsilon && delta < 180.0f - hue_epsilon) {
        first += 360.0f;
      } else if (delta > -180.0f + hue_epsilon && delta <= hue_epsilon) {
        second += 360.0f;
      }
      break;
    case color_mix_hue_method::increasing:
      if (second < first) second += 360.0f;
      break;
    case color_mix_hue_method::decreasing:
      if (first < second) first += 360.0f;
      break;
  }
}

// Item values arrive as [r, g, b, a] already living in Color3's space
// (issue #110): construct the typed values directly — a color_cast from sRGB
// here is exactly the gamut clamp this pipeline must avoid — and return the
// mixed channels unclamped so multi-item chains keep out-of-gamut values
// until the final output conversion.
template <typename Color3>
inline std::array<float, 4> mix_in_rectangular_space(const std::array<float, 4>& a, const std::array<float, 4>& b,
                                                     const color_mix_weights& weights) {
  const float alpha_a = std::clamp(a[3], 0.0f, 1.0f);
  const float alpha_b = std::clamp(b[3], 0.0f, 1.0f);
  const float mixed_alpha = alpha_a * weights.first + alpha_b * weights.second;

  const Color3 ca{a[0], a[1], a[2]};
  const Color3 cb{b[0], b[1], b[2]};

  float c0 =
      ca.template get_index<0>() * alpha_a * weights.first + cb.template get_index<0>() * alpha_b * weights.second;
  float c1 =
      ca.template get_index<1>() * alpha_a * weights.first + cb.template get_index<1>() * alpha_b * weights.second;
  float c2 =
      ca.template get_index<2>() * alpha_a * weights.first + cb.template get_index<2>() * alpha_b * weights.second;

  if (mixed_alpha > 0.0f) {
    c0 /= mixed_alpha;
    c1 /= mixed_alpha;
    c2 /= mixed_alpha;
  }

  return {c0, c1, c2, std::clamp(mixed_alpha * weights.alpha_multiplier, 0.0f, 1.0f)};
}

template <typename Color3>
inline std::array<float, 4> mix_in_polar_space(const std::array<float, 4>& a, const std::array<float, 4>& b,
                                               const color_mix_weights& weights, color_mix_hue_method hue_method) {
  const float alpha_a = std::clamp(a[3], 0.0f, 1.0f);
  const float alpha_b = std::clamp(b[3], 0.0f, 1.0f);
  const float mixed_alpha = alpha_a * weights.first + alpha_b * weights.second;

  const Color3 ca{a[0], a[1], a[2]};
  const Color3 cb{b[0], b[1], b[2]};

  float hue_a = ca.template get_index<2>();
  float hue_b = cb.template get_index<2>();
  // CSS Color 4: a powerless hue (chroma ≈ 0) is missing and takes the other
  // item's hue before any hue-method adjustment (issue #51 — white's OkLCH
  // hue is atan2 noise at C ≈ 6e-8 and used to steer the mix to teal).
  operations::interpolate::details::substitute_powerless_hue(ca, cb, hue_a, hue_b);
  adjust_hues_for_interpolation(hue_a, hue_b, hue_method);

  float c0 =
      ca.template get_index<0>() * alpha_a * weights.first + cb.template get_index<0>() * alpha_b * weights.second;
  float c1 =
      ca.template get_index<1>() * alpha_a * weights.first + cb.template get_index<1>() * alpha_b * weights.second;

  if (mixed_alpha > 0.0f) {
    c0 /= mixed_alpha;
    c1 /= mixed_alpha;
  }

  return {c0, c1, normalize_hue_degrees(hue_a * weights.first + hue_b * weights.second),
          std::clamp(mixed_alpha * weights.alpha_multiplier, 0.0f, 1.0f)};
}

inline std::array<float, 4> mix_colors_in_space(const color_mix_interpolation_method& method,
                                                const std::array<float, 4>& a, const std::array<float, 4>& b,
                                                const color_mix_weights& weights) {
  switch (method.space) {
    case color_mix_space::srgb:
      return mix_in_rectangular_space<core::rgbf_t>(a, b, weights);
    case color_mix_space::srgb_linear:
      return mix_in_rectangular_space<core::linear_rgbf_t>(a, b, weights);
    case color_mix_space::lab:
      return mix_in_rectangular_space<core::cielab_t>(a, b, weights);
    case color_mix_space::lch:
      return mix_in_polar_space<core::cielch_t>(a, b, weights, method.hue_method);
    case color_mix_space::oklab:
      return mix_in_rectangular_space<core::oklab_t>(a, b, weights);
    case color_mix_space::oklch:
      return mix_in_polar_space<core::oklch_t>(a, b, weights, method.hue_method);
    case color_mix_space::xyz:
      return mix_in_rectangular_space<core::xyz_t>(a, b, weights);
    case color_mix_space::display_p3:
      return mix_in_rectangular_space<core::display_p3f_t>(a, b, weights);
    case color_mix_space::display_p3_linear:
      return mix_in_rectangular_space<core::linear_display_p3f_t>(a, b, weights);
    case color_mix_space::a98_rgb:
      return mix_in_rectangular_space<core::adobe_rgbf_t>(a, b, weights);
    case color_mix_space::a98_rgb_linear:
      return mix_in_rectangular_space<core::linear_adobe_rgbf_t>(a, b, weights);
    case color_mix_space::prophoto_rgb:
      return mix_in_rectangular_space<core::prophoto_rgbf_t>(a, b, weights);
    case color_mix_space::prophoto_rgb_linear:
      return mix_in_rectangular_space<core::linear_prophoto_rgbf_t>(a, b, weights);
  }

  return mix_in_rectangular_space<core::rgbf_t>(a, b, weights);
}

/**
 * @brief Mixed-space [r, g, b, a] to the pipeline's sRGB float output.
 *
 * This is the single gamut-clamp boundary of color-mix() (issue #110): the
 * mixed value may legitimately sit outside sRGB — conversion clamps it here
 * and nowhere earlier.
 */
inline core::rgbaf_t color_mix_space_to_rgbaf(const std::array<float, 4>& v, color_mix_space space) {
  core::rgbaf_t out{};
  switch (space) {
    case color_mix_space::srgb:
      out = operations::conversion::color_cast<core::rgbaf_t>(core::rgbf_t{v[0], v[1], v[2]});
      break;
    case color_mix_space::srgb_linear:
      out = operations::conversion::color_cast<core::rgbaf_t>(core::linear_rgbf_t{v[0], v[1], v[2]});
      break;
    case color_mix_space::lab:
      out = operations::conversion::color_cast<core::rgbaf_t>(core::cielab_t{v[0], v[1], v[2]});
      break;
    case color_mix_space::lch:
      out = operations::conversion::color_cast<core::rgbaf_t>(core::cielch_t{v[0], v[1], v[2]});
      break;
    case color_mix_space::oklab:
      out = operations::conversion::color_cast<core::rgbaf_t>(core::oklab_t{v[0], v[1], v[2]});
      break;
    case color_mix_space::oklch:
      out = operations::conversion::color_cast<core::rgbaf_t>(core::oklch_t{v[0], v[1], v[2]});
      break;
    case color_mix_space::xyz:
      out = operations::conversion::color_cast<core::rgbaf_t>(core::xyz_t{v[0], v[1], v[2]});
      break;
    case color_mix_space::display_p3:
      out = operations::conversion::color_cast<core::rgbaf_t>(core::display_p3f_t{v[0], v[1], v[2]});
      break;
    case color_mix_space::display_p3_linear:
      out = operations::conversion::color_cast<core::rgbaf_t>(core::linear_display_p3f_t{v[0], v[1], v[2]});
      break;
    case color_mix_space::a98_rgb:
      out = operations::conversion::color_cast<core::rgbaf_t>(core::adobe_rgbf_t{v[0], v[1], v[2]});
      break;
    case color_mix_space::a98_rgb_linear:
      out = operations::conversion::color_cast<core::rgbaf_t>(core::linear_adobe_rgbf_t{v[0], v[1], v[2]});
      break;
    case color_mix_space::prophoto_rgb:
      out = operations::conversion::color_cast<core::rgbaf_t>(core::prophoto_rgbf_t{v[0], v[1], v[2]});
      break;
    case color_mix_space::prophoto_rgb_linear:
      out = operations::conversion::color_cast<core::rgbaf_t>(core::linear_prophoto_rgbf_t{v[0], v[1], v[2]});
      break;
  }
  out.a() = std::clamp(v[3], 0.0f, 1.0f);
  return out;
}

inline std::optional<resolved_color_mix> resolve_color_mix_items(const parsed_color_mix& parsed,
                                                                 const parse_css_color_context& context) {
  if (parsed.items.empty()) return std::nullopt;

  resolved_color_mix resolved;
  resolved.method = parsed.method;

  size_t missing_count = 0;
  float explicit_sum = 0.0f;
  for (const auto& item : parsed.items) {
    if (item.weight) {
      explicit_sum += std::clamp(*item.weight, 0.0f, 1.0f);
    } else {
      ++missing_count;
    }
  }

  const float fill_weight = missing_count == 0
                                ? 0.0f
                                : (explicit_sum <= 0.0f && missing_count == parsed.items.size()
                                       ? 1.0f / static_cast<float>(missing_count)
                                       : std::max(0.0f, 1.0f - explicit_sum) / static_cast<float>(missing_count));

  float total = 0.0f;
  for (const auto& item : parsed.items) {
    // Typed into the mix space — no sRGB detour (issue #110).
    auto color = parse_color_mix_item_space(item.color, resolved.method.space, context);
    if (!color) return std::nullopt;
    const float weight = item.weight ? std::clamp(*item.weight, 0.0f, 1.0f) : fill_weight;
    resolved.items.push_back(resolved_color_mix_item{*color, weight});
    total += weight;
  }

  if (total <= 0.0f) return std::nullopt;

  resolved.alpha_multiplier = total < 1.0f ? total : 1.0f;
  for (auto& item : resolved.items) {
    item.weight /= total;
  }
  return resolved;
}

inline core::rgbaf_t evaluate_color_mix(const resolved_color_mix& resolved) {
  if (resolved.items.size() == 1) {
    auto out = resolved.items.front().color;
    out[3] = std::clamp(out[3] * resolved.alpha_multiplier, 0.0f, 1.0f);
    return color_mix_space_to_rgbaf(out, resolved.method.space);
  }

  std::array<float, 4> mixed = resolved.items.front().color;
  float accumulated_weight = resolved.items.front().weight;
  for (size_t index = 1; index < resolved.items.size(); ++index) {
    const float combined = accumulated_weight + resolved.items[index].weight;
    color_mix_weights pair;
    pair.first = accumulated_weight / combined;
    pair.second = resolved.items[index].weight / combined;
    pair.alpha_multiplier = 1.0f;
    mixed = mix_colors_in_space(resolved.method, mixed, resolved.items[index].color, pair);
    accumulated_weight = combined;
  }

  mixed[3] = std::clamp(mixed[3] * resolved.alpha_multiplier, 0.0f, 1.0f);
  return color_mix_space_to_rgbaf(mixed, resolved.method.space);
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

  auto parsed = parse_color_mix_arguments(inner);
  if (!parsed) {
    c.i = save;
    return std::nullopt;
  }

  auto resolved = resolve_color_mix_items(*parsed, context);
  if (!resolved) {
    c.i = save;
    return std::nullopt;
  }

  return evaluate_color_mix(*resolved);
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
  // Parsers whose core type has no alpha channel report it via this sink;
  // 1.0 is the CSS default when "/ alpha" is omitted.
  float alpha = 1.0f;
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
  if (auto r = parse_lab_function(sub, &alpha)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    auto out = operations::conversion::color_cast<core::rgba8_t>(*r);
    out.a() = sub.alpha_byte_from_double(alpha);
    return out;
  }
  sub.i = 0;
  if (auto r = parse_lch_function(sub, &alpha)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    auto out = operations::conversion::color_cast<core::rgba8_t>(*r);
    out.a() = sub.alpha_byte_from_double(alpha);
    return out;
  }
  sub.i = 0;
  if (auto r = parse_oklab_function(sub, &alpha)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    auto out = operations::conversion::color_cast<core::rgba8_t>(*r);
    out.a() = sub.alpha_byte_from_double(alpha);
    return out;
  }
  sub.i = 0;
  if (auto r = parse_oklch_function(sub, &alpha)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    auto out = operations::conversion::color_cast<core::rgba8_t>(*r);
    out.a() = sub.alpha_byte_from_double(alpha);
    return out;
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
  // Parsers whose core type has no alpha channel report it via this sink;
  // 1.0 is the CSS default when "/ alpha" is omitted.
  float alpha = 1.0f;
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
  if (auto r = parse_lab_function(sub, &alpha)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    auto out = operations::conversion::color_cast<core::rgbaf_t>(*r);
    out.a() = alpha;
    return out;
  }
  sub.i = 0;
  if (auto r = parse_lch_function(sub, &alpha)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    auto out = operations::conversion::color_cast<core::rgbaf_t>(*r);
    out.a() = alpha;
    return out;
  }
  sub.i = 0;
  if (auto r = parse_oklab_function(sub, &alpha)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    auto out = operations::conversion::color_cast<core::rgbaf_t>(*r);
    out.a() = alpha;
    return out;
  }
  sub.i = 0;
  if (auto r = parse_oklch_function(sub, &alpha)) {
    if (!sub.eof()) return std::nullopt;
    c.i = a + sub.i;
    auto out = operations::conversion::color_cast<core::rgbaf_t>(*r);
    out.a() = alpha;
    return out;
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
