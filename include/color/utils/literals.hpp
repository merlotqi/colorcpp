/**
 * @file literals.hpp
 * @brief Color literal operators
 *
 * Provides user-defined literal operators for creating color values
 * directly in source code. Supports RGB/HEX, HSV, and HSL color formats
 * with compile-time parsing and validation.
 *
 * @author Color Library Team
 * @date 2025
 * @version 1.0
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include "../core/hsl.hpp"
#include "../core/hsv.hpp"
#include "../core/rgb.hpp"

namespace color::literals {

/**
 * @brief Implementation details for literal parsing
 *
 * Contains helper functions for parsing hexadecimal and decimal values
 * from character sequences at compile time.
 */
namespace detail {
/**
 * @brief Convert a hexadecimal character to its integer value
 *
 * @param c Hexadecimal character ('0'-'9', 'a'-'f', 'A'-'F')
 * @return Integer value (0-15) of the hexadecimal character
 */
constexpr uint32_t hex_to_int(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return 0;
}

/**
 * @brief Parse decimal digits from character sequence
 *
 * @tparam Chars Character sequence representing a decimal number
 * @return Parsed decimal value as uint64_t
 */
template <char... Chars>
constexpr uint64_t parse_dec() {
  constexpr char s[] = {Chars...};
  uint64_t res = 0;
  for (char c : s) {
    if (c >= '0' && c <= '9') {
      res = res * 10 + (c - '0');
    } else if (c == '\'') {
      continue;  // Skip digit separators
    }
  }
  return res;
}

/**
 * @brief Parse hexadecimal digits from character sequence
 *
 * @tparam Chars Character sequence representing a hexadecimal number
 * @return Parsed hexadecimal value as uint64_t
 */
template <char... Chars>
constexpr uint64_t parse_hex() {
  constexpr char s[] = {Chars...};
  constexpr size_t n = sizeof(s);
  size_t start = (n > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) ? 2 : 0;

  uint64_t res = 0;
  for (size_t i = start; i < n; ++i) {
    if (s[i] == '\'') continue;  // Skip digit separators
    res = (res << 4) | hex_to_int(s[i]);
  }
  return res;
}
}  // namespace detail

/**
 * @brief RGB/HEX literal operators namespace
 *
 * Provides user-defined literal operators for creating RGB colors
 * from hexadecimal values at compile time.
 */
namespace literals {

/**
 * @brief RGB color literal operator
 *
 * Creates an RGB color from a hexadecimal literal.
 * Format: 0xRRGGBB_rgb or RRGGBB_rgb
 *
 * @tparam Chars Character sequence representing hexadecimal RGB value
 * @return RGB color with parsed red, green, and blue components
 */
template <char... Chars>
constexpr auto operator"" _rgb() {
  constexpr uint32_t val = static_cast<uint32_t>(detail::parse_hex<Chars...>());
  constexpr uint8_t r = (val >> 16) & 0xFF;
  constexpr uint8_t g = (val >> 8) & 0xFF;
  constexpr uint8_t b = val & 0xFF;
  return core::rgb8<r, g, b>{};
}

/**
 * @brief HEX color literal operator (alias for _rgb)
 *
 * Creates an RGB color from a hexadecimal literal.
 * Format: 0xRRGGBB_hex or RRGGBB_hex
 *
 * @tparam Chars Character sequence representing hexadecimal RGB value
 * @return RGB color with parsed red, green, and blue components
 */
template <char... Chars>
constexpr auto operator"" _hex() {
  return operator"" _rgb<Chars...>();
}

/**
 * @brief HSV color literal operator
 *
 * Creates an HSV color from a decimal literal.
 * Format: H*10000 + S*100 + V (e.g., 120100100_hsv)
 *
 * @tparam Chars Character sequence representing HSV values
 * @return HSV color with parsed hue, saturation, and value components
 */
template <char... Chars>
constexpr auto operator"" _hsv() {
  constexpr uint64_t val = detail::parse_dec<Chars...>();
  constexpr int h = static_cast<int>(val / 10000);
  constexpr int s = static_cast<int>((val / 100) % 100);
  constexpr int v = static_cast<int>(val % 100);
  return core::hsv_int<h, s, v>{};
}

/**
 * @brief HSL color literal operator
 *
 * Creates an HSL color from a decimal literal.
 * Format: H*10000 + S*100 + L (e.g., 24010050_hsl)
 *
 * @tparam Chars Character sequence representing HSL values
 * @return HSL color with parsed hue, saturation, and lightness components
 */
template <char... Chars>
constexpr auto operator"" _hsl() {
  constexpr uint64_t val = detail::parse_dec<Chars...>();
  constexpr int h = static_cast<int>(val / 10000);
  constexpr int s = static_cast<int>((val / 100) % 100);
  constexpr int l = static_cast<int>(val % 100);
  return core::hsl_int<h, s, l>{};
}

/**
 * @brief Simplified HSV hue literal operator
 *
 * Creates an HSV color with specified hue and default saturation/value.
 * Format: H_hsv_hue (e.g., 120_hsv_hue)
 *
 * @tparam Chars Character sequence representing hue value
 * @return HSV color with specified hue, saturation=100, value=100
 */
template <char... Chars>
constexpr auto operator"" _hsv_hue() {
  constexpr int h = static_cast<int>(detail::parse_dec<Chars...>());
  return core::hsv_int<h, 100, 100>{};
}

/**
 * @brief Simplified HSL hue literal operator
 *
 * Creates an HSL color with specified hue and default saturation/lightness.
 * Format: H_hsl_hue (e.g., 240_hsl_hue)
 *
 * @tparam Chars Character sequence representing hue value
 * @return HSL color with specified hue, saturation=100, lightness=50
 */
template <char... Chars>
constexpr auto operator"" _hsl_hue() {
  constexpr int h = static_cast<int>(detail::parse_dec<Chars...>());
  return core::hsl_int<h, 100, 50>{};
}

}  // namespace literals

}  // namespace color::literals
