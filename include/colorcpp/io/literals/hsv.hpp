/**
 * @file hsv.hpp
 * @brief HSV/HSVA literal operators (decimal encoded).
 */

#pragma once

#include <colorcpp/core/hsv.hpp>
#include <colorcpp/io/literals/details.hpp>

namespace colorcpp::io::literals {

/**
 * @name HSV Literal Operators
 *
 * Encoding: val = H * 1'000'000 + S * 1'000 + V
 *   - H: 0–360 (degrees), S/V: 000–100 (three-digit percentage)
 * Example: 210'080'090_hsv → hsv_float_t{210, 0.80f, 0.90f}  (S/V stored as [0,1])
 *
 * 4-channel encoding: val = H * 1'000'000'000 + S * 1'000'000 + V * 1'000 + A
 * Example: 210'080'090'100_hsva → hsva_float_t{210, 0.80f, 0.90f, 1.0f}  (S/V/A stored as [0,1])
 * @{
 */

/** @brief HSV template literal; encoding documented in the "HSV Literal Operators" block above. */
template <char... Chars>
constexpr auto operator"" _hsv() {
  constexpr uint64_t val = details::parse_dec_template<Chars...>();
  constexpr uint64_t h = val / 1000000ULL;
  constexpr uint64_t s = (val / 1000ULL) % 1000ULL;
  constexpr uint64_t v = val % 1000ULL;
  static_assert(h <= 360, "colorcpp: _hsv H out of range (0–360)");
  static_assert(s <= 100, "colorcpp: _hsv S out of range (000–100)");
  static_assert(v <= 100, "colorcpp: _hsv V out of range (000–100)");
  return core::hsv_float_t{static_cast<float>(h), static_cast<float>(s) / 100.0f, static_cast<float>(v) / 100.0f};
}

/** @brief HSVA template literal (four packed fields). */
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
  return core::hsva_float_t{static_cast<float>(h), static_cast<float>(s) / 100.0f, static_cast<float>(v) / 100.0f,
                            static_cast<float>(a) / 100.0f};
}

/** @} */

}  // namespace colorcpp::io::literals
