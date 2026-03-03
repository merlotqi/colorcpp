/**
 * @file constants.hpp
 * @brief Predefined color constants for common color names
 *
 * Provides a comprehensive collection of predefined color constants using
 * standard color names and their corresponding RGB values. All colors are
 * defined using the rgba8_t type with full opacity (alpha=255).
 *
 * The color constants are organized into logical groups for easy access
 * and include both basic colors and extended color palettes.
 *
 * @author Merlot.Qi
 */

#pragma once

#include <color/core/rgb.hpp>
#include <color/literals/literals.hpp>

namespace color::constants {

using namespace color::literals;

// Basic RGB colors
inline constexpr auto black = 0x000000_rgb;
inline constexpr auto white = 0xFFFFFF_rgb;
inline constexpr auto red = 0xFF0000_rgb;
inline constexpr auto lime = 0x00FF00_rgb;  // Bright green
inline constexpr auto blue = 0x0000FF_rgb;
inline constexpr auto yellow = 0xFFFF00_rgb;
inline constexpr auto cyan = 0x00FFFF_rgb;
inline constexpr auto magenta = 0xFF00FF_rgb;
inline constexpr auto aqua = 0x00FFFF_rgb;     // Same as cyan
inline constexpr auto fuchsia = 0xFF00FF_rgb;  // Same as magenta

// W3C standard colors
inline constexpr auto maroon = 0x800000_rgb;
inline constexpr auto darkred = 0x8B0000_rgb;
inline constexpr auto brown = 0xA52A2A_rgb;
inline constexpr auto firebrick = 0xB22222_rgb;
inline constexpr auto indianred = 0xCD5C5C_rgb;
inline constexpr auto lightcoral = 0xF08080_rgb;
inline constexpr auto salmon = 0xFA8072_rgb;
inline constexpr auto darksalmon = 0xE9967A_rgb;
inline constexpr auto lightsalmon = 0xFFA07A_rgb;
inline constexpr auto orange = 0xFFA500_rgb;
inline constexpr auto darkorange = 0xFF8C00_rgb;
inline constexpr auto coral = 0xFF7F50_rgb;
inline constexpr auto tomato = 0xFF6347_rgb;
inline constexpr auto orangered = 0xFF4500_rgb;
inline constexpr auto gold = 0xFFD700_rgb;
inline constexpr auto goldenrod = 0xDAA520_rgb;
inline constexpr auto darkgoldenrod = 0xB8860B_rgb;
inline constexpr auto lightgoldenrodyellow = 0xFAFAD2_rgb;
inline constexpr auto palegoldenrod = 0xEEE8AA_rgb;
inline constexpr auto khaki = 0xF0E68C_rgb;
inline constexpr auto darkkhaki = 0xBDB76B_rgb;
inline constexpr auto olive = 0x808000_rgb;
inline constexpr auto olivedrab = 0x6B8E23_rgb;
inline constexpr auto yellowgreen = 0x9ACD32_rgb;
inline constexpr auto darkolivegreen = 0x556B2F_rgb;
inline constexpr auto green = 0x008000_rgb;  // W3C standard green (dark)
inline constexpr auto greenyellow = 0xADFF2F_rgb;
inline constexpr auto chartreuse = 0x7FFF00_rgb;
inline constexpr auto lawngreen = 0x7CFC00_rgb;
inline constexpr auto springgreen = 0x00FF7F_rgb;
inline constexpr auto mediumspringgreen = 0x00FA9A_rgb;
inline constexpr auto lightgreen = 0x90EE90_rgb;
inline constexpr auto palegreen = 0x98FB98_rgb;
inline constexpr auto darkseagreen = 0x8FBC8F_rgb;
inline constexpr auto mediumseagreen = 0x3CB371_rgb;
inline constexpr auto seagreen = 0x2E8B57_rgb;
inline constexpr auto forestgreen = 0x228B22_rgb;
inline constexpr auto darkgreen = 0x006400_rgb;
inline constexpr auto teal = 0x008080_rgb;
inline constexpr auto darkcyan = 0x008B8B_rgb;
inline constexpr auto lightcyan = 0xE0FFFF_rgb;
inline constexpr auto paleturquoise = 0xAFEEEE_rgb;
inline constexpr auto aquamarine = 0x7FFFD4_rgb;
inline constexpr auto turquoise = 0x40E0D0_rgb;
inline constexpr auto mediumturquoise = 0x48D1CC_rgb;
inline constexpr auto darkturquoise = 0x00CED1_rgb;
inline constexpr auto lightseagreen = 0x20B2AA_rgb;
inline constexpr auto cadetblue = 0x5F9EA0_rgb;
inline constexpr auto navy = 0x000080_rgb;
inline constexpr auto darkblue = 0x00008B_rgb;
inline constexpr auto mediumblue = 0x0000CD_rgb;
inline constexpr auto royalblue = 0x4169E1_rgb;
inline constexpr auto steelblue = 0x4682B4_rgb;
inline constexpr auto dodgerblue = 0x1E90FF_rgb;
inline constexpr auto deepskyblue = 0x00BFFF_rgb;
inline constexpr auto skyblue = 0x87CEEB_rgb;
inline constexpr auto lightskyblue = 0x87CEFA_rgb;
inline constexpr auto lightsteelblue = 0xB0C4DE_rgb;
inline constexpr auto lightblue = 0xADD8E6_rgb;
inline constexpr auto powderblue = 0xB0E0E6_rgb;
inline constexpr auto cornflowerblue = 0x6495ED_rgb;
inline constexpr auto midnightblue = 0x191970_rgb;
inline constexpr auto purple = 0x800080_rgb;
inline constexpr auto darkmagenta = 0x8B008B_rgb;
inline constexpr auto darkviolet = 0x9400D3_rgb;
inline constexpr auto darkorchid = 0x9932CC_rgb;
inline constexpr auto blueviolet = 0x8A2BE2_rgb;
inline constexpr auto mediumpurple = 0x9370DB_rgb;
inline constexpr auto mediumslateblue = 0x7B68EE_rgb;
inline constexpr auto slateblue = 0x6A5ACD_rgb;
inline constexpr auto darkslateblue = 0x483D8B_rgb;
inline constexpr auto lavender = 0xE6E6FA_rgb;
inline constexpr auto thistle = 0xD8BFD8_rgb;
inline constexpr auto plum = 0xDDA0DD_rgb;
inline constexpr auto orchid = 0xDA70D6_rgb;
inline constexpr auto violet = 0xEE82EE_rgb;
inline constexpr auto mediumorchid = 0xBA55D3_rgb;
inline constexpr auto pink = 0xFFC0CB_rgb;
inline constexpr auto lightpink = 0xFFB6C1_rgb;
inline constexpr auto hotpink = 0xFF69B4_rgb;
inline constexpr auto deeppink = 0xFF1493_rgb;
inline constexpr auto palevioletred = 0xDB7093_rgb;
inline constexpr auto mediumvioletred = 0xC71585_rgb;
inline constexpr auto crimson = 0xDC143C_rgb;

// Gray series
inline constexpr auto gray = 0x808080_rgb;
inline constexpr auto grey = 0x808080_rgb;  // Same as gray
inline constexpr auto darkgray = 0xA9A9A9_rgb;
inline constexpr auto darkgrey = 0xA9A9A9_rgb;
inline constexpr auto silver = 0xC0C0C0_rgb;
inline constexpr auto lightgray = 0xD3D3D3_rgb;
inline constexpr auto lightgrey = 0xD3D3D3_rgb;
inline constexpr auto gainsboro = 0xDCDCDC_rgb;
inline constexpr auto whitesmoke = 0xF5F5F5_rgb;
inline constexpr auto dimgray = 0x696969_rgb;
inline constexpr auto dimgrey = 0x696969_rgb;
inline constexpr auto slategray = 0x708090_rgb;
inline constexpr auto slategrey = 0x708090_rgb;
inline constexpr auto lightslategray = 0x778899_rgb;
inline constexpr auto lightslategrey = 0x778899_rgb;

// Additional named colors
inline constexpr auto ivory = 0xFFFFF0_rgb;
inline constexpr auto beige = 0xF5F5DC_rgb;
inline constexpr auto wheat = 0xF5DEB3_rgb;
inline constexpr auto bisque = 0xFFE4C4_rgb;
inline constexpr auto blanchedalmond = 0xFFEBCD_rgb;
inline constexpr auto cornsilk = 0xFFF8DC_rgb;
inline constexpr auto lemonchiffon = 0xFFFACD_rgb;
inline constexpr auto lightyellow = 0xFFFFE0_rgb;
inline constexpr auto honeydew = 0xF0FFF0_rgb;
inline constexpr auto mintcream = 0xF5FFFA_rgb;
inline constexpr auto azure = 0xF0FFFF_rgb;
inline constexpr auto aliceblue = 0xF0F8FF_rgb;
inline constexpr auto ghostwhite = 0xF8F8FF_rgb;
inline constexpr auto seashell = 0xFFF5EE_rgb;
inline constexpr auto snow = 0xFFFAFA_rgb;
inline constexpr auto floralwhite = 0xFFFAF0_rgb;
inline constexpr auto linen = 0xFAF0E6_rgb;
inline constexpr auto oldlace = 0xFDF5E6_rgb;
inline constexpr auto antiquewhite = 0xFAEBD7_rgb;
inline constexpr auto papayawhip = 0xFFEFD5_rgb;
inline constexpr auto peachpuff = 0xFFDAB9_rgb;
inline constexpr auto moccasin = 0xFFE4B5_rgb;
inline constexpr auto navajowhite = 0xFFDEAD_rgb;
inline constexpr auto mistyrose = 0xFFE4E1_rgb;
inline constexpr auto lavenderblush = 0xFFF0F5_rgb;
inline constexpr auto indigo = 0x4B0082_rgb;
inline constexpr auto chocolate = 0xD2691E_rgb;
inline constexpr auto peru = 0xCD853F_rgb;
inline constexpr auto sandybrown = 0xF4A460_rgb;
inline constexpr auto burlywood = 0xDEB887_rgb;
inline constexpr auto tan = 0xD2B48C_rgb;
inline constexpr auto rosybrown = 0xBC8F8F_rgb;
inline constexpr auto sienna = 0xA0522D_rgb;
inline constexpr auto saddlebrown = 0x8B4513_rgb;

}  // namespace color::constants
