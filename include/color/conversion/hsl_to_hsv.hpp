#pragma once

#include <cmath>

#include "../core/hsl.hpp"
#include "../core/hsv.hpp"

namespace color::conversion {

// HSL到HSV转换器
template <typename HSLType>
struct hsl_to_hsv {
  using hsl_type = HSLType;
  using value_type = typename HSLType::value_type;

  // 获取HSL分量
  static constexpr value_type h = HSLType::h;
  static constexpr value_type s = HSLType::s;
  static constexpr value_type l = HSLType::l;

  // 归一化HSL值
  static constexpr value_type h_norm = h / (std::is_integral_v<value_type> ? 1.0 : 1.0);
  static constexpr value_type s_norm = s / (std::is_integral_v<value_type> ? 100.0 : 1.0);
  static constexpr value_type l_norm = l / (std::is_integral_v<value_type> ? 100.0 : 1.0);

  // 计算HSV值
  static constexpr value_type v = l_norm + s_norm * std::min(l_norm, 1.0 - l_norm);
  static constexpr value_type s_hsv = (v == 0.0) ? 0.0 : (2.0 * (1.0 - l_norm / v));

  // 根据输入类型选择输出类型 - 统一使用整型HSV
  using type = core::hsv_int<static_cast<int>(std::round(h_norm)), static_cast<int>(std::round(s_hsv * 100.0)),
                             static_cast<int>(std::round(v * 100.0))>;
};

// 类型别名
template <typename HSLType>
using hsl_to_hsv_t = typename hsl_to_hsv<HSLType>::type;

}  // namespace color::conversion
