/**
 * @file hwb.hpp
 * @brief HWB/HWBA literal operators (decimal encoded).
 */

#pragma once

#include <colorcpp/core/hwb.hpp>
#include <colorcpp/io/literals/details.hpp>

namespace colorcpp::io::literals {

/**
 * @name HWB Literal Operators
 *
 * Encoding: val = H * 1'000'000 + W * 1'000 + B
 *   - H: 0–360 (degrees), W/B: 000–100 (three-digit percentage)
 * Example: 120'050'075_hwb → hwb_float_t{120, 0.50f, 0.75f}
 *
 * 4-channel encoding: val = H * 1'000'000'000 + W * 1'000'000 + B * 1'000 + A
 * Example: 120'050'075'100_hwba → hwba_float_t{120, 0.50f, 0.75f, 1.0f}
 * @{
 */

/** @brief HWB template literal; encoding documented in the "HWB Literal Operators" block above. */
template <char... Chars>
constexpr auto operator""_hwb() {
  constexpr auto parsed = details::parse_dec_template<Chars...>();
  static_assert(parsed.digits >= 7 && parsed.digits <= 9,
                "colorcpp: _hwb requires 7–9 digits (HHHWWWBBB format, "
                "leading zeros on the first field may be omitted). "
                "Got an unexpected number of digits — check your literal.");
  constexpr uint64_t h = parsed.value / 1000000ULL;
  constexpr uint64_t w = (parsed.value / 1000ULL) % 1000ULL;
  constexpr uint64_t b = parsed.value % 1000ULL;
  static_assert(h <= 360, "colorcpp: _hwb H out of range (0–360)");
  static_assert(w <= 100, "colorcpp: _hwb W out of range (000–100)");
  static_assert(b <= 100, "colorcpp: _hwb B out of range (000–100)");
  return core::hwb_float_t{static_cast<float>(h), static_cast<float>(w) / 100.0f, static_cast<float>(b) / 100.0f};
}

/** @brief HWBA template literal (four packed fields). */
template <char... Chars>
constexpr auto operator""_hwba() {
  constexpr auto parsed = details::parse_dec_template<Chars...>();
  static_assert(parsed.digits >= 10 && parsed.digits <= 12,
                "colorcpp: _hwba requires 10–12 digits (HHHWWWBBBAAA format, "
                "leading zeros on the first field may be omitted). "
                "Got an unexpected number of digits — check your literal.");
  constexpr uint64_t h = parsed.value / 1000000000ULL;
  constexpr uint64_t w = (parsed.value / 1000000ULL) % 1000ULL;
  constexpr uint64_t b = (parsed.value / 1000ULL) % 1000ULL;
  constexpr uint64_t a = parsed.value % 1000ULL;
  static_assert(h <= 360, "colorcpp: _hwba H out of range (0–360)");
  static_assert(w <= 100, "colorcpp: _hwba W out of range (000–100)");
  static_assert(b <= 100, "colorcpp: _hwba B out of range (000–100)");
  static_assert(a <= 100, "colorcpp: _hwba A out of range (000–100)");
  return core::hwba_float_t{static_cast<float>(h), static_cast<float>(w) / 100.0f, static_cast<float>(b) / 100.0f,
                            static_cast<float>(a) / 100.0f};
}

/** @} */

}  // namespace colorcpp::io::literals
