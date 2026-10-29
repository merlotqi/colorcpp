/**
 * @file chromatic_adaptation.hpp
 * @brief Chromatic adaptation transforms (Bradford, Von Kries, CAT02, CAT16, CMCCAT2000, XYZ scaling)
 *        and standard white point constants.
 *
 * Chromatic adaptation converts colors from one white point (illuminant) to another.
 * This is essential when working with color spaces that use different reference whites
 * (e.g., ProPhoto RGB uses D50, most others use D65).
 *
 * All transforms share the same von Kries construction — diagonal scaling in the
 * cone space defined by the transform matrix — and differ only in the cone matrix:
 *
 * - Bradford: recommended for most use cases (ICC profiles).
 * - Von Kries (HPE): simpler, less perceptually uniform.
 * - CAT02 / CAT16: CIECAM02 / CAM16 cone fundamentals; CAT16 is the prerequisite
 *   for CAM16-based appearance models, CAT02 for CIECAM02.
 * - CMCCAT2000: photography / HDR viewing-condition pipelines.
 * - XYZ scaling: plain per-channel white scaling — the von Kries part of the
 *   CIE 1994 chromatic adaptation model (the full CIE 1994 luminance-level
 *   adaptation model is not implemented).
 *
 * @see http://www.brucelindbloom.com/index.html?Eqn_ChromAdapt.html
 * @see Li, Li, Wang, Zu, Luo, Melgosa, Brill, Pointer (2017). Comprehensive color
 *      solutions: CAM16, CAT16, and CAM16-UCS. Color Research & Application.
 * @see Westland, Cheung, Lo (2012). CMCCAT2000. Computational Colour Science Using MATLAB.
 */

#pragma once

#include <cmath>
#include <colorcpp/core/xyz.hpp>
namespace colorcpp::operations::conversion {
// Forward declaration — full definition in conversion.hpp
template <typename To, typename From>
constexpr To color_cast(const From& src);
}  // namespace colorcpp::operations::conversion

