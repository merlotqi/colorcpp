/**
 * @file filter.hpp
 * @brief Color filter operations
 *
 * Provides compile-time color filter operations including grayscale conversion,
 * color inversion, thresholding, and posterization. These operations modify
 * colors in various ways to achieve different visual effects.
 *
 * @author Color Library Team
 * @date 2025
 * @version 1.0
 */

#pragma once

#include "../core/rgb.hpp"

namespace color::operations {

/**
 * @brief Grayscale conversion operation
 *
 * Template struct for converting a color to grayscale using the luminance formula.
 * The luminance formula weights the RGB components to match human perception.
 *
 * @tparam RGBType Source RGB color type
 */
template <typename RGBType>
struct grayscale {
  using rgb_type = RGBType;

  /// @brief Calculated grayscale value using luminance formula
  static constexpr auto gray =
      static_cast<typename RGBType::value_type>(RGBType::r * 0.299 + RGBType::g * 0.587 + RGBType::b * 0.114);

  /// @brief Resulting grayscale RGB type
  using type = core::rgb8<gray, gray, gray>;
};

/**
 * @brief Color inversion operation
 *
 * Template struct for inverting all color channels of an RGB color.
 * Each channel value is subtracted from 255 to produce the complementary color.
 *
 * @tparam RGBType Source RGB color type
 */
template <typename RGBType>
struct invert {
  using rgb_type = RGBType;

  /// @brief Inverted red channel value
  static constexpr auto r = 255 - RGBType::r;
  /// @brief Inverted green channel value
  static constexpr auto g = 255 - RGBType::g;
  /// @brief Inverted blue channel value
  static constexpr auto b = 255 - RGBType::b;

  /// @brief Resulting inverted RGB type
  using type = core::rgb8<r, g, b>;
};

/**
 * @brief Threshold filter operation
 *
 * Template struct for applying a binary threshold to each color channel.
 * Values above the threshold become 255, values below become 0.
 *
 * @tparam RGBType Source RGB color type
 * @tparam Threshold Threshold value (0-255)
 */
template <typename RGBType, int Threshold>
struct threshold {
  using rgb_type = RGBType;
  static_assert(Threshold >= 0 && Threshold <= 255, "Threshold must be 0-255");

  /// @brief Thresholded red channel value
  static constexpr auto r = (RGBType::r > Threshold) ? 255 : 0;
  /// @brief Thresholded green channel value
  static constexpr auto g = (RGBType::g > Threshold) ? 255 : 0;
  /// @brief Thresholded blue channel value
  static constexpr auto b = (RGBType::b > Threshold) ? 255 : 0;

  /// @brief Resulting thresholded RGB type
  using type = core::rgb8<r, g, b>;
};

/**
 * @brief Posterization filter operation
 *
 * Template struct for reducing the number of color levels in an image.
 * Each channel is quantized to discrete levels, creating a poster-like effect.
 *
 * @tparam RGBType Source RGB color type
 * @tparam Levels Number of color levels (1-256)
 */
template <typename RGBType, int Levels>
struct posterize {
  using rgb_type = RGBType;
  static_assert(Levels > 0 && Levels <= 256, "Levels must be 1-256");

  /// @brief Step size for quantization
  static constexpr auto step = 256 / Levels;

  /// @brief Posterized red channel value
  static constexpr auto r = (RGBType::r / step) * step;
  /// @brief Posterized green channel value
  static constexpr auto g = (RGBType::g / step) * step;
  /// @brief Posterized blue channel value
  static constexpr auto b = (RGBType::b / step) * step;

  /// @brief Resulting posterized RGB type
  using type = core::rgb8<r, g, b>;
};

/**
 * @brief Type alias for grayscale conversion result
 *
 * @tparam RGBType Source RGB color type
 * @return RGB color type with equal R, G, B values (grayscale)
 */
template <typename RGBType>
using grayscale_t = typename grayscale<RGBType>::type;

/**
 * @brief Type alias for color inversion result
 *
 * @tparam RGBType Source RGB color type
 * @return RGB color type with inverted channel values
 */
template <typename RGBType>
using invert_t = typename invert<RGBType>::type;

/**
 * @brief Type alias for threshold filter result
 *
 * @tparam RGBType Source RGB color type
 * @tparam Threshold Threshold value (0-255)
 * @return RGB color type with binary channel values
 */
template <typename RGBType, int Threshold>
using threshold_t = typename threshold<RGBType, Threshold>::type;

/**
 * @brief Type alias for posterization filter result
 *
 * @tparam RGBType Source RGB color type
 * @tparam Levels Number of color levels (1-256)
 * @return RGB color type with quantized channel values
 */
template <typename RGBType, int Levels>
using posterize_t = typename posterize<RGBType, Levels>::type;

}  // namespace color::operations
