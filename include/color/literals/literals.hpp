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
 * - HSV/HSVA: Decimal literals with configurable scale
 * - HSL/HSLA: Decimal literals with configurable scale
 * - CMYK: Decimal literals with configurable scale
 * - Hue shortcuts: Simplified hue-only specifications
 *
 * @author Merlot.Qi
 */

#pragma once

#include <color/core/cmyk.hpp>
#include <color/core/hsl.hpp>
#include <color/core/hsv.hpp>
#include <color/core/rgb.hpp>
#include <cstddef>
#include <cstdint>

namespace color::literals {

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
    return {hex_expand(s[offset]), hex_expand(s[offset + 1]), hex_expand(s[offset + 2]), 255};
  } else if (len == 4) {  // #RGBA
    return {hex_expand(s[offset]), hex_expand(s[offset + 1]), hex_expand(s[offset + 2]), hex_expand(s[offset + 3])};
  } else if (len == 6) {  // #RRGGBB
    return {static_cast<uint8_t>((get_val(0) << 4) | get_val(1)), static_cast<uint8_t>((get_val(2) << 4) | get_val(3)),
            static_cast<uint8_t>((get_val(4) << 4) | get_val(5)), 255};
  } else if (len == 8) {  // #RRGGBBAA
    return {static_cast<uint8_t>((get_val(0) << 4) | get_val(1)), static_cast<uint8_t>((get_val(2) << 4) | get_val(3)),
            static_cast<uint8_t>((get_val(4) << 4) | get_val(5)), static_cast<uint8_t>((get_val(6) << 4) | get_val(7))};
  }

  // 格式错误默认返回黑
  return {0, 0, 0, 255};
}

}  // namespace details

