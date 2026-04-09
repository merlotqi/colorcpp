/**
 * @file simd.hpp
 * @brief Internal SIMD helpers for selected delta-E paths.
 */

#pragma once

#include <cmath>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/detail/simd.hpp>

namespace colorcpp::algorithms::delta_e::details {

inline float delta_e_ok_scalar(const core::oklab_t& a, const core::oklab_t& b) noexcept {
  const float dL = a.template get_index<0>() - b.template get_index<0>();
  const float da = a.template get_index<1>() - b.template get_index<1>();
  const float db = a.template get_index<2>() - b.template get_index<2>();

  return std::sqrt(dL * dL + da * da + db * db);
}

inline constexpr bool has_delta_e_ok_fast_path() noexcept {
#if defined(COLORCPP_ENABLE_SIMD) && (defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2))
  return true;
#else
  return false;
#endif
}

#if defined(COLORCPP_ENABLE_SIMD) && (defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2))

inline __m128 load_oklab_x_sse2(const core::oklab_t& c) {
  return _mm_setr_ps(c.template get_index<0>(), c.template get_index<1>(), c.template get_index<2>(), 0.0f);
}

inline float delta_e_ok_sse2(const core::oklab_t& a, const core::oklab_t& b) noexcept {
  const __m128 diff = _mm_sub_ps(load_oklab_x_sse2(a), load_oklab_x_sse2(b));
  const __m128 sq = _mm_mul_ps(diff, diff);
  const __m128 sum = _mm_add_ps(sq, _mm_movehl_ps(sq, sq));
  const __m128 total = _mm_add_ss(sum, _mm_shuffle_ps(sum, sum, _MM_SHUFFLE(1, 1, 1, 1)));
  return _mm_cvtss_f32(_mm_sqrt_ss(total));
}

#endif

inline bool try_delta_e_ok_fast_path(float& out, const core::oklab_t& a, const core::oklab_t& b) noexcept {
#if defined(COLORCPP_ENABLE_SIMD) && (defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2))
  out = delta_e_ok_sse2(a, b);
  return true;
#else
  (void)out;
  (void)a;
  (void)b;
  return false;
#endif
}

}  // namespace colorcpp::algorithms::delta_e::details
