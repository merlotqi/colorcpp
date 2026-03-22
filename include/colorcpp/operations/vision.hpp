#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <colorcpp/operations/conversion.hpp>

// Color vision deficiency (CVD) simulation.
//
// All functions accept any color type registered with color_cast and return
// the same type.  Internally every simulation works in linearized sRGB (via
// sRGB ↔ LMS) so that the projected values are physically meaningful.
//
// References
// • Brettel, Viénot & Mollon (1997) "Computerized simulation of color
//   appearance for dichromats", JOSA A 14(10):2647–2655.
// • Viénot, Brettel & Mollon (1999) "Digital video colourmaps for checking
//   the legibility of displays by dichromats", Color Research & Application
//   24(4):243–252.
// LMS matrix values from the above papers (Hunt–Pointer–Estevez, D65).

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

// Protanopia (absent L cones — reduced red sensitivity)
// Viénot 1999: L' = 2.02344·M − 2.52581·S,  M'=M,  S'=S

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

// Deuteranopia (absent M cones — reduced green sensitivity)
// Viénot 1999: L'=L,  M' = 0.49421·L + 1.24827·S,  S'=S

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

// Tritanopia (absent S cones — reduced blue/yellow sensitivity)
// Brettel, Viénot & Mollon 1997: two-half-plane projection in LMS space.
// The LMS space is split by a boundary plane containing the achromatic axis;
// each half uses its own confusion-line projection for the S channel.
//
// Boundary normal (n_sep): [0.03476, −0.02797, 0]
// Plane 1 (n_sep · lms ≥ 0): S' = −0.01224·L + 0.07203·M
// Plane 2 (n_sep · lms <  0): S' =  0.00316·L + 0.07078·M

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

// Achromatopsia (rod monochromacy — complete absence of color vision)
// Perceived brightness equals CIE Y relative luminance computed in linear light
// (Rec. 709 / sRGB primaries): Y = 0.2126·R + 0.7152·G + 0.0722·B

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
