// color_base.hpp
#pragma once

#include <array>
#include <colorcpp/traits/concepts.hpp>
#include <stdexcept>
#include <tuple>
#include <utility>

namespace colorcpp::core {

template <typename Model>
  requires traits::is_model_traits<Model>
struct basic_color {
  using trait_model = traits::model_traits<Model>;
  static constexpr std::size_t channels = trait_model::channel_size;

  using first_channel = std::tuple_element_t<0, typename trait_model::channels_type>;
  using value_type = typename first_channel::value_type;
  using storage_type = std::array<value_type, channels>;
  storage_type data{};

  template <typename... Args>
    requires(sizeof...(Args) == channels)
  consteval explicit basic_color(Args... args) : data{static_cast<value_type>(args)...} {
    check_range(std::make_index_sequence<channels>{}, args...);
  }

 private:
  template <std::size_t... Is, typename... Args>
  constexpr void check_range(std::index_sequence<Is...>, Args... args) {
    using channel_tuple = typename trait_model::channels_type;

    bool valid = ((args >= std::tuple_element_t<Is, channel_tuple>::min &&
                   args <= std::tuple_element_t<Is, channel_tuple>::max) &&
                  ...);

    if (!valid) {
      throw std::runtime_error("Channel Value Out of Range");
    }
  }
};

}  // namespace colorcpp::core
