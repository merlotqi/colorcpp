/**
 * @file delta_e.hpp
 * @brief Color difference metrics: CIELAB D65 (ΔE76 / ΔE94 / ΔE2000) and Oklab Euclidean (ΔE_OK).
 *
 * This is an aggregate header that includes all delta_e components.
 * Individual components can be included separately for faster compilation.
 *
 * @note **CIELAB metrics** (@c delta_e_76, @c delta_e_94, @c delta_e_2000): D65, perceptual scale differs from Oklab.
 * @note **Oklab** (@ref delta_e::delta_e_ok): Euclidean in Oklab; use @ref delta_e::oklab_jnd_typical (~0.02) for a
 *       small-difference threshold consistent with @ref gamut::gamut_clip_perceptual.
 * @note Rule-of-thumb for ΔE2000: <1 imperceptible, 1–2 close inspection, 2–10 obvious, >50 very different.
 */

#pragma once

#include <colorcpp/algorithms/delta_e/de2000.hpp>
#include <colorcpp/algorithms/delta_e/de76.hpp>
#include <colorcpp/algorithms/delta_e/de94.hpp>
#include <colorcpp/algorithms/delta_e/helpers.hpp>
#include <colorcpp/algorithms/delta_e/oklab.hpp>

namespace colorcpp::algorithms::delta_e {
// All functionality is provided by the included sub-modules
}
