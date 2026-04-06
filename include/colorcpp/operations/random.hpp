/**
 * @file random.hpp
 * @brief Random color and harmony generators (RGB, HSL/HSV, golden angle, palette helpers).
 *
 * This is an aggregate header that includes all random generation components.
 * Individual components can be included separately for faster compilation.
 *
 * @par Thread safety
 * A single generator object is **not** thread-safe: implementations use a `mutable` random engine and `const` member
 * functions such as `next()` still advance that engine. Use one generator per thread, external locking, or separate
 * instances. `harmony_generator::next_poisson` also mutates the caller's `samples` vector; synchronize access to that
 * vector if it is shared.
 *
 * @par Perceptual / display spaces
 * `basic_random_generator` draws each channel uniformly in its declared range. For OkLab, CIELAB, OkLCH, CIELCH, etc.,
 * that is an **axis-aligned box** in those coordinates; samples are **not** restricted to sRGB, Display P3, or other
 * display gamuts (clip or map separately if needed).
 *
 * @par HSLA / HSVA / HWBA
 * `basic_hsl_generator` randomizes alpha for four-channel models using `options::a_min` / `a_max` (default full range).
 *
 * @par Convenience functions
 * `random_color` / `random_colors` build a new generator per call. The default seed uses `std::random_device{}()` each
 * time, which is implementation-defined (quality, blocking, and performance vary by platform).
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
 * @param seed Seed for reproducibility. The default constructs a new `std::random_device` per call (see file note).
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
 * @param seed Seed for reproducibility. The default constructs a new `std::random_device` per call (see file note).
 * @return Vector of random colors.
 */
template <typename Color>
std::vector<Color> random_colors(std::size_t count, typename std::mt19937::result_type seed = std::random_device{}()) {
  random::basic_random_generator<Color> gen(seed);
  return gen.generate_n(count);
}

}  // namespace colorcpp::operations
