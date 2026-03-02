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

#include <cassert>
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
template <typename T, intptr_t Scale = 1>
struct basic_rgb {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

  using value_type = T;
  T r, g, b;
  constexpr basic_rgb() : r(0), g(0), b(0) {}
  constexpr basic_rgb(T red, T green, T blue) : r(red), g(green), b(blue) {
    if (!is_valid_val(red, green, blue)) {
      assert(false && "RGB values out of range!");
    }
  }

  template <intptr_t R_raw, intptr_t G_raw, intptr_t B_raw>
  static constexpr basic_rgb make() {
    constexpr T static_r = static_cast<T>(R_raw) / static_cast<T>(Scale);
    constexpr T static_g = static_cast<T>(G_raw) / static_cast<T>(Scale);
    constexpr T static_b = static_cast<T>(B_raw) / static_cast<T>(Scale);
    static_assert(is_valid_val(static_r, static_g, static_b), "RGB value out of range!");
    return {static_r, static_g, static_b};
  }

  static constexpr bool is_valid_val(T rv, T gv, T bv) {
    if constexpr (std::is_floating_point_v<T>) {
      return rv >= 0.0 && rv <= 1.0 && gv >= 0.0 && gv <= 1.0 && bv >= 0.0 && bv <= 1.0;
    } else {
      return rv >= 0 && rv <= 255 && gv >= 0 && gv <= 255 && bv >= 0 && bv <= 255;
    }
  }

