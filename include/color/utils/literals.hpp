/**
 * @file literals.hpp
 * @brief Modern Color literal operators
 *
 * Provides user-defined literal operators for creating color objects at compile time.
 * Supports both template-based (compile-time type) and value-based (constexpr instance) literals.
 *
 * @author Merlot.Qi
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include "../core/hsl.hpp"
#include "../core/hsv.hpp"
#include "../core/rgb.hpp"

namespace color::literals {

namespace details {
/**
 * @brief Convert a character to its hexadecimal value
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
 * @brief Parse template character sequence as hexadecimal uint32_t
 * @tparam Chars Character pack representing hex digits
 * @return Parsed hexadecimal value
 */
template <char... Chars>
constexpr uint32_t parse_hex_template() {
  constexpr char s[] = {Chars...};
  uint32_t res = 0;
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
 * @tparam Chars Character pack representing decimal digits
 * @return Parsed decimal value
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
}  // namespace details

inline namespace operators {

/**
 * @name RGB/HEX Literal Operators
 * @{
 */

/**
 * @brief Template-based hexadecimal literal operator
 * @tparam Chars Character pack representing hex digits
 * @return RGB color type with compile-time values
 * @note Supports formats like 0xFF0000_hex
 */
template <char... Chars>
constexpr auto operator"" _hex() {
  constexpr uint32_t val = details::parse_hex_template<Chars...>();
  return core::rgb8<(val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF>;
}

/**
 * @brief Template-based RGB literal operator (alias for _hex)
 * @tparam Chars Character pack representing hex digits
 * @return RGB color type with compile-time values
 */
template <char... Chars>
constexpr auto operator"" _rgb() {
  return operator"" _hex<Chars...>();
}

/**
 * @brief Value-based hexadecimal literal operator
 * @param val Hexadecimal value (e.g., 0xFF0000)
 * @return RGB color instance
 */
constexpr auto operator"" _hex(unsigned long long val) {
  return core::rgb8_t{static_cast<uint8_t>((val >> 16) & 0xFF), static_cast<uint8_t>((val >> 8) & 0xFF),
                      static_cast<uint8_t>(val & 0xFF)};
}

/**
 * @brief Value-based RGB literal operator (alias for _hex)
 * @param val Hexadecimal value (e.g., 0xFF0000)
 * @return RGB color instance
 */
constexpr auto operator"" _rgb(unsigned long long val) { return operator"" _hex(val); }

/** @} */

/**
 * @name HSV/HSL Literal Operators
 * @{
 */

/**
 * @brief HSV literal operator
 * @param val Combined HSV value in format HHHSSSVVV
 * @return HSV color instance with Scale=1000
 * @note Each component is 3 digits: hue(0-360), saturation(0-1000), value(0-1000)
 */
constexpr auto operator"" _hsv(unsigned long long val) {
  intptr_t h = static_cast<intptr_t>(val / 1000000);
  intptr_t s = static_cast<intptr_t>((val / 1000) % 1000);
  intptr_t v = static_cast<intptr_t>(val % 1000);
  return core::basic_hsv<intptr_t, 1000>{h, s, v};
}

/**
 * @brief HSL literal operator
 * @param val Combined HSL value in format HHHSSSVVV
 * @return HSL color instance with Scale=1000
 * @note Each component is 3 digits: hue(0-360), saturation(0-1000), lightness(0-1000)
 */
constexpr auto operator"" _hsl(unsigned long long val) {
  intptr_t h = static_cast<intptr_t>(val / 1000000);
  intptr_t s = static_cast<intptr_t>((val / 1000) % 1000);
  intptr_t l = static_cast<intptr_t>(val % 1000);
  return core::basic_hsl<intptr_t, 1000>{h, s, l};
}

/**
 * @brief Template-based HSV literal operator
 * @tparam Chars Character pack representing decimal digits
 * @return HSV color type with compile-time values
 */
template <char... Chars>
constexpr auto operator"" _hsv() {
  constexpr uint64_t val = details::parse_dec_template<Chars...>();
  return operator"" _hsv(val);
}

/**
 * @brief Template-based HSL literal operator
 * @tparam Chars Character pack representing decimal digits
 * @return HSL color type with compile-time values
 */
template <char... Chars>
constexpr auto operator"" _hsl() {
  constexpr uint64_t val = details::parse_dec_template<Chars...>();
  return operator"" _hsl(val);
}

/** @} */

/**
 * @name Hue Shortcut Literal Operators
 * @{
 */

/**
 * @brief HSV hue shortcut literal operator
 * @param h Hue value (0-360)
 * @return HSV color with full saturation and value
 */
constexpr auto operator"" _hsv_hue(unsigned long long h) {
  return core::basic_hsv<intptr_t, 1000>{static_cast<intptr_t>(h), 1000, 1000};
}

/**
 * @brief HSL hue shortcut literal operator
 * @param h Hue value (0-360)
 * @return HSL color with full saturation and 50% lightness
 */
constexpr auto operator"" _hsl_hue(unsigned long long h) {
  return core::basic_hsl<intptr_t, 1000>{static_cast<intptr_t>(h), 1000, 500};
}

/**
 * @brief Template-based HSV hue shortcut literal operator
 * @tparam Chars Character pack representing decimal digits
 * @return HSV color type with compile-time hue
 */
template <char... Chars>
constexpr auto operator"" _hsv_hue() {
  return operator"" _hsv_hue(details::parse_dec_template<Chars...>());
}

/**
 * @brief Template-based HSL hue shortcut literal operator
 * @tparam Chars Character pack representing decimal digits
 * @return HSL color type with compile-time hue
 */
template <char... Chars>
constexpr auto operator"" _hsl_hue() {
  return operator"" _hsl_hue(details::parse_dec_template<Chars...>());
}

/** @} */

}  // namespace operators

}  // namespace color::literals
