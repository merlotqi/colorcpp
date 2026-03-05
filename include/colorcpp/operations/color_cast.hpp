#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/hsv.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/traits/concepts.hpp>

namespace color::operations::conversion {

namespace details {

template <typename Color>
constexpr float to_unit(typename Color::value_type v) {
  using Scale = typename Color::scale_type;
  return static_cast<float>(v) * Scale::num / Scale::den;
}

template <typename Color>
constexpr typename Color::value_type from_unit(float v) {
  using Scale = typename Color::scale_type;
  return static_cast<typename Color::value_type>(v * Scale::den / Scale::num);
}

template <typename RGB, typename HSL>
RGB hsl_to_rgb(const HSL& src) {
  float h = to_unit<HSL>(src.h);
  float s = to_unit<HSL>(src.s);
  float l = to_unit<HSL>(src.l);
  float a = to_unit<HSL>(src.a);

  h /= 360.0;
  s = std::clamp(s, 0.0f, 1.0f);
  l = std::clamp(l, 0.0f, 1.0f);

  auto hue2rgb = [](float p, float q, float t) {
    if (t < 0) t += 1;
    if (t > 1) t -= 1;
    if (t < 1.0 / 6) return p + (q - p) * 6 * t;
    if (t < 1.0 / 2) return q;
    if (t < 2.0 / 3) return p + (q - p) * (2.0 / 3 - t) * 6;
    return p;
  };

  float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
  float p = 2 * l - q;

  float r = hue2rgb(p, q, h + 1.0 / 3);
  float g = hue2rgb(p, q, h);
  float b = hue2rgb(p, q, h - 1.0 / 3);

  return RGB{from_unit<RGB>(r), from_unit<RGB>(g), from_unit<RGB>(b), from_unit<RGB>(a)};
}

template <typename HSL, typename RGB>
HSL rgb_to_hsl(const RGB& src) {
  float r = to_unit<RGB>(src.r);
  float g = to_unit<RGB>(src.g);
  float b = to_unit<RGB>(src.b);
  float a = to_unit<RGB>(src.a);

  float max = std::max({r, g, b});
  float min = std::min({r, g, b});
  float delta = max - min;

  float h = 0;
  float s = 0;
  float l = (max + min) / 2.0;

  if (delta != 0) {
    if (max == r)
      h = (g - b) / delta + (g < b ? 6 : 0);
    else if (max == g)
      h = (b - r) / delta + 2;
    else
      h = (r - g) / delta + 4;

    h /= 6.0;
    s = delta / (1 - std::abs(2 * l - 1));
  }

  return HSL{from_unit<HSL>(h * 360.0), from_unit<HSL>(s), from_unit<HSL>(l), from_unit<HSL>(a)};
}

template <typename RGB, typename HSV>
RGB hsv_to_rgb(const HSV& src) {
  float h = to_unit<HSV>(src.h);
  float s = to_unit<HSV>(src.s);
  float v = to_unit<HSV>(src.v);
  float a = to_unit<HSV>(src.a);

  h /= 360.0;

  int i = static_cast<int>(h * 6);
  float f = h * 6 - i;
  float p = v * (1 - s);
  float q = v * (1 - f * s);
  float t = v * (1 - (1 - f) * s);

  float r, g, b;

  switch (i % 6) {
    case 0:
      r = v;
      g = t;
      b = p;
      break;
    case 1:
      r = q;
      g = v;
      b = p;
      break;
    case 2:
      r = p;
      g = v;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = v;
      break;
    case 4:
      r = t;
      g = p;
      b = v;
      break;
    case 5:
      r = v;
      g = p;
      b = q;
      break;
  }

  return RGB{from_unit<RGB>(r), from_unit<RGB>(g), from_unit<RGB>(b), from_unit<RGB>(a)};
}

template <typename HSV, typename RGB>
HSV rgb_to_hsv(const RGB& src) {
  float r = to_unit<RGB>(src.r);
  float g = to_unit<RGB>(src.g);
  float b = to_unit<RGB>(src.b);
  float a = to_unit<RGB>(src.a);

  float max = std::max({r, g, b});
  float min = std::min({r, g, b});
  float delta = max - min;

  float h = 0;
  float s = (max == 0 ? 0 : delta / max);
  float v = max;

  if (delta != 0) {
    if (max == r)
      h = (g - b) / delta;
    else if (max == g)
      h = 2 + (b - r) / delta;
    else
      h = 4 + (r - g) / delta;

    h /= 6.0;
    if (h < 0) h += 1;
  }

  return HSV{from_unit<HSV>(h * 360.0), from_unit<HSV>(s), from_unit<HSV>(v), from_unit<HSV>(a)};
}

}  // namespace details

template <typename To, typename From>
To color_cast(const From& src) {
  if constexpr (std::is_same_v<typename From::color_tag, typename To::color_tag>) {
    To out{};

    if constexpr (std::is_same_v<typename To::color_tag, ::color::category::rgb>) {
      out.r = details::from_unit<To>(details::to_unit<From>(src.r));
      out.g = details::from_unit<To>(details::to_unit<From>(src.g));
      out.b = details::from_unit<To>(details::to_unit<From>(src.b));
      out.a = details::from_unit<To>(details::to_unit<From>(src.a));
    }

    else if constexpr (std::is_same_v<typename To::color_tag, color::category::hsl>) {
      out.h = details::from_unit<To>(details::to_unit<From>(src.h));
      out.s = details::from_unit<To>(details::to_unit<From>(src.s));
      out.l = details::from_unit<To>(details::to_unit<From>(src.l));
      out.a = details::from_unit<To>(details::to_unit<From>(src.a));
    }

    else if constexpr (std::is_same_v<typename To::color_tag, color::category::hsv>) {
      out.h = details::from_unit<To>(details::to_unit<From>(src.h));
      out.s = details::from_unit<To>(details::to_unit<From>(src.s));
      out.v = details::from_unit<To>(details::to_unit<From>(src.v));
      out.a = details::from_unit<To>(details::to_unit<From>(src.a));
    }

    return out;
  } else if constexpr (std::is_same_v<typename From::color_tag, color::category::hsl> &&
                       std::is_same_v<typename To::color_tag, color::category::rgb>) {
    return details::hsl_to_rgb<To, From>(src);
  }

  else if constexpr (std::is_same_v<typename From::color_tag, color::category::rgb> &&
                     std::is_same_v<typename To::color_tag, color::category::hsl>) {
    return details::rgb_to_hsl<To, From>(src);
  }

  else if constexpr (std::is_same_v<typename From::color_tag, color::category::hsv> &&
                     std::is_same_v<typename To::color_tag, color::category::rgb>) {
    return details::hsv_to_rgb<To, From>(src);
  }

  else if constexpr (std::is_same_v<typename From::color_tag, color::category::rgb> &&
                     std::is_same_v<typename To::color_tag, color::category::hsv>) {
    return details::rgb_to_hsv<To, From>(src);
  } else {
    static_assert(sizeof(To) == 0, "Unsupported color conversion");
  }
}

}  // namespace color::operations::conversion
