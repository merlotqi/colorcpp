#pragma once

#include <concepts>
#include <limits>
#include <type_traits>

namespace colorcpp::traits {

template <typename T>
concept channel_value = std::is_arithmetic_v<T>;

template <typename T>
struct default_range {
  static constexpr T min = static_cast<T>(0);
  static constexpr T max = std::is_floating_point_v<T> ? static_cast<T>(1) : std::numeric_limits<T>::max();
};

template <typename Channel, typename T>
struct channel_traits {
  static constexpr T min = default_range<T>::min;
  static constexpr T max = default_range<T>::max;
};

template <typename Tag, typename T, T Min, T Max>
struct packed_channel {
  using tag_type = Tag;
  using value_type = T;
  static constexpr T min = Min;
  static constexpr T max = Max;
};

template <typename Model>
struct model_traits;

template <typename T>
concept is_model_traits = requires {
  { model_traits<T>::channel_size } -> std::convertible_to<std::size_t>;
  typename model_traits<T>::channels_type;
};

}  // namespace colorcpp::traits
