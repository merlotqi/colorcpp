/**
 * @file concepts.hpp
 * @brief Color type concepts and traits (Updated for Scale system)
 *
 * Provides Modern C++ concepts and type traits to support the Scale-based
 * color system, ensuring compile-time safety across different color spaces.
 */

#pragma once

#include <type_traits>

namespace color::category {

struct rgb {};
struct hsv {};
struct hsl {};
struct cmyk {};

}  // namespace color::category

namespace color::traits {

template <typename T, typename = void>
struct is_color : std::false_type {};

template <typename T>
struct is_color<T, typename std::enable_if<std::is_class<T>::value &&
                                           std::is_same<typename T::value_type, typename T::value_type>::value>::type>
    : std::true_type {};

template <typename T>
constexpr bool is_color_v = is_color<T>::value;

template <typename T, typename = void>
struct is_rgb : std::false_type {};

template <typename T>
struct is_rgb<T,
              typename std::enable_if<is_color_v<T> && std::is_same<typename T::color_tag, category::rgb>::value>::type>
    : std::true_type {};

template <typename T>
constexpr bool is_rgb_v = is_rgb<T>::value;

template <typename T, typename = void>
struct is_hsv : std::false_type {};

template <typename T>
struct is_hsv<T,
              typename std::enable_if<is_color_v<T> && std::is_same<typename T::color_tag, category::hsv>::value>::type>
    : std::true_type {};

template <typename T>
constexpr bool is_hsv_v = is_hsv<T>::value;

template <typename T, typename = void>
struct is_hsl : std::false_type {};

template <typename T>
struct is_hsl<T,
              typename std::enable_if<is_color_v<T> && std::is_same<typename T::color_tag, category::hsl>::value>::type>
    : std::true_type {};

template <typename T>
constexpr bool is_hsl_v = is_hsl<T>::value;

template <typename T, typename = void>
struct is_cmyk : std::false_type {};

template <typename T>
struct is_cmyk<
    T, typename std::enable_if<is_color_v<T> && std::is_same<typename T::color_tag, category::cmyk>::value>::type>
    : std::true_type {};

template <typename T>
constexpr bool is_cmyk_v = is_cmyk<T>::value;

template <typename T, typename = void>
struct has_alpha : std::false_type {};

template <typename T>
struct has_alpha<T,
                 typename std::enable_if<is_color_v<T> && std::is_member_object_pointer<decltype(&T::a)>::value>::type>
    : std::true_type {};

template <typename T>
constexpr bool has_alpha_v = has_alpha<T>::value;

template <typename From, typename To>
struct is_convertible_color : std::integral_constant<bool, is_color_v<From> && is_color_v<To> > {};

template <typename From, typename To>
constexpr bool is_convertible_color_v = is_convertible_color<From, To>::value;

}  // namespace color::traits
