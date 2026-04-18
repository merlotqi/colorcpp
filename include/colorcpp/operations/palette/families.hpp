/**
 * @file families.hpp
 * @brief Warm, cool, and neutral palette families.
 */

#pragma once

#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/palette/palette_set.hpp>
#include <colorcpp/operations/palette/scales.hpp>

namespace colorcpp::operations::palette::families {

/**
 * @brief Generate a warm palette family (reds, oranges, yellows).
 */
template <typename Color>
auto warm(const Color& base, size_t count = 5) {
  using namespace conversion;
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
auto cool(const Color& base, size_t count = 5) {
  using namespace conversion;
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
auto neutral(const Color& base, size_t count = 5) {
  using namespace conversion;
  auto hsl = color_cast<core::hsla_float_t>(base);

  core::hsla_float_t neutral_start{hsl.template get_index<0>(), 0.1f, 0.2f, 1.0f};
  core::hsla_float_t neutral_end{hsl.template get_index<0>(), 0.1f, 0.9f, 1.0f};

  return visual_scale(color_cast<Color>(neutral_start), color_cast<Color>(neutral_end), count);
}

}  // namespace colorcpp::operations::palette::families