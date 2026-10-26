/**
 * @file details.hpp
 * @brief Porter-Duff compositing helper and blend-space transforms.
 */

#pragma once

#include <colorcpp/core/linear_rgb.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::operations::blend::details {

/**
 * @brief Porter-Duff general compositing: Cs·αs·(1-αd) + Cd·αd·(1-αs) + B·αs·αd
 */
template <typename Fn>
constexpr float composite(float dst, float da, float src, float sa, Fn&& blend_fn) {
  float mixed = blend_fn(dst, src);
  return src * sa * (1.0f - da) + dst * da * (1.0f - sa) + da * sa * mixed;
}

/**
 * @brief Linearize encoded sRGB channels, keeping alpha (and the container type).
 *
 * The returned @c rgbaf_t carries linear values in its RGB channels — it is a
 * dumb float container here, not encoded sRGB.
 */
inline core::rgbaf_t to_linear_working(const core::rgbaf_t& c) {
  const auto lin = conversion::color_cast<core::linear_rgbaf_t>(c);
  return core::rgbaf_t{static_cast<float>(lin.template get_index<0>()), static_cast<float>(lin.template get_index<1>()),
                       static_cast<float>(lin.template get_index<2>()), static_cast<float>(lin.template get_index<3>())};
}

/**
 * @brief Encode linear-sRGB working channels back to gamma-encoded sRGB.
 */
inline core::rgbaf_t from_linear_working(const core::rgbaf_t& lin) {
  const core::linear_rgbaf_t l{
      static_cast<float>(lin.template get_index<0>()), static_cast<float>(lin.template get_index<1>()),
      static_cast<float>(lin.template get_index<2>()), static_cast<float>(lin.template get_index<3>())};
  return conversion::color_cast<core::rgbaf_t>(l);
}

}  // namespace colorcpp::operations::blend::details
