/**
 * @file hyab.hpp
 * @brief Hybrid absolute-Bernard colour difference ΔE_HyAB in CIELAB D65.
 *
 * ΔE_HyAB = |ΔL*| + √(Δa*² + Δb*²). Combines an absolute lightness term with
 * the planar chroma distance, avoiding the hue-collapse artefact of plain
 * Euclidean ΔE76 for large lightness differences.
 *
 * @see Abasi, S., Akyüz, M. S., & Lenz, R. (2020). Accuracy of Color
 *      Difference Formulae in Complex Scenes. Journal of Imaging.
 */

#pragma once

#include <cmath>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::delta_e {

/**
 * @brief Hybrid colour difference |ΔL*| + √(Δa*² + Δb*²) in CIELAB D65.
 * @tparam ColorA, ColorB Any types supported by @ref
 *         operations::conversion::color_cast to @c cielab_t.
 */
template <typename ColorA, typename ColorB>
float delta_e_hyab(const ColorA& a, const ColorB& b) {
  using namespace operations::conversion;

  const auto la = color_cast<core::cielab_t>(a);
  const auto lb = color_cast<core::cielab_t>(b);

  const float d_l = static_cast<float>(lb.l()) - static_cast<float>(la.l());
  const float d_a = static_cast<float>(lb.a()) - static_cast<float>(la.a());
  const float d_b = static_cast<float>(lb.b()) - static_cast<float>(la.b());
  return std::abs(d_l) + std::hypot(d_a, d_b);
}

}  // namespace colorcpp::algorithms::delta_e
