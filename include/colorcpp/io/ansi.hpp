/**
 * @file ansi.hpp
 * @brief ANSI terminal color utilities for debugging and visualization.
 *
 * Provides functions to print colored swatches, palettes, gradients,
 * and contrast previews to the terminal using ANSI escape codes.
 */

#pragma once

#include <array>
#include <cmath>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

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

/**
 * @brief Preserve and restore stream formatting state.
 */
class ostream_state_guard {
 public:
  explicit ostream_state_guard(std::ostream& os)
      : os_(os), flags_(os.flags()), precision_(os.precision()), fill_(os.fill()) {}

  ~ostream_state_guard() {
    os_.flags(flags_);
    os_.precision(precision_);
    os_.fill(fill_);
  }

 private:
  std::ostream& os_;
  std::ios::fmtflags flags_;
  std::streamsize precision_;
  char fill_;
};

template <typename T, typename = void>
struct has_data_size : std::false_type {};

template <typename T>
struct has_data_size<T,
                     std::void_t<decltype(std::declval<const T&>().data()), decltype(std::declval<const T&>().size())>>
    : std::true_type {};

// xterm 6×6×6 cube component levels (indices 16-231) and the 16 base ANSI
// colors (xterm defaults;0-7 normal, 8-15 bright — matching the CSS named
// colors of the dark/bright variants).
inline constexpr std::array<uint8_t, 6> kCubeLevels = {0, 95, 135, 175, 215, 255};

inline constexpr std::array<std::array<uint8_t, 3>, 16> kAnsi16 = {{
    {{0, 0, 0}},
    {{128, 0, 0}},
    {{0, 128, 0}},
    {{128, 128, 0}},
    {{0, 0, 128}},
    {{128, 0, 128}},
    {{0, 128, 128}},
    {{192, 192, 192}},
    {{128, 128, 128}},
    {{255, 0, 0}},
    {{0, 255, 0}},
    {{255, 255, 0}},
    {{0, 0, 255}},
    {{255, 0, 255}},
    {{0, 255, 255}},
    {{255, 255, 255}},
}};

inline constexpr int sq_diff(int a, int b) {
  const int d = a - b;
  return d * d;
}

}  // namespace details

// ===== ANSI Escape Sequences =====

/** @brief ANSI reset sequence. */
inline constexpr const char* reset() { return "\033[0m"; }

/** @brief ANSI bold sequence. */
inline constexpr const char* bold() { return "\033[1m"; }

/**
 * @brief Output color depth for fg()/bg() and the print writers (issue #116).
 *
 * The default everywhere is @ref level::truecolor, preserving the original
 * 24-bit behaviour; pass @ref level::ansi256 or @ref level::ansi16 for
 * terminals without truecolor support.
 */
enum class level {
  truecolor,
  ansi256,
  ansi16
};

/**
 * @brief Nearest xterm 256-color index for an RGB triple.
 *
 * Searches the 6×6×6 cube (16-231) and the 24-step gray ramp (232-255) with
 * squared Euclidean distance; ties prefer the cube (lower index). System
 * colors 0-15 are skipped — their RGB values depend on the terminal theme.
 */
inline uint8_t quantize_ansi256(uint8_t r, uint8_t g, uint8_t b) {
  // Nearest cube entry: Euclidean distance decomposes per channel, so the
  // per-channel nearest level is the nearest cube point.
  auto nearest_level = [](int v) {
    int best_i = 0;
    int best_e = 255;
    for (int i = 0; i < 6; ++i) {
      const int lvl = details::kCubeLevels[i];
      const int e = lvl > v ? lvl - v : v - lvl;
      if (e < best_e) {
        best_e = e;
        best_i = i;
      }
    }
    return best_i;
  };
  const int ri = nearest_level(r);
  const int gi = nearest_level(g);
  const int bi = nearest_level(b);
  int best_idx = 16 + ri * 36 + gi * 6 + bi;
  int best_d = details::sq_diff(details::kCubeLevels[ri], r) + details::sq_diff(details::kCubeLevels[gi], g) +
               details::sq_diff(details::kCubeLevels[bi], b);

  // Gray ramp v = 8 + 10k: the quadratic is minimized near the mean, and the
  // discrete minimum is one of the integers bracketing (mean - 8) / 10.
  const int mean = (static_cast<int>(r) + g + b) / 3;
  int k0 = (mean - 8 + 5) / 10;  // rounded (truncates toward zero; window below covers it)
  if (k0 < 0) k0 = 0;
  if (k0 > 23) k0 = 23;
  for (int k = k0 - 1; k <= k0 + 1; ++k) {
    if (k < 0 || k > 23) continue;
    const int v = 8 + 10 * k;
    const int d = details::sq_diff(v, r) + details::sq_diff(v, g) + details::sq_diff(v, b);
    if (d < best_d) {  // strict: cube wins ties
      best_d = d;
      best_idx = 232 + k;
    }
  }
  return static_cast<uint8_t>(best_idx);
}

