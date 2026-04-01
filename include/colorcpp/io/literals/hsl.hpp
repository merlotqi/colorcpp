/**
 * @file hsl.hpp
 * @brief HSL/HSLA literal operators (decimal encoded).
 */

#pragma once

#include <colorcpp/core/hsl.hpp>
#include <colorcpp/io/literals/details.hpp>

namespace colorcpp::io::literals {

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

/** @brief HSL template literal; encoding documented in the "HSL Literal Operators" block above. */
template <char... Chars>
constexpr auto operator"" _hsl() {
  constexpr uint64_t val = details::parse_dec_template<Chars...>();
  constexpr uint64_t h = val / 1000000ULL;
  constexpr uint64_t s = (val / 1000ULL) % 1000ULL;
  constexpr uint64_t l = val % 1000ULL;
  static_assert(h <= 360, "colorcpp: _hsl H out of range (0–360)");
  static_assert(s <= 100, "colorcpp: _hsl S out of range (000–100)");
  static_assert(l <= 100, "colorcpp: _hsl L out of range (000–100)");
  // S and L are written as percentages (000–100) but stored as [0,1]
  return core::hsl_float_t{static_cast<float>(h), static_cast<float>(s) / 100.0f, static_cast<float>(l) / 100.0f};
}

/** @brief HSLA template literal (four packed decimal fields; A as 000–100 → stored [0,1]). */
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
  return core::hsla_float_t{static_cast<float>(h), static_cast<float>(s) / 100.0f, static_cast<float>(l) / 100.0f,
                            static_cast<float>(a) / 100.0f};
}

/** @} */

}  // namespace colorcpp::io::literals
