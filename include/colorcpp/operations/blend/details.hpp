/**
 * @file details.hpp
 * @brief Porter-Duff compositing helper.
 */

#pragma once

namespace colorcpp::operations::blend::details {

/**
 * @brief Porter-Duff general compositing: Cs·αs·(1-αd) + Cd·αd·(1-αs) + B·αs·αd
 */
template <typename Fn>
constexpr float composite(float dst, float da, float src, float sa, Fn&& blend_fn) {
  float mixed = blend_fn(dst, src);
  return src * sa * (1.0f - da) + dst * da * (1.0f - sa) + da * sa * mixed;
}

}  // namespace colorcpp::operations::blend::details
