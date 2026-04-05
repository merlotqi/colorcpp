/**
 * @file machado.hpp
 * @brief CVD simulation via Machado–Oliveira–Fernandes (2009) RGB matrices with adjustable severity.
 *
 * Applies (1−s)·I + s·M to **linear** sRGB, then re-encodes gamma. @p severity 0 leaves the color unchanged;
 * @p severity 1 is full dichromacy for the chosen deficiency.
 *
 * This complements @ref simulate_protanopia / @ref simulate_deuteranopia / @ref simulate_tritanopia (Brettel et al.),
 * which are fixed full simulations in LMS space.
 *
 * @see Machado, Oliveira, Fernandes, "A Physiologically-based Model for Simulation of Color Vision Deficiency"
 */

#pragma once

#include <algorithm>
#include <array>
#include <colorcpp/algorithms/vision/details.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::vision {

namespace machado_detail {

using mat3 = std::array<std::array<float, 3>, 3>;

// Full-dichromacy RGB→RGB matrices on linear sRGB (Machado et al. 2009; as used in colour-science).
inline constexpr mat3 kProtanopia = {{
    {0.367322f, 0.860646f, -0.227968f},
    {0.280085f, 0.672501f, 0.047413f},
    {-0.011820f, 0.042940f, 0.968881f},
}};

inline constexpr mat3 kDeuteranopia = {{
    {0.152286f, 1.052583f, -0.055868f},
    {0.114503f, 0.786281f, 0.099216f},
    {-0.003882f, -0.048116f, 1.051998f},
}};

inline constexpr mat3 kTritanopia = {{
    {1.255528f, -0.076749f, -0.178779f},
    {-0.078411f, 0.930809f, 0.147602f},
    {0.004733f, 0.691367f, 0.303900f},
}};

inline void blend_identity_matrix(float s, const mat3& m, float out[3][3]) {
  s = std::clamp(s, 0.0f, 1.0f);
  const float t = 1.0f - s;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      out[i][j] = (i == j ? t : 0.0f) + s * m[static_cast<size_t>(i)][static_cast<size_t>(j)];
    }
  }
}

inline details::vec3 mat_mul(const float m[3][3], const details::vec3& v) {
  return {m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2], m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2],
          m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2]};
}

template <typename Color>
Color simulate_machado_impl(const Color& c, float severity, const mat3& m_full) {
  using namespace operations::conversion;
  float m[3][3];
  blend_identity_matrix(severity, m_full, m);

  auto rgb = color_cast<core::rgbaf_t>(c);
  const float a = rgb.template get_index<3>();

  const details::vec3 lin = {details::linearize(rgb.template get_index<0>()),
                             details::linearize(rgb.template get_index<1>()),
                             details::linearize(rgb.template get_index<2>())};

  const details::vec3 out_lin = mat_mul(m, lin);
  return color_cast<Color>(core::rgbaf_t{details::gamma_encode(out_lin[0]), details::gamma_encode(out_lin[1]),
                                         details::gamma_encode(out_lin[2]), a});
}

}  // namespace machado_detail

/**
 * @brief Protanomaly / protanopia simulation (L-cone) with severity in [0, 1].
 */
template <typename Color>
Color simulate_protan_machado(const Color& c, float severity = 1.0f) {
  return machado_detail::simulate_machado_impl(c, severity, machado_detail::kProtanopia);
}

/**
 * @brief Deuteranomaly / deuteranopia simulation (M-cone) with severity in [0, 1].
 */
template <typename Color>
Color simulate_deutan_machado(const Color& c, float severity = 1.0f) {
  return machado_detail::simulate_machado_impl(c, severity, machado_detail::kDeuteranopia);
}

/**
 * @brief Tritanomaly / tritanopia simulation (S-cone) with severity in [0, 1].
 */
template <typename Color>
Color simulate_tritan_machado(const Color& c, float severity = 1.0f) {
  return machado_detail::simulate_machado_impl(c, severity, machado_detail::kTritanopia);
}

}  // namespace colorcpp::algorithms::vision
