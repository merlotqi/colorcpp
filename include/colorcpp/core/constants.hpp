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

#include <colorcpp/core/rgb.hpp>
#include <cstdint>


/**
 * @brief Named @ref colorcpp::core::rgba8_t constants (full opacity) for common and W3C color names.
 */
namespace colorcpp::constants {

namespace detail {

// Compile-time 0xRRGGBB → rgba8_t at full opacity. Lives in core so the
// constants layer does not depend on the io layer's literal operators.
constexpr core::rgba8_t rgb8(std::uint32_t value) noexcept {
  return core::rgba8_t{static_cast<std::uint8_t>((value >> 16) & 0xFF), static_cast<std::uint8_t>((value >> 8) & 0xFF),
                       static_cast<std::uint8_t>(value & 0xFF), 255};
}

}  // namespace detail

// Basic RGB colors
inline constexpr auto black = detail::rgb8(0x000000);
inline constexpr auto white = detail::rgb8(0xFFFFFF);
inline constexpr auto red = detail::rgb8(0xFF0000);
inline constexpr auto lime = detail::rgb8(0x00FF00);  // Bright green
inline constexpr auto blue = detail::rgb8(0x0000FF);
inline constexpr auto yellow = detail::rgb8(0xFFFF00);
inline constexpr auto cyan = detail::rgb8(0x00FFFF);
inline constexpr auto magenta = detail::rgb8(0xFF00FF);
inline constexpr auto aqua = detail::rgb8(0x00FFFF);     // Same as cyan
inline constexpr auto fuchsia = detail::rgb8(0xFF00FF);  // Same as magenta

// W3C standard colors
inline constexpr auto maroon = detail::rgb8(0x800000);
inline constexpr auto darkred = detail::rgb8(0x8B0000);
inline constexpr auto brown = detail::rgb8(0xA52A2A);
inline constexpr auto firebrick = detail::rgb8(0xB22222);
inline constexpr auto indianred = detail::rgb8(0xCD5C5C);
inline constexpr auto lightcoral = detail::rgb8(0xF08080);
inline constexpr auto salmon = detail::rgb8(0xFA8072);
inline constexpr auto darksalmon = detail::rgb8(0xE9967A);
inline constexpr auto lightsalmon = detail::rgb8(0xFFA07A);
inline constexpr auto orange = detail::rgb8(0xFFA500);
inline constexpr auto darkorange = detail::rgb8(0xFF8C00);
inline constexpr auto coral = detail::rgb8(0xFF7F50);
inline constexpr auto tomato = detail::rgb8(0xFF6347);
inline constexpr auto orangered = detail::rgb8(0xFF4500);
inline constexpr auto gold = detail::rgb8(0xFFD700);
inline constexpr auto goldenrod = detail::rgb8(0xDAA520);
inline constexpr auto darkgoldenrod = detail::rgb8(0xB8860B);
inline constexpr auto lightgoldenrodyellow = detail::rgb8(0xFAFAD2);
inline constexpr auto palegoldenrod = detail::rgb8(0xEEE8AA);
inline constexpr auto khaki = detail::rgb8(0xF0E68C);
inline constexpr auto darkkhaki = detail::rgb8(0xBDB76B);
inline constexpr auto olive = detail::rgb8(0x808000);
inline constexpr auto olivedrab = detail::rgb8(0x6B8E23);
inline constexpr auto yellowgreen = detail::rgb8(0x9ACD32);
inline constexpr auto darkolivegreen = detail::rgb8(0x556B2F);
inline constexpr auto green = detail::rgb8(0x008000);  // W3C standard green (dark)
inline constexpr auto greenyellow = detail::rgb8(0xADFF2F);
inline constexpr auto chartreuse = detail::rgb8(0x7FFF00);
inline constexpr auto lawngreen = detail::rgb8(0x7CFC00);
inline constexpr auto springgreen = detail::rgb8(0x00FF7F);
inline constexpr auto mediumspringgreen = detail::rgb8(0x00FA9A);
inline constexpr auto lightgreen = detail::rgb8(0x90EE90);
inline constexpr auto palegreen = detail::rgb8(0x98FB98);
inline constexpr auto darkseagreen = detail::rgb8(0x8FBC8F);
inline constexpr auto mediumseagreen = detail::rgb8(0x3CB371);
inline constexpr auto seagreen = detail::rgb8(0x2E8B57);
inline constexpr auto forestgreen = detail::rgb8(0x228B22);
inline constexpr auto darkgreen = detail::rgb8(0x006400);
inline constexpr auto teal = detail::rgb8(0x008080);
inline constexpr auto darkcyan = detail::rgb8(0x008B8B);
inline constexpr auto lightcyan = detail::rgb8(0xE0FFFF);
inline constexpr auto paleturquoise = detail::rgb8(0xAFEEEE);
inline constexpr auto aquamarine = detail::rgb8(0x7FFFD4);
inline constexpr auto turquoise = detail::rgb8(0x40E0D0);
inline constexpr auto mediumturquoise = detail::rgb8(0x48D1CC);
inline constexpr auto darkturquoise = detail::rgb8(0x00CED1);
inline constexpr auto lightseagreen = detail::rgb8(0x20B2AA);
inline constexpr auto cadetblue = detail::rgb8(0x5F9EA0);
inline constexpr auto navy = detail::rgb8(0x000080);
inline constexpr auto darkblue = detail::rgb8(0x00008B);
inline constexpr auto mediumblue = detail::rgb8(0x0000CD);
inline constexpr auto royalblue = detail::rgb8(0x4169E1);
inline constexpr auto steelblue = detail::rgb8(0x4682B4);
inline constexpr auto dodgerblue = detail::rgb8(0x1E90FF);
inline constexpr auto deepskyblue = detail::rgb8(0x00BFFF);
inline constexpr auto skyblue = detail::rgb8(0x87CEEB);
inline constexpr auto lightskyblue = detail::rgb8(0x87CEFA);
inline constexpr auto lightsteelblue = detail::rgb8(0xB0C4DE);
inline constexpr auto lightblue = detail::rgb8(0xADD8E6);
inline constexpr auto powderblue = detail::rgb8(0xB0E0E6);
inline constexpr auto cornflowerblue = detail::rgb8(0x6495ED);
inline constexpr auto midnightblue = detail::rgb8(0x191970);
inline constexpr auto purple = detail::rgb8(0x800080);
inline constexpr auto darkmagenta = detail::rgb8(0x8B008B);
inline constexpr auto darkviolet = detail::rgb8(0x9400D3);
inline constexpr auto darkorchid = detail::rgb8(0x9932CC);
inline constexpr auto blueviolet = detail::rgb8(0x8A2BE2);
inline constexpr auto mediumpurple = detail::rgb8(0x9370DB);
inline constexpr auto mediumslateblue = detail::rgb8(0x7B68EE);
inline constexpr auto slateblue = detail::rgb8(0x6A5ACD);
inline constexpr auto darkslateblue = detail::rgb8(0x483D8B);
inline constexpr auto lavender = detail::rgb8(0xE6E6FA);
inline constexpr auto thistle = detail::rgb8(0xD8BFD8);
inline constexpr auto plum = detail::rgb8(0xDDA0DD);
inline constexpr auto orchid = detail::rgb8(0xDA70D6);
inline constexpr auto violet = detail::rgb8(0xEE82EE);
inline constexpr auto mediumorchid = detail::rgb8(0xBA55D3);
inline constexpr auto pink = detail::rgb8(0xFFC0CB);
inline constexpr auto lightpink = detail::rgb8(0xFFB6C1);
inline constexpr auto hotpink = detail::rgb8(0xFF69B4);
inline constexpr auto deeppink = detail::rgb8(0xFF1493);
inline constexpr auto palevioletred = detail::rgb8(0xDB7093);
inline constexpr auto mediumvioletred = detail::rgb8(0xC71585);
inline constexpr auto crimson = detail::rgb8(0xDC143C);

// Gray series
inline constexpr auto gray = detail::rgb8(0x808080);
inline constexpr auto grey = detail::rgb8(0x808080);  // Same as gray
inline constexpr auto darkgray = detail::rgb8(0xA9A9A9);
inline constexpr auto darkgrey = detail::rgb8(0xA9A9A9);
inline constexpr auto silver = detail::rgb8(0xC0C0C0);
inline constexpr auto lightgray = detail::rgb8(0xD3D3D3);
inline constexpr auto lightgrey = detail::rgb8(0xD3D3D3);
inline constexpr auto gainsboro = detail::rgb8(0xDCDCDC);
inline constexpr auto whitesmoke = detail::rgb8(0xF5F5F5);
inline constexpr auto dimgray = detail::rgb8(0x696969);
inline constexpr auto dimgrey = detail::rgb8(0x696969);
inline constexpr auto slategray = detail::rgb8(0x708090);
inline constexpr auto slategrey = detail::rgb8(0x708090);
inline constexpr auto lightslategray = detail::rgb8(0x778899);
inline constexpr auto lightslategrey = detail::rgb8(0x778899);

// Additional named colors
inline constexpr auto ivory = detail::rgb8(0xFFFFF0);
inline constexpr auto beige = detail::rgb8(0xF5F5DC);
inline constexpr auto wheat = detail::rgb8(0xF5DEB3);
inline constexpr auto bisque = detail::rgb8(0xFFE4C4);
inline constexpr auto blanchedalmond = detail::rgb8(0xFFEBCD);
inline constexpr auto cornsilk = detail::rgb8(0xFFF8DC);
inline constexpr auto lemonchiffon = detail::rgb8(0xFFFACD);
inline constexpr auto lightyellow = detail::rgb8(0xFFFFE0);
inline constexpr auto honeydew = detail::rgb8(0xF0FFF0);
inline constexpr auto mintcream = detail::rgb8(0xF5FFFA);
inline constexpr auto azure = detail::rgb8(0xF0FFFF);
inline constexpr auto aliceblue = detail::rgb8(0xF0F8FF);
inline constexpr auto ghostwhite = detail::rgb8(0xF8F8FF);
inline constexpr auto seashell = detail::rgb8(0xFFF5EE);
inline constexpr auto snow = detail::rgb8(0xFFFAFA);
inline constexpr auto floralwhite = detail::rgb8(0xFFFAF0);
inline constexpr auto linen = detail::rgb8(0xFAF0E6);
inline constexpr auto oldlace = detail::rgb8(0xFDF5E6);
inline constexpr auto antiquewhite = detail::rgb8(0xFAEBD7);
inline constexpr auto papayawhip = detail::rgb8(0xFFEFD5);
inline constexpr auto peachpuff = detail::rgb8(0xFFDAB9);
inline constexpr auto moccasin = detail::rgb8(0xFFE4B5);
inline constexpr auto navajowhite = detail::rgb8(0xFFDEAD);
inline constexpr auto mistyrose = detail::rgb8(0xFFE4E1);
inline constexpr auto lavenderblush = detail::rgb8(0xFFF0F5);
inline constexpr auto indigo = detail::rgb8(0x4B0082);
inline constexpr auto chocolate = detail::rgb8(0xD2691E);
inline constexpr auto peru = detail::rgb8(0xCD853F);
inline constexpr auto sandybrown = detail::rgb8(0xF4A460);
inline constexpr auto burlywood = detail::rgb8(0xDEB887);
inline constexpr auto tan = detail::rgb8(0xD2B48C);
inline constexpr auto rosybrown = detail::rgb8(0xBC8F8F);
inline constexpr auto sienna = detail::rgb8(0xA0522D);
inline constexpr auto saddlebrown = detail::rgb8(0x8B4513);

}  // namespace colorcpp::constants