  constexpr bool is_valid() const { return is_valid_val(r, g, b); }
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
using rgb8_t = basic_rgb<uint8_t, 1>;
template <uint8_t R, uint8_t G, uint8_t B>
inline constexpr rgb8_t rgb8 = rgb8_t::make<R, G, B>();

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
using rgbf_t = basic_rgb<double, 1000>;
template <intptr_t R, intptr_t G, intptr_t B>
inline constexpr rgbf_t rgbf = rgbf_t::make<R, G, B>();

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
using rgb_percent = basic_rgb<int>;

/** @} */

namespace colors {
// Basic RGB colors
inline constexpr rgb8_t black = rgb8<0, 0, 0>;
inline constexpr rgb8_t white = rgb8<255, 255, 255>;
inline constexpr rgb8_t red = rgb8<255, 0, 0>;
inline constexpr rgb8_t lime = rgb8<0, 255, 0>;  // Bright green
inline constexpr rgb8_t blue = rgb8<0, 0, 255>;
inline constexpr rgb8_t yellow = rgb8<255, 255, 0>;
inline constexpr rgb8_t cyan = rgb8<0, 255, 255>;
inline constexpr rgb8_t magenta = rgb8<255, 0, 255>;
inline constexpr rgb8_t aqua = rgb8<0, 255, 255>;     // Same as cyan
inline constexpr rgb8_t fuchsia = rgb8<255, 0, 255>;  // Same as magenta

// W3C standard colors
inline constexpr rgb8_t maroon = rgb8<128, 0, 0>;
inline constexpr rgb8_t darkred = rgb8<139, 0, 0>;
inline constexpr rgb8_t brown = rgb8<165, 42, 42>;
inline constexpr rgb8_t firebrick = rgb8<178, 34, 34>;
inline constexpr rgb8_t indianred = rgb8<205, 92, 92>;
inline constexpr rgb8_t lightcoral = rgb8<240, 128, 128>;
inline constexpr rgb8_t salmon = rgb8<250, 128, 114>;
inline constexpr rgb8_t darksalmon = rgb8<233, 150, 122>;
inline constexpr rgb8_t lightsalmon = rgb8<255, 160, 122>;
inline constexpr rgb8_t orange = rgb8<255, 165, 0>;
inline constexpr rgb8_t darkorange = rgb8<255, 140, 0>;
inline constexpr rgb8_t coral = rgb8<255, 127, 80>;
inline constexpr rgb8_t tomato = rgb8<255, 99, 71>;
inline constexpr rgb8_t orangered = rgb8<255, 69, 0>;
inline constexpr rgb8_t gold = rgb8<255, 215, 0>;
inline constexpr rgb8_t goldenrod = rgb8<218, 165, 32>;
inline constexpr rgb8_t darkgoldenrod = rgb8<184, 134, 11>;
inline constexpr rgb8_t lightgoldenrodyellow = rgb8<250, 250, 210>;
inline constexpr rgb8_t palegoldenrod = rgb8<238, 232, 170>;
inline constexpr rgb8_t khaki = rgb8<240, 230, 140>;
inline constexpr rgb8_t darkkhaki = rgb8<189, 183, 107>;
inline constexpr rgb8_t olive = rgb8<128, 128, 0>;
inline constexpr rgb8_t olivedrab = rgb8<107, 142, 35>;
inline constexpr rgb8_t yellowgreen = rgb8<154, 205, 50>;
inline constexpr rgb8_t darkolivegreen = rgb8<85, 107, 47>;
inline constexpr rgb8_t green = rgb8<0, 128, 0>;  // W3C standard green (dark)
inline constexpr rgb8_t greenyellow = rgb8<173, 255, 47>;
inline constexpr rgb8_t chartreuse = rgb8<127, 255, 0>;
inline constexpr rgb8_t lawngreen = rgb8<124, 252, 0>;
inline constexpr rgb8_t springgreen = rgb8<0, 255, 127>;
inline constexpr rgb8_t mediumspringgreen = rgb8<0, 250, 154>;
inline constexpr rgb8_t lightgreen = rgb8<144, 238, 144>;
inline constexpr rgb8_t palegreen = rgb8<152, 251, 152>;
inline constexpr rgb8_t darkseagreen = rgb8<143, 188, 143>;
inline constexpr rgb8_t mediumseagreen = rgb8<60, 179, 113>;
inline constexpr rgb8_t seagreen = rgb8<46, 139, 87>;
inline constexpr rgb8_t forestgreen = rgb8<34, 139, 34>;
inline constexpr rgb8_t darkgreen = rgb8<0, 100, 0>;
inline constexpr rgb8_t teal = rgb8<0, 128, 128>;
inline constexpr rgb8_t darkcyan = rgb8<0, 139, 139>;
inline constexpr rgb8_t lightcyan = rgb8<224, 255, 255>;
inline constexpr rgb8_t paleturquoise = rgb8<175, 238, 238>;
inline constexpr rgb8_t aquamarine = rgb8<127, 255, 212>;
inline constexpr rgb8_t turquoise = rgb8<64, 224, 208>;
inline constexpr rgb8_t mediumturquoise = rgb8<72, 209, 204>;
inline constexpr rgb8_t darkturquoise = rgb8<0, 206, 209>;
inline constexpr rgb8_t lightseagreen = rgb8<32, 178, 170>;
inline constexpr rgb8_t cadetblue = rgb8<95, 158, 160>;
inline constexpr rgb8_t navy = rgb8<0, 0, 128>;
inline constexpr rgb8_t darkblue = rgb8<0, 0, 139>;
inline constexpr rgb8_t mediumblue = rgb8<0, 0, 205>;
inline constexpr rgb8_t royalblue = rgb8<65, 105, 225>;
inline constexpr rgb8_t steelblue = rgb8<70, 130, 180>;
inline constexpr rgb8_t dodgerblue = rgb8<30, 144, 255>;
inline constexpr rgb8_t deepskyblue = rgb8<0, 191, 255>;
inline constexpr rgb8_t skyblue = rgb8<135, 206, 235>;
inline constexpr rgb8_t lightskyblue = rgb8<135, 206, 250>;
inline constexpr rgb8_t lightsteelblue = rgb8<176, 196, 222>;
inline constexpr rgb8_t lightblue = rgb8<173, 216, 230>;
inline constexpr rgb8_t powderblue = rgb8<176, 224, 230>;
inline constexpr rgb8_t cornflowerblue = rgb8<100, 149, 237>;
inline constexpr rgb8_t midnightblue = rgb8<25, 25, 112>;
inline constexpr rgb8_t purple = rgb8<128, 0, 128>;
inline constexpr rgb8_t darkmagenta = rgb8<139, 0, 139>;
inline constexpr rgb8_t darkviolet = rgb8<148, 0, 211>;
inline constexpr rgb8_t darkorchid = rgb8<153, 50, 204>;
inline constexpr rgb8_t blueviolet = rgb8<138, 43, 226>;
inline constexpr rgb8_t mediumpurple = rgb8<147, 112, 219>;
inline constexpr rgb8_t mediumslateblue = rgb8<123, 104, 238>;
inline constexpr rgb8_t slateblue = rgb8<106, 90, 205>;
inline constexpr rgb8_t darkslateblue = rgb8<72, 61, 139>;
inline constexpr rgb8_t lavender = rgb8<230, 230, 250>;
inline constexpr rgb8_t thistle = rgb8<216, 191, 216>;
inline constexpr rgb8_t plum = rgb8<221, 160, 221>;
inline constexpr rgb8_t orchid = rgb8<218, 112, 214>;
inline constexpr rgb8_t violet = rgb8<238, 130, 238>;
inline constexpr rgb8_t mediumorchid = rgb8<186, 85, 211>;
inline constexpr rgb8_t pink = rgb8<255, 192, 203>;
inline constexpr rgb8_t lightpink = rgb8<255, 182, 193>;
inline constexpr rgb8_t hotpink = rgb8<255, 105, 180>;
inline constexpr rgb8_t deeppink = rgb8<255, 20, 147>;
inline constexpr rgb8_t palevioletred = rgb8<219, 112, 147>;
inline constexpr rgb8_t mediumvioletred = rgb8<199, 21, 133>;
inline constexpr rgb8_t crimson = rgb8<220, 20, 60>;

// Gray series
inline constexpr rgb8_t gray = rgb8<128, 128, 128>;
inline constexpr rgb8_t grey = rgb8<128, 128, 128>;  // Same as gray
inline constexpr rgb8_t darkgray = rgb8<169, 169, 169>;
inline constexpr rgb8_t darkgrey = rgb8<169, 169, 169>;
inline constexpr rgb8_t silver = rgb8<192, 192, 192>;
inline constexpr rgb8_t lightgray = rgb8<211, 211, 211>;
inline constexpr rgb8_t lightgrey = rgb8<211, 211, 211>;
inline constexpr rgb8_t gainsboro = rgb8<220, 220, 220>;
inline constexpr rgb8_t whitesmoke = rgb8<245, 245, 245>;
inline constexpr rgb8_t dimgray = rgb8<105, 105, 105>;
inline constexpr rgb8_t dimgrey = rgb8<105, 105, 105>;
inline constexpr rgb8_t slategray = rgb8<112, 128, 144>;
inline constexpr rgb8_t slategrey = rgb8<112, 128, 144>;
inline constexpr rgb8_t lightslategray = rgb8<119, 136, 153>;
inline constexpr rgb8_t lightslategrey = rgb8<119, 136, 153>;

// Additional named colors
inline constexpr rgb8_t ivory = rgb8<255, 255, 240>;
inline constexpr rgb8_t beige = rgb8<245, 245, 220>;
inline constexpr rgb8_t wheat = rgb8<245, 222, 179>;
inline constexpr rgb8_t bisque = rgb8<255, 228, 196>;
inline constexpr rgb8_t blanchedalmond = rgb8<255, 235, 205>;
inline constexpr rgb8_t cornsilk = rgb8<255, 248, 220>;
inline constexpr rgb8_t lemonchiffon = rgb8<255, 250, 205>;
inline constexpr rgb8_t lightyellow = rgb8<255, 255, 224>;
inline constexpr rgb8_t honeydew = rgb8<240, 255, 240>;
inline constexpr rgb8_t mintcream = rgb8<245, 255, 250>;
inline constexpr rgb8_t azure = rgb8<240, 255, 255>;
inline constexpr rgb8_t aliceblue = rgb8<240, 248, 255>;
inline constexpr rgb8_t ghostwhite = rgb8<248, 248, 255>;
inline constexpr rgb8_t seashell = rgb8<255, 245, 238>;
inline constexpr rgb8_t snow = rgb8<255, 250, 250>;
inline constexpr rgb8_t floralwhite = rgb8<255, 250, 240>;
inline constexpr rgb8_t linen = rgb8<250, 240, 230>;
inline constexpr rgb8_t oldlace = rgb8<253, 245, 230>;
inline constexpr rgb8_t antiquewhite = rgb8<250, 235, 215>;
inline constexpr rgb8_t papayawhip = rgb8<255, 239, 213>;
inline constexpr rgb8_t peachpuff = rgb8<255, 218, 185>;
inline constexpr rgb8_t moccasin = rgb8<255, 228, 181>;
inline constexpr rgb8_t navajowhite = rgb8<255, 222, 173>;
inline constexpr rgb8_t mistyrose = rgb8<255, 228, 225>;
inline constexpr rgb8_t lavenderblush = rgb8<255, 240, 245>;
inline constexpr rgb8_t indigo = rgb8<75, 0, 130>;
inline constexpr rgb8_t chocolate = rgb8<210, 105, 30>;
inline constexpr rgb8_t peru = rgb8<205, 133, 63>;
inline constexpr rgb8_t sandybrown = rgb8<244, 164, 96>;
inline constexpr rgb8_t burlywood = rgb8<222, 184, 135>;
inline constexpr rgb8_t tan = rgb8<210, 180, 140>;
inline constexpr rgb8_t rosybrown = rgb8<188, 143, 143>;
inline constexpr rgb8_t sienna = rgb8<160, 82, 45>;
inline constexpr rgb8_t saddlebrown = rgb8<139, 69, 19>;

}  // namespace colors

}  // namespace color::core
