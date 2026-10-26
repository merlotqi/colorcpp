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
#include <colorcpp/operations/blend/simd.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::operations::blend {

namespace details {

inline bool is_non_separable(blend_mode mode) noexcept {
  return mode == blend_mode::hue || mode == blend_mode::saturation || mode == blend_mode::color_blend ||
         mode == blend_mode::luminosity;
}

/**
 * @brief Core compositing on @c rgbaf_t working channels.
 *
 * Channels at the API boundary are straight (un-premultiplied); the
 * Porter-Duff steps below weight them by alpha (premultiplied form,
 * W3C Compositing and Blending Level 1 general formula) and the result is
 * un-premultiplied again via division by the output alpha.
 */
inline core::rgbaf_t blend_rgbaf(const core::rgbaf_t& d, const core::rgbaf_t& s, blend_mode mode, float opacity) {
  core::rgbaf_t fast_result;
  if (try_blend_rgbaf_fast_path(fast_result, d, s, mode, opacity)) {
    return fast_result;
  }

  float da = d.template get_index<3>();
  float sa = s.template get_index<3>() * std::clamp(opacity, 0.0f, 1.0f);

  float out_a = sa + da * (1.0f - sa);

  if (is_non_separable(mode)) {
    if (out_a <= 0.0f) return core::rgbaf_t{0.0f, 0.0f, 0.0f, 0.0f};

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

    return core::rgbaf_t{comp3(d3.r, s3.r, mixed.r), comp3(d3.g, s3.g, mixed.g), comp3(d3.b, s3.b, mixed.b), out_a};
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

  return core::rgbaf_t{apply(d.template get_index<0>(), s.template get_index<0>()),
                       apply(d.template get_index<1>(), s.template get_index<1>()),
                       apply(d.template get_index<2>(), s.template get_index<2>()), out_a};
}

}  // namespace details

/**
 * @brief Composite @p src over @p dst using @p mode and effective source alpha scaled by @p opacity.
 *
 * Compositing contract: channels are straight (un-premultiplied) alpha at the
 * API boundary; the Porter-Duff steps are computed in premultiplied form
 * (Cs·αs·(1-αd) + Cd·αd·(1-αs) + B·αs·αd, W3C Compositing and Blending Level 1)
 * and un-premultiplied via division by the output alpha. Results are clamped
 * to [0, 1].
 *
 * @tparam Color Any color type supported by @ref conversion::color_cast to @c rgbaf_t.
 * @param dst Destination (bottom) color.
 * @param src Source (top) color.
 * @param mode Blend/compositing mode (W3C-style for RGB; special modes use HSL luminance/saturation decomposition).
 * @param opacity Extra multiplier on source alpha, clamped to [0, 1].
 * @param space Working space for the blend formulas — @c blend_space::srgb (default,
 *        W3C/CSS parity: formulas on gamma-encoded sRGB) or @c blend_space::linear
 *        (linearize, composite, re-encode: "linear light" compositing). Ignored for
 *        the non-separable modes, which W3C defines on sRGB HSL only.
 */
template <typename Color>
Color blend(const Color& dst, const Color& src, blend_mode mode = blend_mode::normal, float opacity = 1.0f,
            blend_space space = blend_space::srgb) {
  using namespace conversion;

  auto d = color_cast<core::rgbaf_t>(dst);
  auto s = color_cast<core::rgbaf_t>(src);

  const bool use_linear = (space == blend_space::linear) && !details::is_non_separable(mode);
  if (use_linear) {
    d = details::to_linear_working(d);
    s = details::to_linear_working(s);
  }

  core::rgbaf_t result = details::blend_rgbaf(d, s, mode, opacity);

  if (use_linear) result = details::from_linear_working(result);
  return color_cast<Color>(result);
}

}  // namespace colorcpp::operations::blend
