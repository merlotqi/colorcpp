/**
 * @file blend.hpp
 * @brief Advanced Color blending operations
 *
 * Provides both linear interpolation and standard graphics blending modes
 * (Multiply, Screen, Overlay) for compile-time and run-time use.
 *
 * @author Merlot.Qi
 */

#pragma once

#include <type_traits>

#include "../conversion/conversion.hpp"

namespace color::operations {

namespace details {

template <typename ColorType>
static constexpr double get_scale_v() {
  if constexpr (std::is_integral_v<typename ColorType::value_type> && ColorType::scale() == 1) {
    return 255.0;
  } else {
    return static_cast<double>(ColorType::scale());
  }
}

template <typename T>
static constexpr T lerp_channel(T c1, T c2, int ratio) {
  return static_cast<T>((static_cast<intptr_t>(c1) * (100 - ratio) + static_cast<intptr_t>(c2) * ratio) / 100);
}
}  // namespace details

template <typename Color1, typename Color2>
constexpr Color1 blend(const Color1& c1, const Color2& c2, int ratio) {
  auto c2_mapped = conversion::convert<Color1>(c2);
  return Color1{details::lerp_channel(c1.r, c2_mapped.r, ratio), details::lerp_channel(c1.g, c2_mapped.g, ratio),
                details::lerp_channel(c1.b, c2_mapped.b, ratio)};
}

template <typename Color1, typename Color2>
constexpr Color1 multiply(const Color1& c1, const Color2& c2) {
  auto c2_m = conversion::convert<Color1>(c2);
  constexpr double s = details::get_scale_v<Color1>();
  auto op = [s](auto v1, auto v2) {
    return static_cast<typename Color1::value_type>((static_cast<double>(v1) * v2) / s);
  };
  return Color1{op(c1.r, c2_m.r), op(c1.g, c2_m.g), op(c1.b, c2_m.b)};
}

template <typename Color1, typename Color2>
constexpr Color1 screen(const Color1& c1, const Color2& c2) {
  auto c2_m = conversion::convert<Color1>(c2);
  constexpr double s = details::get_scale_v<Color1>();
  auto op = [s](auto v1, auto v2) { return static_cast<typename Color1::value_type>(s - ((s - v1) * (s - v2) / s)); };
  return Color1{op(c1.r, c2_m.r), op(c1.g, c2_m.g), op(c1.b, c2_m.b)};
}

template <typename Color1, typename Color2>
constexpr Color1 overlay(const Color1& c1, const Color2& c2) {
  auto c2_m = conversion::convert<Color1>(c2);
  constexpr double s = details::get_scale_v<Color1>();
  auto op = [s](auto v1, auto v2) {
    double f1 = static_cast<double>(v1);
    double f2 = static_cast<double>(v2);
    double res = (f1 < s / 2.0) ? (2.0 * f1 * f2 / s) : (s - 2.0 * (s - f1) * (s - f2) / s);
    return static_cast<typename Color1::value_type>(res);
  };
  return Color1{op(c1.r, c2_m.r), op(c1.g, c2_m.g), op(c1.b, c2_m.b)};
}

template <typename Color1, typename Color2, int Ratio>
struct blend_op {
  using type = decltype(blend(std::declval<Color1>(), std::declval<Color2>(), Ratio));
  static constexpr type value = blend(Color1{}, Color2{}, Ratio);
};

template <typename Color1, typename Color2>
struct multiply_op {
  using type = decltype(multiply(std::declval<Color1>(), std::declval<Color2>()));
  static constexpr type value = multiply(Color1{}, Color2{});
};

template <typename Color1, typename Color2>
struct screen_op {
  using type = decltype(screen(std::declval<Color1>(), std::declval<Color2>()));
  static constexpr type value = screen(Color1{}, Color2{});
};

template <typename Color1, typename Color2>
struct overlay_op {
  using type = decltype(overlay(std::declval<Color1>(), std::declval<Color2>()));
  static constexpr type value = overlay(Color1{}, Color2{});
};

template <typename Color1, typename Color2, int Ratio>
using blend_t = typename blend_op<Color1, Color2, Ratio>::type;

template <typename Color1, typename Color2>
using multiply_t = typename multiply_op<Color1, Color2>::type;

template <typename Color1, typename Color2>
using screen_t = typename screen_op<Color1, Color2>::type;

template <typename Color1, typename Color2>
using overlay_t = typename overlay_op<Color1, Color2>::type;

namespace blend_ops {
template <typename Color1, typename Color2>
using mix_50 = blend_t<Color1, Color2, 50>;

template <typename Color1, typename Color2>
using mix_75 = blend_t<Color1, Color2, 75>;

template <typename Color1, typename Color2>
using mix_25 = blend_t<Color1, Color2, 25>;
}  // namespace blend_ops

}  // namespace color::operations
