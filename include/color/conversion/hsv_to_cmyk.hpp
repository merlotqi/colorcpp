#pragma once

#include <cmath>

#include "../core/cmyk.hpp"
#include "../core/hsv.hpp"

namespace color::conversion {

// HSV到CMYK转换器
template <typename HSVType>
struct hsv_to_cmyk {
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

  // 先转换到RGB
  static constexpr value_type c = v_norm * s_norm;
  static constexpr value_type x = c * (1.0 - std::abs(std::fmod(h_norm / 60.0, 2.0) - 1.0));
  static constexpr value_type m = v_norm - c;

  static constexpr value_type r_temp = (h_norm < 60.0)    ? c
                                       : (h_norm < 120.0) ? x
                                       : (h_norm < 240.0) ? 0.0
                                       : (h_norm < 300.0) ? x
                                                          : c;

  static constexpr value_type g_temp = (h_norm < 60.0) ? x : (h_norm < 180.0) ? c : (h_norm < 240.0) ? x : 0.0;

  static constexpr value_type b_temp = (h_norm < 120.0) ? 0.0 : (h_norm < 180.0) ? x : (h_norm < 300.0) ? c : x;

  static constexpr value_type r = r_temp + m;
  static constexpr value_type g = g_temp + m;
  static constexpr value_type b = b_temp + m;

  // 再从RGB转换到CMYK
  static constexpr value_type max_val =
      (r > g) ? ((r > b) ? r : b) : ((g > b) ? g : b);

  static constexpr value_type c_cmyk = 1.0 - r;
  static constexpr value_type m_cmyk = 1.0 - g;
  static constexpr value_type y_cmyk = 1.0 - b;
  static constexpr value_type k = 1.0 - max_val;

  // 计算最终CMYK值（如果K为1，则CMY为0）
  static constexpr value_type c_final = (k == 1.0) ? 0.0 : ((c_cmyk - k) / (1.0 - k));
  static constexpr value_type m_final = (k == 1.0) ? 0.0 : ((m_cmyk - k) / (1.0 - k));
  static constexpr value_type y_final = (k == 1.0) ? 0.0 : ((y_cmyk - k) / (1.0 - k));

  // 根据输入类型选择输出类型 - 统一使用整型CMYK
  using type = core::cmyk_int<static_cast<int>(std::round(c_final * 100.0)),
                               static_cast<int>(std::round(m_final * 100.0)),
                               static_cast<int>(std::round(y_final * 100.0)),
                               static_cast<int>(std::round(k * 100.0))>;
};

// 类型别名
template <typename HSVType>
using hsv_to_cmyk_t = typename hsv_to_cmyk<HSVType>::type;

}  // namespace color::conversion
