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

#include <color/core/rgb.hpp>

namespace color::palettes::web {

namespace safe_colors {
inline constexpr auto black = core::rgba8<0, 0, 0>;
inline constexpr auto white = core::rgba8<255, 255, 255>;
inline constexpr auto red = core::rgba8<255, 0, 0>;
inline constexpr auto green = core::rgba8<0, 255, 0>;
inline constexpr auto blue = core::rgba8<0, 0, 255>;
inline constexpr auto yellow = core::rgba8<255, 255, 0>;
inline constexpr auto cyan = core::rgba8<0, 255, 255>;
inline constexpr auto magenta = core::rgba8<255, 0, 255>;
inline constexpr auto silver = core::rgba8<192, 192, 192>;
inline constexpr auto gray = core::rgba8<128, 128, 128>;
inline constexpr auto maroon = core::rgba8<128, 0, 0>;
inline constexpr auto olive = core::rgba8<128, 128, 0>;
inline constexpr auto navy = core::rgba8<0, 0, 128>;
inline constexpr auto purple = core::rgba8<128, 0, 128>;
inline constexpr auto teal = core::rgba8<0, 128, 128>;
inline constexpr auto lime = core::rgba8<0, 255, 0>;
inline constexpr auto fuchsia = core::rgba8<255, 0, 255>;
inline constexpr auto aqua = core::rgba8<0, 255, 255>;

// Extended Web safe colors
inline constexpr auto dark_red = core::rgba8<128, 0, 0>;
inline constexpr auto dark_green = core::rgba8<0, 128, 0>;
inline constexpr auto dark_blue = core::rgba8<0, 0, 128>;
inline constexpr auto dark_yellow = core::rgba8<128, 128, 0>;
inline constexpr auto dark_cyan = core::rgba8<0, 128, 128>;
inline constexpr auto dark_magenta = core::rgba8<128, 0, 128>;

inline constexpr auto light_red = core::rgba8<255, 128, 128>;
inline constexpr auto light_green = core::rgba8<128, 255, 128>;
inline constexpr auto light_blue = core::rgba8<128, 128, 255>;
inline constexpr auto light_yellow = core::rgba8<255, 255, 128>;
inline constexpr auto light_cyan = core::rgba8<128, 255, 255>;
inline constexpr auto light_magenta = core::rgba8<255, 128, 255>;
}  // namespace safe_colors

// Grayscale palette
namespace grayscale {
inline constexpr auto black = core::rgba8<0, 0, 0>;
inline constexpr auto dark_gray = core::rgba8<64, 64, 64>;
inline constexpr auto medium_gray = core::rgba8<128, 128, 128>;
inline constexpr auto light_gray = core::rgba8<192, 192, 192>;
inline constexpr auto white = core::rgba8<255, 255, 255>;
}  // namespace grayscale

// Warm color palette
namespace warm {
inline constexpr auto red = core::rgba8<255, 0, 0>;
inline constexpr auto orange = core::rgba8<255, 165, 0>;
inline constexpr auto yellow = core::rgba8<255, 255, 0>;
inline constexpr auto pink = core::rgba8<255, 192, 203>;
inline constexpr auto brown = core::rgba8<165, 42, 42>;
inline constexpr auto gold = core::rgba8<255, 215, 0>;
}  // namespace warm

// Cool color palette
namespace cool {
inline constexpr auto blue = core::rgba8<0, 0, 255>;
inline constexpr auto cyan = core::rgba8<0, 255, 255>;
inline constexpr auto green = core::rgba8<0, 255, 0>;
inline constexpr auto purple = core::rgba8<128, 0, 128>;
inline constexpr auto navy = core::rgba8<0, 0, 128>;
inline constexpr auto teal = core::rgba8<0, 128, 128>;
}  // namespace cool

}  // namespace color::palettes::web