/** @brief RGB value of an xterm 256-color index (0-15 use the default system palette). */
inline std::array<uint8_t, 3> ansi256_rgb(uint8_t index) {
  if (index >= 16 && index <= 231) {
    const int c = index - 16;
    return {details::kCubeLevels[c / 36], details::kCubeLevels[(c / 6) % 6], details::kCubeLevels[c % 6]};
  }
  if (index >= 232) {
    const auto v = static_cast<uint8_t>(8 + 10 * (index - 232));
    return {v, v, v};
  }
  const auto& c = details::kAnsi16[index];
  return {c[0], c[1], c[2]};
}

/** @brief Nearest of the 16 base ANSI colors (squared Euclidean distance, lower index wins ties). */
inline uint8_t quantize_ansi16(uint8_t r, uint8_t g, uint8_t b) {
  int best_idx = 0;
  int best_d = 1 << 30;
  for (int i = 0; i < 16; ++i) {
    const auto& c = details::kAnsi16[i];
    const int d = details::sq_diff(c[0], r) + details::sq_diff(c[1], g) + details::sq_diff(c[2], b);
    if (d < best_d) {
      best_d = d;
      best_idx = i;
    }
  }
  return static_cast<uint8_t>(best_idx);
}

/** @brief ANSI foreground color at the requested depth (default 24-bit). */
inline std::string fg(uint8_t r, uint8_t g, uint8_t b, level lv = level::truecolor) {
  switch (lv) {
    case level::truecolor:
      return "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
    case level::ansi256:
      return "\033[38;5;" + std::to_string(quantize_ansi256(r, g, b)) + "m";
    case level::ansi16: {
      const int idx = quantize_ansi16(r, g, b);
      return "\033[" + std::to_string(idx < 8 ? 30 + idx : 82 + idx) + "m";  // 30-37 or 90-97
    }
  }
  return {};
}

/** @brief ANSI background color at the requested depth (default 24-bit). */
inline std::string bg(uint8_t r, uint8_t g, uint8_t b, level lv = level::truecolor) {
  switch (lv) {
    case level::truecolor:
      return "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
    case level::ansi256:
      return "\033[48;5;" + std::to_string(quantize_ansi256(r, g, b)) + "m";
    case level::ansi16: {
      const int idx = quantize_ansi16(r, g, b);
      return "\033[" + std::to_string(idx < 8 ? 40 + idx : 92 + idx) + "m";  // 40-47 or 100-107
    }
  }
  return {};
}

/** @brief ANSI foreground color from rgba8_t. */
inline std::string fg(const core::rgba8_t& c, level lv = level::truecolor) { return fg(c.r(), c.g(), c.b(), lv); }

/** @brief ANSI background color from rgba8_t. */
inline std::string bg(const core::rgba8_t& c, level lv = level::truecolor) { return bg(c.r(), c.g(), c.b(), lv); }

// ===== Print Functions =====

/**
 * @brief Print a colored swatch block with hex value.
 * @param os Output stream.
 * @param c Any color type.
 * @param width Number of block characters to print.
 * @param lv Output color depth (issue #116).
 */
template <typename Color>
void print_swatch(std::ostream& os, const Color& c, int width = 6, level lv = level::truecolor) {
  auto rgba = details::to_rgba8(c);
  os << bg(rgba, lv) << std::string(width, ' ') << reset();
}

