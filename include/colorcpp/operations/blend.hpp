/**
 * @file blend.hpp
 * @brief Porter–Duff–style compositing and W3C-style blend modes between two colors (converted via @ref
 * conversion::color_cast).
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/operations/conversion.hpp>

#include "colorcpp/core/rgb.hpp"

/** @brief Alpha-aware blending in linearized sRGB space with selectable blend mode. */
namespace colorcpp::operations::blend {

/** @brief Separable RGB blend modes plus HSL-channel blend modes (hue, saturation, color, luminosity). */
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
  exclusion,    // a + b - 2ab  (low-contrast difference)
  hard_light,   // overlay with src/dst roles swapped
  soft_light,   // subtle dodge/burn (W3C formula)
  color_dodge,  // brighten dst toward src
  color_burn,   // darken dst toward src
  divide,       // dst / src

  hue,          // hue of src,  saturation+luminosity of dst
  saturation,   // saturation of src, hue+luminosity of dst
  color_blend,  // hue+saturation of src, luminosity of dst
  luminosity,   // luminosity of src, hue+saturation of dst
};

namespace details {

// Separable blend functions (per-channel, inputs in [0,1])

struct algorithms {
  static constexpr float normal(float /*a*/, float b) { return b; }
  static constexpr float multiply(float a, float b) { return a * b; }
  static constexpr float screen(float a, float b) { return 1.0f - (1.0f - a) * (1.0f - b); }
  static constexpr float darken(float a, float b) { return std::min(a, b); }
  static constexpr float lighten(float a, float b) { return std::max(a, b); }
  static constexpr float addition(float a, float b) { return std::min(1.0f, a + b); }
  static constexpr float subtraction(float a, float b) { return std::max(0.0f, a - b); }
  static constexpr float difference(float a, float b) { return std::fabs(a - b); }
  static constexpr float exclusion(float a, float b) { return a + b - 2.0f * a * b; }

  static constexpr float overlay(float a, float b) {
    return (a < 0.5f) ? (2.0f * a * b) : (1.0f - 2.0f * (1.0f - a) * (1.0f - b));
  }

  // hard_light = overlay with roles swapped (src controls the threshold)
  static constexpr float hard_light(float a, float b) { return overlay(b, a); }

  static constexpr float color_dodge(float a, float b) {
    if (b >= 1.0f) return 1.0f;
    return std::min(1.0f, a / (1.0f - b));
  }

  static constexpr float color_burn(float a, float b) {
    if (b <= 0.0f) return 0.0f;
    return 1.0f - std::min(1.0f, (1.0f - a) / b);
  }

  static constexpr float divide(float a, float b) {
    if (b <= 0.0f) return 1.0f;
    return std::min(1.0f, a / b);
  }

  // W3C soft-light formula (matches Photoshop behaviour).
  // Not constexpr: std::sqrt is not constexpr before C++23.
  static float soft_light(float a, float b) {
    if (b <= 0.5f) {
      return a - (1.0f - 2.0f * b) * a * (1.0f - a);
    }
    float d = (a <= 0.25f) ? (((16.0f * a - 12.0f) * a + 4.0f) * a) : std::sqrt(a);
    return a + (2.0f * b - 1.0f) * (d - a);
  }
};

// Porter-Duff general compositing: Cs·αs·(1-αd) + Cd·αd·(1-αs) + B·αs·αd

template <typename Fn>
constexpr float composite(float dst, float da, float src, float sa, Fn&& blend_fn) {
  float mixed = blend_fn(dst, src);
  return src * sa * (1.0f - da) + dst * da * (1.0f - sa) + da * sa * mixed;
}

struct rgb3 {
  float r, g, b;
};

inline float lum(rgb3 c) { return 0.299f * c.r + 0.587f * c.g + 0.114f * c.b; }

inline float sat(rgb3 c) { return std::max({c.r, c.g, c.b}) - std::min({c.r, c.g, c.b}); }

inline rgb3 clip_color(rgb3 c) {
  float l = lum(c);
  float n = std::min({c.r, c.g, c.b});
  float x = std::max({c.r, c.g, c.b});
  if (n < 0.0f) {
    // d == 0 when all channels are equal (achromatic): scaling is a no-op, skip.
    float d = l - n;
    if (d > 0.0f) {
      c.r = l + (c.r - l) * l / d;
      c.g = l + (c.g - l) * l / d;
      c.b = l + (c.b - l) * l / d;
    }
  }
  if (x > 1.0f) {
    // d == 0 when all channels are equal (achromatic): scaling is a no-op, skip.
    float d = x - l;
    if (d > 0.0f) {
      c.r = l + (c.r - l) * (1.0f - l) / d;
      c.g = l + (c.g - l) * (1.0f - l) / d;
      c.b = l + (c.b - l) * (1.0f - l) / d;
    }
  }
  return c;
}

inline rgb3 set_lum(rgb3 c, float l) {
  float d = l - lum(c);
  return clip_color({c.r + d, c.g + d, c.b + d});
}

// Adjusts saturation while preserving per-component hue ordering (W3C SetSat)
inline rgb3 set_sat(rgb3 c, float s) {
  float* p[3] = {&c.r, &c.g, &c.b};
  // 3-element sort so p[0]=min, p[1]=mid, p[2]=max
  if (*p[0] > *p[1]) std::swap(p[0], p[1]);
  if (*p[1] > *p[2]) std::swap(p[1], p[2]);
  if (*p[0] > *p[1]) std::swap(p[0], p[1]);

  float cmin = *p[0], cmid = *p[1], cmax = *p[2];
  if (cmax > cmin) {
    *p[1] = (cmid - cmin) * s / (cmax - cmin);
    *p[2] = s;
  } else {
    *p[1] = 0.0f;
    *p[2] = 0.0f;
  }
  *p[0] = 0.0f;
  return c;
}

inline rgb3 ns_hue(rgb3 dst, rgb3 src) { return set_lum(set_sat(src, sat(dst)), lum(dst)); }
inline rgb3 ns_saturation(rgb3 dst, rgb3 src) { return set_lum(set_sat(dst, sat(src)), lum(dst)); }
inline rgb3 ns_color(rgb3 dst, rgb3 src) { return set_lum(src, lum(dst)); }
inline rgb3 ns_luminosity(rgb3 dst, rgb3 src) { return set_lum(dst, lum(src)); }

}  // namespace details

