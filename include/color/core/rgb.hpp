/**
 * @file rgb.hpp
 * @brief RGB color space core implementation
 *
 * Provides basic RGB color template class and common color aliases.
 * Supports 8-bit integer RGB and floating-point RGB representations.
 *
 * @author Color Library Team
 * @date 2025
 * @version 1.0
 */

#pragma once

#include <cstdint>
#include <type_traits>

namespace color::core {

/**
 * @brief Basic RGB color template class
 *
 * Template class for representing RGB colors with compile-time validation.
 * Supports both integer (0-255) and floating-point (0.0-1.0) value ranges.
 *
 * @tparam T Value type (must be arithmetic)
 * @tparam R_raw Raw red component value
 * @tparam G_raw Raw green component value
 * @tparam B_raw Raw blue component value
 * @tparam Scale Scaling factor for value conversion
 */
template <typename T, intptr_t R_raw, intptr_t G_raw, intptr_t B_raw, intptr_t Scale = 1>
struct basic_rgb {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

  using value_type = T;

  /// @brief Red component value (converted from raw value using scale)
  static constexpr T r = static_cast<T>(R_raw) / static_cast<T>(Scale);
  /// @brief Green component value (converted from raw value using scale)
  static constexpr T g = static_cast<T>(G_raw) / static_cast<T>(Scale);
  /// @brief Blue component value (converted from raw value using scale)
  static constexpr T b = static_cast<T>(B_raw) / static_cast<T>(Scale);

  /**
   * @brief Validate that RGB values are within acceptable ranges
   * @return true if values are valid, false otherwise
   */
  static constexpr bool is_valid() {
    if constexpr (std::is_floating_point_v<T>) {
      return r >= 0.0 && r <= 1.0 && g >= 0.0 && g <= 1.0 && b >= 0.0 && b <= 1.0;
    } else {
      return r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255;
    }
  }

  static_assert(is_valid(), "RGB values must be in valid range (0-255 for int, 0.0-1.0 for float)");
};

/**
 * @name RGB Color Type Aliases
 * @{
 */

/**
 * @brief 8-bit integer RGB color type
 *
 * Represents RGB colors using 8-bit integer values (0-255 range).
 * Scale is set to 1, so values are stored directly.
 *
 * @tparam R Red component (0-255)
 * @tparam G Green component (0-255)
 * @tparam B Blue component (0-255)
 */
template <uint8_t R, uint8_t G, uint8_t B>
using rgb8 = basic_rgb<uint8_t, R, G, B, 1>;

/**
 * @brief Floating-point RGB color type
 *
 * Represents RGB colors using double precision floating-point values (0.0-1.0 range).
 * Scale is set to 1000, mapping integer inputs to floating-point range.
 *
 * @tparam R Red component (scaled from integer to 0.0-1.0)
 * @tparam G Green component (scaled from integer to 0.0-1.0)
 * @tparam B Blue component (scaled from integer to 0.0-1.0)
 */
template <int R, int G, int B>
using rgbf = basic_rgb<double, R, G, B, 1000>;

/**
 * @brief Percentage-based RGB color type
 *
 * Represents RGB colors using integer percentage values (0-100 range).
 *
 * @tparam R Red component percentage (0-100)
 * @tparam G Green component percentage (0-100)
 * @tparam B Blue component percentage (0-100)
 */
template <int R, int G, int B>
using rgb_percent = basic_rgb<int, R, G, B>;

/** @} */

namespace colors {
using black = rgb8<0, 0, 0>;
using white = rgb8<255, 255, 255>;
using red = rgb8<255, 0, 0>;
using green = rgb8<0, 255, 0>;
using blue = rgb8<0, 0, 255>;
using yellow = rgb8<255, 255, 0>;
using cyan = rgb8<0, 255, 255>;
using magenta = rgb8<255, 0, 255>;
using gray = rgb8<128, 128, 128>;
using silver = rgb8<192, 192, 192>;
using maroon = rgb8<128, 0, 0>;
using olive = rgb8<128, 128, 0>;
using navy = rgb8<0, 0, 128>;
using purple = rgb8<128, 0, 128>;
using teal = rgb8<0, 128, 128>;
using lime = rgb8<0, 255, 0>;
using fuchsia = rgb8<255, 0, 255>;
using aqua = rgb8<0, 255, 255>;
}  // namespace colors

}  // namespace color::core
