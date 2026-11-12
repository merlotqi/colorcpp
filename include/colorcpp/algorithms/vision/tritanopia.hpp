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
 *
 * Wing selection and coefficients follow coloraide's BRETTEL_TRITAN: the
 * separator passes through the neutral axis, so the two gray-preserving
 * wings meet exactly on it.
 *
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

  // coloraide BRETTEL_TRITAN: the separator runs through the neutral axis,
  // so wing w2 (red side, L/M > 1.90) and wing w1 (green side) meet exactly
  // on the neutral axis — both preserve gray. The previous predicate
  // (L/M >= 0.805) was never false inside the sRGB cube, leaving plane 2
  // unreachable, and its coefficients matched neither wing (issue #102).
  const float S_sim = (0.344781556122f * lms[0] - 0.6551784438780001f * lms[1] > 0.0f)
                          ? (-0.002573851092f * lms[0] + 0.053656722528f * lms[1])   // wing w2 (red-side)
                          : (-0.060107569254f * lms[0] + 0.162986387256f * lms[1]);  // wing w1 (green-side)

  const vec3 lms_sim = {lms[0], lms[1], S_sim};

  auto out = details::mat3_mul(details::kLMStoRGB, lms_sim);
  return color_cast<Color>(
      core::rgbaf_t{details::gamma_encode(out[0]), details::gamma_encode(out[1]), details::gamma_encode(out[2]), a});
}

}  // namespace colorcpp::algorithms::vision
