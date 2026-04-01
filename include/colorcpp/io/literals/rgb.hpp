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

/** @brief Runtime numeric literal: RRGGBB hex → rgba8_t (alpha=255). See template @c operator"" _rgb for examples. */
constexpr auto operator"" _rgb(unsigned long long val) {
  if (val > 0xFFFFFF) {
    throw std::out_of_range("colorcpp: _rgb value exceeds 0xFFFFFF (24-bit limit)");
  }
  return core::rgba8_t{static_cast<uint8_t>((val >> 16) & 0xFF), static_cast<uint8_t>((val >> 8) & 0xFF),
                       static_cast<uint8_t>(val & 0xFF), 255};
}

/** @brief Runtime numeric literal: RRGGBBAA hex → rgba8_t. */
constexpr auto operator"" _rgba(unsigned long long val) {
  if (val > 0xFFFFFFFF) {
    throw std::out_of_range("colorcpp: _rgba value exceeds 0xFFFFFFFF (32-bit limit)");
  }
  return core::rgba8_t{static_cast<uint8_t>((val >> 24) & 0xFF), static_cast<uint8_t>((val >> 16) & 0xFF),
                       static_cast<uint8_t>((val >> 8) & 0xFF), static_cast<uint8_t>(val & 0xFF)};
}

/** @brief Runtime numeric literal: AARRGGBB hex → rgba8_t. */
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

}  // namespace colorcpp::io::literals
