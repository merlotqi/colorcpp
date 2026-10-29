/**
 * @file jzazbz.hpp
 * @brief Euclidean color difference in Jzazbz (Safdar et al. 2017).
 *
 * Jzazbz is designed for HDR / wide-gamut content; its Euclidean distance is
 * the natural companion metric for @ref core::jzazbz_t pipelines. Typical
 * SDR-scale differences land in the 0.01–0.05 range (Jz spans [0, 1]) —
 * there is no community-standard JND constant, so compare against your own
 * content statistics rather than reusing ΔE2000 or Oklab thresholds.
 */

#pragma once

#include <cmath>
#include <colorcpp/core/jzazbz.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::delta_e {

/**
 * @brief Euclidean distance √(ΔJz² + Δaz² + Δbz²) in Jzazbz.
 * @tparam ColorA, ColorB Any types supported by @ref
 *         operations::conversion::color_cast to @c jzazbz_t.
 */
template <typename ColorA, typename ColorB>
float delta_e_jz(const ColorA& a, const ColorB& b) {
  using namespace operations::conversion;

  const auto ja = color_cast<core::jzazbz_t>(a);
  const auto jb = color_cast<core::jzazbz_t>(b);

  const float d_j = static_cast<float>(jb.jz()) - static_cast<float>(ja.jz());
  const float d_a = static_cast<float>(jb.az()) - static_cast<float>(ja.az());
  const float d_b = static_cast<float>(jb.bz()) - static_cast<float>(ja.bz());
  return std::hypot(std::hypot(d_j, d_a), d_b);
}

}  // namespace colorcpp::algorithms::delta_e
