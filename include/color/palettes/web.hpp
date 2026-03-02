#pragma once

#include "../core/rgb.hpp"

namespace color::palettes::web {

namespace safe_colors {
using black = core::rgb8<0, 0, 0>;
using white = core::rgb8<255, 255, 255>;
using red = core::rgb8<255, 0, 0>;
using green = core::rgb8<0, 255, 0>;
using blue = core::rgb8<0, 0, 255>;
using yellow = core::rgb8<255, 255, 0>;
using cyan = core::rgb8<0, 255, 255>;
using magenta = core::rgb8<255, 0, 255>;
using silver = core::rgb8<192, 192, 192>;
using gray = core::rgb8<128, 128, 128>;
using maroon = core::rgb8<128, 0, 0>;
using olive = core::rgb8<128, 128, 0>;
using navy = core::rgb8<0, 0, 128>;
using purple = core::rgb8<128, 0, 128>;
using teal = core::rgb8<0, 128, 128>;
using lime = core::rgb8<0, 255, 0>;
using fuchsia = core::rgb8<255, 0, 255>;
using aqua = core::rgb8<0, 255, 255>;

// 扩展Web安全色
using dark_red = core::rgb8<128, 0, 0>;
using dark_green = core::rgb8<0, 128, 0>;
using dark_blue = core::rgb8<0, 0, 128>;
using dark_yellow = core::rgb8<128, 128, 0>;
using dark_cyan = core::rgb8<0, 128, 128>;
using dark_magenta = core::rgb8<128, 0, 128>;

using light_red = core::rgb8<255, 128, 128>;
using light_green = core::rgb8<128, 255, 128>;
using light_blue = core::rgb8<128, 128, 255>;
using light_yellow = core::rgb8<255, 255, 128>;
using light_cyan = core::rgb8<128, 255, 255>;
using light_magenta = core::rgb8<255, 128, 255>;
}  // namespace safe_colors

// 灰度调色板
namespace grayscale {
using black = core::rgb8<0, 0, 0>;
using dark_gray = core::rgb8<64, 64, 64>;
using medium_gray = core::rgb8<128, 128, 128>;
using light_gray = core::rgb8<192, 192, 192>;
using white = core::rgb8<255, 255, 255>;
}  // namespace grayscale

// 暖色调调色板
namespace warm {
using red = core::rgb8<255, 0, 0>;
using orange = core::rgb8<255, 165, 0>;
using yellow = core::rgb8<255, 255, 0>;
using pink = core::rgb8<255, 192, 203>;
using brown = core::rgb8<165, 42, 42>;
using gold = core::rgb8<255, 215, 0>;
}  // namespace warm

// 冷色调调色板
namespace cool {
using blue = core::rgb8<0, 0, 255>;
using cyan = core::rgb8<0, 255, 255>;
using green = core::rgb8<0, 255, 0>;
using purple = core::rgb8<128, 0, 128>;
using navy = core::rgb8<0, 0, 128>;
using teal = core::rgb8<0, 128, 128>;
}  // namespace cool

}  // namespace color::palettes::web
