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

/**
 * @brief Color space in which the blend function and compositing are evaluated.
 *
 * - @c srgb: formulas apply to gamma-encoded sRGB channels. This is the
 *   W3C Compositing and Blending Level 1 behaviour (CSS parity).
 * - @c linear: inputs are linearized first, formulas apply to linear-sRGB
 *   channels, and the result is re-encoded to sRGB gamma ("linear light"
 *   compositing, as in Photoshop's linear-light blending).
 *
 * Non-separable modes (@c hue, @c saturation, @c color_blend, @c luminosity)
 * are defined by W3C on sRGB HSL and always use @c srgb; the argument does
 * not affect them.
 */
enum class blend_space {
  srgb,
  linear
};

}  // namespace colorcpp::operations::blend
