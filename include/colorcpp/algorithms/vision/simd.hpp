/**
 * @file simd.hpp
 * @brief Internal SIMD helpers for selected vision paths.
 */

#pragma once

#include <algorithm>
#include <colorcpp/algorithms/vision/details.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/detail/simd.hpp>

namespace colorcpp::algorithms::vision::details {

template <typename Matrix>
inline core::rgbaf_t simulate_machado_rgbaf_scalar(const core::rgbaf_t& rgb, float severity,
                                                   const Matrix& m_full) noexcept {
  severity = std::clamp(severity, 0.0f, 1.0f);
  const float t = 1.0f - severity;
  const float alpha = rgb.template get_index<3>();

  const float r_lin = linearize(rgb.template get_index<0>());
  const float g_lin = linearize(rgb.template get_index<1>());
  const float b_lin = linearize(rgb.template get_index<2>());

  const float out_r =
      (t + severity * m_full[0][0]) * r_lin + (severity * m_full[0][1]) * g_lin + (severity * m_full[0][2]) * b_lin;
  const float out_g =
      (severity * m_full[1][0]) * r_lin + (t + severity * m_full[1][1]) * g_lin + (severity * m_full[1][2]) * b_lin;
  const float out_b =
      (severity * m_full[2][0]) * r_lin + (severity * m_full[2][1]) * g_lin + (t + severity * m_full[2][2]) * b_lin;

  return core::rgbaf_t{gamma_encode(out_r), gamma_encode(out_g), gamma_encode(out_b), alpha};
}

// This path stays behind an extra opt-in for now: current scalar APIs still spend most
// of their time in gamma/linearize work, so SSE2 only sped up the matrix stage and did
// not show a reliable end-to-end win in release benchmarks.
inline constexpr bool machado_fast_path_requested() noexcept {
#if defined(COLORCPP_ENABLE_SIMD) && defined(COLORCPP_ENABLE_EXPERIMENTAL_VISION_MACHADO_SIMD)
  return true;
#else
  return false;
#endif
}

inline constexpr bool has_machado_fast_path() noexcept {
  return machado_fast_path_requested() && colorcpp::detail::simd::has_sse2;
}

#if defined(COLORCPP_ENABLE_SIMD) && defined(COLORCPP_ENABLE_EXPERIMENTAL_VISION_MACHADO_SIMD) && \
    (defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2))

template <typename Matrix>
inline core::rgbaf_t simulate_machado_rgbaf_sse2(const core::rgbaf_t& rgb, float severity,
                                                 const Matrix& m_full) noexcept {
  severity = std::clamp(severity, 0.0f, 1.0f);
  const float t = 1.0f - severity;
  const float alpha = rgb.template get_index<3>();

  const float r_lin = linearize(rgb.template get_index<0>());
  const float g_lin = linearize(rgb.template get_index<1>());
  const float b_lin = linearize(rgb.template get_index<2>());

  const __m128 col_r = _mm_setr_ps(t + severity * m_full[0][0], severity * m_full[1][0], severity * m_full[2][0], 0.0f);
  const __m128 col_g = _mm_setr_ps(severity * m_full[0][1], t + severity * m_full[1][1], severity * m_full[2][1], 0.0f);
  const __m128 col_b = _mm_setr_ps(severity * m_full[0][2], severity * m_full[1][2], t + severity * m_full[2][2], 0.0f);

  const __m128 out_lin =
      _mm_add_ps(_mm_add_ps(_mm_mul_ps(col_r, _mm_set1_ps(r_lin)), _mm_mul_ps(col_g, _mm_set1_ps(g_lin))),
                 _mm_mul_ps(col_b, _mm_set1_ps(b_lin)));

  const float out_r = _mm_cvtss_f32(out_lin);
  const float out_g = _mm_cvtss_f32(_mm_shuffle_ps(out_lin, out_lin, _MM_SHUFFLE(1, 1, 1, 1)));
  const float out_b = _mm_cvtss_f32(_mm_shuffle_ps(out_lin, out_lin, _MM_SHUFFLE(2, 2, 2, 2)));

  return core::rgbaf_t{gamma_encode(out_r), gamma_encode(out_g), gamma_encode(out_b), alpha};
}

#endif

template <typename Matrix>
inline bool try_simulate_machado_rgbaf_fast_path(core::rgbaf_t& out, const core::rgbaf_t& rgb, float severity,
                                                 const Matrix& m_full) noexcept {
#if defined(COLORCPP_ENABLE_SIMD) && defined(COLORCPP_ENABLE_EXPERIMENTAL_VISION_MACHADO_SIMD) && \
    (defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2))
  out = simulate_machado_rgbaf_sse2(rgb, severity, m_full);
  return true;
#else
  (void)out;
  (void)rgb;
  (void)severity;
  (void)m_full;
  return false;
#endif
}

}  // namespace colorcpp::algorithms::vision::details
