#pragma once

#include <cstdint>
#include <type_traits>

namespace color::core {

// 基础RGB颜色模板 (C++17兼容)
template <typename T, intptr_t R_raw, intptr_t G_raw, intptr_t B_raw,
          intptr_t Scale = 1>
struct basic_rgb {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

  using value_type = T;

  // 在类内部进行转换，避开了 C++17 对模板参数类型的限制
  static constexpr T r = static_cast<T>(R_raw) / static_cast<T>(Scale);
  static constexpr T g = static_cast<T>(G_raw) / static_cast<T>(Scale);
  static constexpr T b = static_cast<T>(B_raw) / static_cast<T>(Scale);

  static constexpr bool is_valid() {
    if constexpr (std::is_floating_point_v<T>) {
      return r >= 0.0 && r <= 1.0 && g >= 0.0 && g <= 1.0 && b >= 0.0 &&
             b <= 1.0;
    } else {
      return r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255;
    }
  }

  static_assert(
      is_valid(),
      "RGB values must be in valid range (0-255 for int, 0.0-1.0 for float)");
};

// ---------------------------------------------------------
// 2. 别名定义 (完美解决你的需求)
// ---------------------------------------------------------

// 8位整型RGB：Scale 为 1，直接存 R, G, B
template <uint8_t R, uint8_t G, uint8_t B>
using rgb8 = basic_rgb<uint8_t, R, G, B, 1>;

// 浮点型RGB：Scale 为 1000，将传入的整数映射到 0.0 - 1.0
template <int R, int G, int B> using rgbf = basic_rgb<double, R, G, B, 1000>;

// 百分比RGB (0-100)
template <int R, int G, int B> using rgb_percent = basic_rgb<int, R, G, B>;

// 常用颜色常量
namespace colors {
using black = rgb8<0, 0, 0>;
using white = rgb8<255, 255, 255>;
using red = rgb8<255, 0, 0>;
using green = rgb8<0, 255, 0>;
using blue = rgb8<0, 0, 255>;
using yellow = rgb8<255, 255, 0>;
using cyan = rgb8<0, 255, 255>;
using magenta = rgb8<255, 0, 255>;
using gray = rgb8<128, 128, 128>;
using silver = rgb8<192, 192, 192>;
using maroon = rgb8<128, 0, 0>;
using olive = rgb8<128, 128, 0>;
using navy = rgb8<0, 0, 128>;
using purple = rgb8<128, 0, 128>;
using teal = rgb8<0, 128, 128>;
using lime = rgb8<0, 255, 0>;
using fuchsia = rgb8<255, 0, 255>;
using aqua = rgb8<0, 255, 255>;
} // namespace colors

} // namespace color::core