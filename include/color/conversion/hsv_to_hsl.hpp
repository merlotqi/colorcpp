#pragma once

#include <cmath>
#include <type_traits>

#include "../core/hsl.hpp"
#include "../core/hsv.hpp"

namespace color::conversion {

// HSV到HSL转换器
template <typename HSVType>
struct hsv_to_hsl {
  using hsv_type = HSVType;
  using value_type = typename HSVType::value_type;

  // 获取HSV分量
  static constexpr value_type h = HSVType::h;
  static constexpr value_type s = HSVType::s;
  static constexpr value_type v = HSVType::v;

  // 归一化HSV值
  static constexpr value_type h_norm = h / (std::is_integral_v<value_type> ? 1.0 : 1.0);
  static constexpr value_type s_norm = s / (std::is_integral_v<value_type> ? 100.0 : 1.0);
  static constexpr value_type v_norm = v / (std::is_integral_v<value_type> ? 100.0 : 1.0);

  // 计算HSL值
  static constexpr value_type l = v_norm * (2.0 - s_norm) / 2.0;
  static constexpr value_type s_hsl = (l == 0.0 || l == 1.0) ? 0.0 : (v_norm - l) / std::min(l, 1.0 - l);

  // 根据输入类型选择输出类型 - 统一使用整型HSL
  using type = core::hsl_int<static_cast<int>(std::round(h_norm)), static_cast<int>(std::round(s_hsl * 100.0)),
                             static_cast<int>(std::round(l * 100.0))>;
};

// 类型别名
template <typename HSVType>
using hsv_to_hsl_t = typename hsv_to_hsl<HSVType>::type;

}  // namespace color::conversion
