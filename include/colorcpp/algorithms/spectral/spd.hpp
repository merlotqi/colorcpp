/**
 * @file spd.hpp
 * @brief Sampled spectral power distribution on a regular wavelength grid.
 *
 * A thin value type over (start, step, samples): the CIE datasets (CMFs,
 * standard illuminants) and typical measurement data (reflectance,
 * LED/SPD captures) all arrive on regular 5/10 nm grids, which keeps the
 * integrator trivial and the tables constexpr-friendly. Sampling outside
 * the grid clamps to the edge values — the same semantics as numpy's
 * interp, which the colour-science oracle uses.
 */

#pragma once

#include <cstddef>
#include <utility>
#include <vector>

namespace colorcpp::algorithms::spectral {

/**
 * @brief Sampled SPD on a regular wavelength grid.
 *
 * Values are in whatever unit the caller uses (radiance, reflectance,
 * relative power); integration only cares about the shape and scale.
 */
struct spd {
  float start_nm = 380.0f;  ///< Wavelength of the first sample.
  float step_nm = 5.0f;     ///< Spacing between samples (> 0).
  std::vector<float> values;

  spd() = default;

  /** @brief Construct from grid parameters and samples (values.size() >= 1). */
  spd(float start, float step, std::vector<float> v) : start_nm(start), step_nm(step), values(std::move(v)) {}

  std::size_t size() const noexcept { return values.size(); }
  bool empty() const noexcept { return values.empty(); }

  /** @brief Wavelength of sample @p i. */
  float nm_at(std::size_t i) const noexcept { return start_nm + step_nm * static_cast<float>(i); }

  /**
   * @brief Linearly interpolated value at @p nm.
   *
   * Outside the sampled grid the edge values are held constant (np.interp
   * semantics) so integrations over a wider CMF grid stay well-defined;
   * an empty spd samples as 0.
   */
  float sample(float nm) const noexcept {
    if (values.empty()) return 0.0f;
    if (values.size() == 1) return values.front();
    const float t = (nm - start_nm) / step_nm;
    if (t <= 0.0f) return values.front();
    const float last = static_cast<float>(values.size() - 1);
    if (t >= last) return values.back();
    const auto i = static_cast<std::size_t>(t);
    const float frac = t - static_cast<float>(i);
    return values[i] + frac * (values[i + 1] - values[i]);
  }
};

}  // namespace colorcpp::algorithms::spectral
