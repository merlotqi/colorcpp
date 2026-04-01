/**
 * @file oklab.hpp
 * @brief Oklab/OkLCH literal operators (decimal encoded).
 */

#pragma once

#include <colorcpp/core/oklab.hpp>
#include <colorcpp/io/literals/details.hpp>

namespace colorcpp::io::literals {

/**
 * @name Oklab Literal Operators
 *
 * Encoding: val = L * 1'000'000 + A * 1'000 + B
 *   - L: 0–100 (percentage, 0–1 in storage), A/B: 000–100 (mapped to -0.5–0.5)
 * Example: 050'050'050_oklab → oklab_t{0.50f, 0.0f, 0.0f}  (neutral gray)
 *
 * 4-channel encoding: val = L * 1'000'000'000 + A * 1'000'000 + B * 1'000 + Alpha
 * Example: 050'050'050'100_oklaba → oklaba_t{0.50f, 0.0f, 0.0f, 1.0f}
 * @{
 */

/** @brief Oklab template literal; encoding documented in the "Oklab Literal Operators" block above. */
template <char... Chars>
constexpr auto operator"" _oklab() {
  constexpr uint64_t val = details::parse_dec_template<Chars...>();
  constexpr uint64_t l = val / 1000000ULL;
  constexpr uint64_t a = (val / 1000ULL) % 1000ULL;
  constexpr uint64_t b = val % 1000ULL;
  static_assert(l <= 100, "colorcpp: _oklab L out of range (000–100)");
  static_assert(a <= 100, "colorcpp: _oklab A out of range (000–100)");
  static_assert(b <= 100, "colorcpp: _oklab B out of range (000–100)");
  return core::oklab_t{static_cast<float>(l) / 100.0f, static_cast<float>(a) / 100.0f - 0.5f,
                       static_cast<float>(b) / 100.0f - 0.5f};
}

/**
 * @name OkLCH Literal Operators
 *
 * Encoding: val = L * 1'000'000 + C * 1'000 + H
 *   - L: 0–100 (percentage, 0–1 in storage), C: 000–100 (mapped to 0–0.4), H: 0–360 (degrees)
 * Example: 050'040'120_oklch → oklch_t{0.50f, 0.40f, 120.0f}
 *
 * 4-channel encoding: val = L * 1'000'000'000 + C * 1'000'000 + H * 1'000 + Alpha
 * Example: 050'040'120'100_oklcha → oklcha_t{0.50f, 0.40f, 120.0f, 1.0f}
 * @{
 */

/** @brief OkLCH template literal; encoding documented in the "OkLCH Literal Operators" block above. */
template <char... Chars>
constexpr auto operator"" _oklch() {
  constexpr uint64_t val = details::parse_dec_template<Chars...>();
  constexpr uint64_t l = val / 1000000ULL;
  constexpr uint64_t c = (val / 1000ULL) % 1000ULL;
  constexpr uint64_t h = val % 1000ULL;
  static_assert(l <= 100, "colorcpp: _oklch L out of range (000–100)");
  static_assert(c <= 100, "colorcpp: _oklch C out of range (000–100)");
  static_assert(h <= 360, "colorcpp: _oklch H out of range (0–360)");
  return core::oklch_t{static_cast<float>(l) / 100.0f, static_cast<float>(c) / 100.0f, static_cast<float>(h)};
}

/** @} */

}  // namespace colorcpp::io::literals
