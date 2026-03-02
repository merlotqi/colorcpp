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
template <typename T, intptr_t H_raw, intptr_t S_raw, intptr_t V_raw, intptr_t Scale = 1>
struct basic_hsv {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

  using value_type = T;

  /// @brief Hue component value (converted from raw value using scale)
  static constexpr T h = static_cast<T>(H_raw) / static_cast<T>(Scale);
  /// @brief Saturation component value (converted from raw value using scale)
  static constexpr T s = static_cast<T>(S_raw) / static_cast<T>(Scale);
  /// @brief Value component value (converted from raw value using scale)
  static constexpr T v = static_cast<T>(V_raw) / static_cast<T>(Scale);

  /**
   * @brief Validate that HSV values are within acceptable ranges
   * @return true if values are valid, false otherwise
   */
  static constexpr bool is_valid() {
    if constexpr (std::is_floating_point_v<T>) {
      return h >= 0.0 && h < 360.0 && s >= 0.0 && s <= 1.0 && v >= 0.0 && v <= 1.0;
    } else {
      return h >= 0 && h < 360 && s >= 0 && s <= 100 && v >= 0 && v <= 100;
    }
  }

  static_assert(is_valid(),
                "HSV values must be in valid range (0-360 for H, 0-100 for S/V for int, 0.0-1.0 for float)");
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
template <int H, int S, int V>
using hsv_int = basic_hsv<int, H, S, V, 1>;

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
template <int H, int S, int V>
using hsv_float = basic_hsv<double, H, S, V, 1000>;

/** @} */

namespace colors {
// Basic hue colors (saturation 100%, value 100%)
using red_hsv = hsv_int<0, 100, 100>;
using orange_hsv = hsv_int<30, 100, 100>;
using yellow_hsv = hsv_int<60, 100, 100>;
using chartreuse_hsv = hsv_int<90, 100, 100>;
using green_hsv = hsv_int<120, 100, 100>;
using springgreen_hsv = hsv_int<150, 100, 100>;
using cyan_hsv = hsv_int<180, 100, 100>;
using azure_hsv = hsv_int<210, 100, 100>;
using blue_hsv = hsv_int<240, 100, 100>;
using violet_hsv = hsv_int<270, 100, 100>;
using magenta_hsv = hsv_int<300, 100, 100>;
using rose_hsv = hsv_int<330, 100, 100>;

// Grayscale series (saturation 0%)
using black_hsv = hsv_int<0, 0, 0>;    // Black
using gray1_hsv = hsv_int<0, 0, 25>;   // Dark gray
using gray2_hsv = hsv_int<0, 0, 50>;   // Medium gray
using gray3_hsv = hsv_int<0, 0, 75>;   // Light gray
using silver_hsv = hsv_int<0, 0, 75>;  // Silver (same as gray3_hsv)
using white_hsv = hsv_int<0, 0, 100>;  // White

// W3C standard colors in HSV
using maroon_hsv = hsv_int<0, 100, 50>;        // Maroon
using red_hsv_w3c = hsv_int<0, 100, 100>;      // Red
using olive_hsv = hsv_int<60, 100, 50>;        // Olive
using yellow_hsv_w3c = hsv_int<60, 100, 100>;  // Yellow
using green_hsv_w3c = hsv_int<120, 100, 50>;   // Green (W3C standard is dark green)
using lime_hsv = hsv_int<120, 100, 100>;       // Lime (bright green)
using teal_hsv = hsv_int<180, 100, 50>;        // Teal
using cyan_hsv_w3c = hsv_int<180, 100, 100>;   // Cyan
using navy_hsv = hsv_int<240, 100, 50>;        // Navy
using blue_hsv_w3c = hsv_int<240, 100, 100>;   // Blue
using purple_hsv = hsv_int<300, 100, 50>;      // Purple
using fuchsia_hsv = hsv_int<300, 100, 100>;    // Fuchsia
using aqua_hsv = hsv_int<180, 100, 100>;       // Aqua (same as cyan_hsv_w3c)

// Common variants (different saturation and value)
using darkred_hsv = hsv_int<0, 100, 30>;       // Dark red
using brown_hsv = hsv_int<30, 100, 40>;        // Brown
using darkorange_hsv = hsv_int<30, 100, 70>;   // Dark orange
using gold_hsv = hsv_int<45, 100, 85>;         // Gold
using darkgreen_hsv = hsv_int<120, 100, 30>;   // Dark green
using forestgreen_hsv = hsv_int<120, 80, 40>;  // Forest green
using seagreen_hsv = hsv_int<150, 80, 50>;     // Sea green
using darkcyan_hsv = hsv_int<180, 100, 40>;    // Dark cyan
using deepblue_hsv = hsv_int<240, 100, 40>;    // Deep blue
using indigo_hsv = hsv_int<260, 100, 45>;      // Indigo
using darkpurple_hsv = hsv_int<280, 100, 40>;  // Dark purple
using pink_hsv = hsv_int<340, 30, 100>;        // Pink
using hotpink_hsv = hsv_int<330, 70, 100>;     // Hot pink
using lightpink_hsv = hsv_int<350, 20, 100>;   // Light pink

// Low saturation colors (soft/muted colors)
using softred_hsv = hsv_int<0, 50, 90>;       // Soft red
using softorange_hsv = hsv_int<30, 50, 90>;   // Soft orange
using softyellow_hsv = hsv_int<60, 50, 90>;   // Soft yellow
using softgreen_hsv = hsv_int<120, 50, 90>;   // Soft green
using softblue_hsv = hsv_int<240, 50, 90>;    // Soft blue
using softpurple_hsv = hsv_int<280, 50, 90>;  // Soft purple

// High saturation medium value colors
using vibrantred_hsv = hsv_int<0, 100, 80>;       // Vibrant red
using vibrantorange_hsv = hsv_int<30, 100, 80>;   // Vibrant orange
using vibrantyellow_hsv = hsv_int<60, 100, 80>;   // Vibrant yellow
using vibrantgreen_hsv = hsv_int<120, 100, 80>;   // Vibrant green
using vibrantblue_hsv = hsv_int<240, 100, 80>;    // Vibrant blue
using vibrantpurple_hsv = hsv_int<280, 100, 80>;  // Vibrant purple

// Light colors (high value, low saturation)
using lightred_hsv = hsv_int<0, 30, 100>;        // Light red
using lightorange_hsv = hsv_int<30, 30, 100>;    // Light orange
using lightyellow_hsv = hsv_int<60, 30, 100>;    // Light yellow
using lightgreen_hsv = hsv_int<120, 30, 100>;    // Light green
using lightblue_hsv = hsv_int<210, 30, 100>;     // Light blue
using lightpurple_hsv = hsv_int<280, 30, 100>;   // Light purple
using lightcyan_hsv = hsv_int<180, 30, 100>;     // Light cyan
using lightmagenta_hsv = hsv_int<300, 30, 100>;  // Light magenta

// Dark colors (low value)
using deepred_hsv = hsv_int<0, 100, 25>;        // Deep red
using deeporange_hsv = hsv_int<30, 100, 25>;    // Deep orange
using deepyellow_hsv = hsv_int<60, 100, 25>;    // Deep yellow
using deepgreen_hsv = hsv_int<120, 100, 25>;    // Deep green
using deepblue_hsv2 = hsv_int<240, 100, 25>;    // Deep blue
using deeppurple_hsv = hsv_int<280, 100, 25>;   // Deep purple
using deepcyan_hsv = hsv_int<180, 100, 25>;     // Deep cyan
using deepmagenta_hsv = hsv_int<300, 100, 25>;  // Deep magenta
}  // namespace colors

}  // namespace color::core
