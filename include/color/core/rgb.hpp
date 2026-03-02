/**
 * @file rgb.hpp
 * @brief RGB color space core implementation
 *
 * Provides basic RGB color template class and common color aliases.
 * Supports 8-bit integer RGB and floating-point RGB representations.
 *
 * @author Merlot.Qi
 * 
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
// Basic RGB colors
using black = rgb8<0, 0, 0>;
using white = rgb8<255, 255, 255>;
using red = rgb8<255, 0, 0>;
using lime = rgb8<0, 255, 0>;  // Bright green
using blue = rgb8<0, 0, 255>;
using yellow = rgb8<255, 255, 0>;
using cyan = rgb8<0, 255, 255>;
using magenta = rgb8<255, 0, 255>;
using aqua = rgb8<0, 255, 255>;     // Same as cyan
using fuchsia = rgb8<255, 0, 255>;  // Same as magenta

// W3C standard colors
using maroon = rgb8<128, 0, 0>;
using darkred = rgb8<139, 0, 0>;
using brown = rgb8<165, 42, 42>;
using firebrick = rgb8<178, 34, 34>;
using indianred = rgb8<205, 92, 92>;
using lightcoral = rgb8<240, 128, 128>;
using salmon = rgb8<250, 128, 114>;
using darksalmon = rgb8<233, 150, 122>;
using lightsalmon = rgb8<255, 160, 122>;
using orange = rgb8<255, 165, 0>;
using darkorange = rgb8<255, 140, 0>;
using coral = rgb8<255, 127, 80>;
using tomato = rgb8<255, 99, 71>;
using orangered = rgb8<255, 69, 0>;
using gold = rgb8<255, 215, 0>;
using goldenrod = rgb8<218, 165, 32>;
using darkgoldenrod = rgb8<184, 134, 11>;
using lightgoldenrodyellow = rgb8<250, 250, 210>;
using palegoldenrod = rgb8<238, 232, 170>;
using khaki = rgb8<240, 230, 140>;
using darkkhaki = rgb8<189, 183, 107>;
using olive = rgb8<128, 128, 0>;
using olivedrab = rgb8<107, 142, 35>;
using yellowgreen = rgb8<154, 205, 50>;
using darkolivegreen = rgb8<85, 107, 47>;
using green = rgb8<0, 128, 0>;  // W3C standard green (dark)
using greenyellow = rgb8<173, 255, 47>;
using chartreuse = rgb8<127, 255, 0>;
using lawngreen = rgb8<124, 252, 0>;
using springgreen = rgb8<0, 255, 127>;
using mediumspringgreen = rgb8<0, 250, 154>;
using lightgreen = rgb8<144, 238, 144>;
using palegreen = rgb8<152, 251, 152>;
using darkseagreen = rgb8<143, 188, 143>;
using mediumseagreen = rgb8<60, 179, 113>;
using seagreen = rgb8<46, 139, 87>;
using forestgreen = rgb8<34, 139, 34>;
using darkgreen = rgb8<0, 100, 0>;
using teal = rgb8<0, 128, 128>;
using darkcyan = rgb8<0, 139, 139>;
using lightcyan = rgb8<224, 255, 255>;
using paleturquoise = rgb8<175, 238, 238>;
using aquamarine = rgb8<127, 255, 212>;
using turquoise = rgb8<64, 224, 208>;
using mediumturquoise = rgb8<72, 209, 204>;
using darkturquoise = rgb8<0, 206, 209>;
using lightseagreen = rgb8<32, 178, 170>;
using cadetblue = rgb8<95, 158, 160>;
using navy = rgb8<0, 0, 128>;
using darkblue = rgb8<0, 0, 139>;
using mediumblue = rgb8<0, 0, 205>;
using royalblue = rgb8<65, 105, 225>;
using steelblue = rgb8<70, 130, 180>;
using dodgerblue = rgb8<30, 144, 255>;
using deepskyblue = rgb8<0, 191, 255>;
using skyblue = rgb8<135, 206, 235>;
using lightskyblue = rgb8<135, 206, 250>;
using lightsteelblue = rgb8<176, 196, 222>;
using lightblue = rgb8<173, 216, 230>;
using powderblue = rgb8<176, 224, 230>;
using cornflowerblue = rgb8<100, 149, 237>;
using midnightblue = rgb8<25, 25, 112>;
using purple = rgb8<128, 0, 128>;
using darkmagenta = rgb8<139, 0, 139>;
using darkviolet = rgb8<148, 0, 211>;
using darkorchid = rgb8<153, 50, 204>;
using blueviolet = rgb8<138, 43, 226>;
using mediumpurple = rgb8<147, 112, 219>;
using mediumslateblue = rgb8<123, 104, 238>;
using slateblue = rgb8<106, 90, 205>;
using darkslateblue = rgb8<72, 61, 139>;
using lavender = rgb8<230, 230, 250>;
using thistle = rgb8<216, 191, 216>;
using plum = rgb8<221, 160, 221>;
using orchid = rgb8<218, 112, 214>;
using violet = rgb8<238, 130, 238>;
using mediumorchid = rgb8<186, 85, 211>;
using pink = rgb8<255, 192, 203>;
using lightpink = rgb8<255, 182, 193>;
using hotpink = rgb8<255, 105, 180>;
using deeppink = rgb8<255, 20, 147>;
using palevioletred = rgb8<219, 112, 147>;
using mediumvioletred = rgb8<199, 21, 133>;
using crimson = rgb8<220, 20, 60>;

// Gray series
using gray = rgb8<128, 128, 128>;
using grey = rgb8<128, 128, 128>;  // Same as gray
using darkgray = rgb8<169, 169, 169>;
using darkgrey = rgb8<169, 169, 169>;
using silver = rgb8<192, 192, 192>;
using lightgray = rgb8<211, 211, 211>;
using lightgrey = rgb8<211, 211, 211>;
using gainsboro = rgb8<220, 220, 220>;
using whitesmoke = rgb8<245, 245, 245>;
using dimgray = rgb8<105, 105, 105>;
using dimgrey = rgb8<105, 105, 105>;
using slategray = rgb8<112, 128, 144>;
using slategrey = rgb8<112, 128, 144>;
using lightslategray = rgb8<119, 136, 153>;
using lightslategrey = rgb8<119, 136, 153>;

// Additional named colors
using ivory = rgb8<255, 255, 240>;
using beige = rgb8<245, 245, 220>;
using wheat = rgb8<245, 222, 179>;
using bisque = rgb8<255, 228, 196>;
using blanchedalmond = rgb8<255, 235, 205>;
using cornsilk = rgb8<255, 248, 220>;
using lemonchiffon = rgb8<255, 250, 205>;
using lightyellow = rgb8<255, 255, 224>;
using lightgoldenrodyellow = rgb8<250, 250, 210>;
using honeydew = rgb8<240, 255, 240>;
using mintcream = rgb8<245, 255, 250>;
using azure = rgb8<240, 255, 255>;
using aliceblue = rgb8<240, 248, 255>;
using ghostwhite = rgb8<248, 248, 255>;
using seashell = rgb8<255, 245, 238>;
using snow = rgb8<255, 250, 250>;
using floralwhite = rgb8<255, 250, 240>;
using linen = rgb8<250, 240, 230>;
using oldlace = rgb8<253, 245, 230>;
using antiquewhite = rgb8<250, 235, 215>;
using papayawhip = rgb8<255, 239, 213>;
using peachpuff = rgb8<255, 218, 185>;
using moccasin = rgb8<255, 228, 181>;
using navajowhite = rgb8<255, 222, 173>;
using mistyrose = rgb8<255, 228, 225>;
using lavenderblush = rgb8<255, 240, 245>;
using thistle = rgb8<216, 191, 216>;
using orchid = rgb8<218, 112, 214>;
using plum = rgb8<221, 160, 221>;
using violet = rgb8<238, 130, 238>;
using mediumorchid = rgb8<186, 85, 211>;
using darkorchid = rgb8<153, 50, 204>;
using blueviolet = rgb8<138, 43, 226>;
using indigo = rgb8<75, 0, 130>;
using chocolate = rgb8<210, 105, 30>;
using peru = rgb8<205, 133, 63>;
using sandybrown = rgb8<244, 164, 96>;
using burlywood = rgb8<222, 184, 135>;
using tan = rgb8<210, 180, 140>;
using rosybrown = rgb8<188, 143, 143>;
using sienna = rgb8<160, 82, 45>;
using saddlebrown = rgb8<139, 69, 19>;
using chocolate = rgb8<210, 105, 30>;
using peru = rgb8<205, 133, 63>;
}  // namespace colors

}  // namespace color::core
