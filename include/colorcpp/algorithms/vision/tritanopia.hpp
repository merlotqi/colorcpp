/**
 * @file tritanopia.hpp
 * @brief Tritanopia (S-cone absence) simulation using Brettel two-plane model.
 */

#pragma once

#include <colorcpp/algorithms/vision/details.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::vision {

/**
 * @brief Tritanopia simulation (S-cone absence; two-plane Brettel model).
 * @tparam Color Any color type supported by conversion::color_cast.
 * @param c Input color.
 * @return Simulated color for tritanopia.
 */
template <typename Color>
Color simulate_tritanopia(const Color& c) {
  using namespace operations::conversion;
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

}  // namespace colorcpp::algorithms::vision
