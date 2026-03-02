#pragma once

#include <type_traits>
#include <cstdint>

namespace color::core {

// 基础HSV颜色模板 (C++17兼容)
template <typename T, intptr_t H_raw, intptr_t S_raw, intptr_t V_raw,
          intptr_t Scale = 1>
struct basic_hsv {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

  using value_type = T;

  // 在类内部进行转换，避开了 C++17 对模板参数类型的限制
  static constexpr T h = static_cast<T>(H_raw) / static_cast<T>(Scale);
  static constexpr T s = static_cast<T>(S_raw) / static_cast<T>(Scale);
  static constexpr T v = static_cast<T>(V_raw) / static_cast<T>(Scale);

  static constexpr bool is_valid() {
    if constexpr (std::is_floating_point_v<T>) {
      return h >= 0.0 && h < 360.0 && s >= 0.0 && s <= 1.0 && v >= 0.0 &&
             v <= 1.0;
    } else {
      return h >= 0 && h < 360 && s >= 0 && s <= 100 && v >= 0 && v <= 100;
    }
  }

  static_assert(
      is_valid(),
      "HSV values must be in valid range (0-360 for H, 0-100 for S/V for int, 0.0-1.0 for float)");
};

// ---------------------------------------------------------
// 2. 别名定义 (完美解决你的需求)
// ---------------------------------------------------------

// 整型HSV (H: 0-359, S: 0-100, V: 0-100)
template <int H, int S, int V>
using hsv_int = basic_hsv<int, H, S, V, 1>;

// 浮点型HSV (H: 0-360, S: 0-1, V: 0-1)
template <int H, int S, int V>
using hsv_float = basic_hsv<double, H, S, V, 1000>;

// 常用HSV颜色常量
namespace colors {
    using red_hsv = hsv_int<0, 100, 100>;
    using green_hsv = hsv_int<120, 100, 100>;
    using blue_hsv = hsv_int<240, 100, 100>;
    using yellow_hsv = hsv_int<60, 100, 100>;
    using cyan_hsv = hsv_int<180, 100, 100>;
    using magenta_hsv = hsv_int<300, 100, 100>;
    using black_hsv = hsv_int<0, 0, 0>;
    using white_hsv = hsv_int<0, 0, 100>;
    using gray_hsv = hsv_int<0, 0, 50>;
}

} // namespace color::core
