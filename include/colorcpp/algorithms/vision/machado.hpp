/**
 * @file machado.hpp
 * @brief CVD simulation via Machado–Oliveira–Fernandes (2009) RGB matrices with adjustable severity.
 *
 * The model provides precomputed matrices for linear sRGB at 10% severity
 * steps (0–100%); intermediate severities interpolate linearly between
 * adjacent tables. Severity 0 leaves the color unchanged; severity 1 is full
 * dichromacy for the chosen deficiency.
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
#include <colorcpp/algorithms/vision/simd.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::vision {

namespace machado_detail {

using mat3 = std::array<std::array<float, 3>, 3>;

// Published Machado et al. (2009) matrices at 10% severity steps (0–100%),
// official supplementary data (identical to colour-science/colorspacious).
// Every matrix preserves the neutral axis (each row sums to 1).
constexpr mat3 make_mat3(const float (&v)[9]) { return {{{v[0], v[1], v[2]}, {v[3], v[4], v[5]}, {v[6], v[7], v[8]}}}; }

inline constexpr std::array<mat3, 11> kProtanopiaSeverity = {{
    make_mat3({1.000000f, 0.000000f, 0.000000f, 0.000000f, 1.000000f, 0.000000f, 0.000000f, 0.000000f, 1.000000f}),
    make_mat3({0.856167f, 0.182038f, -0.038205f, 0.029342f, 0.955115f, 0.015544f, -0.002880f, -0.001563f, 1.004443f}),
    make_mat3({0.734766f, 0.334872f, -0.069637f, 0.051840f, 0.919198f, 0.028963f, -0.004928f, -0.004209f, 1.009137f}),
    make_mat3({0.630323f, 0.465641f, -0.095964f, 0.069181f, 0.890046f, 0.040773f, -0.006308f, -0.007724f, 1.014032f}),
    make_mat3({0.539009f, 0.579343f, -0.118352f, 0.082546f, 0.866121f, 0.051332f, -0.007136f, -0.011959f, 1.019095f}),
    make_mat3({0.458064f, 0.679578f, -0.137642f, 0.092785f, 0.846313f, 0.060902f, -0.007494f, -0.016807f, 1.024301f}),
    make_mat3({0.385450f, 0.769005f, -0.154455f, 0.100526f, 0.829802f, 0.069673f, -0.007442f, -0.022190f, 1.029632f}),
    make_mat3({0.319627f, 0.849633f, -0.169261f, 0.106241f, 0.815969f, 0.077790f, -0.007025f, -0.028051f, 1.035076f}),
    make_mat3({0.259411f, 0.923008f, -0.182420f, 0.110296f, 0.804340f, 0.085364f, -0.006276f, -0.034346f, 1.040622f}),
    make_mat3({0.203876f, 0.990338f, -0.194214f, 0.112975f, 0.794542f, 0.092483f, -0.005222f, -0.041043f, 1.046265f}),
    make_mat3({0.152286f, 1.052583f, -0.204868f, 0.114503f, 0.786281f, 0.099216f, -0.003882f, -0.048116f, 1.051998f}),
}};

inline constexpr std::array<mat3, 11> kDeuteranopiaSeverity = {{
    make_mat3({1.000000f, 0.000000f, 0.000000f, 0.000000f, 1.000000f, 0.000000f, 0.000000f, 0.000000f, 1.000000f}),
    make_mat3({0.866435f, 0.177704f, -0.044139f, 0.049567f, 0.939063f, 0.011370f, -0.003453f, 0.007233f, 0.996220f}),
    make_mat3({0.760729f, 0.319078f, -0.079807f, 0.090568f, 0.889315f, 0.020117f, -0.006027f, 0.013325f, 0.992702f}),
    make_mat3({0.675425f, 0.433850f, -0.109275f, 0.125303f, 0.847755f, 0.026942f, -0.007950f, 0.018572f, 0.989378f}),
    make_mat3({0.605511f, 0.528560f, -0.134071f, 0.155318f, 0.812366f, 0.032316f, -0.009376f, 0.023176f, 0.986200f}),
    make_mat3({0.547494f, 0.607765f, -0.155259f, 0.181692f, 0.781742f, 0.036566f, -0.010410f, 0.027275f, 0.983136f}),
    make_mat3({0.498864f, 0.674741f, -0.173604f, 0.205199f, 0.754872f, 0.039929f, -0.011131f, 0.030969f, 0.980162f}),
    make_mat3({0.457771f, 0.731899f, -0.189670f, 0.226409f, 0.731012f, 0.042579f, -0.011595f, 0.034333f, 0.977261f}),
    make_mat3({0.422823f, 0.781057f, -0.203881f, 0.245752f, 0.709602f, 0.044646f, -0.011843f, 0.037423f, 0.974421f}),
    make_mat3({0.392952f, 0.823610f, -0.216562f, 0.263559f, 0.690210f, 0.046232f, -0.011910f, 0.040281f, 0.971630f}),
    make_mat3({0.367322f, 0.860646f, -0.227968f, 0.280085f, 0.672501f, 0.047413f, -0.011820f, 0.042940f, 0.968881f}),
}};

inline constexpr std::array<mat3, 11> kTritanopiaSeverity = {{
    make_mat3({1.000000f, 0.000000f, 0.000000f, 0.000000f, 1.000000f, 0.000000f, 0.000000f, 0.000000f, 1.000000f}),
    make_mat3({0.926670f, 0.092514f, -0.019184f, 0.021191f, 0.964503f, 0.014306f, 0.008437f, 0.054813f, 0.936750f}),
    make_mat3({0.895720f, 0.133330f, -0.029050f, 0.029997f, 0.945400f, 0.024603f, 0.013027f, 0.104707f, 0.882266f}),
    make_mat3({0.905871f, 0.127791f, -0.033662f, 0.026856f, 0.941251f, 0.031893f, 0.013410f, 0.148296f, 0.838294f}),
    make_mat3({0.948035f, 0.089490f, -0.037526f, 0.014364f, 0.946792f, 0.038844f, 0.010853f, 0.193991f, 0.795156f}),
    make_mat3({1.017277f, 0.027029f, -0.044306f, -0.006113f, 0.958479f, 0.047634f, 0.006379f, 0.248708f, 0.744913f}),
    make_mat3({1.104996f, -0.046633f, -0.058363f, -0.032137f, 0.971635f, 0.060503f, 0.001336f, 0.317922f, 0.680742f}),
    make_mat3({1.193214f, -0.109812f, -0.083402f, -0.058496f, 0.979410f, 0.079086f, -0.002346f, 0.403492f, 0.598854f}),
    make_mat3({1.257728f, -0.139648f, -0.118081f, -0.078003f, 0.975409f, 0.102594f, -0.003316f, 0.501214f, 0.502102f}),
    make_mat3({1.278864f, -0.125333f, -0.153531f, -0.084748f, 0.957674f, 0.127074f, -0.000989f, 0.601151f, 0.399838f}),
    make_mat3({1.255528f, -0.076749f, -0.178779f, -0.078411f, 0.930809f, 0.147602f, 0.004733f, 0.691367f, 0.303900f}),
}};

// Full-dichromacy matrices at severity 100% (last table entry).
inline constexpr mat3 kProtanopia = kProtanopiaSeverity[10];
inline constexpr mat3 kDeuteranopia = kDeuteranopiaSeverity[10];
inline constexpr mat3 kTritanopia = kTritanopiaSeverity[10];

// Linear interpolation between the two matrices bracketing the requested
// severity on the published 10%-step grid.
inline mat3 severity_matrix(const std::array<mat3, 11>& table, float severity) noexcept {
  // NaN and non-positive severities degrade to the identity matrix so the
  // table index below can never see an undefined value.
  if (!(severity > 0.0f)) return table[0];
  severity = std::min(severity, 1.0f);
  const float pos = severity * 10.0f;
  const int lo = std::min(static_cast<int>(pos), 9);
  const float t = pos - static_cast<float>(lo);
  mat3 m{};
  for (int r = 0; r < 3; ++r) {
    for (int c = 0; c < 3; ++c) {
      m[r][c] = table[lo][r][c] + t * (table[lo + 1][r][c] - table[lo][r][c]);
    }
  }
  return m;
}

template <typename Color, typename Table>
Color simulate_machado_impl(const Color& c, float severity, const Table& table) {
  using namespace operations::conversion;
  // Non-positive severities (and NaN, which fails every comparison) leave
  // the color unchanged.
  if (!(severity > 0.0f)) return c;

  const mat3 m = severity_matrix(table, severity);

  auto rgb = color_cast<core::rgbaf_t>(c);
  core::rgbaf_t out{};
  // The severity blend is already baked into m; feed the kernels a full
  // (identity-free) matrix via severity 1.
  if (details::try_simulate_machado_rgbaf_fast_path(out, rgb, 1.0f, m)) {
    return color_cast<Color>(out);
  }

  return color_cast<Color>(details::simulate_machado_rgbaf_scalar(rgb, 1.0f, m));
}

}  // namespace machado_detail

/**
 * @brief Protanomaly / protanopia simulation (L-cone) with severity in [0, 1].
 */
template <typename Color>
Color simulate_protan_machado(const Color& c, float severity = 1.0f) {
  return machado_detail::simulate_machado_impl(c, severity, machado_detail::kProtanopiaSeverity);
}

/**
 * @brief Deuteranomaly / deuteranopia simulation (M-cone) with severity in [0, 1].
 */
template <typename Color>
Color simulate_deutan_machado(const Color& c, float severity = 1.0f) {
  return machado_detail::simulate_machado_impl(c, severity, machado_detail::kDeuteranopiaSeverity);
}

/**
 * @brief Tritanomaly / tritanopia simulation (S-cone) with severity in [0, 1].
 */
template <typename Color>
Color simulate_tritan_machado(const Color& c, float severity = 1.0f) {
  return machado_detail::simulate_machado_impl(c, severity, machado_detail::kTritanopiaSeverity);
}

}  // namespace colorcpp::algorithms::vision
