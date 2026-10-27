/**
 * @file rgb.hpp
 * @brief RGB/RGBA literal operators (hex, numeric).
 */

#pragma once

#include <colorcpp/core/rgb.hpp>
#include <colorcpp/io/literals/details.hpp>

namespace colorcpp::io::literals {

/**
 * @name RGB/RGBA Literal Operators
 * @{
 */

/**
 * @brief Template literal: compile-time RRGGBB hex → rgba8_t (alpha=255)
 * @example 0xFF0000_rgb → rgba8_t{255, 0, 0, 255}
 */
template <char... Chars>
constexpr auto operator""_rgb() {
  constexpr auto parsed = details::parse_hex_template<Chars...>();
  static_assert(parsed.digits == 6,
                "colorcpp: _rgb requires exactly 6 hex digits (RRGGBB format). "
                "Got a different number of digits — check your hex literal.");
  static_assert(parsed.value <= 0xFFFFFF, "colorcpp: _rgb template value exceeds 0xFFFFFF (24-bit limit)");
  return core::rgba8_t{(parsed.value >> 16) & 0xFF, (parsed.value >> 8) & 0xFF, parsed.value & 0xFF, 255};
}

/**
 * @brief Template literal: compile-time RRGGBBAA hex → rgba8_t
 * @example 0xFF000080_rgba → rgba8_t{255, 0, 0, 128}
 */
template <char... Chars>
constexpr auto operator""_rgba() {
  constexpr auto parsed = details::parse_hex_template<Chars...>();
  static_assert(parsed.digits == 8,
                "colorcpp: _rgba requires exactly 8 hex digits (RRGGBBAA format). "
                "Got a different number of digits — check your hex literal.");
  static_assert(parsed.value <= 0xFFFFFFFF, "colorcpp: _rgba template value exceeds 0xFFFFFFFF (32-bit limit)");
  return core::rgba8_t{(parsed.value >> 24) & 0xFF, (parsed.value >> 16) & 0xFF, (parsed.value >> 8) & 0xFF,
                       parsed.value & 0xFF};
}

/**
 * @brief Template literal: compile-time AARRGGBB hex → rgba8_t
 * @example 0x80FF0000_argb → rgba8_t{255, 0, 0, 128}
 */
template <char... Chars>
constexpr auto operator""_argb() {
  constexpr auto parsed = details::parse_hex_template<Chars...>();
  static_assert(parsed.digits == 8,
                "colorcpp: _argb requires exactly 8 hex digits (AARRGGBB format). "
                "Got a different number of digits — check your hex literal.");
  static_assert(parsed.value <= 0xFFFFFFFF, "colorcpp: _argb template value exceeds 0xFFFFFFFF (32-bit limit)");
  return core::rgba8_t{(parsed.value >> 16) & 0xFF, (parsed.value >> 8) & 0xFF, parsed.value & 0xFF,
                       (parsed.value >> 24) & 0xFF};
}

/* Numeric UDLs are template-only on purpose: the removed
 * operator""_rgb/rgba/argb(unsigned long long) overloads silently accepted
 * wrong digit counts (0xFF0000_rgba decoded as transparent green) and put a
 * runtime throw into a compile-time feature. Digit checking lives in the
 * static_asserts above; tests/compile_fail/ pins it. */

/**
 * @brief String literal: "#RGB", "#RGBA", "#RRGGBB", "#RRGGBBAA" → rgba8_t
 * @example "#FF6347"_hex → rgba8_t{255, 99, 71, 255}
 */
constexpr auto operator""_hex(const char* str, size_t len) { return details::parse_hex_string(str, len); }

/** @} */

}  // namespace colorcpp::io::literals
