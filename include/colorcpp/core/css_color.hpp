/**
 * @file css_color.hpp
 * @brief Parse CSS Color Level 4 subset: hex, rgb()/rgba(), hsl()/hsla() into @ref colorcpp::core::rgba8_t.
 *
 * @note Not supported yet: named colors, @c color(), @c lab()/ @c lch()/ @c oklab()/ @c oklch(), relative syntax,
 *       @c hwb(), @c device-cmyk(). See README "CSS color parsing".
 */

#pragma once

#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <optional>
#include <string_view>

namespace colorcpp::core {

namespace css_details {

constexpr bool is_space(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f';
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

inline std::optional<rgba8_t> parse_hex(std::string_view s) {
  if (s.empty() || s[0] != '#') return std::nullopt;
  s.remove_prefix(1);
  size_t len = s.size();

  auto get_val = [&](size_t i) -> std::optional<int> {
    int v = to_hex_digit(s[i]);
    if (v < 0) return std::nullopt;
    return v;
  };

  if (len == 3) {
    if (to_hex_digit(s[0]) < 0 || to_hex_digit(s[1]) < 0 || to_hex_digit(s[2]) < 0) return std::nullopt;
    return rgba8_t{hex_expand(s[0]), hex_expand(s[1]), hex_expand(s[2]), 255};
  }
  if (len == 4) {
    for (size_t i = 0; i < 4; ++i)
      if (to_hex_digit(s[i]) < 0) return std::nullopt;
    return rgba8_t{hex_expand(s[0]), hex_expand(s[1]), hex_expand(s[2]), hex_expand(s[3])};
  }
  if (len == 6) {
    for (size_t i = 0; i < 6; ++i)
      if (to_hex_digit(s[i]) < 0) return std::nullopt;
    auto a0 = get_val(0), a1 = get_val(1), a2 = get_val(2), a3 = get_val(3), a4 = get_val(4), a5 = get_val(5);
    return rgba8_t{static_cast<uint8_t>((*a0 << 4) | *a1), static_cast<uint8_t>((*a2 << 4) | *a3),
                    static_cast<uint8_t>((*a4 << 4) | *a5), 255};
  }
  if (len == 8) {
    for (size_t i = 0; i < 8; ++i)
      if (to_hex_digit(s[i]) < 0) return std::nullopt;
    auto v0 = get_val(0), v1 = get_val(1), v2 = get_val(2), v3 = get_val(3);
    auto v4 = get_val(4), v5 = get_val(5), v6 = get_val(6), v7 = get_val(7);
    return rgba8_t{static_cast<uint8_t>((*v0 << 4) | *v1), static_cast<uint8_t>((*v2 << 4) | *v3),
                    static_cast<uint8_t>((*v4 << 4) | *v5), static_cast<uint8_t>((*v6 << 4) | *v7)};
  }
  return std::nullopt;
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

inline std::optional<rgba8_t> parse_rgb_function(Cursor& c) {
  bool rgba = c.consume_ci("rgba");
  if (!rgba && !c.consume_ci("rgb")) return std::nullopt;
  c.skip_ws();
  if (!c.consume_char('(')) return std::nullopt;
  c.skip_ws();

  size_t checkpoint = c.i;
  auto try_comma = [&]() -> std::optional<rgba8_t> {
    c.i = checkpoint;
    Cursor d{c.s, c.i};
    auto a = d.parse_component_value();
    if (!a) return std::nullopt;
    d.skip_ws();
    if (!d.consume_char(',')) return std::nullopt;
    auto b = d.parse_component_value();
    if (!b) return std::nullopt;
    d.skip_ws();
    if (!d.consume_char(',')) return std::nullopt;
    auto g = d.parse_component_value();
    if (!g) return std::nullopt;
    uint8_t r = static_cast<uint8_t>(d.channel_from_component(*a));
    uint8_t gch = static_cast<uint8_t>(d.channel_from_component(*b));
    uint8_t bch = static_cast<uint8_t>(d.channel_from_component(*g));
    d.skip_ws();
    uint8_t alpha = 255;
    if (d.consume_char(',')) {
      auto av = d.parse_alpha_value();
      if (!av) return std::nullopt;
      alpha = d.alpha_byte_from_double(*av);
    }
    d.skip_ws();
    if (!d.consume_char(')')) return std::nullopt;
    d.skip_ws();
    if (!d.eof()) return std::nullopt;
    c.i = d.i;
    return rgba8_t{r, gch, bch, alpha};
  };

  auto try_space = [&]() -> std::optional<rgba8_t> {
    c.i = checkpoint;
    Cursor d{c.s, c.i};
    auto a = d.parse_component_value();
    if (!a) return std::nullopt;
    d.skip_ws();
    auto b = d.parse_component_value();
    if (!b) return std::nullopt;
    d.skip_ws();
    auto g = d.parse_component_value();
    if (!g) return std::nullopt;
    uint8_t r = static_cast<uint8_t>(d.channel_from_component(*a));
    uint8_t gch = static_cast<uint8_t>(d.channel_from_component(*b));
    uint8_t bch = static_cast<uint8_t>(d.channel_from_component(*g));
    d.skip_ws();
    uint8_t alpha = 255;
    if (d.consume_char('/')) {
      auto av = d.parse_alpha_value();
      if (!av) return std::nullopt;
      alpha = d.alpha_byte_from_double(*av);
    }
    d.skip_ws();
    if (!d.consume_char(')')) return std::nullopt;
    d.skip_ws();
    if (!d.eof()) return std::nullopt;
    c.i = d.i;
    return rgba8_t{r, gch, bch, alpha};
  };

  if (auto r = try_comma()) return r;
  if (auto r = try_space()) return r;
  return std::nullopt;
}

inline std::optional<rgba8_t> parse_hsl_function(Cursor& c) {
  bool hsla = c.consume_ci("hsla");
  if (!hsla && !c.consume_ci("hsl")) return std::nullopt;
  c.skip_ws();
  if (!c.consume_char('(')) return std::nullopt;
  c.skip_ws();

  size_t checkpoint = c.i;

  auto try_comma = [&]() -> std::optional<rgba8_t> {
    c.i = checkpoint;
    Cursor d{c.s, c.i};
    auto h = d.parse_hue_angle();
    if (!h) return std::nullopt;
    d.skip_ws();
    if (!d.consume_char(',')) return std::nullopt;
    auto s = d.parse_hsl_sl();
    if (!s) return std::nullopt;
    d.skip_ws();
    if (!d.consume_char(',')) return std::nullopt;
    auto l = d.parse_hsl_sl();
    if (!l) return std::nullopt;
    float alpha_f = 1.0f;
    d.skip_ws();
    if (d.consume_char(',')) {
      auto av = d.parse_alpha_value();
      if (!av) return std::nullopt;
      double a = *av;
      if (a < 0.0) a = 0.0;
      if (a > 1.0) a = 1.0;
      alpha_f = static_cast<float>(a);
    }
    d.skip_ws();
    if (!d.consume_char(')')) return std::nullopt;
    d.skip_ws();
    if (!d.eof()) return std::nullopt;
    hsla_float_t hf{static_cast<float>(*h), static_cast<float>(*s), static_cast<float>(*l), alpha_f};
    rgba8_t out = operations::conversion::color_cast<rgba8_t>(hf);
    c.i = d.i;
    return out;
  };

  auto try_space = [&]() -> std::optional<rgba8_t> {
    c.i = checkpoint;
    Cursor d{c.s, c.i};
    auto h = d.parse_hue_angle();
    if (!h) return std::nullopt;
    d.skip_ws();
    auto s = d.parse_hsl_sl();
    if (!s) return std::nullopt;
    d.skip_ws();
    auto l = d.parse_hsl_sl();
    if (!l) return std::nullopt;
    float alpha_f = 1.0f;
    d.skip_ws();
    if (d.consume_char('/')) {
      auto av = d.parse_alpha_value();
      if (!av) return std::nullopt;
      double a = *av;
      if (a < 0.0) a = 0.0;
      if (a > 1.0) a = 1.0;
      alpha_f = static_cast<float>(a);
    }
    d.skip_ws();
    if (!d.consume_char(')')) return std::nullopt;
    d.skip_ws();
    if (!d.eof()) return std::nullopt;
    hsla_float_t hf{static_cast<float>(*h), static_cast<float>(*s), static_cast<float>(*l), alpha_f};
    rgba8_t out = operations::conversion::color_cast<rgba8_t>(hf);
    c.i = d.i;
    return out;
  };

  if (auto r = try_comma()) return r;
  if (auto r = try_space()) return r;
  return std::nullopt;
}

}  // namespace css_details

/**
 * @brief Parse a CSS color string into sRGB 8-bit with alpha.
 * @return @c std::nullopt on syntax error or unsupported form.
 */
inline std::optional<rgba8_t> parse_css_color_rgba8(std::string_view str) {
  using namespace css_details;
  trim(str);
  if (str.empty()) return std::nullopt;

  if (str[0] == '#') return parse_hex(str);

  Cursor c{str, 0};
  if (auto r = parse_rgb_function(c)) return r;
  c.i = 0;
  if (auto r = parse_hsl_function(c)) return r;

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

}  // namespace colorcpp::core
