/**
 * @file hsv.hpp
 * @brief HSV color space core implementation
 *
 * Provides basic HSV color template class and common color aliases.
 * Supports integer and floating-point HSV representations.
 *
 * @author Merlot.Qi
 *
 */

#pragma once

#include <cassert>
#include <cstdint>
#include <type_traits>

namespace color::core {

/**
 * @brief Basic HSV color template class
 *
 * Template class for representing HSV colors with compile-time validation.
 * Supports both integer (H: 0-359, S/V: 0-100) and floating-point (H: 0-360, S/V: 0.0-1.0) value ranges.
 *
 * @tparam T Value type (must be arithmetic)
 * @tparam H_raw Raw hue component value
 * @tparam S_raw Raw saturation component value
 * @tparam V_raw Raw value component value
 * @tparam Scale Scaling factor for value conversion
 */
template <typename T, intptr_t Scale = 1>
struct basic_hsv {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

  using value_type = T;

  T h, s, v;

  constexpr basic_hsv() : h(0), s(0), v(0) {}

  constexpr basic_hsv(T hue, T saturation, T value) : h(hue), s(saturation), v(value) {
    if (!is_valid_val(hue, saturation, value)) {
      assert(false && "HSV values out of range!");
    }
  }

  template <intptr_t H_raw, intptr_t S_raw, intptr_t V_raw>
  static constexpr basic_hsv make() {
    constexpr T static_h = static_cast<T>(H_raw) / static_cast<T>(Scale);
    constexpr T static_s = static_cast<T>(S_raw) / static_cast<T>(Scale);
    constexpr T static_v = static_cast<T>(V_raw) / static_cast<T>(Scale);
    static_assert(is_valid_val(static_h, static_s, static_v), "HSV value out of range!");
    return {static_h, static_s, static_v};
  }

  static constexpr bool is_valid_val(T hv, T sv, T vv) {
    if constexpr (std::is_floating_point_v<T>) {
      return hv >= 0.0 && hv < 360.0 && sv >= 0.0 && sv <= 1.0 && vv >= 0.0 && vv <= 1.0;
    } else {
      return hv >= 0 && hv < 360 && sv >= 0 && sv <= 100 && vv >= 0 && vv <= 100;
    }
  }

