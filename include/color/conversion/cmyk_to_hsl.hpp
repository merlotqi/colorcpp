#pragma once

#include <cmath>

#include "../core/cmyk.hpp"
#include "../core/hsl.hpp"

namespace color::conversion {

// CMYK到HSL转换器
template <typename CMYKType>
struct cmyk_to_hsl {
  using cmyk_type = CMYKType;
  using value_type = typename CMYKType::value_type;

  // 获取CMYK分量
  static constexpr value_type c = CMYKType::c;
  static constexpr value_type m = CMYKType::m;
  static constexpr value_type y = CMYKType::y;
  static constexpr value_type k = CMYKType::k;

  // 归一化CMYK值
  static constexpr value_type c_norm = c / (std::is_integral_v<value_type> ? 100.0 : 1.0);
  static constexpr value_type m_norm = m / (std::is_integral_v<value_type> ? 100.0 : 1.0);
  static constexpr value_type y_norm = y / (std::is_integral_v<value_type> ? 100.0 : 1.0);
  static constexpr value_type k_norm = k / (std::is_integral_v<value_type> ? 100.0 : 1.0);

  // 先转换到RGB
  static constexpr value_type r = (1.0 - c_norm) * (1.0 - k_norm);
  static constexpr value_type g = (1.0 - m_norm) * (1.0 - k_norm);
  static constexpr value_type b = (1.0 - y_norm) * (1.0 - k_norm);

  // 再从RGB转换到HSL
  static constexpr value_type max_val =
      (r > g) ? ((r > b) ? r : b) : ((g > b) ? g : b);
  static constexpr value_type min_val =
      (r < g) ? ((r < b) ? r : b) : ((g < b) ? g : b);
  static constexpr value_type delta = max_val - min_val;

  // 计算色相 (Hue)
  static constexpr value_type compute_hue() {
    if (delta == 0) return 0.0;

    value_type hue = 0.0;
    if (max_val == r) {
      hue = 60.0 * std::fmod((g - b) / delta, 6.0);
    } else if (max_val == g) {
      hue = 60.0 * ((b - r) / delta + 2.0);
    } else {
      hue = 60.0 * ((r - g) / delta + 4.0);
    }

    return hue < 0.0 ? hue + 360.0 : hue;
  }

  static constexpr value_type h = compute_hue();

  // 计算亮度 (Lightness)
  static constexpr value_type l = (max_val + min_val) / 2.0;

  // 计算饱和度 (Saturation)
  static constexpr value_type s = (delta == 0.0) ? 0.0 : (delta / (1.0 - std::abs(2.0 * l - 1.0)));

  // 根据输入类型选择输出类型 - 统一使用整型HSL
  using type = core::hsl_int<static_cast<int>(std::round(h)), static_cast<int>(std::round(s * 100.0)),
                             static_cast<int>(std::round(l * 100.0))>;
};

// 类型别名
template <typename CMYKType>
using cmyk_to_hsl_t = typename cmyk_to_hsl<CMYKType>::type;

}  // namespace color::conversion
