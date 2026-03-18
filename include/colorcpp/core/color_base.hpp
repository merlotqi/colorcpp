#pragma once

#include <array>
#include <colorcpp/traits/concepts.hpp>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>

namespace colorcpp::core {

template <typename Model, typename = void>
struct basic_color;

template <typename Model>
struct basic_color<Model, std::enable_if_t<traits::is_model_traits_v<Model>>> {
  using trait_model = traits::model_traits<Model>;

  using channels_tuple = typename trait_model::channels_type;

  static constexpr std::size_t channels = trait_model::channel_size;

  using first_channel = std::tuple_element_t<0, channels_tuple>;

  using value_type = typename first_channel::value_type;

  using storage_type = std::array<value_type, channels>;

  storage_type data{};

 public:
  constexpr basic_color() = default;

  template <typename... Args, typename = std::enable_if_t<sizeof...(Args) == channels>>
  constexpr explicit basic_color(Args... args) : data{static_cast<value_type>(args)...} {
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

    data[idx] = static_cast<value_type>(v);
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

 private:
  template <std::size_t... Is, typename... Args>
  constexpr void check_range(std::index_sequence<Is...>, Args... args) {
    bool valid = ((args >= std::tuple_element_t<Is, channels_tuple>::min &&
                   args <= std::tuple_element_t<Is, channels_tuple>::max) &&
                  ...);

    if (!valid) {
      throw std::out_of_range("colorcpp: channel value out of range");
    }
  }
};

template <typename Model>
using basic_color_t = basic_color<Model>;

}  // namespace colorcpp::core

namespace colorcpp {

template <typename Tag, typename Color>
constexpr auto& get(Color& c) {
  return c.template get<Tag>();
}

template <typename Tag, typename Color>
constexpr const auto& get(const Color& c) {
  return c.template get<Tag>();
}

template <std::size_t I, typename Color>
constexpr auto& get(Color& c) {
  return c.template get_index<I>();
}

template <std::size_t I, typename Color>
constexpr const auto& get(const Color& c) {
  return c.template get_index<I>();
}

}  // namespace colorcpp

namespace std {

template <typename Model>
struct tuple_size<colorcpp::core::basic_color<Model>>
    : std::integral_constant<std::size_t, colorcpp::traits::model_traits<Model>::channel_size> {};

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
