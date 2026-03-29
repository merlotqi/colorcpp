/**
 * @file achromatopsia.hpp
 * @brief Achromatopsia (total color blindness) simulation - grayscale from luminance.
 */

#pragma once

#include <colorcpp/algorithms/vision/details.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::vision {

/**
 * @brief Achromatopsia simulation: grayscale from linear Rec. 709 luminance.
 * @tparam Color Any color type supported by conversion::color_cast.
 * @param c Input color.
 * @return Simulated grayscale color for achromatopsia.
 */
template <typename Color>
Color simulate_achromatopsia(const Color& c) {
  using namespace operations::conversion;

  auto rgb = color_cast<core::rgbaf_t>(c);
  const float a = rgb.template get_index<3>();

  const float r_lin = details::linearize(rgb.template get_index<0>());
  const float g_lin = details::linearize(rgb.template get_index<1>());
  const float b_lin = details::linearize(rgb.template get_index<2>());

  const float Y = 0.2126f * r_lin + 0.7152f * g_lin + 0.0722f * b_lin;
  const float gray = details::gamma_encode(Y);

  return color_cast<Color>(core::rgbaf_t{gray, gray, gray, a});
}

}  // namespace colorcpp::algorithms::vision
