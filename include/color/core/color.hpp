#pragma once

#include "rgb.hpp"
#include "hsl.hpp"
#include "hsv.hpp"
#include "cmyk.hpp"
#include "../conversion/rgb_to_hsv.hpp"
#include "../conversion/hsv_to_rgb.hpp"
#include "../operations/blend.hpp"
#include "../operations/adjust.hpp"
#include "../operations/filter.hpp"

namespace color::core {

// 主颜色类模板，提供统一的接口
template <typename T, intptr_t R_raw, intptr_t G_raw, intptr_t B_raw,
          intptr_t Scale = 1>
class Color {
public:
  using value_type = T;
  using rgb_type = basic_rgb<T, R_raw, G_raw, B_raw, Scale>;
  using hsv_type = conversion::rgb_to_hsv_t<rgb_type>;

  // RGB 值
  static constexpr T r = rgb_type::r;
  static constexpr T g = rgb_type::g;
  static constexpr T b = rgb_type::b;

  // HSV 值
  static constexpr T h = hsv_type::h;
  static constexpr T s = hsv_type::s;
  static constexpr T v = hsv_type::v;

  // 转换方法
  static constexpr rgb_type to_rgb() { return rgb_type{}; }
  static constexpr hsv_type to_hsv() { return hsv_type{}; }

  // 操作方法
  template <int Percent>
  using lighten = Color<T, static_cast<intptr_t>(r * (100 + Percent) / 100),
                       static_cast<intptr_t>(g * (100 + Percent) / 100),
                       static_cast<intptr_t>(b * (100 + Percent) / 100), Scale>;

  template <int Percent>
  using darken = Color<T, static_cast<intptr_t>(r * (100 - Percent) / 100),
                      static_cast<intptr_t>(g * (100 - Percent) / 100),
                      static_cast<intptr_t>(b * (100 - Percent) / 100), Scale>;

  template <typename OtherColor, int Ratio>
  using blend = Color<T,
                      static_cast<intptr_t>((r * (100 - Ratio) + OtherColor::r * Ratio) / 100),
                      static_cast<intptr_t>((g * (100 - Ratio) + OtherColor::g * Ratio) / 100),
                      static_cast<intptr_t>((b * (100 - Ratio) + OtherColor::b * Ratio) / 100), Scale>;
};

// 便捷别名
template <uint8_t R, uint8_t G, uint8_t B>
using Color8 = Color<uint8_t, R, G, B, 1>;

template <int R, int G, int B>
using ColorF = Color<double, R, G, B, 1000>;

// 常用颜色常量 (使用不同的命名空间避免冲突)
namespace color_constants {
using black = Color8<0, 0, 0>;
using white = Color8<255, 255, 255>;
using red = Color8<255, 0, 0>;
using green = Color8<0, 255, 0>;
using blue = Color8<0, 0, 255>;
using yellow = Color8<255, 255, 0>;
using cyan = Color8<0, 255, 255>;
using magenta = Color8<255, 0, 255>;
using gray = Color8<128, 128, 128>;
using silver = Color8<192, 192, 192>;
using maroon = Color8<128, 0, 0>;
using olive = Color8<128, 128, 0>;
using navy = Color8<0, 0, 128>;
using purple = Color8<128, 0, 128>;
using teal = Color8<0, 128, 128>;
using lime = Color8<0, 255, 0>;
using fuchsia = Color8<255, 0, 255>;
using aqua = Color8<0, 255, 255>;
}

} // namespace color::core
