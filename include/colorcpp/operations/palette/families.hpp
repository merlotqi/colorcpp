/**
 * @file families.hpp
 * @brief Warm, cool, and neutral palette families.
 */

#pragma once

#include <colorcpp/core/palette_set.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/palette/scales.hpp>

namespace colorcpp::operations::palette {

/**
 * @brief Generate a warm palette family (reds, oranges, yellows).
 */
template <typename Color>
core::palette_set<Color> warm(const Color& base, size_t count = 5) {
  using namespace operations::conversion;
  auto hsl = color_cast<core::hsla_float_t>(base);

  core::hsla_float_t warm_start{0.0f, hsl.template get_index<1>(), hsl.template get_index<2>(),
                                hsl.template get_index<3>()};
  core::hsla_float_t warm_end{60.0f, hsl.template get_index<1>(), hsl.template get_index<2>(),
                              hsl.template get_index<3>()};

  return visual_scale(color_cast<Color>(warm_start), color_cast<Color>(warm_end), count);
}

/**
 * @brief Generate a cool palette family (blues, greens, purples).
 */
template <typename Color>
core::palette_set<Color> cool(const Color& base, size_t count = 5) {
  using namespace operations::conversion;
  auto hsl = color_cast<core::hsla_float_t>(base);

  core::hsla_float_t cool_start{180.0f, hsl.template get_index<1>(), hsl.template get_index<2>(),
                                hsl.template get_index<3>()};
  core::hsla_float_t cool_end{300.0f, hsl.template get_index<1>(), hsl.template get_index<2>(),
                              hsl.template get_index<3>()};

  return visual_scale(color_cast<Color>(cool_start), color_cast<Color>(cool_end), count);
}

/**
 * @brief Generate a neutral palette family with the source hue as tint.
 */
template <typename Color>
core::palette_set<Color> neutral(const Color& base, size_t count = 5) {
  using namespace operations::conversion;
  auto hsl = color_cast<core::hsla_float_t>(base);

  // Preserve the base alpha like warm()/cool() do.
  const float alpha = hsl.template get_index<3>();
  core::hsla_float_t neutral_start{hsl.template get_index<0>(), 0.1f, 0.2f, alpha};
  core::hsla_float_t neutral_end{hsl.template get_index<0>(), 0.1f, 0.9f, alpha};

  return visual_scale(color_cast<Color>(neutral_start), color_cast<Color>(neutral_end), count);
}

}  // namespace colorcpp::operations::palette
