#pragma once

#include "../core/rgb.hpp"
#include "../core/hsv.hpp"
#include <cmath>

namespace color::conversion {

// HSV到RGB转换器
template<typename HSVType>
struct hsv_to_rgb {
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
    
    // 计算中间值
    static constexpr value_type c = v_norm * s_norm;
    static constexpr value_type x = c * (1.0 - std::abs(std::fmod(h_norm / 60.0, 2.0) - 1.0));
    static constexpr value_type m = v_norm - c;
    
    // 计算RGB分量
    static constexpr value_type r_temp = (h_norm < 60.0) ? c :
        (h_norm < 120.0) ? x :
        (h_norm < 240.0) ? 0.0 :
        (h_norm < 300.0) ? x : c;
    
    static constexpr value_type g_temp = (h_norm < 60.0) ? x :
        (h_norm < 180.0) ? c :
        (h_norm < 240.0) ? x : 0.0;
    
    static constexpr value_type b_temp = (h_norm < 120.0) ? 0.0 :
        (h_norm < 180.0) ? x :
        (h_norm < 300.0) ? c : x;
    
    // 最终RGB值
    static constexpr value_type r = r_temp + m;
    static constexpr value_type g = g_temp + m;
    static constexpr value_type b = b_temp + m;
    
    // 根据输入类型选择输出类型 - 统一使用RGB8
    using type = core::rgb8<
        static_cast<uint8_t>(std::round(r * 255.0)),
        static_cast<uint8_t>(std::round(g * 255.0)),
        static_cast<uint8_t>(std::round(b * 255.0))
    >;
};

// 类型别名
template<typename HSVType>
using hsv_to_rgb_t = typename hsv_to_rgb<HSVType>::type;

} // namespace color::conversion