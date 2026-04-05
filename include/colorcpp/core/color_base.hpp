/**
 * @file color_base.hpp
 * @brief @ref colorcpp::core::basic_color — @c std::array storage for trait-defined color models.
 */

#pragma once

#include <array>
#include <colorcpp/core/concepts.hpp>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>

namespace colorcpp::core {

/**
 * @brief Primary template; only specializations for models with @ref colorcpp::traits::model_traits are defined.
 * @tparam Model A tag type (e.g. rgb::model::rgba8) described by model_traits.
 */
template <typename Model, typename = void>
struct basic_color;

/**
 * @brief Color value as a contiguous @c std::array of channel scalars (see @ref colorcpp::traits::channels_storage_t).
 *
 * All channels in a model share one scalar type @c value_type (required by @ref colorcpp::traits::channels_storage).
 *
 * @tparam Model Color model tag with @ref colorcpp::traits::model_traits (channels, min/max per channel).
 *
 * Channel access:
 * - @c get<Tag>() / @c get_index<I>() for read/write references.
 * - @c set<Tag>(v) validates @c v against the channel range; throws @c std::out_of_range if invalid.
 *
 * Construction from channel values performs the same range check (throws @c std::out_of_range).
 */
template <typename Model>
struct basic_color<Model, std::enable_if_t<traits::is_model_traits_v<Model>>> {
  using trait_model = traits::model_traits<Model>;

  using channels_tuple = typename trait_model::channels_type;

  static constexpr std::size_t channels = trait_model::channel_size;

  using first_channel = std::tuple_element_t<0, channels_tuple>;

  using value_type = typename first_channel::value_type;

  template <std::size_t I>
  using channel_value_t = typename std::tuple_element_t<I, channels_tuple>::value_type;

  using storage_type = traits::channels_storage_t<channels_tuple>;

  storage_type data{};

 public:
  constexpr basic_color() = default;

  template <typename... Args, typename = std::enable_if_t<sizeof...(Args) == channels>>
  constexpr explicit basic_color(Args... args) : data{make_storage(std::make_index_sequence<channels>{}, args...)} {
    check_range(std::make_index_sequence<channels>{}, args...);
  }

  template <typename Tag>
  constexpr auto& get() {
    static_assert(traits::has_channel_tag_v<Model, Tag>,
                  "colorcpp: requested channel tag does not exist in this model");

    constexpr std::size_t idx = traits::channel_index_v<Model, Tag>;
    return data[idx];
  }

  template <typename Tag>
  constexpr const auto& get() const {
    static_assert(traits::has_channel_tag_v<Model, Tag>,
                  "colorcpp: requested channel tag does not exist in this model");

    constexpr std::size_t idx = traits::channel_index_v<Model, Tag>;
    return data[idx];
  }

  template <typename Tag, typename T>
  constexpr void set(T v) {
    static_assert(traits::has_channel_tag_v<Model, Tag>,
                  "colorcpp: requested channel tag does not exist in this model");

    constexpr std::size_t idx = traits::channel_index_v<Model, Tag>;
    using channel_t = std::tuple_element_t<idx, channels_tuple>;

    if (!(v >= static_cast<T>(channel_t::min) && v <= static_cast<T>(channel_t::max))) {
      throw std::out_of_range("colorcpp: channel value out of range");
    }

    data[idx] = static_cast<channel_value_t<idx>>(v);
  }

  template <std::size_t I>
  constexpr auto& get_index() {
    static_assert(I < channels, "colorcpp: channel index out of range");
    return data[I];
  }

  template <std::size_t I>
  constexpr const auto& get_index() const {
    static_assert(I < channels, "colorcpp: channel index out of range");
    return data[I];
  }

  /**
   * @brief Exact equality comparison.
   * @param other Color to compare with.
   * @return True if all channels are exactly equal.
   */
  constexpr bool operator==(const basic_color& other) const noexcept {
    return equal_impl(other, std::make_index_sequence<channels>{});
  }

  /**
   * @brief Exact inequality comparison.
   * @param other Color to compare with.
   * @return True if any channel differs.
   */
  constexpr bool operator!=(const basic_color& other) const noexcept { return !(*this == other); }

 private:
  // Helper for exact equality comparison
  template <std::size_t... Is>
  constexpr bool equal_impl(const basic_color& other, std::index_sequence<Is...>) const noexcept {
    return ((get_index<Is>() == other.get_index<Is>()) && ...);
  }

  template <std::size_t... Is, typename... Args>
  constexpr static storage_type make_storage(std::index_sequence<Is...>, Args... args) {
    auto tp = std::make_tuple(args...);
    return storage_type{static_cast<value_type>(static_cast<channel_value_t<Is>>(std::get<Is>(tp)))...};
  }

  // Validates each arg against its channel's [min, max] using the original (uncast) values,
  // so out-of-range inputs are detected before any narrowing conversion.
  template <std::size_t... Is, typename... Args>
  constexpr void check_range(std::index_sequence<Is...>, Args... args) {
    auto tp = std::make_tuple(args...);
    bool valid = ((std::get<Is>(tp) >= std::tuple_element_t<Is, channels_tuple>::min &&
                   std::get<Is>(tp) <= std::tuple_element_t<Is, channels_tuple>::max) &&
                  ...);

    if (!valid) {
      throw std::out_of_range("colorcpp: channel value out of range");
    }
  }
};

/** @brief Alias for @ref basic_color. */
template <typename Model>
using basic_color_t = basic_color<Model>;

}  // namespace colorcpp::core

namespace colorcpp {

/**
 * @brief Access a channel by tag on any color type that exposes @c template get<Tag>().
 * @tparam Tag Channel tag type (e.g. rgb::channel::r_tag).
 */
template <typename Tag, typename Color>
constexpr auto& get(Color& c) {
  return c.template get<Tag>();
}

/** @overload */
template <typename Tag, typename Color>
constexpr const auto& get(const Color& c) {
  return c.template get<Tag>();
}

/**
 * @brief Access channel @p I (0-based) on @ref colorcpp::core::basic_color.
 * @tparam I Channel index less than model_traits<Model>::channel_size.
 */
template <std::size_t I, typename Color>
constexpr auto& get(Color& c) {
  return c.template get_index<I>();
}

/** @overload */
template <std::size_t I, typename Color>
constexpr const auto& get(const Color& c) {
  return c.template get_index<I>();
}

}  // namespace colorcpp

namespace std {

template <typename Model>
struct tuple_size<colorcpp::core::basic_color<Model>>
    : std::integral_constant<std::size_t, colorcpp::traits::model_traits<Model>::channel_size> {};

// tuple_element<I>::type matches channel I descriptor; storage is std::array<value_type,N>.
template <std::size_t I, typename Model>
struct tuple_element<I, colorcpp::core::basic_color<Model>> {
  using channels = typename colorcpp::traits::model_traits<Model>::channels_type;
  using type = typename std::tuple_element_t<I, channels>::value_type;
};

template <std::size_t I, typename Model>
constexpr auto& get(colorcpp::core::basic_color<Model>& c) {
  return c.template get_index<I>();
}

template <std::size_t I, typename Model>
constexpr const auto& get(const colorcpp::core::basic_color<Model>& c) {
  return c.template get_index<I>();
}

template <typename Tag, typename Model>
constexpr auto& get(colorcpp::core::basic_color<Model>& c) {
  return c.template get<Tag>();
}

template <typename Tag, typename Model>
constexpr const auto& get(const colorcpp::core::basic_color<Model>& c) {
  return c.template get<Tag>();
}

}  // namespace std
