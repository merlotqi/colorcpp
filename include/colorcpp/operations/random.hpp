/**
 * @file random.hpp
 * @brief Random color and harmony generators (RGB, HSL/HSV, golden angle, palette helpers).
 *
 * This is an aggregate header that includes all random generation components.
 * Individual components can be included separately for faster compilation.
 */

#pragma once

#include <colorcpp/core/cielab.hpp>
#include <colorcpp/core/cmyk.hpp>
#include <colorcpp/core/display_p3.hpp>
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/hsv.hpp>
#include <colorcpp/core/hwb.hpp>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/random/basic.hpp>
#include <colorcpp/operations/random/details.hpp>
#include <colorcpp/operations/random/golden_angle.hpp>
#include <colorcpp/operations/random/harmony.hpp>
#include <colorcpp/operations/random/hsl.hpp>

/**
 * @namespace colorcpp::operations::random
 * @brief Random color and harmony generators.
 */
namespace colorcpp::operations::random {

/** @name RGB Generators */
///@{
using rgb8_generator = basic_random_generator<core::rgb8_t>;
using rgba8_generator = basic_random_generator<core::rgba8_t>;
using rgbf_generator = basic_random_generator<core::rgbf_t>;
using rgbaf_generator = basic_random_generator<core::rgbaf_t>;
///@}

/** @name HSL/HSV Generators */
///@{
using hsl_generator = basic_hsl_generator<core::hsl_float_t>;
using hsla_generator = basic_hsl_generator<core::hsla_float_t>;
using hsv_generator = basic_hsl_generator<core::hsv_float_t>;
using hsva_generator = basic_hsl_generator<core::hsva_float_t>;
///@}

/** @name HWB Generator */
///@{
using hwb_generator = basic_hsl_generator<core::hwb_float_t>;
///@}

/** @name OkLab/OkLCH Generators */
///@{
using oklab_generator = basic_random_generator<core::oklab_t>;
using oklch_generator = basic_random_generator<core::oklch_t>;
///@}

/** @name CIELAB/CIELCH Generators */
///@{
using cielab_generator = basic_random_generator<core::cielab_t>;
using cielch_generator = basic_random_generator<core::cielch_t>;
///@}

/** @name CMYK Generator */
///@{
using cmyk_generator = basic_random_generator<core::cmyk8_t>;
using cmyk_float_generator = basic_random_generator<core::cmyk_float_t>;
///@}

/** @name Display P3 Generators */
///@{
using display_p3_generator = basic_random_generator<core::display_p3f_t>;
using display_p3a_generator = basic_random_generator<core::display_p3af_t>;
///@}

/** @name Harmony Generator */
///@{
using harmony_gen = harmony_generator<core::hsla_float_t>;
///@}

/** @name Golden Angle Generator */
///@{
using golden_gen = golden_angle_generator<core::hsl_float_t>;
///@}

}  // namespace colorcpp::operations::random

namespace colorcpp::operations {

/**
 * @brief Generate a random color of the specified type.
 *
 * This is a convenience function that creates a random color generator
 * and returns a single random color.
 *
 * @tparam Color Color type to generate.
 * @param seed Optional seed for reproducibility (default: random_device).
 * @return Random color of the specified type.
 *
 * Example:
 * @code
 * auto color = random_color<core::rgbf_t>();
 * auto color2 = random_color<core::hsl_float_t>(12345);
 * @endcode
 */
template <typename Color>
Color random_color(typename std::mt19937::result_type seed = std::random_device{}()) {
  random::basic_random_generator<Color> gen(seed);
  return gen.next();
}

/**
 * @brief Generate multiple random colors of the specified type.
 *
 * @tparam Color Color type to generate.
 * @param count Number of colors to generate.
 * @param seed Optional seed for reproducibility (default: random_device).
 * @return Vector of random colors.
 */
template <typename Color>
std::vector<Color> random_colors(std::size_t count, typename std::mt19937::result_type seed = std::random_device{}()) {
  random::basic_random_generator<Color> gen(seed);
  return gen.generate_n(count);
}

}  // namespace colorcpp::operations
