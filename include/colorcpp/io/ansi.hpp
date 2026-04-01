/**
 * @file ansi.hpp
 * @brief ANSI terminal color utilities for debugging and visualization.
 *
 * Provides functions to print colored swatches, palettes, gradients,
 * and contrast previews to the terminal using ANSI escape codes.
 */

#pragma once

#include <cmath>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

namespace colorcpp::io::ansi {

namespace details {

/**
 * @brief Convert any color type to rgba8_t for ANSI output.
 */
template <typename Color>
core::rgba8_t to_rgba8(const Color& c) {
  return operations::conversion::color_cast<core::rgba8_t>(c);
}

/**
 * @brief Format an integer with fixed width.
 */
inline std::string fmt_int(int v, int width = 3) {
  std::ostringstream ss;
  ss << std::setw(width) << v;
  return ss.str();
}

}  // namespace details

// ===== ANSI Escape Sequences =====

/** @brief ANSI reset sequence. */
inline constexpr const char* reset() { return "\033[0m"; }

/** @brief ANSI bold sequence. */
inline constexpr const char* bold() { return "\033[1m"; }

/** @brief ANSI foreground color (24-bit). */
inline std::string fg(uint8_t r, uint8_t g, uint8_t b) {
  return "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

/** @brief ANSI background color (24-bit). */
inline std::string bg(uint8_t r, uint8_t g, uint8_t b) {
  return "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

/** @brief ANSI foreground color from rgba8_t. */
inline std::string fg(const core::rgba8_t& c) { return fg(c.r(), c.g(), c.b()); }

/** @brief ANSI background color from rgba8_t. */
inline std::string bg(const core::rgba8_t& c) { return bg(c.r(), c.g(), c.b()); }

// ===== Print Functions =====

/**
 * @brief Print a colored swatch block with hex value.
 * @param os Output stream.
 * @param c Any color type.
 * @param width Number of block characters to print.
 */
template <typename Color>
void print_swatch(std::ostream& os, const Color& c, int width = 6) {
  auto rgba = details::to_rgba8(c);
  os << bg(rgba) << std::string(width, ' ') << reset();
}

/**
 * @brief Print a color with full info: swatch + hex + RGB.
 * @param os Output stream.
 * @param c Any color type.
 * @param label Optional label.
 */
template <typename Color>
void print_color(std::ostream& os, const Color& c, std::string_view label = "") {
  auto rgba = details::to_rgba8(c);

  // Swatch
  os << bg(rgba) << "      " << reset() << " ";

  // Label
  if (!label.empty()) {
    os << bold() << label << reset() << " ";
  }

  // Hex
  os << "#" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(rgba.r()) << std::setw(2)
     << static_cast<int>(rgba.g()) << std::setw(2) << static_cast<int>(rgba.b()) << std::setw(2)
     << static_cast<int>(rgba.a()) << std::dec << std::setfill(' ');

  // RGB
  os << "  RGB(" << static_cast<int>(rgba.r()) << "," << static_cast<int>(rgba.g()) << "," << static_cast<int>(rgba.b())
     << ")";
}

/**
 * @brief Print a color with extended info: swatch + hex + RGB + HSL.
 * @param os Output stream.
 * @param c Any color type (converted to rgba8 internally for display).
 * @param label Optional label.
 */
template <typename Color>
void print_color_verbose(std::ostream& os, const Color& c, std::string_view label = "") {
  auto rgba = details::to_rgba8(c);

  // Swatch
  os << bg(rgba) << "      " << reset() << " ";

  // Label
  if (!label.empty()) {
    os << bold() << label << reset() << " ";
  }

  // Hex
  os << "#" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(rgba.r()) << std::setw(2)
     << static_cast<int>(rgba.g()) << std::setw(2) << static_cast<int>(rgba.b()) << std::setw(2)
     << static_cast<int>(rgba.a()) << std::dec << std::setfill(' ');

  // RGB
  os << "  RGB(" << static_cast<int>(rgba.r()) << "," << static_cast<int>(rgba.g()) << "," << static_cast<int>(rgba.b())
     << ")";

  // HSL (compute from RGB)
  float r = rgba.r() / 255.0f;
  float g = rgba.g() / 255.0f;
  float b = rgba.b() / 255.0f;
  float max_c = std::max({r, g, b});
  float min_c = std::min({r, g, b});
  float l = (max_c + min_c) / 2.0f;

  if (max_c == min_c) {
    os << "  HSL(0°,0%," << static_cast<int>(std::round(l * 100)) << "%)";
  } else {
    float d = max_c - min_c;
    float s = l > 0.5f ? d / (2.0f - max_c - min_c) : d / (max_c + min_c);
    float h = 0.0f;
    if (max_c == r)
      h = fmodf((g - b) / d + (g < b ? 6.0f : 0.0f), 6.0f);
    else if (max_c == g)
      h = (b - r) / d + 2.0f;
    else
      h = (r - g) / d + 4.0f;
    h *= 60.0f;
    os << "  HSL(" << static_cast<int>(std::round(h)) << "°," << static_cast<int>(std::round(s * 100)) << "%,"
       << static_cast<int>(std::round(l * 100)) << "%)";
  }
}

/**
 * @brief Print a palette of colors in a single row.
 * @param os Output stream.
 * @param colors Span of any color type.
 * @param swatch_width Width of each swatch.
 */
template <typename Color>
void print_palette(std::ostream& os, const Color* colors, std::size_t count, int swatch_width = 6) {
  for (std::size_t i = 0; i < count; ++i) {
    auto rgba = details::to_rgba8(colors[i]);
    os << bg(rgba) << std::string(swatch_width, ' ') << reset();
  }
  os << "\n";
}

/**
 * @brief Print a gradient between two colors.
 * @param os Output stream.
 * @param a Start color.
 * @param b End color.
 * @param steps Number of gradient steps.
 */
template <typename ColorA, typename ColorB>
void print_gradient(std::ostream& os, const ColorA& a, const ColorB& b, int steps = 16) {
  auto ra = details::to_rgba8(a);
  auto rb = details::to_rgba8(b);

  for (int i = 0; i < steps; ++i) {
    float t = static_cast<float>(i) / static_cast<float>(steps - 1);
    auto r = static_cast<uint8_t>(std::round(ra.r() + (rb.r() - ra.r()) * t));
    auto g = static_cast<uint8_t>(std::round(ra.g() + (rb.g() - ra.g()) * t));
    auto bl = static_cast<uint8_t>(std::round(ra.b() + (rb.b() - ra.b()) * t));
    os << bg(r, g, bl) << "  " << reset();
  }
  os << "\n";
}

/**
 * @brief Compute relative luminance per WCAG 2.0.
 */
inline float relative_luminance(uint8_t r, uint8_t g, uint8_t b) {
  auto linearize = [](uint8_t v) -> float {
    float s = v / 255.0f;
    return s <= 0.03928f ? s / 12.92f : std::pow((s + 0.055f) / 1.055f, 2.4f);
  };
  return 0.2126f * linearize(r) + 0.7152f * linearize(g) + 0.0722f * linearize(b);
}

/**
 * @brief Compute WCAG contrast ratio between two colors.
 */
inline float contrast_ratio(const core::rgba8_t& a, const core::rgba8_t& b) {
  float la = relative_luminance(a.r(), a.g(), a.b());
  float lb = relative_luminance(b.r(), b.g(), b.b());
  float lighter = std::max(la, lb);
  float darker = std::min(la, lb);
  return (lighter + 0.05f) / (darker + 0.05f);
}

/**
 * @brief Get WCAG compliance level string.
 */
inline std::string_view wcag_level(float ratio) {
  if (ratio >= 7.0f) return "AAA";
  if (ratio >= 4.5f) return "AA";
  if (ratio >= 3.0f) return "AA Large";
  return "Fail";
}

/**
 * @brief Print contrast check between foreground and background colors.
 * @param os Output stream.
 * @param fg_color Foreground (text) color.
 * @param bg_color Background color.
 */
template <typename FgColor, typename BgColor>
void print_contrast(std::ostream& os, const FgColor& fg_color, const BgColor& bg_color) {
  auto fg_rgba = details::to_rgba8(fg_color);
  auto bg_rgba = details::to_rgba8(bg_color);

  float ratio = contrast_ratio(fg_rgba, bg_rgba);
  auto level = wcag_level(ratio);

  // Preview: text on background
  os << bg(bg_rgba) << fg(fg_rgba) << bold() << "  Aa  " << reset() << " ";

  // Info
  os << "#" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(fg_rgba.r()) << std::setw(2)
     << static_cast<int>(fg_rgba.g()) << std::setw(2) << static_cast<int>(fg_rgba.b()) << std::dec << std::setfill(' ')
     << " on #";
  os << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(bg_rgba.r()) << std::setw(2)
     << static_cast<int>(bg_rgba.g()) << std::setw(2) << static_cast<int>(bg_rgba.b()) << std::dec << std::setfill(' ');

  os << "  contrast: " << std::fixed << std::setprecision(1) << ratio << ":1  " << level;
}

}  // namespace colorcpp::io::ansi
