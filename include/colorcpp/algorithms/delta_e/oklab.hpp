/**
 * @file oklab.hpp
 * @brief Euclidean color difference in Oklab (Björn Ottosson).
 *
 * This is **not** CIEDE2000 / ΔE*ab: those operate in CIELAB D65 (see @ref de76.hpp, @ref de2000.hpp).
 * Oklab Euclidean distance aligns with CSS Color 4/5 interpolation and with @ref gamut::gamut_clip_perceptual,
 * which uses a ~0.02 Oklab-space JND-style threshold.
 *
 * @see https://www.w3.org/TR/css-color-4/#ok-lab
 */

#pragma once

#include <colorcpp/algorithms/delta_e/simd.hpp>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::delta_e {

/** @brief Typical small-notice threshold in Oklab Euclidean space (matches gamut perceptual clip search). */
inline constexpr float oklab_jnd_typical = 0.02f;

/**
 * @brief Euclidean distance √(ΔL² + Δa² + Δb²) in Oklab.
 * @tparam ColorA, ColorB Any types supported by @ref operations::conversion::color_cast to @c oklab_t.
 */
template <typename ColorA, typename ColorB>
float delta_e_ok(const ColorA& a, const ColorB& b) {
  using namespace operations::conversion;

  const auto la = color_cast<core::oklab_t>(a);
  const auto lb = color_cast<core::oklab_t>(b);

  float out = 0.0f;
  if (details::try_delta_e_ok_fast_path(out, la, lb)) {
    return out;
  }

  return details::delta_e_ok_scalar(la, lb);
}

/**
 * @brief True if @ref delta_e_ok is below @p threshold (default @ref oklab_jnd_typical).
 */
template <typename ColorA, typename ColorB>
bool is_visually_same_ok(const ColorA& a, const ColorB& b, float threshold = oklab_jnd_typical) {
  return delta_e_ok(a, b) < threshold;
}

}  // namespace colorcpp::algorithms::delta_e
