/**
 * @file details.hpp
 * @brief Internal helpers for CSS color string parsing.
 */

#pragma once

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <optional>
#include <string>
#include <string_view>

namespace colorcpp::io::css::details {

constexpr bool is_space(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f'; }

inline bool equals_ci(std::string_view a, std::string_view b) {
  if (a.size() != b.size()) return false;
  for (size_t i = 0; i < a.size(); ++i) {
    if (std::tolower(static_cast<unsigned char>(a[i])) != std::tolower(static_cast<unsigned char>(b[i]))) return false;
  }
  return true;
}

inline void trim(std::string_view& s) {
  while (!s.empty() && is_space(s.front())) s.remove_prefix(1);
  while (!s.empty() && is_space(s.back())) s.remove_suffix(1);
}

inline int to_hex_digit(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return -1;
}

inline uint8_t hex_expand(char c) {
  int v = to_hex_digit(c);
  if (v < 0) return 0;
  return static_cast<uint8_t>((v << 4) | v);
}

struct Cursor {
  std::string_view s;
  size_t i = 0;

  void skip_ws() {
    while (i < s.size() && is_space(s[i])) ++i;
  }

  bool eof() const { return i >= s.size(); }

  char peek() const { return eof() ? '\0' : s[i]; }

  bool consume_char(char c) {
    skip_ws();
    if (!eof() && s[i] == c) {
      ++i;
      return true;
    }
    return false;
  }

  bool consume_ci(std::string_view lit) {
    skip_ws();
    if (i + lit.size() > s.size()) return false;
    for (size_t k = 0; k < lit.size(); ++k) {
      char a = s[i + k];
      char b = lit[k];
      if (std::tolower(static_cast<unsigned char>(a)) != std::tolower(static_cast<unsigned char>(b))) return false;
    }
    i += lit.size();
    return true;
  }

  std::optional<double> parse_number() {
    skip_ws();
    if (eof()) return std::nullopt;
    size_t start = i;
    if (s[i] == '+' || s[i] == '-') ++i;
    bool has_digit = false;
    while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) {
      has_digit = true;
      ++i;
    }
    if (i < s.size() && s[i] == '.') {
      ++i;
      while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) {
        has_digit = true;
        ++i;
      }
    }
    if (!has_digit) return std::nullopt;
    std::string buf(s.substr(start, i - start));
    char* end = nullptr;
    double v = std::strtod(buf.c_str(), &end);
    if (end == buf.c_str()) return std::nullopt;
    return v;
  }

  /** Parse rgb/hsl component: number [optional %] */
  std::optional<std::pair<double, bool>> parse_component_value() {
    auto num = parse_number();
    if (!num) return std::nullopt;
    skip_ws();
    bool pct = false;
    if (!eof() && s[i] == '%') {
      pct = true;
      ++i;
    }
    return std::pair<double, bool>{*num, pct};
  }

  std::optional<double> parse_alpha_value() {
    auto cv = parse_component_value();
    if (!cv) return std::nullopt;
    double v = cv->first;
    if (cv->second) {
      if (v < 0.0) v = 0.0;
      if (v > 100.0) v = 100.0;
      return v / 100.0;
    }
    return v;
  }

  int channel_from_component(const std::pair<double, bool>& cv) {
    double v = cv.first;
    if (cv.second) {
      if (v < 0.0) v = 0.0;
      if (v > 100.0) v = 100.0;
      long x = std::lround(v * 255.0 / 100.0);
      if (x < 0) return 0;
      if (x > 255) return 255;
      return static_cast<int>(x);
    }
    if (v < 0.0) v = 0.0;
    if (v > 255.0) v = 255.0;
    return static_cast<int>(std::lround(v));
  }

  /** sRGB @c rgb() / @c rgba() channel as linearized 0–1 float (no 8-bit quantization). */
  float channel_float_01_from_rgb_component(const std::pair<double, bool>& cv) {
    double v = cv.first;
    if (cv.second) {
      v = std::clamp(v, 0.0, 100.0) / 100.0;
      return static_cast<float>(v);
    }
    v = std::clamp(v, 0.0, 255.0);
    return static_cast<float>(v / 255.0);
  }

