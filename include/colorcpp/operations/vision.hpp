/**
 * @file vision.hpp
 * @brief Color vision deficiency simulation (Brettel / Viénot / Mollon) in linear sRGB / LMS space.
 */

#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <colorcpp/operations/conversion.hpp>

/**
 * @brief CVD simulation; input/output types match via @ref conversion::color_cast (linear sRGB ↔ LMS).
 * @note References: Brettel et al. 1997; Viénot et al. 1999 (HPE LMS, D65).
 */
namespace colorcpp::operations::vision {

namespace details {

// sRGB ↔ linear (IEC 61966-2-1)

inline float linearize(float v) noexcept {
  return (v <= 0.04045f) ? (v / 12.92f) : std::pow((v + 0.055f) / 1.055f, 2.4f);
}

inline float gamma_encode(float v) noexcept {
  v = std::clamp(v, 0.0f, 1.0f);
  return (v <= 0.0031308f) ? (12.92f * v) : (1.055f * std::pow(v, 1.0f / 2.4f) - 0.055f);
}

using vec3 = std::array<float, 3>;

inline vec3 mat3_mul(const float M[3][3], const vec3& v) noexcept {
  return {M[0][0] * v[0] + M[0][1] * v[1] + M[0][2] * v[2], M[1][0] * v[0] + M[1][1] * v[1] + M[1][2] * v[2],
          M[2][0] * v[0] + M[2][1] * v[1] + M[2][2] * v[2]};
}

// Linearized sRGB → LMS  (Hunt–Pointer–Estevez, D65 normalization)
// Source: Viénot et al. 1999, Table 1
constexpr float kRGBtoLMS[3][3] = {
    {0.31399022f, 0.63951294f, 0.04649755f},
    {0.15537241f, 0.75789446f, 0.08670142f},
    {0.01775239f, 0.10944209f, 0.87256922f},
};

// LMS → linearized sRGB  (exact inverse of kRGBtoLMS)
constexpr float kLMStoRGB[3][3] = {
    {5.47221206f, -4.64196010f, 0.16963708f},
    {-1.12524191f, 2.29317094f, -0.16789520f},
    {0.02980165f, -0.19318073f, 1.16364789f},
};

}  // namespace details

/** @brief Protanopia simulation (L-cone absence). */
template <typename Color>
Color simulate_protanopia(const Color& c) {
  using namespace conversion;
  using details::vec3;

  auto rgb = color_cast<core::rgbaf_t>(c);
  const float a = rgb.template get_index<3>();

  const vec3 lin = {details::linearize(rgb.template get_index<0>()), details::linearize(rgb.template get_index<1>()),
                    details::linearize(rgb.template get_index<2>())};

  auto lms = details::mat3_mul(details::kRGBtoLMS, lin);

  const vec3 lms_sim = {2.02344354f * lms[1] - 2.52580743f * lms[2], lms[1], lms[2]};

  auto out = details::mat3_mul(details::kLMStoRGB, lms_sim);
  return color_cast<Color>(
      core::rgbaf_t{details::gamma_encode(out[0]), details::gamma_encode(out[1]), details::gamma_encode(out[2]), a});
}

/** @brief Deuteranopia simulation (M-cone absence). */
template <typename Color>
Color simulate_deuteranopia(const Color& c) {
  using namespace conversion;
  using details::vec3;

  auto rgb = color_cast<core::rgbaf_t>(c);
  const float a = rgb.template get_index<3>();

  const vec3 lin = {details::linearize(rgb.template get_index<0>()), details::linearize(rgb.template get_index<1>()),
                    details::linearize(rgb.template get_index<2>())};

  auto lms = details::mat3_mul(details::kRGBtoLMS, lin);

  const vec3 lms_sim = {lms[0], 0.49420696f * lms[0] + 1.24827352f * lms[2], lms[2]};

  auto out = details::mat3_mul(details::kLMStoRGB, lms_sim);
  return color_cast<Color>(
      core::rgbaf_t{details::gamma_encode(out[0]), details::gamma_encode(out[1]), details::gamma_encode(out[2]), a});
}

/** @brief Tritanopia simulation (S-cone absence; two-plane Brettel model). */
template <typename Color>
Color simulate_tritanopia(const Color& c) {
  using namespace conversion;
  using details::vec3;

  auto rgb = color_cast<core::rgbaf_t>(c);
  const float a = rgb.template get_index<3>();

  const vec3 lin = {details::linearize(rgb.template get_index<0>()), details::linearize(rgb.template get_index<1>()),
                    details::linearize(rgb.template get_index<2>())};

  auto lms = details::mat3_mul(details::kRGBtoLMS, lin);

  const float S_sim = (0.03476f * lms[0] - 0.02797f * lms[1] >= 0.0f)
                          ? (-0.01224f * lms[0] + 0.07203f * lms[1])  // plane 1 (red-side)
                          : (0.00316f * lms[0] + 0.07078f * lms[1]);  // plane 2 (blue-side)

  const vec3 lms_sim = {lms[0], lms[1], S_sim};

  auto out = details::mat3_mul(details::kLMStoRGB, lms_sim);
  return color_cast<Color>(
      core::rgbaf_t{details::gamma_encode(out[0]), details::gamma_encode(out[1]), details::gamma_encode(out[2]), a});
}

/** @brief Achromatopsia: grayscale from linear Rec. 709 luminance. */
template <typename Color>
Color simulate_achromatopsia(const Color& c) {
  using namespace conversion;

  auto rgb = color_cast<core::rgbaf_t>(c);
  const float a = rgb.template get_index<3>();

  const float r_lin = details::linearize(rgb.template get_index<0>());
  const float g_lin = details::linearize(rgb.template get_index<1>());
  const float b_lin = details::linearize(rgb.template get_index<2>());

  const float Y = 0.2126f * r_lin + 0.7152f * g_lin + 0.0722f * b_lin;
  const float gray = details::gamma_encode(Y);

  return color_cast<Color>(core::rgbaf_t{gray, gray, gray, a});
}

}  // namespace colorcpp::operations::vision
