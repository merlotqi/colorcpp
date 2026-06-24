/**
 * @file chromatic_adaptation.hpp
 * @brief Chromatic adaptation transforms (Bradford, Von Kries) and standard white point constants.
 *
 * Chromatic adaptation converts colors from one white point (illuminant) to another.
 * This is essential when working with color spaces that use different reference whites
 * (e.g., ProPhoto RGB uses D50, most others use D65).
 *
 * Bradford is the recommended method for most use cases (used in ICC profiles).
 * Von Kries is simpler but less perceptually accurate.
 *
 * @see http://www.brucelindbloom.com/index.html?Eqn_ChromAdapt.html
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/xyz.hpp>
namespace colorcpp::operations::conversion {
// Forward declaration — full definition in conversion.hpp
template <typename To, typename From>
constexpr To color_cast(const From& src);
}

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
// Von Kries Chromatic Adaptation (diagonal scaling in LMS space)
// =============================================================================

namespace details {

// Von Kries LMS matrix (Hunt-Pointer-Estevez, normalized to D65)
inline constexpr float M_VK[9] = {
    0.40024f,  0.70760f, -0.08081f,
   -0.22630f,  1.16532f,  0.04570f,
    0.00000f,  0.00000f,  0.91822f,
};

// Inverse Von Kries LMS matrix
inline constexpr float M_VK_inv[9] = {
    1.8599364f, -1.1293816f,  0.2198974f,
    0.3611914f,  0.6388125f, -0.0000064f,
    0.0000000f,  0.0000000f,  1.0890636f,
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
  using namespace operations::conversion;

  auto src = color_cast<core::xyz_t>(color);

  // Compute LMS of source and destination whites
  float lms_src[3], lms_dst[3];
  for (int i = 0; i < 3; ++i) {
    lms_src[i] = details::M_VK[i * 3 + 0] * (&src_white.x())[0] +
                 details::M_VK[i * 3 + 1] * (&src_white.y())[0] +
                 details::M_VK[i * 3 + 2] * (&src_white.z())[0];
    lms_dst[i] = details::M_VK[i * 3 + 0] * (&dst_white.x())[0] +
                 details::M_VK[i * 3 + 1] * (&dst_white.y())[0] +
                 details::M_VK[i * 3 + 2] * (&dst_white.z())[0];
  }

  // Convert source color to LMS
  float lms[3];
  for (int i = 0; i < 3; ++i) {
    lms[i] = details::M_VK[i * 3 + 0] * src.x() +
             details::M_VK[i * 3 + 1] * src.y() +
             details::M_VK[i * 3 + 2] * src.z();
  }

  // Apply diagonal scaling
  for (int i = 0; i < 3; ++i) {
    if (lms_src[i] != 0.0f) {
      lms[i] *= lms_dst[i] / lms_src[i];
    }
  }

  // Convert back to XYZ
  float x = details::M_VK_inv[0] * lms[0] + details::M_VK_inv[1] * lms[1] + details::M_VK_inv[2] * lms[2];
  float y = details::M_VK_inv[3] * lms[0] + details::M_VK_inv[4] * lms[1] + details::M_VK_inv[5] * lms[2];
  float z = details::M_VK_inv[6] * lms[0] + details::M_VK_inv[7] * lms[1] + details::M_VK_inv[8] * lms[2];

  return color_cast<ColorT>(core::xyz_t{x, y, z});
}

// =============================================================================
// Bradford Chromatic Adaptation (recommended for most use cases)
// =============================================================================

namespace details {

// Bradford LMS matrix (from Bruce Lindbloom)
inline constexpr float M_BFD[9] = {
     0.8951000f,  0.2664000f, -0.1614000f,
    -0.7502000f,  1.7135000f,  0.0367000f,
     0.0389000f, -0.0685000f,  1.0296000f,
};

// Inverse Bradford LMS matrix
inline constexpr float M_BFD_inv[9] = {
     0.9869929f, -0.1470543f,  0.1599627f,
     0.4323053f,  0.5183603f,  0.0492912f,
    -0.0085287f,  0.0400428f,  0.9684867f,
};

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
  using namespace operations::conversion;

  auto src = color_cast<core::xyz_t>(color);

  // Compute LMS of source and destination whites
  float lms_src[3], lms_dst[3];
  for (int i = 0; i < 3; ++i) {
    lms_src[i] = details::M_BFD[i * 3 + 0] * (&src_white.x())[0] +
                 details::M_BFD[i * 3 + 1] * (&src_white.y())[0] +
                 details::M_BFD[i * 3 + 2] * (&src_white.z())[0];
    lms_dst[i] = details::M_BFD[i * 3 + 0] * (&dst_white.x())[0] +
                 details::M_BFD[i * 3 + 1] * (&dst_white.y())[0] +
                 details::M_BFD[i * 3 + 2] * (&dst_white.z())[0];
  }

  // Convert source color to LMS
  float lms[3];
  for (int i = 0; i < 3; ++i) {
    lms[i] = details::M_BFD[i * 3 + 0] * src.x() +
             details::M_BFD[i * 3 + 1] * src.y() +
             details::M_BFD[i * 3 + 2] * src.z();
  }

  // Apply diagonal scaling
  for (int i = 0; i < 3; ++i) {
    if (lms_src[i] != 0.0f) {
      lms[i] *= lms_dst[i] / lms_src[i];
    }
  }

  // Convert back to XYZ
  float x = details::M_BFD_inv[0] * lms[0] + details::M_BFD_inv[1] * lms[1] + details::M_BFD_inv[2] * lms[2];
  float y = details::M_BFD_inv[3] * lms[0] + details::M_BFD_inv[4] * lms[1] + details::M_BFD_inv[5] * lms[2];
  float z = details::M_BFD_inv[6] * lms[0] + details::M_BFD_inv[7] * lms[1] + details::M_BFD_inv[8] * lms[2];

  return color_cast<ColorT>(core::xyz_t{x, y, z});
}

}  // namespace colorcpp::algorithms::chromatic_adaptation
