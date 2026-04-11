/**
 * @file simd.hpp
 * @brief Internal compile-time SIMD capability detection.
 *
 * SIMD fast paths in colorcpp are opt-in via @c COLORCPP_ENABLE_SIMD and fall back to
 * the scalar implementation when the current compiler/target does not expose a supported ISA.
 */

#pragma once

#if defined(COLORCPP_ENABLE_SIMD) && (defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2))
#include <emmintrin.h>
#endif

#if defined(COLORCPP_ENABLE_SIMD) && (defined(__ARM_NEON) || defined(__ARM_NEON__))
#include <arm_neon.h>
#endif

namespace colorcpp::detail::simd {

inline constexpr bool enabled =
#if defined(COLORCPP_ENABLE_SIMD)
    true;
#else
    false;
#endif

inline constexpr bool has_sse2 =
#if defined(COLORCPP_ENABLE_SIMD) && (defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2))
    true;
#else
    false;
#endif

inline constexpr bool has_neon =
#if defined(COLORCPP_ENABLE_SIMD) && (defined(__ARM_NEON) || defined(__ARM_NEON__))
    true;
#else
    false;
#endif

inline constexpr bool available = has_sse2 || has_neon;

}  // namespace colorcpp::detail::simd
