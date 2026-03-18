/**
 * @file literals.hpp
 * @brief Modern Color literal operators with explicit Alpha support
 *
 * This header provides a comprehensive set of literal operators for creating
 * color values in various color spaces at compile time. The operators support
 * both template-based (compile-time) and runtime literal syntaxes.
 *
 * Supported color spaces:
 * - RGB/RGBA: Hexadecimal and numeric literals
 * - HSV/HSVA: Decimal literals with fixed-width encoding
 * - HSL/HSLA: Decimal literals with fixed-width encoding
 * - CMYK:     Decimal literals with fixed-width encoding
 *
 * HSL/HSV encoding convention (3-channel):
 *   val = H * 1'000'000 + S * 1'000 + L  (S and L are 3-digit, 000–100)
 *   Example: 120'050'075_hsl  → hsl_float_t{120, 50, 75}
 *
 * HSL/HSV encoding convention (4-channel with alpha):
 *   val = H * 1'000'000'000 + S * 1'000'000 + L * 1'000 + A
 *   Example: 120'050'075'100_hsla → hsla_float_t{120, 50, 75, 100}
 *
 * CMYK encoding convention:
 *   val = C * 1'000'000'000 + M * 1'000'000 + Y * 1'000 + K  (each field 000–100)
 *   Example: 050'030'000'020_cmyk → cmyk8_t{50, 30, 0, 20}
 *
 * @author Merlot.Qi
 */

#pragma once

#include <colorcpp/core/cmyk.hpp>
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/hsv.hpp>
#include <colorcpp/core/rgb.hpp>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace colorcpp::literals {

namespace details {
/**
 * @brief Convert character to hexadecimal value
 * @param c Character to convert ('0'-'9', 'a'-'f', 'A'-'F')
 * @return Hexadecimal value (0-15) or 0 for invalid characters
 */
constexpr uint32_t char_to_hex(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return 0;
}

/**
 * @brief Parse template character sequence as hexadecimal uint64_t
 * @tparam Chars Character pack representing the hexadecimal literal
 * @return Parsed hexadecimal value as uint64_t
 * @note Supports optional 0x/0X prefix and digit separators (')
 */
template <char... Chars>
constexpr uint64_t parse_hex_template() {
  constexpr char s[] = {Chars...};
  uint64_t res = 0;
  constexpr size_t n = sizeof(s);
  size_t start = (n > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) ? 2 : 0;

  for (size_t i = start; i < n; ++i) {
    if (s[i] == '\'') continue;
    res = (res << 4) | char_to_hex(s[i]);
  }
  return res;
}

/**
 * @brief Parse template character sequence as decimal uint64_t
 * @tparam Chars Character pack representing the decimal literal
 * @return Parsed decimal value as uint64_t
 * @note Supports digit separators (')
 */
template <char... Chars>
constexpr uint64_t parse_dec_template() {
  constexpr char s[] = {Chars...};
  uint64_t res = 0;
  for (char c : s) {
    if (c >= '0' && c <= '9') {
      res = res * 10 + (c - '0');
    } else if (c == '\'')
      continue;
  }
  return res;
}

constexpr uint8_t hex_expand(char c) {
  unsigned v = static_cast<uint8_t>(char_to_hex(c));
  return (v << 4) | v;
}

constexpr core::rgba8_t parse_hex_string(const char* s, size_t n) {
  size_t offset = (n > 0 && s[0] == '#') ? 1 : 0;
  size_t len = n - offset;

  auto get_val = [&](size_t i) { return char_to_hex(s[offset + i]); };

  if (len == 3) {  // #RGB
    return core::rgba8_t{hex_expand(s[offset]), hex_expand(s[offset + 1]), hex_expand(s[offset + 2]), 255};
  } else if (len == 4) {  // #RGBA
    return core::rgba8_t{hex_expand(s[offset]), hex_expand(s[offset + 1]), hex_expand(s[offset + 2]),
                         hex_expand(s[offset + 3])};
  } else if (len == 6) {  // #RRGGBB
    return core::rgba8_t{static_cast<uint8_t>((get_val(0) << 4) | get_val(1)),
                         static_cast<uint8_t>((get_val(2) << 4) | get_val(3)),
                         static_cast<uint8_t>((get_val(4) << 4) | get_val(5)), 255};
  } else if (len == 8) {  // #RRGGBBAA
    return core::rgba8_t{
        static_cast<uint8_t>((get_val(0) << 4) | get_val(1)), static_cast<uint8_t>((get_val(2) << 4) | get_val(3)),
        static_cast<uint8_t>((get_val(4) << 4) | get_val(5)), static_cast<uint8_t>((get_val(6) << 4) | get_val(7))};
  } else {
    throw std::invalid_argument("colorcpp: invalid hex string length. Expected 3, 4, 6, or 8 characters.");
  }
}

}  // namespace details

