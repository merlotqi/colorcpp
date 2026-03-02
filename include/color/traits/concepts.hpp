#pragma once

#include <type_traits>

#include "../core/cmyk.hpp"
#include "../core/hsl.hpp"
#include "../core/hsv.hpp"
#include "../core/rgb.hpp"

namespace color::traits {

// 颜色类型特征
template <typename T>
struct is_color : std::false_type {};

template <typename T, T R, T G, T B>
struct is_color<core::basic_rgb<T, R, G, B>> : std::true_type {};

template <typename T, T H, T S, T V>
struct is_color<core::basic_hsv<T, H, S, V>> : std::true_type {};

template <typename T, T H, T S, T L>
struct is_color<core::basic_hsl<T, H, S, L>> : std::true_type {};

template <typename T, T C, T M, T Y, T K>
struct is_color<core::basic_cmyk<T, C, M, Y, K>> : std::true_type {};

template <typename T>
constexpr bool is_color_v = is_color<T>::value;

// RGB颜色特征
template <typename T>
struct is_rgb : std::false_type {};

template <typename T, T R, T G, T B>
struct is_rgb<core::basic_rgb<T, R, G, B>> : std::true_type {};

template <typename T>
constexpr bool is_rgb_v = is_rgb<T>::value;

// HSV颜色特征
template <typename T>
struct is_hsv : std::false_type {};

template <typename T, T H, T S, T V>
struct is_hsv<core::basic_hsv<T, H, S, V>> : std::true_type {};

template <typename T>
constexpr bool is_hsv_v = is_hsv<T>::value;

// HSL颜色特征
template <typename T>
struct is_hsl : std::false_type {};

template <typename T, T H, T S, T L>
struct is_hsl<core::basic_hsl<T, H, S, L>> : std::true_type {};

template <typename T>
constexpr bool is_hsl_v = is_hsl<T>::value;

// CMYK颜色特征
template <typename T>
struct is_cmyk : std::false_type {};

template <typename T, T C, T M, T Y, T K>
struct is_cmyk<core::basic_cmyk<T, C, M, Y, K>> : std::true_type {};

template <typename T>
constexpr bool is_cmyk_v = is_cmyk<T>::value;

// 可转换颜色特征
template <typename From, typename To>
struct is_convertible_color {
  static constexpr bool value = is_color_v<From> && is_color_v<To>;
};

template <typename From, typename To>
constexpr bool is_convertible_color_v = is_convertible_color<From, To>::value;

// 可操作颜色特征
template <typename T>
struct is_operable_color {
  static constexpr bool value = is_rgb_v<T>;
};

template <typename T>
constexpr bool is_operable_color_v = is_operable_color<T>::value;

}  // namespace color::traits
