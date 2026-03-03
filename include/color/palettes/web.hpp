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
inline constexpr auto black = core::rgb8<0, 0, 0>;
inline constexpr auto white = core::rgb8<255, 255, 255>;
inline constexpr auto red = core::rgb8<255, 0, 0>;
inline constexpr auto green = core::rgb8<0, 255, 0>;
inline constexpr auto blue = core::rgb8<0, 0, 255>;
inline constexpr auto yellow = core::rgb8<255, 255, 0>;
inline constexpr auto cyan = core::rgb8<0, 255, 255>;
inline constexpr auto magenta = core::rgb8<255, 0, 255>;
inline constexpr auto silver = core::rgb8<192, 192, 192>;
inline constexpr auto gray = core::rgb8<128, 128, 128>;
inline constexpr auto maroon = core::rgb8<128, 0, 0>;
inline constexpr auto olive = core::rgb8<128, 128, 0>;
inline constexpr auto navy = core::rgb8<0, 0, 128>;
inline constexpr auto purple = core::rgb8<128, 0, 128>;
inline constexpr auto teal = core::rgb8<0, 128, 128>;
inline constexpr auto lime = core::rgb8<0, 255, 0>;
inline constexpr auto fuchsia = core::rgb8<255, 0, 255>;
inline constexpr auto aqua = core::rgb8<0, 255, 255>;

// Extended Web safe colors
inline constexpr auto dark_red = core::rgb8<128, 0, 0>;
inline constexpr auto dark_green = core::rgb8<0, 128, 0>;
inline constexpr auto dark_blue = core::rgb8<0, 0, 128>;
inline constexpr auto dark_yellow = core::rgb8<128, 128, 0>;
inline constexpr auto dark_cyan = core::rgb8<0, 128, 128>;
inline constexpr auto dark_magenta = core::rgb8<128, 0, 128>;

inline constexpr auto light_red = core::rgb8<255, 128, 128>;
inline constexpr auto light_green = core::rgb8<128, 255, 128>;
inline constexpr auto light_blue = core::rgb8<128, 128, 255>;
inline constexpr auto light_yellow = core::rgb8<255, 255, 128>;
inline constexpr auto light_cyan = core::rgb8<128, 255, 255>;
inline constexpr auto light_magenta = core::rgb8<255, 128, 255>;
}  // namespace safe_colors

// Grayscale palette
namespace grayscale {
inline constexpr auto black = core::rgb8<0, 0, 0>;
inline constexpr auto dark_gray = core::rgb8<64, 64, 64>;
inline constexpr auto medium_gray = core::rgb8<128, 128, 128>;
inline constexpr auto light_gray = core::rgb8<192, 192, 192>;
inline constexpr auto white = core::rgb8<255, 255, 255>;
}  // namespace grayscale

// Warm color palette
namespace warm {
inline constexpr auto red = core::rgb8<255, 0, 0>;
inline constexpr auto orange = core::rgb8<255, 165, 0>;
inline constexpr auto yellow = core::rgb8<255, 255, 0>;
inline constexpr auto pink = core::rgb8<255, 192, 203>;
inline constexpr auto brown = core::rgb8<165, 42, 42>;
inline constexpr auto gold = core::rgb8<255, 215, 0>;
}  // namespace warm

// Cool color palette
namespace cool {
inline constexpr auto blue = core::rgb8<0, 0, 255>;
inline constexpr auto cyan = core::rgb8<0, 255, 255>;
inline constexpr auto green = core::rgb8<0, 255, 0>;
inline constexpr auto purple = core::rgb8<128, 0, 128>;
inline constexpr auto navy = core::rgb8<0, 0, 128>;
inline constexpr auto teal = core::rgb8<0, 128, 128>;
}  // namespace cool

}  // namespace color::palettes::web
