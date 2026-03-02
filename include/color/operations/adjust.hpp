/**
 * @file adjust.hpp
 * @brief Color adjustment operations
 *
 * Provides compile-time color adjustment operations including lightening,
 * saturating, and hue shifting. All operations work in HSV color space for
 * natural color manipulation.
 *
 * @author Merlot.Qi
 *
 */

#pragma once

#include "../conversion/conversion.hpp"

namespace color::operations {

struct details {
  static constexpr intptr_t apply_percent(intptr_t value, int percent, intptr_t scale) {
    if (percent >= 0) {
      return value + (scale - value) * percent / 100;
    } else {
      return value + (value * percent / 100);
    }
  }

  static constexpr intptr_t clamp(intptr_t val, intptr_t min_v, intptr_t max_v) {
    return (val < min_v) ? min_v : ((val > max_v) ? max_v : val);
  }
};

template <typename ColorType>
constexpr auto lighten(const ColorType& c, int percent) {
  auto hsv = conversion::to_hsv<typename ColorType::value_type, 100>(c);
  hsv.v = details::clamp(details::apply_percent(hsv.v, percent, 100), 0, 100);
  return conversion::convert<ColorType>(hsv);
}

template <typename ColorType>
constexpr auto saturate(const ColorType& c, int percent) {
  auto hsv = conversion::to_hsv<typename ColorType::value_type, 100>(c);
  hsv.s = details::clamp(details::apply_percent(hsv.s, percent, 100), 0, 100);
  return conversion::convert<ColorType>(hsv);
}

template <typename ColorType>
constexpr auto hue_shift(const ColorType& c, int degrees) {
  auto hsv = conversion::to_hsv<typename ColorType::value_type, 100>(c);
  intptr_t new_h = (hsv.h + degrees) % 360;
  hsv.h = (new_h < 0) ? (new_h + 360) : new_h;
  return conversion::convert<ColorType>(hsv);
}

template <typename RGBType, int Percent>
struct lighten_op {
  static constexpr auto value() { return lighten(RGBType{}, Percent); }
  using type = decltype(value());
};

template <typename RGBType, int Percent>
struct saturate_op {
  static constexpr auto value() { return saturate(RGBType{}, Percent); }
  using type = decltype(value());
};

template <typename RGBType, int Degrees>
struct hue_shift_op {
  static constexpr auto value() { return hue_shift(RGBType{}, Degrees); }
  using type = decltype(value());
};

template <typename RGBType, int Percent>
using lighten_t = typename lighten_op<RGBType, Percent>::type;

template <typename RGBType, int Percent>
using saturate_t = typename saturate_op<RGBType, Percent>::type;

template <typename RGBType, int Degrees>
using hue_shift_t = typename hue_shift_op<RGBType, Degrees>::type;

}  // namespace color::operations
