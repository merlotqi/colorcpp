#pragma once

#include <cmath>

#include "../core/cmyk.hpp"
#include "../core/rgb.hpp"

namespace color::conversion {

// RGB到CMYK转换器
template <typename RGBType>
struct rgb_to_cmyk {
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

  // 计算最大值
  static constexpr value_type max_val =
      (r_norm > g_norm) ? ((r_norm > b_norm) ? r_norm : b_norm) : ((g_norm > b_norm) ? g_norm : b_norm);

  // 计算CMY值
  static constexpr value_type c = 1.0 - r_norm;
  static constexpr value_type m = 1.0 - g_norm;
  static constexpr value_type y = 1.0 - b_norm;
  static constexpr value_type k = 1.0 - max_val;

  // 计算最终CMYK值（如果K为1，则CMY为0）
  static constexpr value_type c_final = (k == 1.0) ? 0.0 : ((c - k) / (1.0 - k));
  static constexpr value_type m_final = (k == 1.0) ? 0.0 : ((m - k) / (1.0 - k));
  static constexpr value_type y_final = (k == 1.0) ? 0.0 : ((y - k) / (1.0 - k));

  // 根据输入类型选择输出类型 - 统一使用整型CMYK
  using type = core::cmyk_int<static_cast<int>(std::round(c_final * 100.0)),
                               static_cast<int>(std::round(m_final * 100.0)),
                               static_cast<int>(std::round(y_final * 100.0)),
                               static_cast<int>(std::round(k * 100.0))>;
};

// 类型别名
template <typename RGBType>
using rgb_to_cmyk_t = typename rgb_to_cmyk<RGBType>::type;

}  // namespace color::conversion
