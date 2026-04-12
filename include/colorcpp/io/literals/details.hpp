/**
 * @file details.hpp
 * @brief Internal helpers for color literal parsing.
 */

#pragma once

#include <colorcpp/core/rgb.hpp>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace colorcpp::io::literals::details {

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
  const uint8_t v = static_cast<uint8_t>(char_to_hex(c) & 0xFFu);
  return static_cast<uint8_t>((static_cast<unsigned>(v) << 4u) | static_cast<unsigned>(v));
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

}  // namespace colorcpp::io::literals::details
