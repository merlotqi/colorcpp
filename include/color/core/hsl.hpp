#pragma once

#include <cstdint>
#include <type_traits>

namespace color::core {

// 基础HSL颜色模板 (C++17兼容)
template <typename T, intptr_t H_raw, intptr_t S_raw, intptr_t L_raw, intptr_t Scale = 1>
struct basic_hsl {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

  using value_type = T;

  // 在类内部进行转换，避开了 C++17 对模板参数类型的限制
  static constexpr T h = static_cast<T>(H_raw) / static_cast<T>(Scale);
  static constexpr T s = static_cast<T>(S_raw) / static_cast<T>(Scale);
  static constexpr T l = static_cast<T>(L_raw) / static_cast<T>(Scale);

  static constexpr bool is_valid() {
    if constexpr (std::is_floating_point_v<T>) {
      return h >= 0.0 && h < 360.0 && s >= 0.0 && s <= 1.0 && l >= 0.0 && l <= 1.0;
    } else {
      return h >= 0 && h < 360 && s >= 0 && s <= 100 && l >= 0 && l <= 100;
    }
  }

  static_assert(is_valid(),
                "HSL values must be in valid range (0-360 for H, 0-100 for S/L for int, 0.0-1.0 for float)");
};

// ---------------------------------------------------------
// 2. 别名定义 (完美解决你的需求)
// ---------------------------------------------------------

// 整型HSL (H: 0-359, S: 0-100, L: 0-100)
template <int H, int S, int L>
using hsl_int = basic_hsl<int, H, S, L, 1>;

// 浮点型HSL (H: 0-360, S: 0-1, L: 0-1)
template <int H, int S, int L>
using hsl_float = basic_hsl<double, H, S, L, 1000>;

// 常用HSL颜色常量
namespace colors {
using red_hsl = hsl_int<0, 100, 50>;
using green_hsl = hsl_int<120, 100, 50>;
using blue_hsl = hsl_int<240, 100, 50>;
using yellow_hsl = hsl_int<60, 100, 50>;
using cyan_hsl = hsl_int<180, 100, 50>;
using magenta_hsl = hsl_int<300, 100, 50>;
using black_hsl = hsl_int<0, 0, 0>;
using white_hsl = hsl_int<0, 0, 100>;
using gray_hsl = hsl_int<0, 0, 50>;
}  // namespace colors

}  // namespace color::core
