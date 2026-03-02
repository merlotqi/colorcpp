#pragma once

#include "../core/rgb.hpp"
#include "../core/hsv.hpp"
#include "../conversion/rgb_to_hsv.hpp"
#include "../conversion/hsv_to_rgb.hpp"

namespace color::operations {

// 亮度调整操作
template<typename RGBType, int Percent>
struct lighten {
    using rgb_type = RGBType;
    static_assert(Percent >= -100 && Percent <= 100, "Percent must be -100 to 100");
    
    // 转换到HSV空间进行亮度调整
    using hsv_type = conversion::rgb_to_hsv_t<RGBType>;
    
    // 调整亮度值
    static constexpr int new_v = hsv_type::v + (100 - hsv_type::v) * Percent / 100;
    static constexpr int clamped_v = (new_v < 0) ? 0 : ((new_v > 100) ? 100 : new_v);
    
    // 创建新的HSV类型
    using adjusted_hsv = core::hsv_int<hsv_type::h, hsv_type::s, clamped_v>;
    
    // 转换回RGB
    using type = conversion::hsv_to_rgb_t<adjusted_hsv>;
};

// 饱和度调整操作
template<typename RGBType, int Percent>
struct saturate {
    using rgb_type = RGBType;
    static_assert(Percent >= -100 && Percent <= 100, "Percent must be -100 to 100");
    
    // 转换到HSV空间进行饱和度调整
    using hsv_type = conversion::rgb_to_hsv_t<RGBType>;
    
    // 调整饱和度值
    static constexpr int new_s = hsv_type::s + (100 - hsv_type::s) * Percent / 100;
    static constexpr int clamped_s = (new_s < 0) ? 0 : ((new_s > 100) ? 100 : new_s);
    
    // 创建新的HSV类型
    using adjusted_hsv = core::hsv_int<hsv_type::h, clamped_s, hsv_type::v>;
    
    // 转换回RGB
    using type = conversion::hsv_to_rgb_t<adjusted_hsv>;
};

// 色相偏移操作
template<typename RGBType, int Degrees>
struct hue_shift {
    using rgb_type = RGBType;
    
    // 转换到HSV空间进行色相调整
    using hsv_type = conversion::rgb_to_hsv_t<RGBType>;
    
    // 调整色相值
    static constexpr int new_h = (hsv_type::h + Degrees) % 360;
    static constexpr int clamped_h = (new_h < 0) ? (new_h + 360) : new_h;
    
    // 创建新的HSV类型
    using adjusted_hsv = core::hsv_int<clamped_h, hsv_type::s, hsv_type::v>;
    
    // 转换回RGB
    using type = conversion::hsv_to_rgb_t<adjusted_hsv>;
};

// 类型别名
template<typename RGBType, int Percent>
using lighten_t = typename lighten<RGBType, Percent>::type;

template<typename RGBType, int Percent>
using saturate_t = typename saturate<RGBType, Percent>::type;

template<typename RGBType, int Degrees>
using hue_shift_t = typename hue_shift<RGBType, Degrees>::type;

} // namespace color::operations