/**
 * @brief Print a color with full info: swatch + hex + RGB.
 * @param os Output stream.
 * @param c Any color type.
 * @param label Optional label.
 * @param lv Output color depth (issue #116).
 */
template <typename Color>
void print_color(std::ostream& os, const Color& c, std::string_view label = "", level lv = level::truecolor) {
  const details::ostream_state_guard guard(os);
  auto rgba = details::to_rgba8(c);

  // Swatch
  os << bg(rgba, lv) << "      " << reset() << " ";

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
 * @param lv Output color depth (issue #116).
 */
template <typename Color>
void print_color_verbose(std::ostream& os, const Color& c, std::string_view label = "", level lv = level::truecolor) {
  const details::ostream_state_guard guard(os);
  auto rgba = details::to_rgba8(c);

  // Swatch
  os << bg(rgba, lv) << "      " << reset() << " ";

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
 * @param lv Output color depth (issue #116).
 */
template <typename Color>
void print_palette(std::ostream& os, const Color* colors, std::size_t count, int swatch_width = 6,
                   level lv = level::truecolor) {
  for (std::size_t i = 0; i < count; ++i) {
    auto rgba = details::to_rgba8(colors[i]);
    os << bg(rgba, lv) << std::string(swatch_width, ' ') << reset();
  }
  os << "\n";
}

/**
 * @brief Print a palette from any contiguous container exposing data() and size().
 */
template <typename Container, typename std::enable_if_t<details::has_data_size<Container>::value, int> = 0>
void print_palette(std::ostream& os, const Container& colors, int swatch_width = 6, level lv = level::truecolor) {
  print_palette(os, colors.data(), static_cast<std::size_t>(colors.size()), swatch_width, lv);
}

/**
 * @brief Print a gradient between two colors.
 * @param os Output stream.
 * @param a Start color.
 * @param b End color.
 * @param steps Number of gradient steps.
 * @param lv Output color depth (issue #116).
 */
template <typename ColorA, typename ColorB>
void print_gradient(std::ostream& os, const ColorA& a, const ColorB& b, int steps = 16, level lv = level::truecolor) {
  auto ra = details::to_rgba8(a);
  auto rb = details::to_rgba8(b);

  if (steps <= 0) {
    os << "\n";
    return;
  }
  if (steps == 1) {
    os << bg(ra.r(), ra.g(), ra.b(), lv) << "  " << reset() << "\n";
    return;
  }

  for (int i = 0; i < steps; ++i) {
    float t = static_cast<float>(i) / static_cast<float>(steps - 1);
    auto r = static_cast<uint8_t>(std::round(ra.r() + (rb.r() - ra.r()) * t));
    auto g = static_cast<uint8_t>(std::round(ra.g() + (rb.g() - ra.g()) * t));
    auto bl = static_cast<uint8_t>(std::round(ra.b() + (rb.b() - ra.b()) * t));
    os << bg(r, g, bl, lv) << "  " << reset();
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
 * @param lv Output color depth for the preview swatch (issue #116).
 */
template <typename FgColor, typename BgColor>
void print_contrast(std::ostream& os, const FgColor& fg_color, const BgColor& bg_color, level lv = level::truecolor) {
  const details::ostream_state_guard guard(os);
  auto fg_rgba = details::to_rgba8(fg_color);
  auto bg_rgba = details::to_rgba8(bg_color);

  float ratio = contrast_ratio(fg_rgba, bg_rgba);
  auto grade = wcag_level(ratio);  // not `level`: that now names the enum

  // Preview: text on background
  os << bg(bg_rgba, lv) << fg(fg_rgba, lv) << bold() << "  Aa  " << reset() << " ";

  // Info
  os << "#" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(fg_rgba.r()) << std::setw(2)
     << static_cast<int>(fg_rgba.g()) << std::setw(2) << static_cast<int>(fg_rgba.b()) << std::dec << std::setfill(' ')
     << " on #";
  os << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(bg_rgba.r()) << std::setw(2)
     << static_cast<int>(bg_rgba.g()) << std::setw(2) << static_cast<int>(bg_rgba.b()) << std::dec << std::setfill(' ');

  os << "  contrast: " << std::fixed << std::setprecision(1) << ratio << ":1  " << grade;
}

}  // namespace colorcpp::io::ansi
