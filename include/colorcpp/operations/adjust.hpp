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

#include <algorithm>
#include <cmath>
#include <colorcpp/operations/color_cast.hpp>
#include <colorcpp/traits/concepts.hpp>

namespace color::operations::adjust {

namespace details {

template <typename Color>
inline float to_unit(typename Color::value_type v) {
  using Scale = typename Color::scale_type;
  return static_cast<float>(v) * Scale::num / Scale::den;
}

template <typename Color>
inline typename Color::value_type from_unit(float v) {
  using Scale = typename Color::scale_type;
  return static_cast<typename Color::value_type>(v * Scale::den / Scale::num);
}

template <typename T>
inline T clamp01(T v) {
  return std::clamp(v, T(0), T(1));
}

}  // namespace details

template <typename Color>
Color brightness(const Color& c, float factor) {
  Color result = c;

  if constexpr (traits::is_rgb_v<Color>) {
    float r = details::to_unit<Color>(c.r);
    float g = details::to_unit<Color>(c.g);
    float b = details::to_unit<Color>(c.b);

    r = std::clamp(r * factor, 0.0f, 1.0f);
    g = std::clamp(g * factor, 0.0f, 1.0f);
    b = std::clamp(b * factor, 0.0f, 1.0f);

    result.r = details::from_unit<Color>(r);
    result.g = details::from_unit<Color>(g);
    result.b = details::from_unit<Color>(b);
  } else {
    auto rgb = conversion::color_cast<core::basic_rgba<float, std::ratio<1>>, Color>(c);

    rgb = brightness(rgb, factor);

    result = conversion::color_cast<Color>(rgb);
  }

  return result;
}

template <typename Color>
Color contrast(const Color& c, float factor) {
  Color result = c;

  if constexpr (traits::is_rgb_v<Color>) {
    auto adjust = [&](float x) { return std::clamp((x - 0.5) * factor + 0.5, 0.0, 1.0); };

    float r = adjust(details::to_unit<Color>(c.r));
    float g = adjust(details::to_unit<Color>(c.g));
    float b = adjust(details::to_unit<Color>(c.b));

    result.r = details::from_unit<Color>(r);
    result.g = details::from_unit<Color>(g);
    result.b = details::from_unit<Color>(b);
  } else {
    auto rgb = conversion::color_cast<core::basic_rgba<float, std::ratio<1>>, Color>(c);

    rgb = contrast(rgb, factor);

    result = conversion::color_cast<Color>(rgb);
  }

  return result;
}

template <typename Color>
Color alpha(const Color& c, float factor) {
  Color result = c;

  float a = details::to_unit<Color>(c.a);
  a = details::clamp01(a * factor);

  result.a = details::from_unit<Color>(a);

  return result;
}

template <typename Color>
Color hue_shift(const Color& c, float degrees) {
  if constexpr (traits::is_hsl_v<Color> || traits::is_hsv_v<Color>) {
    Color result = c;

    float h = details::to_unit<Color>(c.h);

    h += degrees;
    h = std::fmod(h, 360.0);
    if (h < 0) h += 360.0;

    result.h = details::from_unit<Color>(h);

    return result;
  } else {
    auto hsl = conversion::color_cast<core::basic_hsla<float, std::ratio<1, 360>, std::ratio<1>>, Color>(c);

    hsl = hue_shift(hsl, degrees);

    return conversion::color_cast<Color>(hsl);
  }
}

template <typename Color>
Color saturation(const Color& c, float factor) {
  if constexpr (traits::is_hsl_v<Color> || traits::is_hsv_v<Color>) {
    Color result = c;

    float s = details::to_unit<Color>(c.s);
    s = details::clamp01(s * factor);

    result.s = details::from_unit<Color>(s);

    return result;
  } else {
    auto hsl = conversion::color_cast<core::basic_hsla<float, std::ratio<1, 360>, std::ratio<1>>, Color>(c);

    hsl = saturation(hsl, factor);

    return conversion::color_cast<Color>(hsl);
  }
}

template <typename Color>
class pipeline {
 public:
  explicit pipeline(Color c) : current(std::move(c)) {}

  pipeline& brightness(float f) {
    current = adjust::brightness(current, f);
    return *this;
  }

  pipeline& contrast(float f) {
    current = adjust::contrast(current, f);
    return *this;
  }

  pipeline& hue(float d) {
    current = adjust::hue_shift(current, d);
    return *this;
  }

  pipeline& saturation(float f) {
    current = adjust::saturation(current, f);
    return *this;
  }

  Color get() const { return current; }

 private:
  Color current;
};

template <typename Color>
pipeline<Color> make_pipeline(Color c) {
  return pipeline<Color>(std::move(c));
}

}  // namespace color::operations::adjust
