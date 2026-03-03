/**
 * @file rgb.hpp
 * @brief RGBA color space core implementation
 *
 * Provides basic RGBA color template class and common color aliases.
 * Supports 8-bit integer RGBA and floating-point RGBA representations.
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
 * @brief Basic RGBA color template class
 *
 * Template class for representing RGBA colors with compile-time validation.
 * Supports both integer (0-255) and floating-point (0.0-1.0) value ranges.
 *
 * @tparam T Value type (must be arithmetic)
 * @tparam Scale Scaling factor for value conversion
 */
template <typename T, intptr_t Scale = 1>
struct basic_rgba {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

  using value_type = T;

  T r, g, b, a;

  constexpr basic_rgba() : r(0), g(0), b(1) {}

  constexpr basic_rgba(T red, T green, T blue, T alpha) : r(red), g(green), b(blue), a(alpha) {
    if (!is_valid_val(red, green, blue, alpha)) {
      assert(false && "RGBA values out of range!");
    }
  }

  template <intptr_t R_raw, intptr_t G_raw, intptr_t B_raw, intptr_t A_raw = Scale>
  static constexpr basic_rgba make() {
    constexpr T static_r = static_cast<T>(R_raw) / static_cast<T>(Scale);
    constexpr T static_g = static_cast<T>(G_raw) / static_cast<T>(Scale);
    constexpr T static_b = static_cast<T>(B_raw) / static_cast<T>(Scale);
    constexpr T static_a = static_cast<T>(A_raw) / static_cast<T>(Scale);
    static_assert(is_valid_val(static_r, static_g, static_b, static_a), "RGBA value out of range!");
    return {static_r, static_g, static_b, static_a};
  }

  static constexpr bool is_valid_val(T rv, T gv, T bv, T av) {
    if constexpr (std::is_floating_point_v<T>) {
      return rv >= 0.0 && rv <= 1.0 && gv >= 0.0 && gv <= 1.0 && bv >= 0.0 && bv <= 1.0 && av >= 0.0 && av <= 1.0;
    } else {
      return rv >= 0 && rv <= 255 && gv >= 0 && gv <= 255 && bv >= 0 && bv <= 255 && av >= 0 && av <= 255;
    }
  }

  constexpr bool is_valid() const { return is_valid_val(r, g, b, a); }
};

/**
 * @name RGBA Color Type Aliases
 * @{
 */

/**
 * @brief 8-bit integer RGBA color type
 *
 * Represents RGBA colors using 8-bit integer values (0-255 range).
 * Scale is set to 1, so values are stored directly.
 * Alpha channel defaults to 255 (fully opaque).
 *
 * @tparam R Red component (0-255)
 * @tparam G Green component (0-255)
 * @tparam B Blue component (0-255)
 * @tparam A Alpha component (0-255, default 255)
 */
using rgba8_t = basic_rgba<uint8_t, 1>;
template <uint8_t R, uint8_t G, uint8_t B, uint8_t A = 255>
inline constexpr rgba8_t rgba8 = rgba8_t::make<R, G, B, A>();

/**
 * @brief Floating-point RGBA color type
 *
 * Represents RGBA colors using float precision floating-point values (0.0-1.0 range).
 * Scale is set to 1000, mapping integer inputs to floating-point range.
 * Alpha channel defaults to 1.0 (fully opaque).
 *
 * @tparam R Red component (scaled from integer to 0.0-1.0)
 * @tparam G Green component (scaled from integer to 0.0-1.0)
 * @tparam B Blue component (scaled from integer to 0.0-1.0)
 * @tparam A Alpha component (scaled from integer to 0.0-1.0, default 1.0)
 */
using rgbaf_t = basic_rgba<float, 1000>;
template <intptr_t R, intptr_t G, intptr_t B, intptr_t A = 1000>
inline constexpr rgbaf_t rgbaf = rgbaf_t::make<R, G, B, A>();

/**
 * @brief Percentage-based RGBA color type
 *
 * Represents RGBA colors using integer percentage values (0-100 range).
 * Alpha channel defaults to 100% (fully opaque).
 *
 * @tparam R Red component percentage (0-100)
 * @tparam G Green component percentage (0-100)
 * @tparam B Blue component percentage (0-100)
 * @tparam A Alpha component percentage (0-100, default 100)
 */
