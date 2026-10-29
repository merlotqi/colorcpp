/**
 * @file itp.hpp
 * @brief IC_T_C P perceptual colour encoding (Dolby / ITU-R BT.2100 PQ) and
 *        ΔE_ITP colour difference (ITU-R BT.2124).
 *
 * Pipeline: XYZ (D65) -> Linear ITU-R BT.2020 -> LMS' -> PQ (ST 2084) ->
 * IC_T_C P. ΔE_ITP = 720 √(ΔI² + (ΔCt/2)² + ΔCp²); a value of 1 is roughly a
 * just-noticeable difference under critical viewing.
 *
 * @see Lu et al. (2016). A Perceptual EOTF for High Dynamic Range Content.
 * @see ITU-R BT.2124 (2019).
 */

#pragma once

#include <cmath>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion/functions/rec2020.hpp>

namespace colorcpp::algorithms::delta_e {

/**
 * @brief IC_T_C P colour encoding triplet (ITU-R BT.2100 PQ).
 */
struct ictcp_t {
  float i = 0.0f;   ///< Intensity (PQ-coded, roughly perceptual lightness).
  float ct = 0.0f;  ///< Chroma tritan axis.
  float cp = 0.0f;  ///< Chroma protan axis.

  constexpr ictcp_t() = default;
  constexpr ictcp_t(float i_, float ct_, float cp_) : i(i_), ct(ct_), cp(cp_) {}
};

namespace details {

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

// PQ-coded LMS' -> IC_T_C P (Dolby 2016).
inline constexpr float M_LMS_P_TO_ICTCP[9] = {2048.0f / 4096.0f,  2048.0f / 4096.0f,   0.0f,
                                              6610.0f / 4096.0f,  -13613.0f / 4096.0f, 7003.0f / 4096.0f,
                                              17933.0f / 4096.0f, -17390.0f / 4096.0f, -543.0f / 4096.0f};
// Inverse of M_LMS_P_TO_ICTCP (adjugate, validated by round-trip).
inline constexpr float M_ICTCP_TO_LMS_P[9] = {1.0f, 0.0086090370f, 0.1110296250f, 1.0f, -0.0086090370f, -0.1110296250f,
                                              1.0f, 0.5600313357f, -0.3206271750f};

// PQ inverse-EOTF (absolute luminance -> code value). Black maps to exactly
// 0 (raw formula yields c1^m2 ≈ 7.3e-7 at Y = 0, which would leak into the
// IC_T_C P chroma channels through the matrix).
inline constexpr float itp_pq_encode(float y) {
  if (y <= 0.0f) return 0.0f;
  y = std::pow(y / ITP_PEAK, ITP_M1);
  return std::pow((ITP_C1 + ITP_C2 * y) / (1.0f + ITP_C3 * y), ITP_M2);
}

// PQ EOTF (code value -> absolute luminance).
inline constexpr float itp_pq_decode(float p) {
  p = std::pow(std::max(p, 0.0f), 1.0f / ITP_M2);
  float num = std::max(p - ITP_C1, 0.0f);
  float den = ITP_C2 - ITP_C3 * p;
  return (den > 0.0f) ? ITP_PEAK * std::pow(num / den, 1.0f / ITP_M1) : 0.0f;
}

}  // namespace details

/**
 * @brief Convert XYZ (D65) tristimulus values to IC_T_C P.
 *
 * Uses the exact ITU-R BT.2020 matrix already registered in the conversion
 * graph, then LMS' and PQ coding per Dolby 2016. Black encodes to (0,0,0).
 *
 * @param xyz Input XYZ (D65), relative scale (Y = 1 is diffuse white ≙ 100
 *            nits against the 10000 cd/m² PQ peak).
 * @return IC_T_C P encoding.
 */
inline ictcp_t xyz_to_ictcp(const core::xyz_t& xyz) {
  using namespace details;

  const auto rgb = operations::conversion::details::xyz_to_linear_rec2020<core::linear_rec2020_rgbf_t>(xyz);

  float lms[3];
  for (int r = 0; r < 3; ++r) {
    lms[r] = M_RGB_TO_LMS[r * 3 + 0] * rgb.r() + M_RGB_TO_LMS[r * 3 + 1] * rgb.g() + M_RGB_TO_LMS[r * 3 + 2] * rgb.b();
  }
  float lms_p[3] = {itp_pq_encode(lms[0]), itp_pq_encode(lms[1]), itp_pq_encode(lms[2])};

  return ictcp_t{
      M_LMS_P_TO_ICTCP[0] * lms_p[0] + M_LMS_P_TO_ICTCP[1] * lms_p[1] + M_LMS_P_TO_ICTCP[2] * lms_p[2],
      M_LMS_P_TO_ICTCP[3] * lms_p[0] + M_LMS_P_TO_ICTCP[4] * lms_p[1] + M_LMS_P_TO_ICTCP[5] * lms_p[2],
      M_LMS_P_TO_ICTCP[6] * lms_p[0] + M_LMS_P_TO_ICTCP[7] * lms_p[1] + M_LMS_P_TO_ICTCP[8] * lms_p[2],
  };
}

/**
 * @brief Convert IC_T_C P back to XYZ (D65) tristimulus values.
 *
 * Inverse of @ref xyz_to_ictcp.
 */
inline core::xyz_t ictcp_to_xyz(const ictcp_t& c) {
  using namespace details;

  float lms_p[3];
  for (int r = 0; r < 3; ++r) {
    lms_p[r] =
        M_ICTCP_TO_LMS_P[r * 3 + 0] * c.i + M_ICTCP_TO_LMS_P[r * 3 + 1] * c.ct + M_ICTCP_TO_LMS_P[r * 3 + 2] * c.cp;
  }
  float lms[3] = {itp_pq_decode(lms_p[0]), itp_pq_decode(lms_p[1]), itp_pq_decode(lms_p[2])};

  const auto rgb = core::linear_rec2020_rgbf_t{
      M_LMS_TO_RGB[0] * lms[0] + M_LMS_TO_RGB[1] * lms[1] + M_LMS_TO_RGB[2] * lms[2],
      M_LMS_TO_RGB[3] * lms[0] + M_LMS_TO_RGB[4] * lms[1] + M_LMS_TO_RGB[5] * lms[2],
      M_LMS_TO_RGB[6] * lms[0] + M_LMS_TO_RGB[7] * lms[1] + M_LMS_TO_RGB[8] * lms[2],
  };
  return operations::conversion::details::linear_rec2020_to_xyz<core::xyz_t>(rgb);
}

/**
 * @brief ΔE_ITP colour difference (ITU-R BT.2124).
 *
 * 720 √(ΔI² + (ΔCt/2)² + ΔCp²). A value of 1 ≈ just-noticeable difference
 * under the most critical adaptation state.
 *
 * @param a, b IC_T_C P encodings (e.g. from @ref xyz_to_ictcp or any colour
 *             cast to XYZ first).
 */
inline float delta_e_itp(const ictcp_t& a, const ictcp_t& b) {
  const float d_i = b.i - a.i;
  const float d_t = (b.ct - a.ct) * 0.5f;
  const float d_p = b.cp - a.cp;
  return 720.0f * std::sqrt(d_i * d_i + d_t * d_t + d_p * d_p);
}

}  // namespace colorcpp::algorithms::delta_e
