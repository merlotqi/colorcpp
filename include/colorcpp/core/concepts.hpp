/**
 * @file concepts.hpp
 * @brief Type traits for color models: channels, tags, storage tuples, and @ref colorcpp::traits::model_traits.
 */

#pragma once

#include <cstdint>
#include <limits>
#include <string_view>
#include <tuple>
#include <type_traits>

/**
 * @brief Compile-time descriptors for color channels and models used by @ref colorcpp::core::basic_color.
 */
namespace colorcpp::traits {

namespace details {

template <typename Tuple, typename Tag, std::size_t I = 0>
struct find_channel_index_impl {
 private:
  static constexpr std::size_t compute() {
    if constexpr (I >= std::tuple_size_v<Tuple>) {
      return std::tuple_size_v<Tuple>;
    } else {
      using channel_t = std::tuple_element_t<I, Tuple>;
      if constexpr (std::is_same_v<typename channel_t::tag_type, Tag>) {
        return I;
      } else {
        return find_channel_index_impl<Tuple, Tag, I + 1>::value;
      }
    }
  }

 public:
  static constexpr std::size_t value = compute();
};

template <typename Tuple, typename Tag>
struct has_channel_tag_impl
    : std::bool_constant<(find_channel_index_impl<Tuple, Tag>::value < std::tuple_size_v<Tuple>)> {};

}  // namespace details

/** @brief Default [min,max] for a scalar type when a channel does not override range. */
template <typename T>
struct default_range {
  static constexpr T min = static_cast<T>(0);

  static constexpr T max = std::is_floating_point_v<T> ? static_cast<T>(1) : std::numeric_limits<T>::max();
};

/** @brief Optional per-channel range override (defaults to @ref default_range). */
template <typename Channel, typename T>
struct channel_traits {
  static constexpr T min = default_range<T>::min;
  static constexpr T max = default_range<T>::max;
};

/**
 * @brief Declares one channel: tag, scalar type, and inclusive range as rational Min/Den … Max/Den.
 * @tparam Den Normalization denominator (often 1 for “natural” LAB/XYZ units).
 */
template <typename Tag, typename T, int64_t Min, int64_t Max, int64_t Den = Max>
struct basic_channel {
  static_assert(Den != 0, "colorcpp: channel denominator cannot be zero");

  using tag_type = Tag;
  using value_type = T;

  static constexpr T min = static_cast<T>(Min) / static_cast<T>(Den);

  static constexpr T max = static_cast<T>(Max) / static_cast<T>(Den);

  static_assert(min <= max, "colorcpp: channel minimum must not exceed maximum");
};

/**
 * @brief Primary template; specialize with @c channels_type (tuple of @ref basic_channel), @c channel_size, optional @c
 * prefix for I/O.
 */
template <typename Model>
struct model_traits {
  static constexpr std::string_view prefix = "";
};

template <typename T, typename = void>
struct is_model_traits : std::false_type {};

template <typename T>
struct is_model_traits<T, std::void_t<decltype(model_traits<T>::channel_size), typename model_traits<T>::channels_type>>
    : std::true_type {};

template <typename T>
inline constexpr bool is_model_traits_v = is_model_traits<T>::value;

/** @brief Whether @p Model's channel tuple includes @p Tag. */
template <typename Model, typename Tag>
struct has_channel_tag {
 private:
  using channels_tuple = typename model_traits<Model>::channels_type;

 public:
  static constexpr bool value = details::has_channel_tag_impl<channels_tuple, Tag>::value;
};

template <typename Model, typename Tag>
inline constexpr bool has_channel_tag_v = has_channel_tag<Model, Tag>::value;

/** @brief Zero-based index of channel @p Tag in @p Model (compile-time error if missing). */
template <typename Model, typename Tag>
struct channel_index {
 private:
  using channels_tuple = typename model_traits<Model>::channels_type;

  static_assert(has_channel_tag_v<Model, Tag>, "colorcpp: requested channel tag does not exist in this color model");

 public:
  static constexpr std::size_t value = details::find_channel_index_impl<channels_tuple, Tag>::value;
};

template <typename Model, typename Tag>
inline constexpr std::size_t channel_index_v = channel_index<Model, Tag>::value;

/**
 * @brief Maps @c channels_type = @c tuple<Ch0,…> to @c tuple<Ch0::value_type,…> for @ref colorcpp::core::basic_color
 * storage.
 */
template <typename ChannelsTuple>
struct channels_storage;

/** @brief Tuple of each channel's @c value_type. */
template <typename... Channels>
struct channels_storage<std::tuple<Channels...>> {
  using type = std::tuple<typename Channels::value_type...>;
};

template <typename ChannelsTuple>
using channels_storage_t = typename channels_storage<ChannelsTuple>::type;

}  // namespace colorcpp::traits
