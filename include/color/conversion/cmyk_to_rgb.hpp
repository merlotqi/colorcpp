#pragma once

#include <cmath>

#include "../core/cmyk.hpp"
#include "../core/rgb.hpp"

namespace color::conversion {

// CMYK到RGB转换器
template <typename CMYKType>
struct cmyk_to_rgb {
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

  // 计算RGB值
  static constexpr value_type r = (1.0 - c_norm) * (1.0 - k_norm);
  static constexpr value_type g = (1.0 - m_norm) * (1.0 - k_norm);
  static constexpr value_type b = (1.0 - y_norm) * (1.0 - k_norm);

  // 根据输入类型选择输出类型 - 统一使用RGB8
  using type = core::rgb8<static_cast<uint8_t>(std::round(r * 255.0)), static_cast<uint8_t>(std::round(g * 255.0)),
                          static_cast<uint8_t>(std::round(b * 255.0))>;
};

// 类型别名
template <typename CMYKType>
using cmyk_to_rgb_t = typename cmyk_to_rgb<CMYKType>::type;

}  // namespace color::conversion