/**
 * @brief Composite @p src over @p dst using @p mode and effective source alpha scaled by @p opacity.
 * @tparam Color Any color type supported by @ref conversion::color_cast to @c rgbaf_t.
 * @param dst Destination (bottom) color.
 * @param src Source (top) color.
 * @param mode Blend/compositing mode (W3C-style for RGB; special modes use HSL luminance/saturation decomposition).
 * @param opacity Extra multiplier on source alpha, clamped to [0, 1].
 */
template <typename Color>
Color blend(const Color& dst, const Color& src, blend_mode mode = blend_mode::normal, float opacity = 1.0f) {
  using namespace conversion;

  auto d = color_cast<core::rgbaf_t>(dst);
  auto s = color_cast<core::rgbaf_t>(src);

  float da = d.template get_index<3>();
  float sa = s.template get_index<3>() * std::clamp(opacity, 0.0f, 1.0f);

  float out_a = sa + da * (1.0f - sa);

  if (mode == blend_mode::hue || mode == blend_mode::saturation || mode == blend_mode::color_blend ||
      mode == blend_mode::luminosity) {
    if (out_a <= 0.0f) return color_cast<Color>(core::rgbaf_t{0.0f, 0.0f, 0.0f, 0.0f});

    details::rgb3 d3{d.template get_index<0>(), d.template get_index<1>(), d.template get_index<2>()};
    details::rgb3 s3{s.template get_index<0>(), s.template get_index<1>(), s.template get_index<2>()};

    details::rgb3 mixed{};
    switch (mode) {
      case blend_mode::hue:
        mixed = details::ns_hue(d3, s3);
        break;
      case blend_mode::saturation:
        mixed = details::ns_saturation(d3, s3);
        break;
      case blend_mode::color_blend:
        mixed = details::ns_color(d3, s3);
        break;
      case blend_mode::luminosity:
        mixed = details::ns_luminosity(d3, s3);
        break;
      default:
        mixed = s3;
    }

    auto comp3 = [&](float d_ch, float s_ch, float m_ch) {
      float res = s_ch * sa * (1.0f - da) + d_ch * da * (1.0f - sa) + da * sa * m_ch;
      return std::clamp(res / out_a, 0.0f, 1.0f);
    };

    core::rgbaf_t result{comp3(d3.r, s3.r, mixed.r), comp3(d3.g, s3.g, mixed.g), comp3(d3.b, s3.b, mixed.b), out_a};
    return color_cast<Color>(result);
  }

  auto apply = [&](float d_ch, float s_ch) -> float {
    if (out_a <= 0.0f) return 0.0f;

    float res_ch = 0.0f;
    using A = details::algorithms;
    switch (mode) {
      case blend_mode::normal:
        res_ch = details::composite(d_ch, da, s_ch, sa, A::normal);
        break;
      case blend_mode::multiply:
        res_ch = details::composite(d_ch, da, s_ch, sa, A::multiply);
        break;
      case blend_mode::screen:
        res_ch = details::composite(d_ch, da, s_ch, sa, A::screen);
        break;
      case blend_mode::overlay:
        res_ch = details::composite(d_ch, da, s_ch, sa, A::overlay);
        break;
      case blend_mode::darken:
        res_ch = details::composite(d_ch, da, s_ch, sa, A::darken);
        break;
      case blend_mode::lighten:
        res_ch = details::composite(d_ch, da, s_ch, sa, A::lighten);
        break;
      case blend_mode::addition:
        res_ch = details::composite(d_ch, da, s_ch, sa, A::addition);
        break;
      case blend_mode::subtraction:
        res_ch = details::composite(d_ch, da, s_ch, sa, A::subtraction);
        break;
      case blend_mode::difference:
        res_ch = details::composite(d_ch, da, s_ch, sa, A::difference);
        break;
      case blend_mode::exclusion:
        res_ch = details::composite(d_ch, da, s_ch, sa, A::exclusion);
        break;
      case blend_mode::hard_light:
        res_ch = details::composite(d_ch, da, s_ch, sa, A::hard_light);
        break;
      case blend_mode::soft_light:
        res_ch = details::composite(d_ch, da, s_ch, sa, A::soft_light);
        break;
      case blend_mode::color_dodge:
        res_ch = details::composite(d_ch, da, s_ch, sa, A::color_dodge);
        break;
      case blend_mode::color_burn:
        res_ch = details::composite(d_ch, da, s_ch, sa, A::color_burn);
        break;
      case blend_mode::divide:
        res_ch = details::composite(d_ch, da, s_ch, sa, A::divide);
        break;
      default:
        break;
    }

    return std::clamp(res_ch / out_a, 0.0f, 1.0f);
  };

  core::rgbaf_t result{apply(d.template get_index<0>(), s.template get_index<0>()),
                       apply(d.template get_index<1>(), s.template get_index<1>()),
                       apply(d.template get_index<2>(), s.template get_index<2>()), out_a};
  return color_cast<Color>(result);
}

}  // namespace colorcpp::operations::blend
