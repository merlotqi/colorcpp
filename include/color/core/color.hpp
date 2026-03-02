#pragma once

#include <cstdint>
#include <type_traits>
#include <utility>

#include "../conversion/cmyk_to_rgb.hpp"
#include "../conversion/hsl_to_rgb.hpp"
#include "../conversion/hsv_to_rgb.hpp"
#include "../conversion/rgb_to_hsv.hpp"
#include "../operations/adjust.hpp"
#include "../operations/blend.hpp"
#include "../operations/filter.hpp"
#include "cmyk.hpp"
#include "hsl.hpp"
#include "hsv.hpp"
#include "rgb.hpp"

namespace color::core {

template <typename T = uint8_t>
class Color {
 public:
  using value_type = T;

 private:
  T r_, g_, b_;

 public:
  // 构造函数
  constexpr Color() : r_(0), g_(0), b_(0) {}

  constexpr Color(T r, T g, T b) : r_(r), g_(g), b_(b) {}

  // 从RGB类型构造
  template <typename RGBType>
  constexpr Color(const RGBType& rgb) : r_(rgb.r), g_(rgb.g), b_(rgb.b) {}

  // 拷贝构造和赋值
  constexpr Color(const Color&) = default;
  constexpr Color& operator=(const Color&) = default;

  // 获取RGB分量
  constexpr T r() const { return r_; }
  constexpr T g() const { return g_; }
  constexpr T b() const { return b_; }

  // 转换为RGB类型
  template <typename RGBType = basic_rgb<T, 0, 0, 0>>
  constexpr RGBType to_rgb() const {
    return RGBType{r_, g_, b_};
  }

  // 转换为HSV类型
  constexpr basic_hsv<T, 0, 0, 0> to_hsv() const {
    using rgb_type = basic_rgb<T, static_cast<intptr_t>(r_), static_cast<intptr_t>(g_), static_cast<intptr_t>(b_)>;
    return conversion::rgb_to_hsv_t<rgb_type>{};
  }

  // 转换为HSL类型
  constexpr basic_hsl<T, 0, 0, 0> to_hsl() const {
    // HSL转换需要实现
    // 这里返回一个默认值，实际应该通过HSL转换器实现
    return basic_hsl<T, 0, 0, 0>{};
  }

  // 转换为CMYK类型
  constexpr basic_cmyk<T, 0, 0, 0, 0> to_cmyk() const {
    // CMYK转换需要实现
    // 这里返回一个默认值，实际应该通过CMYK转换器实现
    return basic_cmyk<T, 0, 0, 0, 0>{};
  }

  // 亮度调整
  constexpr Color lighten(int percent) const {
    auto hsv = to_hsv();
    int new_v = static_cast<int>(hsv.v) + (100 - static_cast<int>(hsv.v)) * percent / 100;
    int clamped_v = (new_v < 0) ? 0 : ((new_v > 100) ? 100 : new_v);
    // 直接计算RGB值，避免模板参数问题
    T r = static_cast<T>(r_);
    T g = static_cast<T>(g_);
    T b = static_cast<T>(b_);
    // 简化的亮度调整
    T factor = static_cast<T>(1.0 + percent / 100.0);
    return Color(static_cast<T>(r * factor), static_cast<T>(g * factor), static_cast<T>(b * factor));
  }

  constexpr Color darken(int percent) const { return lighten(-percent); }

  // 饱和度调整
  constexpr Color saturate(int percent) const {
    // 简化的饱和度调整
    T r = static_cast<T>(r_);
    T g = static_cast<T>(g_);
    T b = static_cast<T>(b_);
    T factor = static_cast<T>(1.0 + percent / 100.0);
    return Color(static_cast<T>(r * factor), static_cast<T>(g * factor), static_cast<T>(b * factor));
  }

  // 色相偏移
  constexpr Color hue_shift(int degrees) const {
    // 简化的色相偏移
    T r = static_cast<T>(r_);
    T g = static_cast<T>(g_);
    T b = static_cast<T>(b_);
    // 简单的RGB循环偏移
    if (degrees % 120 == 0) {
      return Color(g, b, r);
    } else if (degrees % 240 == 0) {
      return Color(b, r, g);
    }
    return Color(r, g, b);
  }