  /** Consume case-insensitive @c none (CSS Color 4 missing component); @ref skip_ws first. */
  bool try_consume_none() {
    skip_ws();
    return consume_ci("none");
  }

  uint8_t alpha_byte_from_double(double a) {
    if (a < 0.0) a = 0.0;
    if (a > 1.0) a = 1.0;
    long x = std::lround(a * 255.0);
    if (x < 0) return 0;
    if (x > 255) return 255;
    return static_cast<uint8_t>(x);
  }

  std::optional<double> parse_hue_angle() {
    auto num = parse_number();
    if (!num) return std::nullopt;
    skip_ws();
    double deg = *num;
    if (consume_ci("deg"))
      ;
    else if (consume_ci("grad"))
      deg = *num * (360.0 / 400.0);
    else if (consume_ci("rad"))
      deg = *num * (180.0 / 3.14159265358979323846);
    else if (consume_ci("turn"))
      deg = *num * 360.0;
    else
      deg = *num;
    deg = std::fmod(deg, 360.0);
    if (deg < 0.0) deg += 360.0;
    return deg;
  }

  // Parse HWB hue angle (CSS Color Level 4 allows degree units)
  std::optional<double> parse_hwb_hue_angle() {
    auto num = parse_number();
    if (!num) return std::nullopt;
    skip_ws();
    double deg = *num;
    if (consume_ci("deg"))
      ;
    else if (consume_ci("grad"))
      deg = *num * (360.0 / 400.0);
    else if (consume_ci("rad"))
      deg = *num * (180.0 / 3.14159265358979323846);
    else if (consume_ci("turn"))
      deg = *num * 360.0;
    else
      deg = *num;
    deg = std::fmod(deg, 360.0);
    if (deg < 0.0) deg += 360.0;
    return deg;
  }

  std::optional<double> parse_hsl_sl() {
    auto cv = parse_component_value();
    if (!cv) return std::nullopt;
    double x = cv->first;
    if (cv->second) {
      if (x < 0.0) x = 0.0;
      if (x > 100.0) x = 100.0;
      return x / 100.0;
    }
    if (x < 0.0) x = 0.0;
    if (x > 100.0) x = 100.0;
    return x / 100.0;
  }
};

/**
 * @brief CSS Color 4 HWB: if whiteness + blackness exceeds 100%, scale both so the sum is 100%.
 * @param w In/out: whiteness as a fraction in [0, 1] (e.g. after @ref Cursor::parse_hsl_sl).
 * @param b In/out: blackness as a fraction in [0, 1].
 * @see https://www.w3.org/TR/css-color-4/#hwb-to-srgb
 */
inline void normalize_hwb_whiteness_blackness(double& w, double& b) {
  const double sum = w + b;
  if (sum > 1.0) {
    w /= sum;
    b /= sum;
  }
}

/** @brief Index of first top-level comma in @p s at or after @p start (parenthesis depth only). */
inline size_t find_top_level_comma(std::string_view s, size_t start = 0) {
  int depth = 0;
  for (size_t i = start; i < s.size(); ++i) {
    const char ch = s[i];
    if (ch == '(') {
      ++depth;
    } else if (ch == ')') {
      if (depth > 0) --depth;
    } else if (ch == ',' && depth == 0) {
      return i;
    }
  }
  return std::string_view::npos;
}

/**
 * @brief After optional whitespace, consume @c '(' and return inner contents up to matching @c ')'.
 * @post On success, @p c.i is immediately after the closing @c ')'.
 */
inline std::optional<std::string_view> consume_parenthesized_contents(Cursor& c) {
  c.skip_ws();
  if (!c.consume_char('(')) return std::nullopt;
  const size_t start = c.i;
  int depth = 1;
  while (c.i < c.s.size()) {
    const char ch = c.s[c.i++];
    if (ch == '(') {
      ++depth;
    } else if (ch == ')') {
      if (--depth == 0) {
        return c.s.substr(start, c.i - 1 - start);
      }
    }
  }
  return std::nullopt;
}

}  // namespace colorcpp::io::css::details
