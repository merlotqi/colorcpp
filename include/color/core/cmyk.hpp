/**
 * @file cmyk.hpp
 * @brief CMYK color space core implementation
 *
 * Provides basic CMYK color template class and common color aliases.
 * Supports integer and floating-point CMYK representations.
 *
 * @author Merlot.Qi
 * 
 */

#pragma once

#include <cstdint>
#include <type_traits>

namespace color::core {

/**
 * @brief Basic CMYK color template class
 *
 * Template class for representing CMYK colors with compile-time validation.
 * Supports both integer (0-100) and floating-point (0.0-1.0) value ranges.
 *
 * @tparam T Value type (must be arithmetic)
 * @tparam C_raw Raw cyan component value
 * @tparam M_raw Raw magenta component value
 * @tparam Y_raw Raw yellow component value
 * @tparam K_raw Raw key (black) component value
 * @tparam Scale Scaling factor for value conversion
 */
template <typename T, intptr_t C_raw, intptr_t M_raw, intptr_t Y_raw, intptr_t K_raw, intptr_t Scale = 1>
struct basic_cmyk {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

  using value_type = T;

  /// @brief Cyan component value (converted from raw value using scale)
  static constexpr T c = static_cast<T>(C_raw) / static_cast<T>(Scale);
  /// @brief Magenta component value (converted from raw value using scale)
  static constexpr T m = static_cast<T>(M_raw) / static_cast<T>(Scale);
  /// @brief Yellow component value (converted from raw value using scale)
  static constexpr T y = static_cast<T>(Y_raw) / static_cast<T>(Scale);
  /// @brief Key (black) component value (converted from raw value using scale)
  static constexpr T k = static_cast<T>(K_raw) / static_cast<T>(Scale);

  /**
   * @brief Validate that CMYK values are within acceptable ranges
   * @return true if values are valid, false otherwise
   */
  static constexpr bool is_valid() {
    if constexpr (std::is_floating_point_v<T>) {
      return c >= 0.0 && c <= 1.0 && m >= 0.0 && m <= 1.0 && y >= 0.0 && y <= 1.0 && k >= 0.0 && k <= 1.0;
    } else {
      return c >= 0 && c <= 100 && m >= 0 && m <= 100 && y >= 0 && y <= 100 && k >= 0 && k <= 100;
    }
  }

  static_assert(is_valid(), "CMYK values must be in valid range (0-100 for int, 0.0-1.0 for float)");
};

/**
 * @name CMYK Color Type Aliases
 * @{
 */

/**
 * @brief Integer CMYK color type
 *
 * Represents CMYK colors using integer values (0-100 range).
 *
 * @tparam C Cyan component (0-100)
 * @tparam M Magenta component (0-100)
 * @tparam Y Yellow component (0-100)
 * @tparam K Key (black) component (0-100)
 */
template <int C, int M, int Y, int K>
using cmyk_int = basic_cmyk<int, C, M, Y, K, 1>;

/**
 * @brief Floating-point CMYK color type
 *
 * Represents CMYK colors using double precision floating-point values (0.0-1.0 range).
 * Scale is set to 1000, mapping integer inputs to floating-point range.
 *
 * @tparam C Cyan component (scaled from integer to 0.0-1.0)
 * @tparam M Magenta component (scaled from integer to 0.0-1.0)
 * @tparam Y Yellow component (scaled from integer to 0.0-1.0)
 * @tparam K Key (black) component (scaled from integer to 0.0-1.0)
 */
template <int C, int M, int Y, int K>
using cmyk_float = basic_cmyk<double, C, M, Y, K, 1000>;

/** @} */

namespace colors {
// Basic CMYK colors
using black_cmyk = cmyk_int<0, 0, 0, 100>;
using white_cmyk = cmyk_int<0, 0, 0, 0>;
using red_cmyk = cmyk_int<0, 100, 100, 0>;
using green_cmyk = cmyk_int<100, 0, 100, 0>;
using blue_cmyk = cmyk_int<100, 100, 0, 0>;
using yellow_cmyk = cmyk_int<0, 0, 100, 0>;
using cyan_cmyk = cmyk_int<100, 0, 0, 0>;
using magenta_cmyk = cmyk_int<0, 100, 0, 0>;

// W3C standard colors in CMYK
using maroon_cmyk = cmyk_int<0, 100, 100, 50>;
using olive_cmyk = cmyk_int<0, 0, 100, 50>;
using navy_cmyk = cmyk_int<100, 100, 0, 50>;
using purple_cmyk = cmyk_int<0, 100, 0, 50>;
using teal_cmyk = cmyk_int<100, 0, 0, 50>;
using silver_cmyk = cmyk_int<0, 0, 0, 25>;
using gray_cmyk = cmyk_int<0, 0, 0, 50>;

// Secondary colors
using orange_cmyk = cmyk_int<0, 50, 100, 0>;
using lime_cmyk = cmyk_int<50, 0, 100, 0>;
using aqua_cmyk = cmyk_int<100, 0, 0, 0>;     // Same as cyan
using fuchsia_cmyk = cmyk_int<0, 100, 0, 0>;  // Same as magenta
}  // namespace colors

}  // namespace color::core