inline namespace operators {

/**
 * @name RGB/RGBA Literal Operators
 * @{
 */

/**
 * @brief Template literal: compile-time RRGGBB hex → rgba8_t (alpha=255)
 * @example 0xFF0000_rgb → rgba8_t{255, 0, 0, 255}
 */
template <char... Chars>
constexpr auto operator"" _rgb() {
  constexpr uint64_t val = details::parse_hex_template<Chars...>();
  static_assert(val <= 0xFFFFFF, "colorcpp: _rgb template value exceeds 0xFFFFFF (24-bit limit)");
  return core::rgba8_t{(val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF, 255};
}

/**
 * @brief Template literal: compile-time RRGGBBAA hex → rgba8_t
 * @example 0xFF000080_rgba → rgba8_t{255, 0, 0, 128}
 */
template <char... Chars>
constexpr auto operator"" _rgba() {
  constexpr uint64_t val = details::parse_hex_template<Chars...>();
  static_assert(val <= 0xFFFFFFFF, "colorcpp: _rgba template value exceeds 0xFFFFFFFF (32-bit limit)");
  return core::rgba8_t{(val >> 24) & 0xFF, (val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF};
}

/**
 * @brief Template literal: compile-time AARRGGBB hex → rgba8_t
 * @example 0x80FF0000_argb → rgba8_t{255, 0, 0, 128}
 */
template <char... Chars>
constexpr auto operator"" _argb() {
  constexpr uint64_t val = details::parse_hex_template<Chars...>();
  static_assert(val <= 0xFFFFFFFF, "colorcpp: _argb template value exceeds 0xFFFFFFFF (32-bit limit)");
  return core::rgba8_t{(val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF, (val >> 24) & 0xFF};
}

/**
 * @brief Numeric literal: RRGGBB hex → rgba8_t (alpha=255)
 * @example 0xFF0000_rgb → rgba8_t{255, 0, 0, 255}
 */
constexpr auto operator"" _rgb(unsigned long long val) {
  if (val > 0xFFFFFF) {
    throw std::out_of_range("colorcpp: _rgb value exceeds 0xFFFFFF (24-bit limit)");
  }
  return core::rgba8_t{static_cast<uint8_t>((val >> 16) & 0xFF), static_cast<uint8_t>((val >> 8) & 0xFF),
                       static_cast<uint8_t>(val & 0xFF), 255};
}

/**
 * @brief Numeric literal: RRGGBBAA hex → rgba8_t
 * @example 0xFF000080_rgba → rgba8_t{255, 0, 0, 128}
 */
constexpr auto operator"" _rgba(unsigned long long val) {
  if (val > 0xFFFFFFFF) {
    throw std::out_of_range("colorcpp: _rgba value exceeds 0xFFFFFFFF (32-bit limit)");
  }
  return core::rgba8_t{static_cast<uint8_t>((val >> 24) & 0xFF), static_cast<uint8_t>((val >> 16) & 0xFF),
                       static_cast<uint8_t>((val >> 8) & 0xFF), static_cast<uint8_t>(val & 0xFF)};
}

/**
 * @brief Numeric literal: AARRGGBB hex → rgba8_t
 * @example 0x80FF0000_argb → rgba8_t{255, 0, 0, 128}
 */
constexpr auto operator"" _argb(unsigned long long val) {
  if (val > 0xFFFFFFFF) {
    throw std::out_of_range("colorcpp: _argb value exceeds 0xFFFFFFFF (32-bit limit)");
  }
  return core::rgba8_t{static_cast<uint8_t>((val >> 16) & 0xFF), static_cast<uint8_t>((val >> 8) & 0xFF),
                       static_cast<uint8_t>(val & 0xFF), static_cast<uint8_t>((val >> 24) & 0xFF)};
}

/**
 * @brief String literal: "#RGB", "#RGBA", "#RRGGBB", "#RRGGBBAA" → rgba8_t
 * @example "#FF6347"_hex → rgba8_t{255, 99, 71, 255}
 */
constexpr auto operator"" _hex(const char* str, size_t len) { return details::parse_hex_string(str, len); }

/** @} */

/**
 * @name HSL Literal Operators
 *
 * Encoding: val = H * 1'000'000 + S * 1'000 + L
 *   - H: 0–360 (degrees), S/L: 000–100 (three-digit percentage)
 * Example: 120'050'075_hsl → hsl_float_t{120, 50, 75}
 *
 * 4-channel encoding: val = H * 1'000'000'000 + S * 1'000'000 + L * 1'000 + A
 * Example: 120'050'075'100_hsla → hsla_float_t{120, 50, 75, 100}
 * @{
 */

// Note: only template forms are provided for HSL/HSV/CMYK.
// The template UDL receives raw characters, so C++ octal parsing (0xxx) is bypassed.
// Write: 120'050'075_hsl  (H=120, S=50, L=75)
//        0'100'050_hsl    (H=0,   S=100, L=50) — leading zero is safe here

template <char... Chars>
constexpr auto operator"" _hsl() {
  constexpr uint64_t val = details::parse_dec_template<Chars...>();
  constexpr uint64_t h = val / 1000000ULL;
  constexpr uint64_t s = (val / 1000ULL) % 1000ULL;
  constexpr uint64_t l = val % 1000ULL;
  static_assert(h <= 360, "colorcpp: _hsl H out of range (0–360)");
  static_assert(s <= 100, "colorcpp: _hsl S out of range (000–100)");
  static_assert(l <= 100, "colorcpp: _hsl L out of range (000–100)");
  return core::hsl_float_t{static_cast<float>(h), static_cast<float>(s), static_cast<float>(l)};
}

template <char... Chars>
constexpr auto operator"" _hsla() {
  constexpr uint64_t val = details::parse_dec_template<Chars...>();
  constexpr uint64_t h = val / 1000000000ULL;
  constexpr uint64_t s = (val / 1000000ULL) % 1000ULL;
  constexpr uint64_t l = (val / 1000ULL) % 1000ULL;
  constexpr uint64_t a = val % 1000ULL;
  static_assert(h <= 360, "colorcpp: _hsla H out of range (0–360)");
  static_assert(s <= 100, "colorcpp: _hsla S out of range (000–100)");
  static_assert(l <= 100, "colorcpp: _hsla L out of range (000–100)");
  static_assert(a <= 100, "colorcpp: _hsla A out of range (000–100)");
  return core::hsla_float_t{static_cast<float>(h), static_cast<float>(s), static_cast<float>(l), static_cast<float>(a)};
}

/** @} */

/**
 * @name HSV Literal Operators
 *
 * Encoding: val = H * 1'000'000 + S * 1'000 + V
 *   - H: 0–360 (degrees), S/V: 000–100 (three-digit percentage)
 * Example: 210'080'090_hsv → hsv_float_t{210, 80, 90}
 *
 * 4-channel encoding: val = H * 1'000'000'000 + S * 1'000'000 + V * 1'000 + A
 * Example: 210'080'090'100_hsva → hsva_float_t{210, 80, 90, 100}
 * @{
 */

template <char... Chars>
constexpr auto operator"" _hsv() {
  constexpr uint64_t val = details::parse_dec_template<Chars...>();
  constexpr uint64_t h = val / 1000000ULL;
  constexpr uint64_t s = (val / 1000ULL) % 1000ULL;
  constexpr uint64_t v = val % 1000ULL;
  static_assert(h <= 360, "colorcpp: _hsv H out of range (0–360)");
  static_assert(s <= 100, "colorcpp: _hsv S out of range (000–100)");
  static_assert(v <= 100, "colorcpp: _hsv V out of range (000–100)");
  return core::hsv_float_t{static_cast<float>(h), static_cast<float>(s), static_cast<float>(v)};
}

template <char... Chars>
constexpr auto operator"" _hsva() {
  constexpr uint64_t val = details::parse_dec_template<Chars...>();
  constexpr uint64_t h = val / 1000000000ULL;
  constexpr uint64_t s = (val / 1000000ULL) % 1000ULL;
  constexpr uint64_t v = (val / 1000ULL) % 1000ULL;
  constexpr uint64_t a = val % 1000ULL;
  static_assert(h <= 360, "colorcpp: _hsva H out of range (0–360)");
  static_assert(s <= 100, "colorcpp: _hsva S out of range (000–100)");
  static_assert(v <= 100, "colorcpp: _hsva V out of range (000–100)");
  static_assert(a <= 100, "colorcpp: _hsva A out of range (000–100)");
  return core::hsva_float_t{static_cast<float>(h), static_cast<float>(s), static_cast<float>(v), static_cast<float>(a)};
}

/** @} */

/**
 * @name CMYK Literal Operators
 *
 * Encoding: val = C * 1'000'000'000 + M * 1'000'000 + Y * 1'000 + K
 *   - Each component: 000–100 (three-digit percentage), maps to cmyk8_t [0,100]
 * Example: 050'030'000'020_cmyk → cmyk8_t{50, 30, 0, 20}
 * @{
 */

template <char... Chars>
constexpr auto operator"" _cmyk() {
  constexpr uint64_t val = details::parse_dec_template<Chars...>();
  constexpr uint64_t c = val / 1000000000ULL;
  constexpr uint64_t m = (val / 1000000ULL) % 1000ULL;
  constexpr uint64_t y = (val / 1000ULL) % 1000ULL;
  constexpr uint64_t k = val % 1000ULL;
  static_assert(c <= 100, "colorcpp: _cmyk C out of range (000–100)");
  static_assert(m <= 100, "colorcpp: _cmyk M out of range (000–100)");
  static_assert(y <= 100, "colorcpp: _cmyk Y out of range (000–100)");
  static_assert(k <= 100, "colorcpp: _cmyk K out of range (000–100)");
  return core::cmyk8_t{static_cast<uint8_t>(c), static_cast<uint8_t>(m), static_cast<uint8_t>(y),
                       static_cast<uint8_t>(k)};
}

/** @} */

}  // namespace operators

}  // namespace colorcpp::literals
