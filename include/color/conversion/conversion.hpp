/**
 * @file conversion.hpp
 * @brief Color space conversion utilities
 *
 * Provides high-level conversion functions and utilities for converting
 * between different color spaces. Includes template-based conversion
 * infrastructure and convenience functions for common conversions.
 *
 * @author Merlot.Qi
 *
 */

#pragma once

#include <type_traits>

#include "cmyk_to_hsl.hpp"
#include "cmyk_to_hsv.hpp"
#include "cmyk_to_rgb.hpp"
#include "hsl_to_cmyk.hpp"
#include "hsl_to_hsv.hpp"
#include "hsl_to_rgb.hpp"
#include "hsv_to_cmyk.hpp"
#include "hsv_to_hsl.hpp"
#include "hsv_to_rgb.hpp"
#include "rgb_to_cmyk.hpp"
#include "rgb_to_hsl.hpp"
#include "rgb_to_hsv.hpp"

namespace color::conversion {

/**
 * @brief Implementation details for color conversion
 *
 * Contains internal conversion logic and template specializations
 * for handling color space conversions.
 */
namespace details {

/**
 * @brief Generic converter template
 *
 * Template struct for converting between color types.
 * Handles both same-type conversions (identity) and cross-type conversions.
 *
 * @tparam TargetType Target color type
 * @tparam SourceType Source color type
 */
template <typename TargetType, typename SourceType>
struct converter {
  /**
   * @brief Apply conversion from source to target type
   *
   * @param src Source color value
   * @return Converted color value of target type
   */
  static constexpr TargetType apply(const SourceType& src) {
    if constexpr (std::is_same<TargetType, SourceType>::value) {
      return src;  // Identity conversion
    } else {
      return convert(src);  // Cross-type conversion
    }
  }
};

}  // namespace details

/**
 * @brief Generic color space conversion function
 *
 * Converts a color from one color space to another using compile-time
 * template resolution. Supports conversions between RGB, HSV, HSL, and CMYK.
 *
 * @tparam TargetType Target color type
 * @tparam SourceType Source color type
 * @param src Source color value to convert
 * @return Converted color value of target type
 */
template <typename TargetType, typename SourceType>
constexpr TargetType convert(const SourceType& src) {
  return details::converter<TargetType, SourceType>::apply(src);
}

/**
 * @brief Convert to HSV color space
 *
 * Convenience function for converting any color type to HSV.
 *
 * @tparam T Value type for the target HSV color
 * @tparam Scale Scaling factor for the target HSV color
 * @tparam SourceType Source color type
 * @param src Source color value
 * @return HSV color with specified value type and scale
 */
template <typename T, intptr_t Scale, typename SourceType>
constexpr auto to_hsv(const SourceType& src) {
  using target_t = core::basic_hsv<T, Scale>;
  return convert<target_t>(src);
}

/**
 * @brief Convert to HSL color space
 *
 * Convenience function for converting any color type to HSL.
 *
 * @tparam T Value type for the target HSL color
 * @tparam Scale Scaling factor for the target HSL color
 * @tparam SourceType Source color type
 * @param src Source color value
 * @return HSL color with specified value type and scale
 */
template <typename T, intptr_t Scale, typename SourceType>
constexpr auto to_hsl(const SourceType& src) {
  using target_t = core::basic_hsl<T, Scale>;
  return convert<target_t>(src);
}

/**
 * @brief Convert to RGB color space
 *
 * Convenience function for converting any color type to RGB.
 *
 * @tparam T Value type for the target RGB color
 * @tparam Scale Scaling factor for the target RGB color
 * @tparam SourceType Source color type
 * @param src Source color value
 * @return RGB color with specified value type and scale
 */
template <typename T, intptr_t Scale, typename SourceType>
constexpr auto to_rgb(const SourceType& src) {
  using target_t = core::basic_rgb<T, Scale>;
  return convert<target_t>(src);
}

/**
 * @brief Convert to CMYK color space
 *
 * Convenience function for converting any color type to CMYK.
 *
 * @tparam T Value type for the target CMYK color
 * @tparam Scale Scaling factor for the target CMYK color
 * @tparam SourceType Source color type
 * @param src Source color value
 * @return CMYK color with specified value type and scale
 */
template <typename T, intptr_t Scale, typename SourceType>
constexpr auto to_cmyk(const SourceType& src) {
  using target_t = core::basic_cmyk<T, Scale>;
  return convert<target_t>(src);
}

}  // namespace color::conversion