inline namespace operators {

/**
 * @name RGB/RGBA Literal Operators
 * @{
 */

/**
 * @brief Template literal: provides compile-time rgba8 type
 * @tparam Chars Character pack representing hex color (e.g., "FF0000")
 * @return Compile-time rgba8 color with alpha=255
 * @note Supports 6-digit hex format (RRGGBB) with optional separators
 * @example 0xFF0000_rgb creates red color at compile time
 */
template <char... Chars>
constexpr auto operator"" _rgb() {
  constexpr uint64_t val = details::parse_hex_template<Chars...>();
  return core::rgba8<(val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF, 255>;
}

/**
 * @brief Template literal: provides compile-time rgba8 type with alpha
 * @tparam Chars Character pack representing hex color (e.g., "FF0000FF")
 * @return Compile-time rgba8 color with specified alpha
 * @note Supports 8-digit hex format (AARRGGBB) with optional separators
 * @example 0xFF000080_rgba creates semi-transparent red at compile time
 */
template <char... Chars>
constexpr auto operator"" _rgba() {
  constexpr uint64_t val = details::parse_hex_template<Chars...>();
  return core::rgba8<(val >> 24) & 0xFF, (val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF>;
}

/**
 * @brief Numeric literal: provides constexpr rgba8_t instance
 * @param val Hexadecimal value (RRGGBB format)
 * @return Runtime rgba8_t color with alpha=255
 * @note Supports 6-digit hex values (0xRRGGBB)
 * @example 0xFF0000_rgb creates red color at runtime
 */
constexpr auto operator"" _rgb(unsigned long long val) {
  return core::rgba8_t{static_cast<uint8_t>((val >> 16) & 0xFF), static_cast<uint8_t>((val >> 8) & 0xFF),
                       static_cast<uint8_t>(val & 0xFF), 255};
}

/**
 * @brief Numeric literal: provides constexpr rgba8_t instance with alpha
 * @param val Hexadecimal value (RRGGBBAA format)
 * @return Runtime rgba8_t color with specified alpha
 * @note Supports 8-digit hex values (0xRRGGBBAA) where alpha is in the least significant byte
 * @example 0xFF000080_rgba creates semi-transparent red at runtime with alpha=0x80
 * @example 0x00FF00FF_rgba creates fully opaque green at runtime
 */
constexpr auto operator"" _rgba(unsigned long long val) {
  return core::rgba8_t{static_cast<uint8_t>((val >> 24) & 0xFF), static_cast<uint8_t>((val >> 16) & 0xFF),
                       static_cast<uint8_t>((val >> 8) & 0xFF), static_cast<uint8_t>(val & 0xFF)};
}

/**
 * @brief Numeric literal: provides constexpr rgba8_t instance with ARGB format
 * @param val Hexadecimal value (AARRGGBB format)
 * @return Runtime rgba8_t color with specified alpha
 * @note Supports 8-digit hex values (0xAARRGGBB) where alpha is in the most significant byte
 * @example 0x80FF0000_argb creates semi-transparent red at runtime with alpha=0x80
 * @example 0xFF00FF00_argb creates fully opaque cyan at runtime
 */
constexpr auto operator"" _argb(unsigned long long val) {
  return core::rgba8_t{static_cast<uint8_t>((val >> 16) & 0xFF), static_cast<uint8_t>((val >> 8) & 0xFF),
                       static_cast<uint8_t>(val & 0xFF), static_cast<uint8_t>((val >> 24) & 0xFF)};
}

constexpr auto operator"" _hex(const char* str, size_t len) { return details::parse_hex_string(str, len); }

/** @} */

/**
 * @name HSV/HSVA Literal Operators
 * @{
 */

/**
 * @brief Numeric literal: creates HSV color with scale 1000
 * @param val Decimal value in format HHHSSTS (Hue, Saturation, Value)
 * @return HSV color with alpha=1000 (opaque)
 * @note Hue: 0-360, Saturation: 0-1000, Value: 0-1000
 * @example 120500750_hsv creates HSV(120°, 50%, 75%)
 */
constexpr auto operator"" _hsv(unsigned long long val) {
  return core::basic_hsva<intptr_t, 1000>{static_cast<intptr_t>(val / 1000000),
                                          static_cast<intptr_t>((val / 1000) % 1000), static_cast<intptr_t>(val % 1000),
                                          1000};
}

/**
 * @brief Numeric literal: creates HSVA color with scale 1000
 * @param val Decimal value in format HHHSSTTA (Hue, Saturation, Value, Alpha)
 * @return HSVA color with specified alpha
 * @note Hue: 0-360, Saturation: 0-1000, Value: 0-1000, Alpha: 0-1000
 * @example 120500750800_hsva creates HSV(120°, 50%, 75%) with 80% alpha
 */
constexpr auto operator"" _hsva(unsigned long long val) {
  return core::basic_hsva<intptr_t, 1000>{
      static_cast<intptr_t>(val / 1000000000), static_cast<intptr_t>((val / 1000000) % 1000),
      static_cast<intptr_t>((val / 1000) % 1000), static_cast<intptr_t>(val % 1000)};
}

/**
 * @brief Template literal: creates HSV color from decimal string
 * @tparam Chars Character pack representing decimal HSV values
 * @return HSV color with alpha=1000 (opaque)
 * @note Same format as numeric literal but parsed at compile time
 */
template <char... Chars>
constexpr auto operator"" _hsv() {
  return operator"" _hsv(details::parse_dec_template<Chars...>());
}

/**
 * @brief Template literal: creates HSVA color from decimal string
 * @tparam Chars Character pack representing decimal HSVA values
 * @return HSVA color with specified alpha
 * @note Same format as numeric literal but parsed at compile time
 */
template <char... Chars>
constexpr auto operator"" _hsva() {
  return operator"" _hsva(details::parse_dec_template<Chars...>());
}

/** @} */

/**
 * @name HSL/HSLA Literal Operators
 * @{
 */

/**
 * @brief Numeric literal: creates HSL color with scale 1000
 * @param val Decimal value in format HHHSSTS (Hue, Saturation, Lightness)
 * @return HSL color with alpha=1000 (opaque)
 * @note Hue: 0-360, Saturation: 0-1000, Lightness: 0-1000
 * @example 240750500_hsl creates HSL(240°, 75%, 50%)
 */
constexpr auto operator"" _hsl(unsigned long long val) {
  return core::basic_hsla<intptr_t, 1000>{static_cast<intptr_t>(val / 1000000),
                                          static_cast<intptr_t>((val / 1000) % 1000), static_cast<intptr_t>(val % 1000),
                                          1000};
}

/**
 * @brief Numeric literal: creates HSLA color with scale 1000
 * @param val Decimal value in format HHHSSTTA (Hue, Saturation, Lightness, Alpha)
 * @return HSLA color with specified alpha
 * @note Hue: 0-360, Saturation: 0-1000, Lightness: 0-1000, Alpha: 0-1000
 * @example 240750500900_hsla creates HSL(240°, 75%, 50%) with 90% alpha
 */
constexpr auto operator"" _hsla(unsigned long long val) {
  return core::basic_hsla<intptr_t, 1000>{
      static_cast<intptr_t>(val / 1000000000), static_cast<intptr_t>((val / 1000000) % 1000),
      static_cast<intptr_t>((val / 1000) % 1000), static_cast<intptr_t>(val % 1000)};
}

/**
 * @brief Template literal: creates HSL color from decimal string
 * @tparam Chars Character pack representing decimal HSL values
 * @return HSL color with alpha=1000 (opaque)
 * @note Same format as numeric literal but parsed at compile time
 */
template <char... Chars>
constexpr auto operator"" _hsl() {
  return operator"" _hsl(details::parse_dec_template<Chars...>());
}

/**
 * @brief Template literal: creates HSLA color from decimal string
 * @tparam Chars Character pack representing decimal HSLA values
 * @return HSLA color with specified alpha
 * @note Same format as numeric literal but parsed at compile time
 */
template <char... Chars>
constexpr auto operator"" _hsla() {
  return operator"" _hsla(details::parse_dec_template<Chars...>());
}

/** @} */

/**
 * @name CMYK Literal Operators
 * @{
 */

/**
 * @brief Numeric literal: creates CMYK color with scale 1000
 * @param val Decimal value in format CCCMMMYYYKKK (Cyan, Magenta, Yellow, Black)
 * @return CMYK color with specified components
 * @note Each component: 0-1000 representing 0-100%
 * @example 100000100000_cmyk creates CMYK(100%, 0%, 100%, 0%) (magenta)
 */
constexpr auto operator"" _cmyk(unsigned long long val) {
  return core::basic_cmyk<intptr_t, 1000>{
      static_cast<intptr_t>(val / 1000000000ULL), static_cast<intptr_t>((val / 1000000ULL) % 1000),
      static_cast<intptr_t>((val / 1000ULL) % 1000), static_cast<intptr_t>(val % 1000)};
}

/**
 * @brief Template literal: creates CMYK color from decimal string
 * @tparam Chars Character pack representing decimal CMYK values
 * @return CMYK color with specified components
 * @note Same format as numeric literal but parsed at compile time
 */
template <char... Chars>
constexpr auto operator"" _cmyk() {
  return operator"" _cmyk(details::parse_dec_template<Chars...>());
}

/** @} */

/**
 * @name Hue Shortcuts (Alpha defaults to opaque)
 * @{
 */

/**
 * @brief Hue shortcut: creates HSV color with maximum saturation and value
 * @param h Hue value (0-360)
 * @return HSV color with S=1000, V=1000, A=1000 (fully saturated, bright, opaque)
 * @note Useful for creating pure, vibrant colors by specifying only hue
 * @example 120_hsv_hue creates pure green (HSV 120°, 100%, 100%)
 */
constexpr auto operator"" _hsv_hue(unsigned long long h) {
  return core::basic_hsva<intptr_t, 1000>{static_cast<intptr_t>(h), 1000, 1000, 1000};
}

/**
 * @brief Hue shortcut: creates HSL color with maximum saturation and 50% lightness
 * @param h Hue value (0-360)
 * @return HSL color with S=1000, L=500, A=1000 (fully saturated, medium brightness, opaque)
 * @note Useful for creating pure, vibrant colors by specifying only hue
 * @example 240_hsl_hue creates pure blue (HSL 240°, 100%, 50%)
 */
constexpr auto operator"" _hsl_hue(unsigned long long h) {
  return core::basic_hsla<intptr_t, 1000>{static_cast<intptr_t>(h), 1000, 500, 1000};
}

/**
 * @brief Template literal: creates HSV hue shortcut from string
 * @tparam Chars Character pack representing hue value
 * @return HSV color with maximum saturation and value
 * @note Same format as numeric literal but parsed at compile time
 */
template <char... Chars>
constexpr auto operator"" _hsv_hue() {
  return operator"" _hsv_hue(details::parse_dec_template<Chars...>());
}

/**
 * @brief Template literal: creates HSL hue shortcut from string
 * @tparam Chars Character pack representing hue value
 * @return HSL color with maximum saturation and 50% lightness
 * @note Same format as numeric literal but parsed at compile time
 */
template <char... Chars>
constexpr auto operator"" _hsl_hue() {
  return operator"" _hsl_hue(details::parse_dec_template<Chars...>());
}

/** @} */

}  // namespace operators

}  // namespace color::literals
