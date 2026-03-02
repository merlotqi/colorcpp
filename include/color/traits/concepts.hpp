/**
 * @file concepts.hpp
 * @brief Color type concepts and traits (Updated for Scale system)
 *
 * Provides Modern C++ concepts and type traits to support the Scale-based
 * color system, ensuring compile-time safety across different color spaces.
 */

#pragma once

#include <cstdint>
#include <type_traits>

#include "../core/cmyk.hpp"
#include "../core/hsl.hpp"
#include "../core/hsv.hpp"
#include "../core/rgb.hpp"

namespace color::traits {

template <typename T>
struct is_color : std::false_type {};

template <typename T, intptr_t Scale>
struct is_color<core::basic_rgb<T, Scale>> : std::true_type {};

template <typename T, intptr_t Scale>
struct is_color<core::basic_hsv<T, Scale>> : std::true_type {};

template <typename T, intptr_t Scale>
struct is_color<core::basic_hsl<T, Scale>> : std::true_type {};

template <typename T, intptr_t Scale>
struct is_color<core::basic_cmyk<T, Scale>> : std::true_type {};

template <typename T>
constexpr bool is_color_v = is_color<T>::value;

template <typename T>
struct is_rgb : std::false_type {};
template <typename T, intptr_t Scale>
struct is_rgb<core::basic_rgb<T, Scale>> : std::true_type {};

template <typename T>
struct is_hsv : std::false_type {};
template <typename T, intptr_t Scale>
struct is_hsv<core::basic_hsv<T, Scale>> : std::true_type {};

template <typename T>
struct is_hsl : std::false_type {};
template <typename T, intptr_t Scale>
struct is_hsl<core::basic_hsl<T, Scale>> : std::true_type {};

template <typename T>
struct is_cmyk : std::false_type {};
template <typename T, intptr_t Scale>
struct is_cmyk<core::basic_cmyk<T, Scale>> : std::true_type {};

#if __cplusplus >= 202002L

template <typename T>
concept ColorType = requires {
  typename T::value_type;
  { T::scale() } -> std::same_as<intptr_t>;
};

template <typename T>
concept RgbColor = ColorType<T> && is_rgb<T>::value;

template <typename T>
concept HsvColor = ColorType<T> && is_hsv<T>::value;

template <typename T>
concept HslColor = ColorType<T> && is_hsl<T>::value;

template <typename T>
concept CmykColor = ColorType<T> && is_cmyk<T>::value;

#endif

template <typename From, typename To>
struct is_convertible_color {
  static constexpr bool value = is_color_v<From> && is_color_v<To>;
};

template <typename From, typename To>
constexpr bool is_convertible_color_v = is_convertible_color<From, To>::value;

template <typename T>
struct is_operable_color {
  static constexpr bool value = is_color_v<T>;
};

template <typename T>
constexpr bool is_operable_color_v = is_operable_color<T>::value;

}  // namespace color::traits
