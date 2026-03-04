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

#include <cmath>
#include <colorcpp/operations/color_cast.hpp>
#include <colorcpp/traits/concepts.hpp>

namespace color::operations::blend {

namespace details {

template <typename Color>
float to_unit(typename Color::value_type v) {
  using Scale = typename Color::scale_type;
  return static_cast<float>(v) * Scale::num / Scale::den;
}

template <typename Color>
typename Color::value_type from_unit(float v) {
  using Scale = typename Color::scale_type;
  return static_cast<typename Color::value_type>(v * Scale::den / Scale::num);
}

inline float alpha_composite(float src_a, float dst_a) { return src_a + dst_a * (1.0 - src_a); }

}  // namespace details

template <typename Color>
Color normal(const Color& src, const Color& dst) {
  auto s = conversion::color_cast<core::basic_rgba<float, std::ratio<1>>, Color>(src);

  auto d = conversion::color_cast<core::basic_rgba<float, std::ratio<1>>, Color>(dst);

  float out_a = details::alpha_composite(s.a, d.a);

  float r = (s.r * s.a + d.r * d.a * (1 - s.a)) / out_a;
  float g = (s.g * s.a + d.g * d.a * (1 - s.a)) / out_a;
  float b = (s.b * s.a + d.b * d.a * (1 - s.a)) / out_a;

  auto out = core::basic_rgba<float, std::ratio<1>>{r, g, b, out_a};

  return conversion::color_cast<Color>(out);
}

template <typename Color>
Color multiply(const Color& src, const Color& dst) {
  auto s = conversion::color_cast<core::basic_rgba<float, std::ratio<1>>, Color>(src);

  auto d = conversion::color_cast<core::basic_rgba<float, std::ratio<1>>, Color>(dst);

  float r = s.r * d.r;
  float g = s.g * d.g;
  float b = s.b * d.b;

  float out_a = details::alpha_composite(s.a, d.a);

  auto out = core::basic_rgba<float, std::ratio<1>>{r, g, b, out_a};

  return conversion::color_cast<Color>(out);
}

template <typename Color>
Color screen(const Color& src, const Color& dst) {
  auto s = conversion::color_cast<core::basic_rgba<float, std::ratio<1>>, Color>(src);

  auto d = conversion::color_cast<core::basic_rgba<float, std::ratio<1>>, Color>(dst);

  float r = 1 - (1 - s.r) * (1 - d.r);
  float g = 1 - (1 - s.g) * (1 - d.g);
  float b = 1 - (1 - s.b) * (1 - d.b);

  float out_a = details::alpha_composite(s.a, d.a);

  auto out = core::basic_rgba<float, std::ratio<1>>{r, g, b, out_a};

  return conversion::color_cast<Color>(out);
}

template <typename Color>
Color overlay(const Color& src, const Color& dst) {
  auto s = conversion::color_cast<core::basic_rgba<float, std::ratio<1>>, Color>(src);

  auto d = conversion::color_cast<core::basic_rgba<float, std::ratio<1>>, Color>(dst);

  auto f = [](float s, float d) { return d < 0.5 ? 2 * s * d : 1 - 2 * (1 - s) * (1 - d); };

  float r = f(s.r, d.r);
  float g = f(s.g, d.g);
  float b = f(s.b, d.b);

  float out_a = details::alpha_composite(s.a, d.a);

  auto out = core::basic_rgba<float, std::ratio<1>>{r, g, b, out_a};

  return conversion::color_cast<Color>(out);
}

enum class mode {
  normal,
  multiply,
  screen,
  overlay
};

template <typename Color>
Color blend(const Color& src, const Color& dst, mode m) {
  switch (m) {
    case mode::normal:
      return normal(src, dst);

    case mode::multiply:
      return multiply(src, dst);

    case mode::screen:
      return screen(src, dst);

    case mode::overlay:
      return overlay(src, dst);

    default:
      return normal(src, dst);
  }
}

}  // namespace color::operations::blend
