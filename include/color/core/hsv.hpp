/**
 * @file hsv.hpp
 * @brief HSVA color space core implementation
 *
 * Provides basic HSVA color template class and common color aliases.
 * Supports integer and floating-point HSVA representations.
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
 * @brief Basic HSVA color template class
 *
 * Template class for representing HSVA colors with compile-time validation.
 * Supports both integer (H: 0-359, S/V: 0-100) and floating-point (H: 0-360, S/V: 0.0-1.0) value ranges.
 *
 * @tparam T Value type (must be arithmetic)
 * @tparam Scale Scaling factor for value conversion
 */
template <typename T, intptr_t Scale = 1>
struct basic_hsva {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

  using value_type = T;

  T h, s, v, a;

  constexpr basic_hsva() : h(0), s(0), v(0), a(1) {}

  constexpr basic_hsva(T hue, T saturation, T value, T alpha) : h(hue), s(saturation), v(value), a(alpha) {
    if (!is_valid_val(hue, saturation, value, alpha)) {
      assert(false && "HSVA values out of range!");
    }
  }

  template <intptr_t H_raw, intptr_t S_raw, intptr_t V_raw, intptr_t A_raw = Scale>
  static constexpr basic_hsva make() {
    constexpr T static_h = static_cast<T>(H_raw) / static_cast<T>(Scale);
    constexpr T static_s = static_cast<T>(S_raw) / static_cast<T>(Scale);
    constexpr T static_v = static_cast<T>(V_raw) / static_cast<T>(Scale);
    constexpr T static_a = static_cast<T>(A_raw) / static_cast<T>(Scale);
    static_assert(is_valid_val(static_h, static_s, static_v, static_a), "HSVA value out of range!");
    return {static_h, static_s, static_v, static_a};
  }

  static constexpr bool is_valid_val(T hv, T sv, T vv, T av) {
    if constexpr (std::is_floating_point_v<T>) {
      return hv >= 0.0 && hv < 360.0 && sv >= 0.0 && sv <= 1.0 && vv >= 0.0 && vv <= 1.0 && av >= 0.0 && av <= 1.0;
    } else {
      return hv >= 0 && hv < 360 && sv >= 0 && sv <= 100 && vv >= 0 && vv <= 100 && av >= 0 && av <= 255;
    }
  }

  constexpr bool is_valid() const { return is_valid_val(h, s, v, a); }
};

/**
 * @name HSVA Color Type Aliases
 * @{
 */

/**
 * @brief Integer HSVA color type
 *
 * Represents HSVA colors using integer values.
 * Hue range: 0-359 degrees, Saturation/Value range: 0-100.
 * Alpha channel defaults to 255 (fully opaque).
 *
 * @tparam H Hue component (0-359)
 * @tparam S Saturation component (0-100)
 * @tparam V Value component (0-100)
 * @tparam A Alpha component (0-255, default 255)
 */
using hsva_int_t = basic_hsva<int, 1>;
template <int H, int S, int V, int A = 255>
inline constexpr hsva_int_t hsva_int = hsva_int_t::make<H, S, V, A>();

/**
 * @brief Floating-point HSVA color type
 *
 * Represents HSVA colors using float precision floating-point values.
 * Hue range: 0-360 degrees, Saturation/Value range: 0.0-1.0.
 * Scale is set to 1000, mapping integer inputs to floating-point range.
 * Alpha channel defaults to 1.0 (fully opaque).
 *
 * @tparam H Hue component (scaled from integer to 0-360)
 * @tparam S Saturation component (scaled from integer to 0.0-1.0)
 * @tparam V Value component (scaled from integer to 0.0-1.0)
 * @tparam A Alpha component (scaled from integer to 0.0-1.0, default 1.0)
 */
using hsva_float_t = basic_hsva<float, 1000>;
template <intptr_t H, intptr_t S, intptr_t V, intptr_t A = 1000>
inline constexpr hsva_float_t hsva_float = hsva_float_t::make<H, S, V, A>();

/** @} */

