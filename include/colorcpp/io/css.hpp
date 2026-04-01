/**
 * @file css_color.hpp
 * @brief Aggregate header for CSS Color Level 4 parsing.
 *
 * Includes hex, rgb/rgba, hsl/hsla, hwb, oklab, oklch, color(display-p3) parsing.
 */

#pragma once

#include <colorcpp/core/cielab.hpp>
#include <colorcpp/core/display_p3.hpp>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/io/css/details.hpp>
#include <colorcpp/io/css/display_p3.hpp>
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
#include <optional>
#include <string_view>

namespace colorcpp::io::css {

/**
 * @brief Parse a CSS color string into sRGB 8-bit with alpha.
 * @return @c std::nullopt on syntax error or unsupported form.
 */
inline std::optional<core::rgba8_t> parse_css_color_rgba8(std::string_view str) {
  details::trim(str);
  if (str.empty()) return std::nullopt;

  // Check for hex color
  if (str[0] == '#') return parse_hex(str);

  // Check for named color
  if (auto named = get_named_color(str)) {
    return named;
  }

  // Check for CSS functions
  details::Cursor c{str, 0};
  if (auto r = parse_rgb_function(c)) return r;
  c.i = 0;
  if (auto r = parse_hsl_function(c)) return r;
  c.i = 0;
  if (auto r = parse_hwb_function(c)) return r;
  c.i = 0;
  if (auto r = parse_lab_function(c)) {
    // Convert lab to rgba8
    return operations::conversion::color_cast<core::rgba8_t>(*r);
  }
  c.i = 0;
  if (auto r = parse_lch_function(c)) {
    // Convert lch to rgba8
    return operations::conversion::color_cast<core::rgba8_t>(*r);
  }
  c.i = 0;
  if (auto r = parse_oklab_function(c)) {
    // Convert oklab to rgba8
    return operations::conversion::color_cast<core::rgba8_t>(*r);
  }
  c.i = 0;
  if (auto r = parse_oklch_function(c)) {
    // Convert oklch to rgba8
    return operations::conversion::color_cast<core::rgba8_t>(*r);
  }
  c.i = 0;
  if (auto r = parse_display_p3_function(c)) {
    // Convert display-p3 to rgba8
    return operations::conversion::color_cast<core::rgba8_t>(*r);
  }

  return std::nullopt;
}

// Forward declarations for specializations
template <typename Color>
inline std::optional<Color> parse_css_color(std::string_view str);

/**
 * @brief Parse CSS color for cielab_t: parse lab() directly (no sRGB 8-bit round-trip).
 */
template <>
inline std::optional<core::cielab_t> parse_css_color<core::cielab_t>(std::string_view str) {
  details::trim(str);
  if (str.empty()) return std::nullopt;

  details::Cursor c{str, 0};
  if (auto r = parse_lab_function(c)) return r;

  return std::nullopt;
}

/**
 * @brief Parse CSS color for cielch_t: parse lch() directly (no sRGB 8-bit round-trip).
 */
template <>
inline std::optional<core::cielch_t> parse_css_color<core::cielch_t>(std::string_view str) {
  details::trim(str);
  if (str.empty()) return std::nullopt;

  details::Cursor c{str, 0};
  if (auto r = parse_lch_function(c)) return r;

  return std::nullopt;
}

/**
 * @brief Parse CSS color for oklab_t: parse oklab() function directly.
 */
template <>
inline std::optional<core::oklab_t> parse_css_color<core::oklab_t>(std::string_view str) {
  details::trim(str);
  if (str.empty()) return std::nullopt;

  details::Cursor c{str, 0};
  if (auto r = parse_oklab_function(c)) return r;

  return std::nullopt;
}

/**
 * @brief Parse CSS color for oklch_t: parse oklch() function directly.
 */
template <>
inline std::optional<core::oklch_t> parse_css_color<core::oklch_t>(std::string_view str) {
  details::trim(str);
  if (str.empty()) return std::nullopt;

  details::Cursor c{str, 0};
  if (auto r = parse_oklch_function(c)) return r;

  return std::nullopt;
}

/**
 * @brief Parse CSS color for display_p3f_t: parse color(display-p3 ...) function directly.
 */
template <>
inline std::optional<core::display_p3f_t> parse_css_color<core::display_p3f_t>(std::string_view str) {
  details::trim(str);
  if (str.empty()) return std::nullopt;

  details::Cursor c{str, 0};
  if (auto r = parse_display_p3_function(c)) return r;

  return std::nullopt;
}

/**
 * @brief Parse CSS color then convert to @p Color via @ref operations::conversion::color_cast.
 */
template <typename Color>
inline std::optional<Color> parse_css_color(std::string_view str) {
  auto r = parse_css_color_rgba8(str);
  if (!r) return std::nullopt;
  return operations::conversion::color_cast<Color>(*r);
}

}  // namespace colorcpp::io::css
