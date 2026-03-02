#pragma once

#include <cmath>

#include "../core/hsl.hpp"
#include "../core/rgb.hpp"

namespace color::conversion {

// RGB到HSL转换器
template <typename RGBType>
struct rgb_to_hsl {
  using rgb_type = RGBType;
  using value_type = typename RGBType::value_type;

  // 获取RGB分量
  static constexpr value_type r = RGBType::r;
  static constexpr value_type g = RGBType::g;
  static constexpr value_type b = RGBType::b;

  // 归一化RGB值 (0.0-1.0)
  static constexpr value_type r_norm = r / (std::is_integral_v<value_type> ? 255.0 : 1.0);
  static constexpr value_type g_norm = g / (std::is_integral_v<value_type> ? 255.0 : 1.0);
  static constexpr value_type b_norm = b / (std::is_integral_v<value_type> ? 255.0 : 1.0);

  // 计算最大值、最小值和差值
  static constexpr value_type max_val =
      (r_norm > g_norm) ? ((r_norm > b_norm) ? r_norm : b_norm) : ((g_norm > b_norm) ? g_norm : b_norm);
  static constexpr value_type min_val =
      (r_norm < g_norm) ? ((r_norm < b_norm) ? r_norm : b_norm) : ((g_norm < b_norm) ? g_norm : b_norm);
  static constexpr value_type delta = max_val - min_val;

  // 计算色相 (Hue)
  static constexpr value_type compute_hue() {
    if (delta == 0) return 0.0;

    value_type hue = 0.0;
    if (max_val == r_norm) {
      hue = 60.0 * std::fmod((g_norm - b_norm) / delta, 6.0);
    } else if (max_val == g_norm) {
      hue = 60.0 * ((b_norm - r_norm) / delta + 2.0);
    } else {
      hue = 60.0 * ((r_norm - g_norm) / delta + 4.0);
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
template <typename RGBType>
using rgb_to_hsl_t = typename rgb_to_hsl<RGBType>::type;

}  // namespace color::conversion
