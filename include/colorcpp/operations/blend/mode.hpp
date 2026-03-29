/**
 * @file mode.hpp
 * @brief Blend mode enumeration.
 */

#pragma once

namespace colorcpp::operations::blend {

/** @brief Separable RGB blend modes plus HSL-channel blend modes (hue, saturation, color, luminosity). */
enum class blend_mode {
  normal,
  multiply,
  screen,
  overlay,
  darken,
  lighten,
  addition,
  subtraction,
  difference,
  exclusion,    // a + b - 2ab  (low-contrast difference)
  hard_light,   // overlay with src/dst roles swapped
  soft_light,   // subtle dodge/burn (W3C formula)
  color_dodge,  // brighten dst toward src
  color_burn,   // darken dst toward src
  divide,       // dst / src

  hue,          // hue of src,  saturation+luminosity of dst
  saturation,   // saturation of src, hue+luminosity of dst
  color_blend,  // hue+saturation of src, luminosity of dst
  luminosity,   // luminosity of src, hue+saturation of dst
};

}  // namespace colorcpp::operations::blend
