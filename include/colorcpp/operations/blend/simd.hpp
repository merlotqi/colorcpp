/**
 * @file simd.hpp
 * @brief Internal SIMD helpers for selected separable blend modes.
 */

#pragma once

#include <algorithm>
#include <array>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/detail/simd.hpp>
#include <colorcpp/operations/blend/mode.hpp>

namespace colorcpp::operations::blend::details {

inline constexpr bool has_simd_fast_path(blend_mode mode) noexcept {
  switch (mode) {
    case blend_mode::normal:
    case blend_mode::multiply:
    case blend_mode::screen:
    case blend_mode::overlay:
    case blend_mode::darken:
    case blend_mode::lighten:
    case blend_mode::addition:
    case blend_mode::subtraction:
    case blend_mode::difference:
    case blend_mode::exclusion:
    case blend_mode::hard_light:
      return true;
    default:
      return false;
  }
}

#if defined(COLORCPP_ENABLE_SIMD) && (defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2))

inline __m128 load_rgbx_sse2(const core::rgbaf_t& c) {
  return _mm_setr_ps(c.template get_index<0>(), c.template get_index<1>(), c.template get_index<2>(), 0.0f);
}

inline __m128 clamp01_sse2(__m128 v) { return _mm_min_ps(_mm_max_ps(v, _mm_set1_ps(0.0f)), _mm_set1_ps(1.0f)); }

inline __m128 blend_select_sse2(__m128 mask, __m128 when_true, __m128 when_false) {
  return _mm_or_ps(_mm_and_ps(mask, when_true), _mm_andnot_ps(mask, when_false));
}

inline __m128 apply_mode_sse2(blend_mode mode, __m128 dst, __m128 src) {
  const __m128 zero = _mm_set1_ps(0.0f);
  const __m128 one = _mm_set1_ps(1.0f);
  const __m128 half = _mm_set1_ps(0.5f);
  const __m128 two = _mm_set1_ps(2.0f);

  switch (mode) {
    case blend_mode::normal:
      return src;
    case blend_mode::multiply:
      return _mm_mul_ps(dst, src);
    case blend_mode::screen:
      return _mm_sub_ps(one, _mm_mul_ps(_mm_sub_ps(one, dst), _mm_sub_ps(one, src)));
    case blend_mode::overlay: {
      const __m128 low = _mm_mul_ps(_mm_mul_ps(two, dst), src);
      const __m128 high = _mm_sub_ps(one, _mm_mul_ps(two, _mm_mul_ps(_mm_sub_ps(one, dst), _mm_sub_ps(one, src))));
      return blend_select_sse2(_mm_cmplt_ps(dst, half), low, high);
    }
    case blend_mode::darken:
      return _mm_min_ps(dst, src);
    case blend_mode::lighten:
      return _mm_max_ps(dst, src);
    case blend_mode::addition:
      return _mm_min_ps(one, _mm_add_ps(dst, src));
    case blend_mode::subtraction:
      return _mm_max_ps(zero, _mm_sub_ps(dst, src));
    case blend_mode::difference: {
      const __m128 sign_mask = _mm_set1_ps(-0.0f);
      return _mm_andnot_ps(sign_mask, _mm_sub_ps(dst, src));
    }
    case blend_mode::exclusion:
      return _mm_sub_ps(_mm_add_ps(dst, src), _mm_mul_ps(two, _mm_mul_ps(dst, src)));
    case blend_mode::hard_light:
      return apply_mode_sse2(blend_mode::overlay, src, dst);
    default:
      return src;
  }
}

inline core::rgbaf_t blend_rgbaf_sse2(const core::rgbaf_t& dst, const core::rgbaf_t& src, blend_mode mode,
                                      float opacity) {
  const float da = dst.template get_index<3>();
  const float sa = src.template get_index<3>() * std::clamp(opacity, 0.0f, 1.0f);
  const float out_a = sa + da * (1.0f - sa);

  if (out_a <= 0.0f) return core::rgbaf_t{0.0f, 0.0f, 0.0f, 0.0f};

  const __m128 vd = load_rgbx_sse2(dst);
  const __m128 vs = load_rgbx_sse2(src);
  const __m128 vm = apply_mode_sse2(mode, vd, vs);

  const __m128 v_src = _mm_mul_ps(vs, _mm_set1_ps(sa * (1.0f - da)));
  const __m128 v_dst = _mm_mul_ps(vd, _mm_set1_ps(da * (1.0f - sa)));
  const __m128 v_mix = _mm_mul_ps(vm, _mm_set1_ps(da * sa));
  const __m128 v_out = clamp01_sse2(_mm_div_ps(_mm_add_ps(_mm_add_ps(v_src, v_dst), v_mix), _mm_set1_ps(out_a)));

  alignas(16) std::array<float, 4> out{};
  _mm_store_ps(out.data(), v_out);
  return core::rgbaf_t{out[0], out[1], out[2], out_a};
}

#endif

inline bool try_blend_rgbaf_fast_path(core::rgbaf_t& out, const core::rgbaf_t& dst, const core::rgbaf_t& src,
                                      blend_mode mode, float opacity) {
  if (!has_simd_fast_path(mode)) return false;

#if defined(COLORCPP_ENABLE_SIMD) && (defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2))
  out = blend_rgbaf_sse2(dst, src, mode, opacity);
  return true;
#else
  (void)out;
  (void)dst;
  (void)src;
  (void)mode;
  (void)opacity;
  return false;
#endif
}

}  // namespace colorcpp::operations::blend::details
