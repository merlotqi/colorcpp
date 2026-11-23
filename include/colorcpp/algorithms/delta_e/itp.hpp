/**
 * @file itp.hpp
 * @brief ΔE_ITP colour difference (ITU-R BT.2124) and back-compat wrappers
 *        for the ICtCp encoding, which now lives in @ref core::ictcp_t.
 *
 * The XYZ <-> ICtCp maths moved to
 * @ref colorcpp::operations::conversion::details (functions/ictcp.hpp) so
 * color_cast and these wrappers share one implementation; the wrappers below
 * keep the historical delta_e entry points working.
 *
 * ΔE_ITP = 720 √(ΔI² + (ΔCt/2)² + ΔCp²); a value of 1 is roughly a
 * just-noticeable difference under critical viewing.
 *
 * @see Lu et al. (2016). A Perceptual EOTF for High Dynamic Range Content.
 * @see ITU-R BT.2124 (2019).
 */

#pragma once

#include <cmath>
#include <colorcpp/core/ictcp.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::delta_e {

/** @brief ICtCp encoding triplet — alias of @ref core::ictcp_t (moved there). */
using ictcp_t = colorcpp::core::ictcp_t;

/**
 * @brief XYZ (D65, relative) -> ICtCp.
 *
 * Thin wrapper over the registered conversion edge (scale convention:
 * relative Y = 1 maps to the 100 cd/m^2 reference white).
 */
inline ictcp_t xyz_to_ictcp(const core::xyz_t& xyz) { return operations::conversion::color_cast<ictcp_t>(xyz); }

/** @brief ICtCp -> XYZ (D65, relative). Inverse of @ref xyz_to_ictcp. */
inline core::xyz_t ictcp_to_xyz(const ictcp_t& c) { return operations::conversion::color_cast<core::xyz_t>(c); }

/**
 * @brief ΔE_ITP colour difference (ITU-R BT.2124).
 *
 * 720 √(ΔI² + (ΔCt/2)² + ΔCp²). A value of 1 ≈ just-noticeable difference
 * under the most critical adaptation state.
 *
 * @param a, b ICtCp encodings (e.g. from @ref xyz_to_ictcp or any colour
 *             cast to XYZ first).
 */
inline float delta_e_itp(const ictcp_t& a, const ictcp_t& b) {
  const float d_i = b.i() - a.i();
  const float d_t = (b.ct() - a.ct()) * 0.5f;
  const float d_p = b.cp() - a.cp();
  return 720.0f * std::sqrt(d_i * d_i + d_t * d_t + d_p * d_p);
}

}  // namespace colorcpp::algorithms::delta_e
