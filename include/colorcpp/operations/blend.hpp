#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/operations/conversion.hpp>

#include "colorcpp/core/rgb.hpp"

namespace colorcpp::operations::blend {

enum class blend_mode {
  normal,
  multiply,
  screen,
  overlay,
  darken,
  lighten,
  addition,
  subtraction,
  difference,
};

namespace details {

struct algorithms {
  static constexpr float normal(float a, float b) { return b; }
  static constexpr float multiply(float a, float b) { return a * b; }
  static constexpr float screen(float a, float b) { return 1.0f - (1.0f - a) * (1.0f - b); }
  static constexpr float darken(float a, float b) { return std::min(a, b); }
  static constexpr float lighten(float a, float b) { return std::max(a, b); }
  static constexpr float addition(float a, float b) { return std::min(1.0f, a + b); }
  static constexpr float subtraction(float a, float b) { return std::max(0.0f, a - b); }
  static constexpr float difference(float a, float b) { return std::abs(a - b); }

  static constexpr float overlay(float a, float b) {
    return (a < 0.5f) ? (2.0f * a * b) : (1.0f - 2.0f * (1.0f - a) * (1.0f - b));
  }
};

template <typename Fn>
constexpr float composite(float dst, float da, float src, float sa, Fn&& blend_fn) {
  float mixed = blend_fn(dst, src);
  return src * sa * (1.0f - da) + dst * da * (1.0f - sa) + da * sa * mixed;
}

}  // namespace details

template <typename Color>
constexpr Color blend(const Color& dst, const Color& src, blend_mode mode = blend_mode::normal) {
  using namespace conversion;

  auto d = color_cast<core::rgbaf_t>(dst);
  auto s = color_cast<core::rgbaf_t>(src);

  float da = d.template get_index<3>();
  float sa = s.template get_index<3>();

  float out_a = sa + da * (1.0f - sa);

  auto apply_blend = [&](float d_ch, float s_ch) {
    if (out_a <= 0.0f) return 0.0f;

    float res_ch = 0.0f;
    switch (mode) {
      case blend_mode::normal:
        res_ch = details::composite(d_ch, da, s_ch, sa, details::algorithms::normal);
        break;
      case blend_mode::multiply:
        res_ch = details::composite(d_ch, da, s_ch, sa, details::algorithms::multiply);
        break;
      case blend_mode::screen:
        res_ch = details::composite(d_ch, da, s_ch, sa, details::algorithms::screen);
        break;
      case blend_mode::overlay:
        res_ch = details::composite(d_ch, da, s_ch, sa, details::algorithms::overlay);
        break;
      case blend_mode::darken:
        res_ch = details::composite(d_ch, da, s_ch, sa, details::algorithms::darken);
        break;
      case blend_mode::lighten:
        res_ch = details::composite(d_ch, da, s_ch, sa, details::algorithms::lighten);
        break;
      case blend_mode::addition:
        res_ch = details::composite(d_ch, da, s_ch, sa, details::algorithms::addition);
        break;
      case blend_mode::subtraction:
        res_ch = details::composite(d_ch, da, s_ch, sa, details::algorithms::subtraction);
        break;
      case blend_mode::difference:
        res_ch = details::composite(d_ch, da, s_ch, sa, details::algorithms::difference);
        break;
    }

    return std::clamp(res_ch / out_a, 0.0f, 1.0f);
  };

  core::rgbaf_t result{apply_blend(d.template get_index<0>(), s.template get_index<0>()),
                       apply_blend(d.template get_index<1>(), s.template get_index<1>()),
                       apply_blend(d.template get_index<2>(), s.template get_index<2>()), out_a};

  return color_cast<Color>(result);
}

}  // namespace colorcpp::operations::blend