namespace colorcpp::algorithms::chromatic_adaptation {

// =============================================================================
// Standard White Points (CIE 1931 XYZ tristimulus, Y = 1.0)
// =============================================================================

/// D65 — CIE Standard Illuminant (noon daylight, correlated color temperature ~6504 K)
inline constexpr core::xyz_t WHITEPOINT_D65{0.95047f, 1.00000f, 1.08883f};

/// D50 — CIE Standard Illuminant (horizon daylight, CCT ~5003 K)
inline constexpr core::xyz_t WHITEPOINT_D50{0.96422f, 1.00000f, 0.82521f};

/// D55 — CIE Standard Illuminant (mid-morning/mid-afternoon daylight, CCT ~5503 K)
inline constexpr core::xyz_t WHITEPOINT_D55{0.95682f, 1.00000f, 0.92149f};

/// A — CIE Standard Illuminant (tungsten filament, CCT ~2856 K)
inline constexpr core::xyz_t WHITEPOINT_A{1.09850f, 1.00000f, 0.35585f};

/// C — CIE Standard Illuminant (filtered tungsten / average daylight, CCT ~6774 K)
inline constexpr core::xyz_t WHITEPOINT_C{0.98074f, 1.00000f, 1.18232f};

// =============================================================================
// Shared von Kries construction (diagonal scaling in a cone space)
// =============================================================================

namespace details {

/**
 * @brief Von Kries-style adaptation in the cone space defined by @p m.
 *
 * xyz' = m_inv * diag(rho_dst / rho_src) * m * xyz, where rho = m * white.
 * Every concrete transform in this header is this construction with a
 * different cone matrix.
 */
template <typename ColorT>
ColorT cone_scale_adapt(const float (&m)[9], const float (&m_inv)[9], const ColorT& color, const core::xyz_t& src_white,
                        const core::xyz_t& dst_white) {
  using namespace operations::conversion;

  auto src = color_cast<core::xyz_t>(color);

  // Compute cone responses of source and destination whites
  float rho_src[3], rho_dst[3];
  for (int i = 0; i < 3; ++i) {
    rho_src[i] =
        m[i * 3 + 0] * (&src_white.x())[0] + m[i * 3 + 1] * (&src_white.y())[0] + m[i * 3 + 2] * (&src_white.z())[0];
    rho_dst[i] =
        m[i * 3 + 0] * (&dst_white.x())[0] + m[i * 3 + 1] * (&dst_white.y())[0] + m[i * 3 + 2] * (&dst_white.z())[0];
  }

  // Convert source color to cone responses
  float cone[3];
  for (int i = 0; i < 3; ++i) {
    cone[i] = m[i * 3 + 0] * src.x() + m[i * 3 + 1] * src.y() + m[i * 3 + 2] * src.z();
  }

  // Apply diagonal scaling
  for (int i = 0; i < 3; ++i) {
    if (rho_src[i] != 0.0f) {
      cone[i] *= rho_dst[i] / rho_src[i];
    }
  }

  // Convert back to XYZ
  float x = m_inv[0] * cone[0] + m_inv[1] * cone[1] + m_inv[2] * cone[2];
  float y = m_inv[3] * cone[0] + m_inv[4] * cone[1] + m_inv[5] * cone[2];
  float z = m_inv[6] * cone[0] + m_inv[7] * cone[1] + m_inv[8] * cone[2];

  return color_cast<ColorT>(core::xyz_t{x, y, z});
}

// Von Kries LMS matrix (Hunt-Pointer-Estevez, normalized to D65)
inline constexpr float M_VK[9] = {
    0.40024f, 0.70760f, -0.08081f, -0.22630f, 1.16532f, 0.04570f, 0.00000f, 0.00000f, 0.91822f,
};

// Inverse Von Kries LMS matrix
inline constexpr float M_VK_inv[9] = {
    1.8599364f, -1.1293816f, 0.2198974f, 0.3611914f, 0.6388125f, -0.0000064f, 0.0000000f, 0.0000000f, 1.0890636f,
};

}  // namespace details

/**
 * @brief Von Kries chromatic adaptation transform.
 *
 * Converts a color from @p src_white to @p dst_white using diagonal scaling
 * in Hunt-Pointer-Estevez LMS cone response space. Simpler and faster than
 * Bradford, but less perceptually uniform for large white point shifts.
 *
 * @tparam ColorT Any color type supported by conversion::color_cast to/from xyz_t.
 * @param color Input color under src_white illuminant.
 * @param src_white XYZ tristimulus of the source white point.
 * @param dst_white XYZ tristimulus of the destination white point.
 * @return Color adapted to dst_white illuminant.
 */
template <typename ColorT>
ColorT von_kries_adapt(const ColorT& color, const core::xyz_t& src_white, const core::xyz_t& dst_white) {
  return details::cone_scale_adapt(details::M_VK, details::M_VK_inv, color, src_white, dst_white);
}

// =============================================================================
// CAT02 / CAT16 / CMCCAT2000 / XYZ scaling cone matrices
// =============================================================================

namespace details {

// Bradford LMS matrix (from Bruce Lindbloom)
inline constexpr float M_BFD[9] = {
    0.8951000f, 0.2664000f, -0.1614000f, -0.7502000f, 1.7135000f, 0.0367000f, 0.0389000f, -0.0685000f, 1.0296000f,
};

// Inverse Bradford LMS matrix
inline constexpr float M_BFD_inv[9] = {
    0.9869929f, -0.1470543f, 0.1599627f, 0.4323053f, 0.5183603f, 0.0492912f, -0.0085287f, 0.0400428f, 0.9684867f,
};

// CAT02 cone matrix (CIECAM02; Li et al. 2017 dataset)
inline constexpr float M_CAT02[9] = {
    0.7328f, 0.4296f, -0.1624f, -0.7036f, 1.6975f, 0.0061f, 0.0030f, 0.0136f, 0.9834f,
};

// Inverse CAT02 cone matrix
inline constexpr float M_CAT02_inv[9] = {
    1.0961238f, -0.2788690f, 0.1827452f, 0.4543690f, 0.4735332f, 0.0720978f, -0.0096276f, -0.0056980f, 1.0153256f,
};

// CAT16 cone matrix (CAM16; Li et al. 2017)
inline constexpr float M_CAT16[9] = {
    0.401288f, 0.650173f, -0.051461f, -0.250268f, 1.204414f, 0.045854f, -0.002079f, 0.048952f, 0.953127f,
};

// Inverse CAT16 cone matrix
inline constexpr float M_CAT16_inv[9] = {
    1.8620679f, -1.0112546f, 0.1491868f, 0.3875265f, 0.6214474f, -0.0089740f, -0.0158415f, -0.0341229f, 1.0499644f,
};

// CMCCAT2000 cone matrix (Westland et al. 2012)
inline constexpr float M_CMCCAT2000[9] = {
    0.7982f, 0.3389f, -0.1371f, -0.5918f, 1.5512f, 0.0406f, 0.0008f, 0.0239f, 0.9753f,
};

// Inverse CMCCAT2000 cone matrix
inline constexpr float M_CMCCAT2000_inv[9] = {
    1.0764500f, -0.2376624f, 0.1612123f, 0.4109643f, 0.5543418f, 0.0346939f, -0.0109538f, -0.0133894f, 1.0243431f,
};

// XYZ scaling = identity cone matrix (von Kries applied directly to XYZ)
inline constexpr float M_XYZ_IDENTITY[9] = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};

}  // namespace details

