/**
 * @file parse_templates.hpp
 * @brief @c parse_css_color template API and specializations.
 */

#pragma once

#include <colorcpp/io/css/display_p3.hpp>
#include <colorcpp/io/css/parse_public.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <optional>
#include <string_view>

namespace colorcpp::io::css {

template <typename Color>
inline std::optional<Color> parse_css_color(std::string_view str);

template <>
inline std::optional<core::cielab_t> parse_css_color<core::cielab_t>(std::string_view str) {
  details::trim(str);
  if (str.empty()) return std::nullopt;
  details::Cursor c{str, 0};
  if (auto r = parse_lab_function(c)) return r;
  return std::nullopt;
}

template <>
inline std::optional<core::cielch_t> parse_css_color<core::cielch_t>(std::string_view str) {
  details::trim(str);
  if (str.empty()) return std::nullopt;
  details::Cursor c{str, 0};
  if (auto r = parse_lch_function(c)) return r;
  return std::nullopt;
}

template <>
inline std::optional<core::oklab_t> parse_css_color<core::oklab_t>(std::string_view str) {
  details::trim(str);
  if (str.empty()) return std::nullopt;
  details::Cursor c{str, 0};
  if (auto r = parse_oklab_function(c)) return r;
  return std::nullopt;
}

template <>
inline std::optional<core::oklch_t> parse_css_color<core::oklch_t>(std::string_view str) {
  details::trim(str);
  if (str.empty()) return std::nullopt;
  details::Cursor c{str, 0};
  if (auto r = parse_oklch_function(c)) return r;
  return std::nullopt;
}

template <>
inline std::optional<core::display_p3f_t> parse_css_color<core::display_p3f_t>(std::string_view str) {
  details::trim(str);
  if (str.empty()) return std::nullopt;
  details::Cursor c{str, 0};
  if (auto r = parse_display_p3_function(c)) return r;
  return std::nullopt;
}

template <>
inline std::optional<core::rgbaf_t> parse_css_color<core::rgbaf_t>(std::string_view str) {
  return parse_css_color_rgbaf(str);
}

template <typename Color>
inline std::optional<Color> parse_css_color(std::string_view str) {
  auto r = parse_css_color_rgba8(str);
  if (!r) return std::nullopt;
  return operations::conversion::color_cast<Color>(*r);
}

}  // namespace colorcpp::io::css
