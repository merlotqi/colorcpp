#pragma once

#include "../core/rgb.hpp"

namespace color::operations {

// 颜色混合操作
template<typename Color1, typename Color2, int Ratio>
struct blend {
    using color1_type = Color1;
    using color2_type = Color2;
    static_assert(Ratio >= 0 && Ratio <= 100, "Ratio must be 0-100");
    
    // 混合单个通道
    template<typename T>
    static constexpr T blend_channel(T c1, T c2) {
        return static_cast<T>((c1 * (100 - Ratio) + c2 * Ratio) / 100);
    }
    
    // 计算混合后的RGB值
    static constexpr auto r = blend_channel(Color1::r, Color2::r);
    static constexpr auto g = blend_channel(Color1::g, Color2::g);
    static constexpr auto b = blend_channel(Color1::b, Color2::b);
    
    // 确定结果类型
    using type = std::conditional_t<
        std::is_same_v<typename Color1::value_type, uint8_t>,
        core::rgb8<r, g, b>,
        core::rgb8<r, g, b>
    >;
};

// 类型别名
template<typename Color1, typename Color2, int Ratio>
using blend_t = typename blend<Color1, Color2, Ratio>::type;

// 常用混合操作
namespace blend_ops {
    template<typename Color1, typename Color2>
    using overlay = blend_t<Color1, Color2, 50>;
    
    template<typename Color1, typename Color2>
    using screen = blend_t<Color1, Color2, 75>;
    
    template<typename Color1, typename Color2>
    using multiply = blend_t<Color1, Color2, 25>;
}

} // namespace color::operations