/**
 * @brief Bradford chromatic adaptation transform.
 *
 * Converts a color from @p src_white to @p dst_white using the Bradford
 * LMS cone response matrix. This is the recommended method for most use
 * cases — it is used in ICC profiles and provides perceptually accurate
 * results across a wide range of white point shifts.
 *
 * @tparam ColorT Any color type supported by conversion::color_cast to/from xyz_t.
 * @param color Input color under src_white illuminant.
 * @param src_white XYZ tristimulus of the source white point.
 * @param dst_white XYZ tristimulus of the destination white point.
 * @return Color adapted to dst_white illuminant.
 */
template <typename ColorT>
ColorT bradford_adapt(const ColorT& color, const core::xyz_t& src_white, const core::xyz_t& dst_white) {
  return details::cone_scale_adapt(details::M_BFD, details::M_BFD_inv, color, src_white, dst_white);
}

/**
 * @brief CAT02 chromatic adaptation transform (CIECAM02 cone fundamentals).
 *
 * @tparam ColorT Any color type supported by conversion::color_cast to/from xyz_t.
 * @param color Input color under src_white illuminant.
 * @param src_white XYZ tristimulus of the source white point.
 * @param dst_white XYZ tristimulus of the destination white point.
 * @return Color adapted to dst_white illuminant.
 */
template <typename ColorT>
ColorT cat02_adapt(const ColorT& color, const core::xyz_t& src_white, const core::xyz_t& dst_white) {
  return details::cone_scale_adapt(details::M_CAT02, details::M_CAT02_inv, color, src_white, dst_white);
}

/**
 * @brief CAT16 chromatic adaptation transform (CAM16 cone fundamentals).
 *
 * Drop-in replacement for CAT02 in appearance-model pipelines; the slightly
 * different cone matrix improves numerical behavior at extreme luminances.
 *
 * @tparam ColorT Any color type supported by conversion::color_cast to/from xyz_t.
 * @param color Input color under src_white illuminant.
 * @param src_white XYZ tristimulus of the source white point.
 * @param dst_white XYZ tristimulus of the destination white point.
 * @return Color adapted to dst_white illuminant.
 */
template <typename ColorT>
ColorT cat16_adapt(const ColorT& color, const core::xyz_t& src_white, const core::xyz_t& dst_white) {
  return details::cone_scale_adapt(details::M_CAT16, details::M_CAT16_inv, color, src_white, dst_white);
}

/**
 * @brief CMCCAT2000 chromatic adaptation transform.
 *
 * Von Kries scaling in the CMCCAT2000 cone space. Commonly used in
 * photography and HDR viewing-condition pipelines.
 *
 * @tparam ColorT Any color type supported by conversion::color_cast to/from xyz_t.
 * @param color Input color under src_white illuminant.
 * @param src_white XYZ tristimulus of the source white point.
 * @param dst_white XYZ tristimulus of the destination white point.
 * @return Color adapted to dst_white illuminant.
 */
template <typename ColorT>
ColorT cmccat2000_adapt(const ColorT& color, const core::xyz_t& src_white, const core::xyz_t& dst_white) {
  return details::cone_scale_adapt(details::M_CMCCAT2000, details::M_CMCCAT2000_inv, color, src_white, dst_white);
}

/**
 * @brief XYZ scaling chromatic adaptation (per-channel white scaling).
 *
 * Scales X, Y, Z independently by the destination/source white ratios —
 * the von Kries part of the CIE 1994 chromatic adaptation model. Provided
 * mainly as a baseline and for CIE 1994 compatibility checks.
 *
 * @tparam ColorT Any color type supported by conversion::color_cast to/from xyz_t.
 * @param color Input color under src_white illuminant.
 * @param src_white XYZ tristimulus of the source white point.
 * @param dst_white XYZ tristimulus of the destination white point.
 * @return Color adapted to dst_white illuminant.
 */
template <typename ColorT>
ColorT xyz_scaling_adapt(const ColorT& color, const core::xyz_t& src_white, const core::xyz_t& dst_white) {
  return details::cone_scale_adapt(details::M_XYZ_IDENTITY, details::M_XYZ_IDENTITY, color, src_white, dst_white);
}

}  // namespace colorcpp::algorithms::chromatic_adaptation