  // 颜色混合
  template <typename OtherColor>
  constexpr Color blend(const OtherColor& other, int ratio) const {
    static_assert(std::is_arithmetic_v<typename OtherColor::value_type>, "OtherColor must have arithmetic value_type");

    T other_r = static_cast<T>(other.r());
    T other_g = static_cast<T>(other.g());
    T other_b = static_cast<T>(other.b());

    T new_r = static_cast<T>((r_ * (100 - ratio) + other_r * ratio) / 100);
    T new_g = static_cast<T>((g_ * (100 - ratio) + other_g * ratio) / 100);
    T new_b = static_cast<T>((b_ * (100 - ratio) + other_b * ratio) / 100);

    return Color(new_r, new_g, new_b);
  }

  // 灰度化
  constexpr Color grayscale() const {
    T gray = static_cast<T>(r_ * 0.299 + g_ * 0.587 + b_ * 0.114);
    return Color(gray, gray, gray);
  }

  // 反色
  constexpr Color invert() const { return Color(255 - r_, 255 - g_, 255 - b_); }

  // 阈值处理
  constexpr Color threshold(int threshold) const {
    T new_r = (r_ > threshold) ? 255 : 0;
    T new_g = (g_ > threshold) ? 255 : 0;
    T new_b = (b_ > threshold) ? 255 : 0;
    return Color(new_r, new_g, new_b);
  }

  // 色调分离
  constexpr Color posterize(int levels) const {
    static_assert(std::is_integral_v<T>, "Posterize requires integer type");
    int step = 256 / levels;
    T new_r = (r_ / step) * step;
    T new_g = (g_ / step) * step;
    T new_b = (b_ / step) * step;
    return Color(new_r, new_g, new_b);
  }

  // 比较操作符
  constexpr bool operator==(const Color& other) const { return r_ == other.r_ && g_ == other.g_ && b_ == other.b_; }

  constexpr bool operator!=(const Color& other) const { return !(*this == other); }
};

// 便捷别名
using Color8 = Color<uint8_t>;
using ColorF = Color<double>;

// RGB类型到动态颜色的隐式转换
template <typename T, intptr_t R_raw, intptr_t G_raw, intptr_t B_raw, intptr_t Scale>
constexpr Color<T> operator+(const basic_rgb<T, R_raw, G_raw, B_raw, Scale>&) {
  return Color<T>(basic_rgb<T, R_raw, G_raw, B_raw, Scale>::r, basic_rgb<T, R_raw, G_raw, B_raw, Scale>::g,
                  basic_rgb<T, R_raw, G_raw, B_raw, Scale>::b);
}

// HSV类型到动态颜色的隐式转换
template <typename T, intptr_t H_raw, intptr_t S_raw, intptr_t V_raw, intptr_t Scale>
constexpr Color<T> operator+(const basic_hsv<T, H_raw, S_raw, V_raw, Scale>& hsv) {
  using rgb_type = conversion::hsv_to_rgb_t<basic_hsv<T, H_raw, S_raw, V_raw, Scale>>;
  return Color<T>(rgb_type::r, rgb_type::g, rgb_type::b);
}

// HSL类型到动态颜色的隐式转换
template <typename T, intptr_t H_raw, intptr_t S_raw, intptr_t L_raw, intptr_t Scale>
constexpr Color<T> operator+(const basic_hsl<T, H_raw, S_raw, L_raw, Scale>& hsl) {
  using rgb_type = conversion::hsl_to_rgb_t<basic_hsl<T, H_raw, S_raw, L_raw, Scale>>;
  return Color<T>(rgb_type::r, rgb_type::g, rgb_type::b);
}

// CMYK类型到动态颜色的隐式转换
template <typename T, intptr_t C_raw, intptr_t M_raw, intptr_t Y_raw, intptr_t K_raw, intptr_t Scale>
constexpr Color<T> operator+(const basic_cmyk<T, C_raw, M_raw, Y_raw, K_raw, Scale>& cmyk) {
  using rgb_type = conversion::cmyk_to_rgb_t<basic_cmyk<T, C_raw, M_raw, Y_raw, K_raw, Scale>>;
  return Color<T>(rgb_type::r, rgb_type::g, rgb_type::b);
}

}  // namespace color::core
