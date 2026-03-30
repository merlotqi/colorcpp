/**
 * @file io.hpp
 * @brief Stream I/O for @ref colorcpp::core::basic_color: text form with optional model prefix, or hex with @c
 * std::hex.
 */

#pragma once

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <iomanip>
#include <colorcpp/core/color_base.hpp>

/** @brief Stream formatting and parsing for core color types. */
namespace colorcpp::core::io {

namespace details {

template <typename T>
constexpr auto to_printable(T val) {
  if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, int8_t>) {
    return static_cast<int>(val);
  } else {
    return val;
  }
}

template <typename Channel, typename T>
constexpr int map_to_255(T val) {
  if constexpr (std::is_integral_v<T>) {
    return static_cast<int>(val);
  } else {
    float range = static_cast<float>(Channel::max) - static_cast<float>(Channel::min);
    if (range <= 0.0) return 0;
    float normalized = (static_cast<float>(val) - static_cast<float>(Channel::min)) / range;
    normalized = std::clamp(normalized, 0.0f, 1.0f);
    return static_cast<int>(normalized * 255.0);
  }
}

template <typename T>
bool read_comp(std::istream& is, T& out) {
  is >> std::ws;
  if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, int8_t>) {
    int temp;
    if (!(is >> temp)) return false;
    out = static_cast<T>(temp);
  } else {
    if (!(is >> out)) return false;
  }
  return true;
}

template <typename Color, std::size_t... Is>
void print_color_text_impl(std::ostream& os, const Color& c, std::index_sequence<Is...>) {
  ((os << (Is > 0 ? ", " : "") << to_printable(c.template get_index<Is>())), ...);
}

template <typename Model, std::size_t... Is>
void print_color_hex_impl(std::ostream& os, const core::basic_color<Model>& c, std::index_sequence<Is...>) {
  using Channels = typename traits::model_traits<Model>::channels_type;
  ((os << std::setfill('0') << std::setw(2)
       << map_to_255<std::tuple_element_t<Is, Channels>>(c.template get_index<Is>())),
   ...);
}

template <typename Model, std::size_t... Is>
bool read_color_impl(std::istream& is,
                     std::array<typename core::basic_color<Model>::value_type, sizeof...(Is)>& temp_data,
                     std::index_sequence<Is...>) {
  using Trait = traits::model_traits<Model>;
  using Channels = typename Trait::channels_type;

  auto skip_sep = [](std::istream& s) {
    s >> std::ws;
    if (s.peek() == ',') s.ignore();
    s >> std::ws;
  };

  auto has_more = [](std::istream& s) {
    s >> std::ws;
    int next = s.peek();
    return std::isdigit(next) || next == '+' || next == '-' || next == '.';
  };

  bool success = true;
  ((success &&
            (has_more(is)
                 ? (read_comp(is, temp_data[Is]) ? ((Is < sizeof...(Is) - 1 ? skip_sep(is) : (void)0), true) : false)
                 : (Is == sizeof...(Is) - 1 ? (temp_data[Is] = std::tuple_element_t<Is, Channels>::max, true) : false))
        ? true
        : (success = false)),
   ...);

  return success;
}

inline bool consume_prefix(std::istream& is, std::string_view prefix, bool& has_paren) {
  is >> std::ws;
  has_paren = false;
  if (prefix.empty()) return true;

  std::string found;
  for (char c : prefix) {
    if (std::tolower(static_cast<unsigned char>(is.peek())) == std::tolower(static_cast<unsigned char>(c))) {
      found += (char)is.get();
    } else {
      for (int i = (int)found.size() - 1; i >= 0; --i) is.putback(found[i]);
      return true;
    }
  }
  is >> std::ws;
  if (is.peek() == '(') {
    is.ignore();
    has_paren = true;
  }
  return true;
}

}  // namespace details

/**
 * @brief Text output: @c prefix(ch0, ch1, …) using @ref colorcpp::traits::model_traits::prefix, or @c #RRGGBB… if @c os
 * @c std::hex is set.
 */
template <typename Model>
std::ostream& operator<<(std::ostream& os, const core::basic_color<Model>& c) {
  using Trait = traits::model_traits<Model>;

  if (os.flags() & std::ios_base::hex) {
    auto old_fill = os.fill();
    os << "#";
    details::print_color_hex_impl(os, c, std::make_index_sequence<Trait::channel_size>{});
    os.fill(old_fill);
    return os;
  }

  if constexpr (Trait::prefix != "") os << Trait::prefix << "(";
  details::print_color_text_impl(os, c, std::make_index_sequence<Trait::channel_size>{});
  if constexpr (Trait::prefix != "") os << ")";

  return os;
}

/**
 * @brief Parse optional prefix and comma-separated channel values; sets @c failbit on failure.
 * @note Omitted trailing channels may default to max (e.g. alpha) per implementation.
 */
template <typename Model>
std::istream& operator>>(std::istream& is, core::basic_color<Model>& c) {
  using Trait = traits::model_traits<Model>;
  using T = typename core::basic_color<Model>::value_type;

  std::array<T, Trait::channel_size> temp_data{};
  bool has_paren = false;

  details::consume_prefix(is, Trait::prefix, has_paren);

  if (details::read_color_impl<Model>(is, temp_data, std::make_index_sequence<Trait::channel_size>{})) {
    if (has_paren) {
      is >> std::ws;
      if (is.peek() == ')') is.ignore();
    }
    c = std::apply([](auto... args) { return core::basic_color<Model>(args...); }, temp_data);
  } else {
    is.setstate(std::ios::failbit);
  }
  return is;
}

}  // namespace colorcpp::core::io
