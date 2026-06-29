/**
 * @file cmyk.hpp
 * @brief CMYK literal operators (decimal encoded).
 */

#pragma once

#include <colorcpp/core/cmyk.hpp>
#include <colorcpp/io/literals/details.hpp>

namespace colorcpp::io::literals {

/**
 * @name CMYK Literal Operators
 *
 * Encoding: val = C * 1'000'000'000 + M * 1'000'000 + Y * 1'000 + K
 *   - Each component: 000–100 (three-digit percentage), maps to cmyk8_t [0,100]
 * Example: 050'030'000'020_cmyk → cmyk8_t{50, 30, 0, 20}
 * @{
 */

/** @brief CMYK template literal; four 000–100 fields → @ref colorcpp::core::cmyk8_t. */
template <char... Chars>
constexpr auto operator""_cmyk() {
  constexpr auto parsed = details::parse_dec_template<Chars...>();
  static_assert(parsed.digits >= 10 && parsed.digits <= 12,
                "colorcpp: _cmyk requires 10–12 digits (CCCMMMYYYKKK format, "
                "leading zeros on the first field may be omitted). "
                "Got an unexpected number of digits — check your literal.");
  constexpr uint64_t c = parsed.value / 1000000000ULL;
  constexpr uint64_t m = (parsed.value / 1000000ULL) % 1000ULL;
  constexpr uint64_t y = (parsed.value / 1000ULL) % 1000ULL;
  constexpr uint64_t k = parsed.value % 1000ULL;
  static_assert(c <= 100, "colorcpp: _cmyk C out of range (000–100)");
  static_assert(m <= 100, "colorcpp: _cmyk M out of range (000–100)");
  static_assert(y <= 100, "colorcpp: _cmyk Y out of range (000–100)");
  static_assert(k <= 100, "colorcpp: _cmyk K out of range (000–100)");
  return core::cmyk8_t{static_cast<uint8_t>(c), static_cast<uint8_t>(m), static_cast<uint8_t>(y),
                       static_cast<uint8_t>(k)};
}

/** @} */

}  // namespace colorcpp::io::literals
