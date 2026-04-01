/**
 * @file protanopia.hpp
 * @brief Protanopia (L-cone absence) simulation.
 */

#pragma once

#include <colorcpp/algorithms/vision/details.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::vision {

/**
 * @brief Protanopia simulation (L-cone absence).
 * @tparam Color Any color type supported by conversion::color_cast.
 * @param c Input color.
 * @return Simulated color for protanopia.
 */
template <typename Color>
Color simulate_protanopia(const Color& c) {
  using namespace operations::conversion;
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

}  // namespace colorcpp::algorithms::vision