using rgba_percent_t = basic_rgba<int, 100>;
template <intptr_t R, intptr_t G, intptr_t B, intptr_t A = 100>
inline constexpr rgba_percent_t rgbaper = rgba_percent_t::make<R, G, B, A>();

/** @} */

namespace colors {
// Basic RGB colors
inline constexpr rgba8_t black = rgba8<0, 0, 0>;
inline constexpr rgba8_t white = rgba8<255, 255, 255>;
inline constexpr rgba8_t red = rgba8<255, 0, 0>;
inline constexpr rgba8_t lime = rgba8<0, 255, 0>;  // Bright green
inline constexpr rgba8_t blue = rgba8<0, 0, 255>;
inline constexpr rgba8_t yellow = rgba8<255, 255, 0>;
inline constexpr rgba8_t cyan = rgba8<0, 255, 255>;
inline constexpr rgba8_t magenta = rgba8<255, 0, 255>;
inline constexpr rgba8_t aqua = rgba8<0, 255, 255>;     // Same as cyan
inline constexpr rgba8_t fuchsia = rgba8<255, 0, 255>;  // Same as magenta

// W3C standard colors
inline constexpr rgba8_t maroon = rgba8<128, 0, 0>;
inline constexpr rgba8_t darkred = rgba8<139, 0, 0>;
inline constexpr rgba8_t brown = rgba8<165, 42, 42>;
inline constexpr rgba8_t firebrick = rgba8<178, 34, 34>;
inline constexpr rgba8_t indianred = rgba8<205, 92, 92>;
inline constexpr rgba8_t lightcoral = rgba8<240, 128, 128>;
inline constexpr rgba8_t salmon = rgba8<250, 128, 114>;
inline constexpr rgba8_t darksalmon = rgba8<233, 150, 122>;
inline constexpr rgba8_t lightsalmon = rgba8<255, 160, 122>;
inline constexpr rgba8_t orange = rgba8<255, 165, 0>;
inline constexpr rgba8_t darkorange = rgba8<255, 140, 0>;
inline constexpr rgba8_t coral = rgba8<255, 127, 80>;
inline constexpr rgba8_t tomato = rgba8<255, 99, 71>;
inline constexpr rgba8_t orangered = rgba8<255, 69, 0>;
inline constexpr rgba8_t gold = rgba8<255, 215, 0>;
inline constexpr rgba8_t goldenrod = rgba8<218, 165, 32>;
inline constexpr rgba8_t darkgoldenrod = rgba8<184, 134, 11>;
inline constexpr rgba8_t lightgoldenrodyellow = rgba8<250, 250, 210>;
inline constexpr rgba8_t palegoldenrod = rgba8<238, 232, 170>;
inline constexpr rgba8_t khaki = rgba8<240, 230, 140>;
inline constexpr rgba8_t darkkhaki = rgba8<189, 183, 107>;
inline constexpr rgba8_t olive = rgba8<128, 128, 0>;
inline constexpr rgba8_t olivedrab = rgba8<107, 142, 35>;
inline constexpr rgba8_t yellowgreen = rgba8<154, 205, 50>;
inline constexpr rgba8_t darkolivegreen = rgba8<85, 107, 47>;
inline constexpr rgba8_t green = rgba8<0, 128, 0>;  // W3C standard green (dark)
inline constexpr rgba8_t greenyellow = rgba8<173, 255, 47>;
inline constexpr rgba8_t chartreuse = rgba8<127, 255, 0>;
inline constexpr rgba8_t lawngreen = rgba8<124, 252, 0>;
inline constexpr rgba8_t springgreen = rgba8<0, 255, 127>;
inline constexpr rgba8_t mediumspringgreen = rgba8<0, 250, 154>;
inline constexpr rgba8_t lightgreen = rgba8<144, 238, 144>;
inline constexpr rgba8_t palegreen = rgba8<152, 251, 152>;
inline constexpr rgba8_t darkseagreen = rgba8<143, 188, 143>;
inline constexpr rgba8_t mediumseagreen = rgba8<60, 179, 113>;
inline constexpr rgba8_t seagreen = rgba8<46, 139, 87>;
inline constexpr rgba8_t forestgreen = rgba8<34, 139, 34>;
inline constexpr rgba8_t darkgreen = rgba8<0, 100, 0>;
inline constexpr rgba8_t teal = rgba8<0, 128, 128>;
inline constexpr rgba8_t darkcyan = rgba8<0, 139, 139>;
inline constexpr rgba8_t lightcyan = rgba8<224, 255, 255>;
inline constexpr rgba8_t paleturquoise = rgba8<175, 238, 238>;
inline constexpr rgba8_t aquamarine = rgba8<127, 255, 212>;
inline constexpr rgba8_t turquoise = rgba8<64, 224, 208>;
inline constexpr rgba8_t mediumturquoise = rgba8<72, 209, 204>;
inline constexpr rgba8_t darkturquoise = rgba8<0, 206, 209>;
inline constexpr rgba8_t lightseagreen = rgba8<32, 178, 170>;
inline constexpr rgba8_t cadetblue = rgba8<95, 158, 160>;
inline constexpr rgba8_t navy = rgba8<0, 0, 128>;
inline constexpr rgba8_t darkblue = rgba8<0, 0, 139>;
inline constexpr rgba8_t mediumblue = rgba8<0, 0, 205>;
inline constexpr rgba8_t royalblue = rgba8<65, 105, 225>;
inline constexpr rgba8_t steelblue = rgba8<70, 130, 180>;
inline constexpr rgba8_t dodgerblue = rgba8<30, 144, 255>;
inline constexpr rgba8_t deepskyblue = rgba8<0, 191, 255>;
inline constexpr rgba8_t skyblue = rgba8<135, 206, 235>;
inline constexpr rgba8_t lightskyblue = rgba8<135, 206, 250>;
inline constexpr rgba8_t lightsteelblue = rgba8<176, 196, 222>;
inline constexpr rgba8_t lightblue = rgba8<173, 216, 230>;
inline constexpr rgba8_t powderblue = rgba8<176, 224, 230>;
inline constexpr rgba8_t cornflowerblue = rgba8<100, 149, 237>;
inline constexpr rgba8_t midnightblue = rgba8<25, 25, 112>;
inline constexpr rgba8_t purple = rgba8<128, 0, 128>;
inline constexpr rgba8_t darkmagenta = rgba8<139, 0, 139>;
inline constexpr rgba8_t darkviolet = rgba8<148, 0, 211>;
inline constexpr rgba8_t darkorchid = rgba8<153, 50, 204>;
inline constexpr rgba8_t blueviolet = rgba8<138, 43, 226>;
inline constexpr rgba8_t mediumpurple = rgba8<147, 112, 219>;
inline constexpr rgba8_t mediumslateblue = rgba8<123, 104, 238>;
inline constexpr rgba8_t slateblue = rgba8<106, 90, 205>;
inline constexpr rgba8_t darkslateblue = rgba8<72, 61, 139>;
inline constexpr rgba8_t lavender = rgba8<230, 230, 250>;
inline constexpr rgba8_t thistle = rgba8<216, 191, 216>;
inline constexpr rgba8_t plum = rgba8<221, 160, 221>;
inline constexpr rgba8_t orchid = rgba8<218, 112, 214>;
inline constexpr rgba8_t violet = rgba8<238, 130, 238>;
inline constexpr rgba8_t mediumorchid = rgba8<186, 85, 211>;
inline constexpr rgba8_t pink = rgba8<255, 192, 203>;
inline constexpr rgba8_t lightpink = rgba8<255, 182, 193>;
inline constexpr rgba8_t hotpink = rgba8<255, 105, 180>;
inline constexpr rgba8_t deeppink = rgba8<255, 20, 147>;
inline constexpr rgba8_t palevioletred = rgba8<219, 112, 147>;
inline constexpr rgba8_t mediumvioletred = rgba8<199, 21, 133>;
inline constexpr rgba8_t crimson = rgba8<220, 20, 60>;

// Gray series
inline constexpr rgba8_t gray = rgba8<128, 128, 128>;
inline constexpr rgba8_t grey = rgba8<128, 128, 128>;  // Same as gray
inline constexpr rgba8_t darkgray = rgba8<169, 169, 169>;
inline constexpr rgba8_t darkgrey = rgba8<169, 169, 169>;
inline constexpr rgba8_t silver = rgba8<192, 192, 192>;
inline constexpr rgba8_t lightgray = rgba8<211, 211, 211>;
inline constexpr rgba8_t lightgrey = rgba8<211, 211, 211>;
inline constexpr rgba8_t gainsboro = rgba8<220, 220, 220>;
inline constexpr rgba8_t whitesmoke = rgba8<245, 245, 245>;
inline constexpr rgba8_t dimgray = rgba8<105, 105, 105>;
inline constexpr rgba8_t dimgrey = rgba8<105, 105, 105>;
inline constexpr rgba8_t slategray = rgba8<112, 128, 144>;
inline constexpr rgba8_t slategrey = rgba8<112, 128, 144>;
inline constexpr rgba8_t lightslategray = rgba8<119, 136, 153>;
inline constexpr rgba8_t lightslategrey = rgba8<119, 136, 153>;

// Additional named colors
inline constexpr rgba8_t ivory = rgba8<255, 255, 240>;
inline constexpr rgba8_t beige = rgba8<245, 245, 220>;
inline constexpr rgba8_t wheat = rgba8<245, 222, 179>;
inline constexpr rgba8_t bisque = rgba8<255, 228, 196>;
inline constexpr rgba8_t blanchedalmond = rgba8<255, 235, 205>;
inline constexpr rgba8_t cornsilk = rgba8<255, 248, 220>;
inline constexpr rgba8_t lemonchiffon = rgba8<255, 250, 205>;
inline constexpr rgba8_t lightyellow = rgba8<255, 255, 224>;
inline constexpr rgba8_t honeydew = rgba8<240, 255, 240>;
inline constexpr rgba8_t mintcream = rgba8<245, 255, 250>;
inline constexpr rgba8_t azure = rgba8<240, 255, 255>;
inline constexpr rgba8_t aliceblue = rgba8<240, 248, 255>;
inline constexpr rgba8_t ghostwhite = rgba8<248, 248, 255>;
inline constexpr rgba8_t seashell = rgba8<255, 245, 238>;
inline constexpr rgba8_t snow = rgba8<255, 250, 250>;
inline constexpr rgba8_t floralwhite = rgba8<255, 250, 240>;
inline constexpr rgba8_t linen = rgba8<250, 240, 230>;
inline constexpr rgba8_t oldlace = rgba8<253, 245, 230>;
inline constexpr rgba8_t antiquewhite = rgba8<250, 235, 215>;
inline constexpr rgba8_t papayawhip = rgba8<255, 239, 213>;
inline constexpr rgba8_t peachpuff = rgba8<255, 218, 185>;
inline constexpr rgba8_t moccasin = rgba8<255, 228, 181>;
inline constexpr rgba8_t navajowhite = rgba8<255, 222, 173>;
inline constexpr rgba8_t mistyrose = rgba8<255, 228, 225>;
inline constexpr rgba8_t lavenderblush = rgba8<255, 240, 245>;
inline constexpr rgba8_t indigo = rgba8<75, 0, 130>;
inline constexpr rgba8_t chocolate = rgba8<210, 105, 30>;
inline constexpr rgba8_t peru = rgba8<205, 133, 63>;
inline constexpr rgba8_t sandybrown = rgba8<244, 164, 96>;
inline constexpr rgba8_t burlywood = rgba8<222, 184, 135>;
inline constexpr rgba8_t tan = rgba8<210, 180, 140>;
inline constexpr rgba8_t rosybrown = rgba8<188, 143, 143>;
inline constexpr rgba8_t sienna = rgba8<160, 82, 45>;
inline constexpr rgba8_t saddlebrown = rgba8<139, 69, 19>;

}  // namespace colors

}  // namespace color::core
