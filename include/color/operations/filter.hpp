/**
 * @file adjust.hpp
 * @brief Complete Color adjustment and filter operations
 *
 * Provides both compile-time (template-based) and run-time (constexpr)
 * color manipulation including HSL/HSV adjustments and RGB filters.
 *
 * @author Merlot.Qi
 */

#pragma once

#include <color/conversion/conversion.hpp>
#include <color/utils/maths.hpp>
#include <type_traits>
#include <utility>

namespace color::operations {

namespace details {

constexpr intptr_t apply_percent(intptr_t value, int percent, intptr_t scale) {
  if (percent >= 0) {
    return value + (scale - value) * percent / 100;
  } else {
    return value + (value * percent / 100);
  }
}

constexpr intptr_t clamp(intptr_t val, intptr_t min_v, intptr_t max_v) {
  return (val < min_v) ? min_v : ((val > max_v) ? max_v : val);
}

template <typename T, intptr_t Scale>
constexpr float get_rgb_max() {
  if constexpr (std::is_integral_v<T> && Scale == 1) {
    return 255.0;
  } else {
    return static_cast<float>(Scale);
  }
}

}  // namespace details

template <typename ColorType>
constexpr auto lighten(const ColorType& c, int percent) {
  using T = typename ColorType::value_type;
  auto hsl = conversion::to_hsl<T, 100>(c);
  hsl.l = details::clamp(details::apply_percent(hsl.l, percent, 100), 0, 100);
  return conversion::convert<ColorType>(hsl);
}

template <typename ColorType>
constexpr auto saturate(const ColorType& c, int percent) {
  using T = typename ColorType::value_type;
  auto hsv = conversion::to_hsv<T, 100>(c);
  hsv.s = details::clamp(details::apply_percent(hsv.s, percent, 100), 0, 100);
  return conversion::convert<ColorType>(hsv);
}

template <typename ColorType>
constexpr auto hue_shift(const ColorType& c, int degrees) {
  using T = typename ColorType::value_type;
  auto hsv = conversion::to_hsv<T, 100>(c);
  intptr_t new_h = (hsv.h + degrees) % 360;
  hsv.h = (new_h < 0) ? (new_h + 360) : new_h;
  return conversion::convert<ColorType>(hsv);
}

template <typename Color1, typename Color2>
constexpr Color1 blend(const Color1& c1, const Color2& c2, int ratio) {
  auto c2_mapped = conversion::to_rgb<Color1>(c2);
  auto b = [ratio](auto v1, auto v2) {
    return static_cast<typename Color1::value_type>(
        (static_cast<intptr_t>(v1) * (100 - ratio) + static_cast<intptr_t>(v2) * ratio) / 100);
  };
  return Color1{b(c1.r, c2_mapped.r), b(c1.g, c2_mapped.g), b(c1.b, c2_mapped.b)};
}

template <typename RGBType>
constexpr RGBType grayscale(const RGBType& c) {
  using T = typename RGBType::value_type;
  T gray = static_cast<T>(c.r * 0.299 + c.g * 0.587 + c.b * 0.114);
  return RGBType{gray, gray, gray};
}

template <typename RGBType>
constexpr RGBType invert(const RGBType& c) {
  using T = typename RGBType::value_type;
  constexpr float max_v = details::get_rgb_max<T, RGBType::scale()>();
  return RGBType{static_cast<T>(max_v - c.r), static_cast<T>(max_v - c.g), static_cast<T>(max_v - c.b)};
}

template <typename RGBType>
constexpr RGBType threshold(const RGBType& c, int thresh = 127) {
  using T = typename RGBType::value_type;
  constexpr float max_v = details::get_rgb_max<T, RGBType::scale()>();
  float t = (thresh / 255.0) * max_v;
  auto filter = [t, max_v](T val) {
    return (static_cast<float>(val) > t) ? static_cast<T>(max_v) : static_cast<T>(0);
  };
  return RGBType{filter(c.r), filter(c.g), filter(c.b)};
}

template <typename RGBType>
constexpr RGBType posterize(const RGBType& c, int levels) {
  if (levels <= 1) return threshold(c, 127);
  using T = typename RGBType::value_type;
  constexpr float max_v = details::get_rgb_max<T, RGBType::scale()>();
  float step = max_v / (levels - 1);
  auto quant = [step](T val) { return static_cast<T>(maths::round(static_cast<float>(val) / step) * step); };
  return RGBType{quant(c.r), quant(c.g), quant(c.b)};
}

template <typename RGBType, int Percent>
struct lighten_op {
  using type = decltype(lighten(std::declval<RGBType>(), Percent));
  static constexpr type value = lighten(RGBType{}, Percent);
};

template <typename RGBType, int Percent>
struct saturate_op {
  using type = decltype(saturate(std::declval<RGBType>(), Percent));
  static constexpr type value = saturate(RGBType{}, Percent);
};

template <typename RGBType, int Degrees>
struct hue_shift_op {
  using type = decltype(hue_shift(std::declval<RGBType>(), Degrees));
  static constexpr type value = hue_shift(RGBType{}, Degrees);
};

template <typename Color1, typename Color2, int Ratio>
struct blend_op {
  using type = decltype(blend(std::declval<Color1>(), std::declval<Color2>(), Ratio));
  static constexpr type value = blend(Color1{}, Color2{}, Ratio);
};

template <typename RGBType>
struct grayscale_op {
  using type = decltype(grayscale(std::declval<RGBType>()));
  static constexpr type value = grayscale(RGBType{});
};

template <typename RGBType>
struct invert_op {
  using type = decltype(invert(std::declval<RGBType>()));
  static constexpr type value = invert(RGBType{});
};

template <typename RGBType, int Percent>
using lighten_t = typename lighten_op<RGBType, Percent>::type;

template <typename RGBType, int Percent>
using saturate_t = typename saturate_op<RGBType, Percent>::type;

template <typename RGBType, int Degrees>
using hue_shift_t = typename hue_shift_op<RGBType, Degrees>::type;

template <typename Color1, typename Color2, int Ratio>
using blend_t = typename blend_op<Color1, Color2, Ratio>::type;

template <typename RGBType>
using grayscale_t = typename grayscale_op<RGBType>::type;

template <typename RGBType>
using invert_t = typename invert_op<RGBType>::type;

}  // namespace color::operations
