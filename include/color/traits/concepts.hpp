/**
 * @file concepts.hpp
 * @brief Color type concepts and traits
 *
 * Provides type traits and concepts for color type validation and
 * categorization. These traits help determine color type categories
 * and validate color operations at compile time.
 *
 * @author Color Library Team
 * @date 2025
 * @version 1.0
 */

#pragma once

#include <type_traits>

#include "../core/cmyk.hpp"
#include "../core/hsl.hpp"
#include "../core/hsv.hpp"
#include "../core/rgb.hpp"

namespace color::traits {

/**
 * @brief Color type trait
 *
 * Determines if a type is a valid color type (RGB, HSV, HSL, or CMYK).
 *
 * @tparam T Type to check
 */
template <typename T>
struct is_color : std::false_type {};

/**
 * @brief RGB color type specialization
 *
 * @tparam T Value type
 * @tparam R Red component value
 * @tparam G Green component value
 * @tparam B Blue component value
 */
template <typename T, T R, T G, T B>
struct is_color<core::basic_rgb<T, R, G, B>> : std::true_type {};

/**
 * @brief HSV color type specialization
 *
 * @tparam T Value type
 * @tparam H Hue component value
 * @tparam S Saturation component value
 * @tparam V Value component value
 */
template <typename T, T H, T S, T V>
struct is_color<core::basic_hsv<T, H, S, V>> : std::true_type {};

/**
 * @brief HSL color type specialization
 *
 * @tparam T Value type
 * @tparam H Hue component value
 * @tparam S Saturation component value
 * @tparam L Lightness component value
 */
template <typename T, T H, T S, T L>
struct is_color<core::basic_hsl<T, H, S, L>> : std::true_type {};

/**
 * @brief CMYK color type specialization
 *
 * @tparam T Value type
 * @tparam C Cyan component value
 * @tparam M Magenta component value
 * @tparam Y Yellow component value
 * @tparam K Key (black) component value
 */
template <typename T, T C, T M, T Y, T K>
struct is_color<core::basic_cmyk<T, C, M, Y, K>> : std::true_type {};

/**
 * @brief Color type trait value alias
 *
 * @tparam T Type to check
 * @return true if T is a color type, false otherwise
 */
template <typename T>
constexpr bool is_color_v = is_color<T>::value;

/**
 * @brief RGB color trait
 *
 * Determines if a type is specifically an RGB color type.
 *
 * @tparam T Type to check
 */
template <typename T>
struct is_rgb : std::false_type {};

/**
 * @brief RGB color trait specialization
 *
 * @tparam T Value type
 * @tparam R Red component value
 * @tparam G Green component value
 * @tparam B Blue component value
 */
template <typename T, T R, T G, T B>
struct is_rgb<core::basic_rgb<T, R, G, B>> : std::true_type {};

/**
 * @brief RGB color trait value alias
 *
 * @tparam T Type to check
 * @return true if T is an RGB color type, false otherwise
 */
template <typename T>
constexpr bool is_rgb_v = is_rgb<T>::value;

/**
 * @brief HSV color trait
 *
 * Determines if a type is specifically an HSV color type.
 *
 * @tparam T Type to check
 */
template <typename T>
struct is_hsv : std::false_type {};

/**
 * @brief HSV color trait specialization
 *
 * @tparam T Value type
 * @tparam H Hue component value
 * @tparam S Saturation component value
 * @tparam V Value component value
 */
template <typename T, T H, T S, T V>
struct is_hsv<core::basic_hsv<T, H, S, V>> : std::true_type {};

/**
 * @brief HSV color trait value alias
 *
 * @tparam T Type to check
 * @return true if T is an HSV color type, false otherwise
 */
template <typename T>
constexpr bool is_hsv_v = is_hsv<T>::value;

/**
 * @brief HSL color trait
 *
 * Determines if a type is specifically an HSL color type.
 *
 * @tparam T Type to check
 */
template <typename T>
struct is_hsl : std::false_type {};

/**
 * @brief HSL color trait specialization
 *
 * @tparam T Value type
 * @tparam H Hue component value
 * @tparam S Saturation component value
 * @tparam L Lightness component value
 */
template <typename T, T H, T S, T L>
struct is_hsl<core::basic_hsl<T, H, S, L>> : std::true_type {};

/**
 * @brief HSL color trait value alias
 *
 * @tparam T Type to check
 * @return true if T is an HSL color type, false otherwise
 */
template <typename T>
constexpr bool is_hsl_v = is_hsl<T>::value;

/**
 * @brief CMYK color trait
 *
 * Determines if a type is specifically a CMYK color type.
 *
 * @tparam T Type to check
 */
template <typename T>
struct is_cmyk : std::false_type {};

/**
 * @brief CMYK color trait specialization
 *
 * @tparam T Value type
 * @tparam C Cyan component value
 * @tparam M Magenta component value
 * @tparam Y Yellow component value
 * @tparam K Key (black) component value
 */
template <typename T, T C, T M, T Y, T K>
struct is_cmyk<core::basic_cmyk<T, C, M, Y, K>> : std::true_type {};

/**
 * @brief CMYK color trait value alias
 *
 * @tparam T Type to check
 * @return true if T is a CMYK color type, false otherwise
 */
template <typename T>
constexpr bool is_cmyk_v = is_cmyk<T>::value;

/**
 * @brief Color conversion trait
 *
 * Determines if two color types can be converted between each other.
 * Both types must be valid color types.
 *
 * @tparam From Source color type
 * @tparam To Target color type
 */
template <typename From, typename To>
struct is_convertible_color {
  static constexpr bool value = is_color_v<From> && is_color_v<To>;
};

/**
 * @brief Color conversion trait value alias
 *
 * @tparam From Source color type
 * @tparam To Target color type
 * @return true if conversion is possible, false otherwise
 */
template <typename From, typename To>
constexpr bool is_convertible_color_v = is_convertible_color<From, To>::value;

/**
 * @brief Color operation trait
 *
 * Determines if a color type supports color operations.
 * Currently only RGB colors support operations.
 *
 * @tparam T Color type to check
 */
template <typename T>
struct is_operable_color {
  static constexpr bool value = is_rgb_v<T>;
};

/**
 * @brief Color operation trait value alias
 *
 * @tparam T Color type to check
 * @return true if operations are supported, false otherwise
 */
template <typename T>
constexpr bool is_operable_color_v = is_operable_color<T>::value;

}  // namespace color::traits
