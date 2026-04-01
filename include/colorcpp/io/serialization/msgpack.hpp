/**
 * @file msgpack.hpp
 * @brief Generic MessagePack serialization adapter for colorcpp colors.
 *
 * This header provides template functions for serializing/deserializing colors
 * to/from MessagePack. Users must specialize @ref msgpack_packer and @ref msgpack_unpacker
 * for their MessagePack library.
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
struct is_msgpack_packer_complete : std::false_type {};

template <typename Packer>
struct is_msgpack_packer_complete<
    Packer,
    std::void_t<decltype(Packer::pack_array(std::declval<typename Packer::packer_type&>(), std::size_t{})),
                decltype(Packer::pack_map(std::declval<typename Packer::packer_type&>(), std::size_t{})),
                decltype(Packer::pack_double(std::declval<typename Packer::packer_type&>(), 0.0)),
                decltype(Packer::pack_string(std::declval<typename Packer::packer_type&>(), std::string_view{}))>>
    : std::true_type {};

template <typename, typename = void>
struct is_msgpack_unpacker_complete : std::false_type {};

template <typename Unpacker>
struct is_msgpack_unpacker_complete<
    Unpacker,
    std::void_t<decltype(Unpacker::is_array(std::declval<typename Unpacker::unpacker_type&>())),
                decltype(Unpacker::is_map(std::declval<typename Unpacker::unpacker_type&>())),
                decltype(Unpacker::array_size(std::declval<typename Unpacker::unpacker_type&>())),
                decltype(Unpacker::get_double(std::declval<typename Unpacker::unpacker_type&>(), std::size_t{})),
                decltype(Unpacker::get_double(std::declval<typename Unpacker::unpacker_type&>(), std::string_view{})),
                decltype(Unpacker::has_key(std::declval<typename Unpacker::unpacker_type&>(), std::string_view{}))>>
    : std::true_type {};

}  // namespace details

/**
 * @brief MessagePack packer traits — users must specialize this for their library.
 *
 * The specialization must provide:
 * @code
 * template <>
 * struct msgpack_packer<my_packer> {
 *   using packer_type = my_packer;
 *   static void pack_array(my_packer& p, std::size_t n);
 *   static void pack_map(my_packer& p, std::size_t n);
 *   static void pack_double(my_packer& p, double v);
 *   static void pack_string(my_packer& p, std::string_view s);
 * };
 * @endcode
 */
template <typename Packer>
struct msgpack_packer;

template <typename Unpacker>
struct msgpack_unpacker;

/**
 * @brief Serialize a color to MessagePack array format.
 */
template <typename Packer, typename Color>
void pack_color(Packer& p, const Color& c) {
  using Traits = msgpack_packer<Packer>;
  static_assert(details::is_msgpack_packer_complete<Traits>::value,
                "msgpack_packer<Packer> specialization is incomplete. "
                "Provide: packer_type alias, pack_array, pack_map, pack_double, pack_string.");
  constexpr std::size_t N = Color::channels;
  std::array<double, N> vals{};
  details::extract_channels(c, vals.data());

  Traits::pack_array(p, N);
  for (std::size_t i = 0; i < N; ++i) {
    Traits::pack_double(p, vals[i]);
  }
}

/**
 * @brief Serialize a color to MessagePack map format with custom names.
 */
template <typename Packer, typename Color>
void pack_color_named(Packer& p, const Color& c, const std::string* names) {
  using Traits = msgpack_packer<Packer>;
  constexpr std::size_t N = Color::channels;
  std::array<double, N> vals{};
  details::extract_channels(c, vals.data());

  Traits::pack_map(p, N);
  for (std::size_t i = 0; i < N; ++i) {
    Traits::pack_string(p, names[i]);
    Traits::pack_double(p, vals[i]);
  }
}

/**
 * @brief Deserialize a color from MessagePack array format.
 */
template <typename Unpacker, typename Color>
std::optional<Color> unpack_color(Unpacker& u) {
  using Traits = msgpack_unpacker<Unpacker>;
  static_assert(details::is_msgpack_unpacker_complete<Traits>::value,
                "msgpack_unpacker<Unpacker> specialization is incomplete. "
                "Provide: unpacker_type alias, is_array, is_map, array_size, get_double, has_key.");
  if (!Traits::is_array(u)) return std::nullopt;

  constexpr std::size_t N = Color::channels;
  if (Traits::array_size(u) < N) return std::nullopt;

  std::array<double, N> vals{};
  for (std::size_t i = 0; i < N; ++i) {
    vals[i] = Traits::get_double(u, i);
  }
  return details::apply_channels<Color>(vals.data());
}

/**
 * @brief Deserialize a color from MessagePack map format with custom names.
 */
template <typename Unpacker, typename Color>
std::optional<Color> unpack_color_named(Unpacker& u, const std::string* names) {
  using Traits = msgpack_unpacker<Unpacker>;
  if (!Traits::is_map(u)) return std::nullopt;

  constexpr std::size_t N = Color::channels;
  std::array<double, N> vals{};
  for (std::size_t i = 0; i < N; ++i) {
    if (!Traits::has_key(u, names[i])) return std::nullopt;
    vals[i] = Traits::get_double(u, names[i]);
  }
  return details::apply_channels<Color>(vals.data());
}

}  // namespace colorcpp::io::serialization
