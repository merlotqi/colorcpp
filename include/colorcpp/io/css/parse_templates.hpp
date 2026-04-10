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
  if (auto exact = parse_exact_css_color<Color>(str)) return exact;
  auto parsed = parse_css_color_rgbaf(str);
  if (!parsed) return std::nullopt;
  return operations::conversion::color_cast<Color>(*parsed);
}

template <typename Color>
inline std::optional<Color> parse_css_color_impl(std::string_view str, const parse_css_color_context& context) {
  if (auto exact = parse_exact_css_color<Color>(str)) return exact;
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
inline std::optional<core::rgbaf_t> parse_css_color<core::rgbaf_t>(
    std::string_view str,
    const parse_css_color_context& context) {
  return parse_css_color_rgbaf(str, context);
}

template <typename Color>
inline std::optional<Color> parse_css_color(std::string_view str, const parse_css_color_context& context) {
  return parse_templates_detail::parse_css_color_impl<Color>(str, context);
}

}  // namespace colorcpp::io::css