  constexpr bool is_valid() const { return is_valid_val(h, s, v); }
};

/**
 * @name HSV Color Type Aliases
 * @{
 */

/**
 * @brief Integer HSV color type
 *
 * Represents HSV colors using integer values.
 * Hue range: 0-359 degrees, Saturation/Value range: 0-100.
 *
 * @tparam H Hue component (0-359)
 * @tparam S Saturation component (0-100)
 * @tparam V Value component (0-100)
 */
using hsv_int_t = basic_hsv<int, 1>;
template <int H, int S, int V>
inline constexpr hsv_int_t hsv_int = hsv_int_t::make<H, S, V>();

/**
 * @brief Floating-point HSV color type
 *
 * Represents HSV colors using double precision floating-point values.
 * Hue range: 0-360 degrees, Saturation/Value range: 0.0-1.0.
 * Scale is set to 1000, mapping integer inputs to floating-point range.
 *
 * @tparam H Hue component (scaled from integer to 0-360)
 * @tparam S Saturation component (scaled from integer to 0.0-1.0)
 * @tparam V Value component (scaled from integer to 0.0-1.0)
 */
using hsv_float_t = basic_hsv<double, 1000>;
template <intptr_t H, intptr_t S, intptr_t V>
inline constexpr hsv_float_t hsv_float = hsv_float_t::make<H, S, V>();

/**
 * @brief Percentage-based HSV color type
 *
 * Represents HSV colors using integer percentage values (0-100 range).
 *
 * @tparam H Hue component percentage (0-100)
 * @tparam S Saturation component percentage (0-100)
 * @tparam V Value component percentage (0-100)
 */
template <int H, int S, int V>
using hsv_percent = basic_hsv<int>;

/** @} */

namespace colors {
// Basic hue colors (saturation 100%, value 100%)
inline constexpr hsv_int_t red_hsv = hsv_int<0, 100, 100>;
inline constexpr hsv_int_t orange_hsv = hsv_int<30, 100, 100>;
inline constexpr hsv_int_t yellow_hsv = hsv_int<60, 100, 100>;
inline constexpr hsv_int_t chartreuse_hsv = hsv_int<90, 100, 100>;
inline constexpr hsv_int_t green_hsv = hsv_int<120, 100, 100>;
inline constexpr hsv_int_t springgreen_hsv = hsv_int<150, 100, 100>;
inline constexpr hsv_int_t cyan_hsv = hsv_int<180, 100, 100>;
inline constexpr hsv_int_t azure_hsv = hsv_int<210, 100, 100>;
inline constexpr hsv_int_t blue_hsv = hsv_int<240, 100, 100>;
inline constexpr hsv_int_t violet_hsv = hsv_int<270, 100, 100>;
inline constexpr hsv_int_t magenta_hsv = hsv_int<300, 100, 100>;
inline constexpr hsv_int_t rose_hsv = hsv_int<330, 100, 100>;

// Grayscale series (saturation 0%)
inline constexpr hsv_int_t black_hsv = hsv_int<0, 0, 0>;    // Black
inline constexpr hsv_int_t gray1_hsv = hsv_int<0, 0, 25>;   // Dark gray
inline constexpr hsv_int_t gray2_hsv = hsv_int<0, 0, 50>;   // Medium gray
inline constexpr hsv_int_t gray3_hsv = hsv_int<0, 0, 75>;   // Light gray
inline constexpr hsv_int_t silver_hsv = hsv_int<0, 0, 75>;  // Silver (same as gray3_hsv)
inline constexpr hsv_int_t white_hsv = hsv_int<0, 0, 100>;  // White

// W3C standard colors in HSV
inline constexpr hsv_int_t maroon_hsv = hsv_int<0, 100, 50>;        // Maroon
inline constexpr hsv_int_t red_hsv_w3c = hsv_int<0, 100, 100>;      // Red
inline constexpr hsv_int_t olive_hsv = hsv_int<60, 100, 50>;        // Olive
inline constexpr hsv_int_t yellow_hsv_w3c = hsv_int<60, 100, 100>;  // Yellow
inline constexpr hsv_int_t green_hsv_w3c = hsv_int<120, 100, 50>;   // Green (W3C standard is dark green)
inline constexpr hsv_int_t lime_hsv = hsv_int<120, 100, 100>;       // Lime (bright green)
inline constexpr hsv_int_t teal_hsv = hsv_int<180, 100, 50>;        // Teal
inline constexpr hsv_int_t cyan_hsv_w3c = hsv_int<180, 100, 100>;   // Cyan
inline constexpr hsv_int_t navy_hsv = hsv_int<240, 100, 50>;        // Navy
inline constexpr hsv_int_t blue_hsv_w3c = hsv_int<240, 100, 100>;   // Blue
inline constexpr hsv_int_t purple_hsv = hsv_int<300, 100, 50>;      // Purple
inline constexpr hsv_int_t fuchsia_hsv = hsv_int<300, 100, 100>;    // Fuchsia
inline constexpr hsv_int_t aqua_hsv = hsv_int<180, 100, 100>;       // Aqua (same as cyan_hsv_w3c)

// Common variants (different saturation and value)
inline constexpr hsv_int_t darkred_hsv = hsv_int<0, 100, 30>;       // Dark red
inline constexpr hsv_int_t brown_hsv = hsv_int<30, 100, 40>;        // Brown
inline constexpr hsv_int_t darkorange_hsv = hsv_int<30, 100, 70>;   // Dark orange
inline constexpr hsv_int_t gold_hsv = hsv_int<45, 100, 85>;         // Gold
inline constexpr hsv_int_t darkgreen_hsv = hsv_int<120, 100, 30>;   // Dark green
inline constexpr hsv_int_t forestgreen_hsv = hsv_int<120, 80, 40>;  // Forest green
inline constexpr hsv_int_t seagreen_hsv = hsv_int<150, 80, 50>;     // Sea green
inline constexpr hsv_int_t darkcyan_hsv = hsv_int<180, 100, 40>;    // Dark cyan
inline constexpr hsv_int_t deepblue_hsv = hsv_int<240, 100, 40>;    // Deep blue
inline constexpr hsv_int_t indigo_hsv = hsv_int<260, 100, 45>;      // Indigo
inline constexpr hsv_int_t darkpurple_hsv = hsv_int<280, 100, 40>;  // Dark purple
inline constexpr hsv_int_t pink_hsv = hsv_int<340, 30, 100>;        // Pink
inline constexpr hsv_int_t hotpink_hsv = hsv_int<330, 70, 100>;     // Hot pink
inline constexpr hsv_int_t lightpink_hsv = hsv_int<350, 20, 100>;   // Light pink

// Low saturation colors (soft/muted colors)
inline constexpr hsv_int_t softred_hsv = hsv_int<0, 50, 90>;       // Soft red
inline constexpr hsv_int_t softorange_hsv = hsv_int<30, 50, 90>;   // Soft orange
inline constexpr hsv_int_t softyellow_hsv = hsv_int<60, 50, 90>;   // Soft yellow
inline constexpr hsv_int_t softgreen_hsv = hsv_int<120, 50, 90>;   // Soft green
inline constexpr hsv_int_t softblue_hsv = hsv_int<240, 50, 90>;    // Soft blue
inline constexpr hsv_int_t softpurple_hsv = hsv_int<280, 50, 90>;  // Soft purple

// High saturation medium value colors
inline constexpr hsv_int_t vibrantred_hsv = hsv_int<0, 100, 80>;       // Vibrant red
inline constexpr hsv_int_t vibrantorange_hsv = hsv_int<30, 100, 80>;   // Vibrant orange
inline constexpr hsv_int_t vibrantyellow_hsv = hsv_int<60, 100, 80>;   // Vibrant yellow
inline constexpr hsv_int_t vibrantgreen_hsv = hsv_int<120, 100, 80>;   // Vibrant green
inline constexpr hsv_int_t vibrantblue_hsv = hsv_int<240, 100, 80>;    // Vibrant blue
inline constexpr hsv_int_t vibrantpurple_hsv = hsv_int<280, 100, 80>;  // Vibrant purple

// Light colors (high value, low saturation)
inline constexpr hsv_int_t lightred_hsv = hsv_int<0, 30, 100>;        // Light red
inline constexpr hsv_int_t lightorange_hsv = hsv_int<30, 30, 100>;    // Light orange
inline constexpr hsv_int_t lightyellow_hsv = hsv_int<60, 30, 100>;    // Light yellow
inline constexpr hsv_int_t lightgreen_hsv = hsv_int<120, 30, 100>;    // Light green
inline constexpr hsv_int_t lightblue_hsv = hsv_int<210, 30, 100>;     // Light blue
inline constexpr hsv_int_t lightpurple_hsv = hsv_int<280, 30, 100>;   // Light purple
inline constexpr hsv_int_t lightcyan_hsv = hsv_int<180, 30, 100>;     // Light cyan
inline constexpr hsv_int_t lightmagenta_hsv = hsv_int<300, 30, 100>;  // Light magenta

// Dark colors (low value)
inline constexpr hsv_int_t deepred_hsv = hsv_int<0, 100, 25>;        // Deep red
inline constexpr hsv_int_t deeporange_hsv = hsv_int<30, 100, 25>;    // Deep orange
inline constexpr hsv_int_t deepyellow_hsv = hsv_int<60, 100, 25>;    // Deep yellow
inline constexpr hsv_int_t deepgreen_hsv = hsv_int<120, 100, 25>;    // Deep green
inline constexpr hsv_int_t deepblue_hsv2 = hsv_int<240, 100, 25>;    // Deep blue
inline constexpr hsv_int_t deeppurple_hsv = hsv_int<280, 100, 25>;   // Deep purple
inline constexpr hsv_int_t deepcyan_hsv = hsv_int<180, 100, 25>;     // Deep cyan
inline constexpr hsv_int_t deepmagenta_hsv = hsv_int<300, 100, 25>;  // Deep magenta
}  // namespace colors

}  // namespace color::core
