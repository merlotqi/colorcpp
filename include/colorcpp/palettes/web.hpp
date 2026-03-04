/**
 * @file web.hpp
 * @brief Web color palettes
 *
 * Provides predefined color palettes commonly used in web development,
 * including web-safe colors, grayscale variations, and warm/cool color schemes.
 * All colors are defined as compile-time RGB values for optimal performance.
 *
 * @author Merlot.Qi
 *
 */

#pragma once

#include <colorcpp/core/rgb.hpp>
#include <colorcpp/literals/literals.hpp>

namespace color::palettes::web {

using namespace color::literals;

namespace safe_colors {
inline constexpr auto black = 0x000000_rgb;
inline constexpr auto white = 0xFFFFFF_rgb;
inline constexpr auto red = 0xFF0000_rgb;
inline constexpr auto green = 0x00FF00_rgb;
inline constexpr auto blue = 0x0000FF_rgb;
inline constexpr auto yellow = 0xFFFF00_rgb;
inline constexpr auto cyan = 0x00FFFF_rgb;
inline constexpr auto magenta = 0xFF00FF_rgb;
inline constexpr auto silver = 0xC0C0C0_rgb;
inline constexpr auto gray = 0x808080_rgb;
inline constexpr auto maroon = 0x800000_rgb;
inline constexpr auto olive = 0x808000_rgb;
inline constexpr auto navy = 0x000080_rgb;
inline constexpr auto purple = 0x800080_rgb;
inline constexpr auto teal = 0x008080_rgb;
inline constexpr auto lime = 0x00FF00_rgb;     // Same as green
inline constexpr auto fuchsia = 0xFF00FF_rgb;  // Same as magenta
inline constexpr auto aqua = 0x00FFFF_rgb;     // Same as cyan

// Extended Web safe colors
inline constexpr auto dark_red = 0x800000_rgb;  // Same as maroon
inline constexpr auto dark_green = 0x008000_rgb;
inline constexpr auto dark_blue = 0x000080_rgb;     // Same as navy
inline constexpr auto dark_yellow = 0x808000_rgb;   // Same as olive
inline constexpr auto dark_cyan = 0x008080_rgb;     // Same as teal
inline constexpr auto dark_magenta = 0x800080_rgb;  // Same as purple

inline constexpr auto light_red = 0xFF8080_rgb;
inline constexpr auto light_green = 0x80FF80_rgb;
inline constexpr auto light_blue = 0x8080FF_rgb;
inline constexpr auto light_yellow = 0xFFFF80_rgb;
inline constexpr auto light_cyan = 0x80FFFF_rgb;
inline constexpr auto light_magenta = 0xFF80FF_rgb;
}  // namespace safe_colors

// Grayscale palette
namespace grayscale {
inline constexpr auto black = 0x000000_rgb;
inline constexpr auto dark_gray = 0x404040_rgb;
inline constexpr auto medium_gray = 0x808080_rgb;
inline constexpr auto light_gray = 0xC0C0C0_rgb;
inline constexpr auto white = 0xFFFFFF_rgb;
}  // namespace grayscale

// Warm color palette
namespace warm {
inline constexpr auto red = 0xFF0000_rgb;
inline constexpr auto orange = 0xFFA500_rgb;
inline constexpr auto yellow = 0xFFFF00_rgb;
inline constexpr auto pink = 0xFFC0CB_rgb;
inline constexpr auto brown = 0xA52A2A_rgb;
inline constexpr auto gold = 0xFFD700_rgb;
}  // namespace warm

// Cool color palette
namespace cool {
inline constexpr auto blue = 0x0000FF_rgb;
inline constexpr auto cyan = 0x00FFFF_rgb;
inline constexpr auto green = 0x00FF00_rgb;
inline constexpr auto purple = 0x800080_rgb;
inline constexpr auto navy = 0x000080_rgb;
inline constexpr auto teal = 0x008080_rgb;
}  // namespace cool

}  // namespace color::palettes::web
