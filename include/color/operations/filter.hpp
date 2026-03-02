#pragma once

#include "../core/rgb.hpp"

namespace color::operations {

// 灰度化操作
template <typename RGBType>
struct grayscale {
  using rgb_type = RGBType;

  // 使用亮度公式计算灰度值
  static constexpr auto gray =
      static_cast<typename RGBType::value_type>(RGBType::r * 0.299 + RGBType::g * 0.587 + RGBType::b * 0.114);

  using type = core::rgb8<gray, gray, gray>;
};

// 反色操作
template <typename RGBType>
struct invert {
  using rgb_type = RGBType;

  static constexpr auto r = 255 - RGBType::r;
  static constexpr auto g = 255 - RGBType::g;
  static constexpr auto b = 255 - RGBType::b;

  using type = core::rgb8<r, g, b>;
};

// 阈值处理操作
template <typename RGBType, int Threshold>
struct threshold {
  using rgb_type = RGBType;
  static_assert(Threshold >= 0 && Threshold <= 255, "Threshold must be 0-255");

  static constexpr auto r = (RGBType::r > Threshold) ? 255 : 0;
  static constexpr auto g = (RGBType::g > Threshold) ? 255 : 0;
  static constexpr auto b = (RGBType::b > Threshold) ? 255 : 0;

  using type = core::rgb8<r, g, b>;
};

// 色调分离操作
template <typename RGBType, int Levels>
struct posterize {
  using rgb_type = RGBType;
  static_assert(Levels > 0 && Levels <= 256, "Levels must be 1-256");

  static constexpr auto step = 256 / Levels;

  static constexpr auto r = (RGBType::r / step) * step;
  static constexpr auto g = (RGBType::g / step) * step;
  static constexpr auto b = (RGBType::b / step) * step;

  using type = core::rgb8<r, g, b>;
};

// 类型别名
template <typename RGBType>
using grayscale_t = typename grayscale<RGBType>::type;

template <typename RGBType>
using invert_t = typename invert<RGBType>::type;

template <typename RGBType, int Threshold>
using threshold_t = typename threshold<RGBType, Threshold>::type;

template <typename RGBType, int Levels>
using posterize_t = typename posterize<RGBType, Levels>::type;

}  // namespace color::operations
