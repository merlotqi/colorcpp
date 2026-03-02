#pragma once

#include <type_traits>
#include <cstdint>

namespace color::core {

// 基础CMYK颜色模板 (C++17兼容)
template <typename T, intptr_t C_raw, intptr_t M_raw, intptr_t Y_raw,
          intptr_t K_raw, intptr_t Scale = 1>
struct basic_cmyk {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

  using value_type = T;

  // 在类内部进行转换，避开了 C++17 对模板参数类型的限制
  static constexpr T c = static_cast<T>(C_raw) / static_cast<T>(Scale);
  static constexpr T m = static_cast<T>(M_raw) / static_cast<T>(Scale);
  static constexpr T y = static_cast<T>(Y_raw) / static_cast<T>(Scale);
  static constexpr T k = static_cast<T>(K_raw) / static_cast<T>(Scale);

  static constexpr bool is_valid() {
    if constexpr (std::is_floating_point_v<T>) {
      return c >= 0.0 && c <= 1.0 && m >= 0.0 && m <= 1.0 && 
             y >= 0.0 && y <= 1.0 && k >= 0.0 && k <= 1.0;
    } else {
      return c >= 0 && c <= 100 && m >= 0 && m <= 100 && 
             y >= 0 && y <= 100 && k >= 0 && k <= 100;
    }
  }

  static_assert(
      is_valid(),
      "CMYK values must be in valid range (0-100 for int, 0.0-1.0 for float)");
};

// ---------------------------------------------------------
// 2. 别名定义 (完美解决你的需求)
// ---------------------------------------------------------

// 整型CMYK (0-100)
template <int C, int M, int Y, int K>
using cmyk_int = basic_cmyk<int, C, M, Y, K, 1>;

// 浮点型CMYK (0-1)
template <int C, int M, int Y, int K>
using cmyk_float = basic_cmyk<double, C, M, Y, K, 1000>;

// 常用CMYK颜色常量
namespace colors {
    using black_cmyk = cmyk_int<0, 0, 0, 100>;
    using white_cmyk = cmyk_int<0, 0, 0, 0>;
    using red_cmyk = cmyk_int<0, 100, 100, 0>;
    using green_cmyk = cmyk_int<100, 0, 100, 0>;
    using blue_cmyk = cmyk_int<100, 100, 0, 0>;
    using yellow_cmyk = cmyk_int<0, 0, 100, 0>;
    using cyan_cmyk = cmyk_int<100, 0, 0, 0>;
    using magenta_cmyk = cmyk_int<0, 100, 0, 0>;
}

} // namespace color::core
