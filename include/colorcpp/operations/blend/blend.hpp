/**
 * @file blend.hpp
 * @brief Main blend function implementation.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/operations/blend/algorithms.hpp>
#include <colorcpp/operations/blend/details.hpp>
#include <colorcpp/operations/blend/mode.hpp>
#include <colorcpp/operations/blend/non_separable.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::operations::blend {

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
