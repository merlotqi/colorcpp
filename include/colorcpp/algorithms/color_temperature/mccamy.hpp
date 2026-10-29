/**
 * @file mccamy.hpp
 * @brief McCamy (1992) closed-form CCT estimation.
 *
 * Port of colour-science's xy_to_CCT_McCamy1992: a cubic in the slope of the
 * line through the sample and an approximation of the Planckian locus
 * inflection. Faster than table-based methods but noticeably less accurate
 * far from the locus; no Duv and no inverse (the cubic is not bijective).
 *
 * @see McCamy, C. S. (1992). Correlation colour temperature by way of the
 *      chromaticity diagram. Color Research & Application, 17(5), 381–387.
 */

#pragma once

#include <colorcpp/algorithms/color_temperature/common.hpp>
#include <colorcpp/core/xyy.hpp>

namespace colorcpp::algorithms::color_temperature {

/**
 * @brief Estimate CCT from CIE xy chromaticity (McCamy 1992).
 *
 * @param xyy Chromaticity sample (Y is ignored).
 * @return Approximate correlated colour temperature in kelvin.
 */
inline float xy_to_cct_mccamy(const core::xyy_t& xyy) {
  const float x = static_cast<float>(xyy.x());
  const float y = static_cast<float>(xyy.y());
  const float n = (x - 0.3320f) / (y - 0.1858f);
  return -449.0f * n * n * n + 3525.0f * n * n - 6823.3f * n + 5520.33f;
}

}  // namespace colorcpp::algorithms::color_temperature
