/**
 * @file ictcp.hpp
 * @brief XYZ <-> ICtCp conversion (ITU-R BT.2100 PQ / Dolby 2016).
 *
 * Pipeline: XYZ (D65, relative) -> linear BT.2020 -> LMS' -> PQ (absolute,
 * ST 2084) -> ICtCp. The relative-to-absolute step maps Y = 1 (diffuse
 * white) to the 100 cd/m^2 SDR reference white, so white encodes to
 * PQ(100) = 0.5080784 — this is the library-wide scale convention shared
 * with Jzazbz (the old raw feed put white at the PQ code for 1 nit).
 *
 * Formerly private to algorithms/delta_e/itp.hpp; now the single
 * implementation behind both color_cast and the delta_e wrappers.
 *
 * @see Lu et al. (2016). A Perceptual EOTF for High Dynamic Range Content.
 */

#pragma once

#include <cmath>
#include <colorcpp/core/ictcp.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion/functions/rec2020.hpp>

namespace colorcpp::operations::conversion::details {

// ITU-R BT.2100 PQ constants (SMPTE ST 2084).
inline constexpr float ITP_M1 = 2610.0f / 16384.0f;
inline constexpr float ITP_M2 = 2523.0f / 32.0f;
inline constexpr float ITP_C1 = 3424.0f / 4096.0f;
inline constexpr float ITP_C2 = 2413.0f / 128.0f;
inline constexpr float ITP_C3 = 2392.0f / 128.0f;
inline constexpr float ITP_PEAK = 10000.0f;

// Linear BT.2020 RGB -> LMS' (ITU-R BT.2100-1).
inline constexpr float M_RGB_TO_LMS[9] = {1688.0f / 4096.0f, 2146.0f / 4096.0f, 262.0f / 4096.0f,
                                          683.0f / 4096.0f,  2951.0f / 4096.0f, 462.0f / 4096.0f,
                                          99.0f / 4096.0f,   309.0f / 4096.0f,  3688.0f / 4096.0f};
// Inverse of M_RGB_TO_LMS (adjugate, validated by round-trip).
inline constexpr float M_LMS_TO_RGB[9] = {3.4366066943f,  -2.5064521187f, 0.0698454243f,  -0.7913295556f, 1.9836004518f,
                                          -0.1922708962f, -0.0259498997f, -0.0989137147f, 1.1248636144f};

// PQ-coded LMS' -> ICtCp (Dolby 2016).
inline constexpr float M_LMS_P_TO_ICTCP[9] = {2048.0f / 4096.0f,  2048.0f / 4096.0f,   0.0f,
                                              6610.0f / 4096.0f,  -13613.0f / 4096.0f, 7003.0f / 4096.0f,
                                              17933.0f / 4096.0f, -17390.0f / 4096.0f, -543.0f / 4096.0f};
// Inverse of M_LMS_P_TO_ICTCP (adjugate, validated by round-trip).
inline constexpr float M_ICTCP_TO_LMS_P[9] = {1.0f, 0.0086090370f, 0.1110296250f, 1.0f, -0.0086090370f, -0.1110296250f,
                                              1.0f, 0.5600313357f, -0.3206271750f};

// PQ inverse-EOTF (absolute luminance -> code value). Black maps to exactly
// 0 (raw formula yields c1^m2 ≈ 7.3e-7 at Y = 0, which would leak into the
// ICtCp chroma channels through the matrix).
inline float itp_pq_encode(float y) {
  if (y <= 0.0f) return 0.0f;
  y = std::pow(y / ITP_PEAK, ITP_M1);
  return std::pow((ITP_C1 + ITP_C2 * y) / (1.0f + ITP_C3 * y), ITP_M2);
}

// PQ EOTF (code value -> absolute luminance).
inline float itp_pq_decode(float p) {
  p = std::pow(std::max(p, 0.0f), 1.0f / ITP_M2);
  float num = std::max(p - ITP_C1, 0.0f);
  float den = ITP_C2 - ITP_C3 * p;
  return (den > 0.0f) ? ITP_PEAK * std::pow(num / den, 1.0f / ITP_M1) : 0.0f;
}

/** @brief XYZ (D65, relative) -> ICtCp; scale to absolute nits before PQ. */
inline core::ictcp_t xyz_to_ictcp_reg(const core::xyz_t& src) {
  const auto rgb = xyz_to_linear_rec2020<core::linear_rec2020_rgbf_t>(src);

  float lms[3];
  for (int r = 0; r < 3; ++r) {
    lms[r] = M_RGB_TO_LMS[r * 3 + 0] * rgb.r() + M_RGB_TO_LMS[r * 3 + 1] * rgb.g() + M_RGB_TO_LMS[r * 3 + 2] * rgb.b();
  }
  // Scale into absolute nits before the absolute PQ step.
  float lms_p[3] = {itp_pq_encode(lms[0] * kRelativeWhiteNits), itp_pq_encode(lms[1] * kRelativeWhiteNits),
                    itp_pq_encode(lms[2] * kRelativeWhiteNits)};

  return core::ictcp_t{
      M_LMS_P_TO_ICTCP[0] * lms_p[0] + M_LMS_P_TO_ICTCP[1] * lms_p[1] + M_LMS_P_TO_ICTCP[2] * lms_p[2],
      M_LMS_P_TO_ICTCP[3] * lms_p[0] + M_LMS_P_TO_ICTCP[4] * lms_p[1] + M_LMS_P_TO_ICTCP[5] * lms_p[2],
      M_LMS_P_TO_ICTCP[6] * lms_p[0] + M_LMS_P_TO_ICTCP[7] * lms_p[1] + M_LMS_P_TO_ICTCP[8] * lms_p[2],
  };
}

/** @brief ICtCp -> XYZ (D65, relative); undo the nits scale after PQ decode. */
inline core::xyz_t ictcp_to_xyz_reg(const core::ictcp_t& src) {
  float lms_p[3];
  for (int r = 0; r < 3; ++r) {
    lms_p[r] = M_ICTCP_TO_LMS_P[r * 3 + 0] * src.i() + M_ICTCP_TO_LMS_P[r * 3 + 1] * src.ct() +
               M_ICTCP_TO_LMS_P[r * 3 + 2] * src.cp();
  }
  // PQ decodes to absolute nits; scale back to the relative XYZ convention.
  float lms[3] = {itp_pq_decode(lms_p[0]) / kRelativeWhiteNits, itp_pq_decode(lms_p[1]) / kRelativeWhiteNits,
                  itp_pq_decode(lms_p[2]) / kRelativeWhiteNits};

  const auto rgb = core::linear_rec2020_rgbf_t{
      M_LMS_TO_RGB[0] * lms[0] + M_LMS_TO_RGB[1] * lms[1] + M_LMS_TO_RGB[2] * lms[2],
      M_LMS_TO_RGB[3] * lms[0] + M_LMS_TO_RGB[4] * lms[1] + M_LMS_TO_RGB[5] * lms[2],
      M_LMS_TO_RGB[6] * lms[0] + M_LMS_TO_RGB[7] * lms[1] + M_LMS_TO_RGB[8] * lms[2],
  };
  return linear_rec2020_to_xyz<core::xyz_t>(rgb);
}

}  // namespace colorcpp::operations::conversion::details
