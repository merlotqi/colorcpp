/**
 * @file json.hpp
 * @brief Generic JSON serialization adapter for colorcpp colors.
 *
 * This header provides template functions for serializing/deserializing colors
 * to/from JSON. Users must specialize @ref json_adapter for their JSON library
 * (e.g. nlohmann::json, rapidjson, simdjson).
 */

#pragma once

#include <array>
#include <colorcpp/io/serialization/details.hpp>
#include <optional>
#include <string>
#include <type_traits>

namespace colorcpp::io::serialization {

namespace details {

template <typename, typename = void>
struct is_json_adapter_complete : std::false_type {};

template <typename Adapter>
struct is_json_adapter_complete<
    Adapter,
    std::void_t<decltype(Adapter::make_array()), decltype(Adapter::make_object()),
                decltype(Adapter::push_back(std::declval<typename Adapter::json_type&>(), 0.0)),
                decltype(Adapter::set(std::declval<typename Adapter::json_type&>(), std::string_view{},
                                      std::declval<typename Adapter::json_type>())),
                decltype(Adapter::set(std::declval<typename Adapter::json_type&>(), std::string_view{}, 0.0)),
                decltype(Adapter::set(std::declval<typename Adapter::json_type&>(), std::string_view{},
                                      std::string_view{})),
                decltype(Adapter::is_array(std::declval<const typename Adapter::json_type&>())),
                decltype(Adapter::is_object(std::declval<const typename Adapter::json_type&>())),
                decltype(Adapter::array_size(std::declval<const typename Adapter::json_type&>())),
                decltype(Adapter::get_double(std::declval<const typename Adapter::json_type&>(), std::size_t{})),
                decltype(Adapter::get_double(std::declval<const typename Adapter::json_type&>(), std::string_view{})),
                decltype(Adapter::has_key(std::declval<const typename Adapter::json_type&>(), std::string_view{}))>>
    : std::true_type {};

}  // namespace details

/**
 * @brief JSON adapter traits — users must specialize this for their JSON library.
 *
 * The specialization must provide the following static methods:
 * @code
 * template <>
 * struct json_adapter<my_json> {
 *   using json_type = my_json;
 *   static my_json make_array();
 *   static my_json make_object();
 *   static void push_back(my_json& arr, double v);
 *   static void set(my_json& obj, std::string_view key, const my_json& val);
 *   static void set(my_json& obj, std::string_view key, double v);
 *   static void set(my_json& obj, std::string_view key, std::string_view v);
 *   static bool is_array(const my_json& j);
 *   static bool is_object(const my_json& j);
 *   static std::size_t array_size(const my_json& j);
 *   static double get_double(const my_json& j, std::size_t index);
 *   static double get_double(const my_json& j, std::string_view key);
 *   static bool has_key(const my_json& j, std::string_view key);
 * };
 * @endcode
 */
template <typename Json>
struct json_adapter;

/**
 * @brief Serialize a color to JSON in compact (array) format.
 */
template <typename Json, typename Color>
Json to_json_compact(const Color& c) {
  using Adapter = json_adapter<Json>;
  static_assert(details::is_json_adapter_complete<Adapter>::value,
                "json_adapter<Json> specialization is incomplete. "
                "Provide: json_type alias, make_array, make_object, push_back, set, "
                "is_array, is_object, array_size, get_double, has_key.");
  constexpr std::size_t N = Color::channels;
  std::array<double, N> vals{};
  details::extract_channels(c, vals.data());

  Json arr = Adapter::make_array();
  for (std::size_t i = 0; i < N; ++i) {
    Adapter::push_back(arr, vals[i]);
  }
  return arr;
}

/**
 * @brief Serialize a color to JSON in named (object) format.
 * @param names Channel names array (size must equal Color::channels).
 */
template <typename Json, typename Color>
Json to_json_named(const Color& c, const std::string* names) {
  using Adapter = json_adapter<Json>;
  constexpr std::size_t N = Color::channels;
  std::array<double, N> vals{};
  details::extract_channels(c, vals.data());

  Json obj = Adapter::make_object();
  for (std::size_t i = 0; i < N; ++i) {
    Adapter::set(obj, names[i], vals[i]);
  }
  return obj;
}

/**
 * @brief Serialize a color to JSON with options.
 */
template <typename Json, typename Color>
Json to_json(const Color& c, const serialization_options& opts = {}) {
  if (opts.format == serialization_format::compact) {
    return to_json_compact<Json>(c);
  }
  // For named format without channel names, use generic ch0, ch1, ...
  constexpr std::size_t N = Color::channels;
  std::array<std::string, N> names;
  for (std::size_t i = 0; i < N; ++i) {
    names[i] = details::default_channel_name(i);
  }
  return to_json_named<Json>(c, names.data());
}

/**
 * @brief Serialize a color to JSON in named format with custom channel names.
 */
template <typename Json, typename Color>
Json to_json(const Color& c, const std::string* channel_names, const serialization_options& opts = {}) {
  if (opts.format == serialization_format::compact) {
    return to_json_compact<Json>(c);
  }
  return to_json_named<Json>(c, channel_names);
}

/**
 * @brief Deserialize a color from JSON array format [r, g, b, ...].
 */
template <typename Json, typename Color>
std::optional<Color> from_json_compact(const Json& j) {
  using Adapter = json_adapter<Json>;
  if (!Adapter::is_array(j)) return std::nullopt;

  constexpr std::size_t N = Color::channels;
  if (Adapter::array_size(j) < N) return std::nullopt;

  std::array<double, N> vals{};
  for (std::size_t i = 0; i < N; ++i) {
    vals[i] = Adapter::get_double(j, i);
  }
  return details::apply_channels<Color>(vals.data());
}

/**
 * @brief Deserialize a color from JSON object format {"ch0": ..., "ch1": ...}.
 */
template <typename Json, typename Color>
std::optional<Color> from_json_named(const Json& j, const std::string* names) {
  using Adapter = json_adapter<Json>;
  if (!Adapter::is_object(j)) return std::nullopt;

  constexpr std::size_t N = Color::channels;
  std::array<double, N> vals{};
  for (std::size_t i = 0; i < N; ++i) {
    if (!Adapter::has_key(j, names[i])) return std::nullopt;
    vals[i] = Adapter::get_double(j, names[i]);
  }
  return details::apply_channels<Color>(vals.data());
}

/**
 * @brief Auto-detect JSON format and deserialize a color.
 */
template <typename Json, typename Color>
std::optional<Color> from_json(const Json& j) {
  using Adapter = json_adapter<Json>;
  if (Adapter::is_array(j)) {
    return from_json_compact<Json, Color>(j);
  }
  if (Adapter::is_object(j)) {
    // Try generic channel names
    constexpr std::size_t N = Color::channels;
    std::array<std::string, N> names;
    for (std::size_t i = 0; i < N; ++i) {
      names[i] = details::default_channel_name(i);
    }
    return from_json_named<Json, Color>(j, names.data());
  }
  return std::nullopt;
}

/**
 * @brief Deserialize a color from JSON object with custom channel names.
 */
template <typename Json, typename Color>
std::optional<Color> from_json(const Json& j, const std::string* channel_names) {
  using Adapter = json_adapter<Json>;
  if (Adapter::is_array(j)) {
    return from_json_compact<Json, Color>(j);
  }
  if (Adapter::is_object(j)) {
    return from_json_named<Json, Color>(j, channel_names);
  }
  return std::nullopt;
}

}  // namespace colorcpp::io::serialization