namespace colors {
// Basic hue colors (saturation 100%, value 100%)
inline constexpr hsva_int_t red_hsva = hsva_int<0, 100, 100>;
inline constexpr hsva_int_t orange_hsva = hsva_int<30, 100, 100>;
inline constexpr hsva_int_t yellow_hsva = hsva_int<60, 100, 100>;
inline constexpr hsva_int_t chartreuse_hsva = hsva_int<90, 100, 100>;
inline constexpr hsva_int_t green_hsva = hsva_int<120, 100, 100>;
inline constexpr hsva_int_t springgreen_hsva = hsva_int<150, 100, 100>;
inline constexpr hsva_int_t cyan_hsva = hsva_int<180, 100, 100>;
inline constexpr hsva_int_t azure_hsva = hsva_int<210, 100, 100>;
inline constexpr hsva_int_t blue_hsva = hsva_int<240, 100, 100>;
inline constexpr hsva_int_t violet_hsva = hsva_int<270, 100, 100>;
inline constexpr hsva_int_t magenta_hsva = hsva_int<300, 100, 100>;
inline constexpr hsva_int_t rose_hsva = hsva_int<330, 100, 100>;

// Grayscale series (saturation 0%)
inline constexpr hsva_int_t black_hsva = hsva_int<0, 0, 0>;    // Black
inline constexpr hsva_int_t gray1_hsva = hsva_int<0, 0, 25>;   // Dark gray
inline constexpr hsva_int_t gray2_hsva = hsva_int<0, 0, 50>;   // Medium gray
inline constexpr hsva_int_t gray3_hsva = hsva_int<0, 0, 75>;   // Light gray
inline constexpr hsva_int_t silver_hsva = hsva_int<0, 0, 75>;  // Silver (same as gray3_hsva)
inline constexpr hsva_int_t white_hsva = hsva_int<0, 0, 100>;  // White

// W3C standard colors in HSV
inline constexpr hsva_int_t maroon_hsva = hsva_int<0, 100, 50>;        // Maroon
inline constexpr hsva_int_t red_hsva_w3c = hsva_int<0, 100, 100>;      // Red
inline constexpr hsva_int_t olive_hsva = hsva_int<60, 100, 50>;        // Olive
inline constexpr hsva_int_t yellow_hsva_w3c = hsva_int<60, 100, 100>;  // Yellow
inline constexpr hsva_int_t green_hsva_w3c = hsva_int<120, 100, 50>;   // Green (W3C standard is dark green)
inline constexpr hsva_int_t lime_hsva = hsva_int<120, 100, 100>;       // Lime (bright green)
inline constexpr hsva_int_t teal_hsva = hsva_int<180, 100, 50>;        // Teal
inline constexpr hsva_int_t cyan_hsva_w3c = hsva_int<180, 100, 100>;   // Cyan
inline constexpr hsva_int_t navy_hsva = hsva_int<240, 100, 50>;        // Navy
inline constexpr hsva_int_t blue_hsva_w3c = hsva_int<240, 100, 100>;   // Blue
inline constexpr hsva_int_t purple_hsva = hsva_int<300, 100, 50>;      // Purple
inline constexpr hsva_int_t fuchsia_hsva = hsva_int<300, 100, 100>;    // Fuchsia
inline constexpr hsva_int_t aqua_hsva = hsva_int<180, 100, 100>;       // Aqua (same as cyan_hsva_w3c)

// Common variants (different saturation and value)
inline constexpr hsva_int_t darkred_hsva = hsva_int<0, 100, 30>;       // Dark red
inline constexpr hsva_int_t brown_hsva = hsva_int<30, 100, 40>;        // Brown
inline constexpr hsva_int_t darkorange_hsva = hsva_int<30, 100, 70>;   // Dark orange
inline constexpr hsva_int_t gold_hsva = hsva_int<45, 100, 85>;         // Gold
inline constexpr hsva_int_t darkgreen_hsva = hsva_int<120, 100, 30>;   // Dark green
inline constexpr hsva_int_t forestgreen_hsva = hsva_int<120, 80, 40>;  // Forest green
inline constexpr hsva_int_t seagreen_hsva = hsva_int<150, 80, 50>;     // Sea green
inline constexpr hsva_int_t darkcyan_hsva = hsva_int<180, 100, 40>;    // Dark cyan
inline constexpr hsva_int_t deepblue_hsva = hsva_int<240, 100, 40>;    // Deep blue
inline constexpr hsva_int_t indigo_hsva = hsva_int<260, 100, 45>;      // Indigo
inline constexpr hsva_int_t darkpurple_hsva = hsva_int<280, 100, 40>;  // Dark purple
inline constexpr hsva_int_t pink_hsva = hsva_int<340, 30, 100>;        // Pink
inline constexpr hsva_int_t hotpink_hsva = hsva_int<330, 70, 100>;     // Hot pink
inline constexpr hsva_int_t lightpink_hsva = hsva_int<350, 20, 100>;   // Light pink

// Low saturation colors (soft/muted colors)
inline constexpr hsva_int_t softred_hsva = hsva_int<0, 50, 90>;       // Soft red
inline constexpr hsva_int_t softorange_hsva = hsva_int<30, 50, 90>;   // Soft orange
inline constexpr hsva_int_t softyellow_hsva = hsva_int<60, 50, 90>;   // Soft yellow
inline constexpr hsva_int_t softgreen_hsva = hsva_int<120, 50, 90>;   // Soft green
inline constexpr hsva_int_t softblue_hsva = hsva_int<240, 50, 90>;    // Soft blue
inline constexpr hsva_int_t softpurple_hsva = hsva_int<280, 50, 90>;  // Soft purple

// High saturation medium value colors
inline constexpr hsva_int_t vibrantred_hsva = hsva_int<0, 100, 80>;       // Vibrant red
inline constexpr hsva_int_t vibrantorange_hsva = hsva_int<30, 100, 80>;   // Vibrant orange
inline constexpr hsva_int_t vibrantyellow_hsva = hsva_int<60, 100, 80>;   // Vibrant yellow
inline constexpr hsva_int_t vibrantgreen_hsva = hsva_int<120, 100, 80>;   // Vibrant green
inline constexpr hsva_int_t vibrantblue_hsva = hsva_int<240, 100, 80>;    // Vibrant blue
inline constexpr hsva_int_t vibrantpurple_hsva = hsva_int<280, 100, 80>;  // Vibrant purple

// Light colors (high value, low saturation)
inline constexpr hsva_int_t lightred_hsva = hsva_int<0, 30, 100>;        // Light red
inline constexpr hsva_int_t lightorange_hsva = hsva_int<30, 30, 100>;    // Light orange
inline constexpr hsva_int_t lightyellow_hsva = hsva_int<60, 30, 100>;    // Light yellow
inline constexpr hsva_int_t lightgreen_hsva = hsva_int<120, 30, 100>;    // Light green
inline constexpr hsva_int_t lightblue_hsva = hsva_int<210, 30, 100>;     // Light blue
inline constexpr hsva_int_t lightpurple_hsva = hsva_int<280, 30, 100>;   // Light purple
inline constexpr hsva_int_t lightcyan_hsva = hsva_int<180, 30, 100>;     // Light cyan
inline constexpr hsva_int_t lightmagenta_hsva = hsva_int<300, 30, 100>;  // Light magenta

// Dark colors (low value)
inline constexpr hsva_int_t deepred_hsva = hsva_int<0, 100, 25>;        // Deep red
inline constexpr hsva_int_t deeporange_hsva = hsva_int<30, 100, 25>;    // Deep orange
inline constexpr hsva_int_t deepyellow_hsva = hsva_int<60, 100, 25>;    // Deep yellow
inline constexpr hsva_int_t deepgreen_hsva = hsva_int<120, 100, 25>;    // Deep green
inline constexpr hsva_int_t deepblue_hsva2 = hsva_int<240, 100, 25>;    // Deep blue
inline constexpr hsva_int_t deeppurple_hsva = hsva_int<280, 100, 25>;   // Deep purple
inline constexpr hsva_int_t deepcyan_hsva = hsva_int<180, 100, 25>;     // Deep cyan
inline constexpr hsva_int_t deepmagenta_hsva = hsva_int<300, 100, 25>;  // Deep magenta
}  // namespace colors

}  // namespace color::core
