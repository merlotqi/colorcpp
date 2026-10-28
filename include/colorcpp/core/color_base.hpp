/**
 * @file color_base.hpp
 * @brief @ref colorcpp::core::basic_color — @c std::array storage for trait-defined color models.
 */

#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <colorcpp/core/concepts.hpp>
#include <functional>
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
 * Colors are plain value types: construction, @c set, and the mutable accessors never validate against the
 * channel's nominal range. Boundary policy lives at the edges — the stream parser rejects out-of-range text
 * (failbit), and @ref colorcpp::core::clamped / @ref colorcpp::core::make_checked provide opt-in factories.
 *
 * Channel access:
 * - @c get<Tag>() / @c get_index<I>() for read/write references.
 * - @c set<Tag>(v) stores @c v (narrowing cast to the channel's value_type).
 *
 * Default construction value-initializes every channel to zero — for alpha models that is transparent black,
 * e.g. @c rgbaf_t{} has @c a == 0. Construct explicitly when an opaque default is required.
 */
template <typename Model>
struct basic_color<Model, std::enable_if_t<traits::is_model_traits_v<Model>>> {
  using trait_model = traits::model_traits<Model>;

  using channels_tuple = typename trait_model::channels_type;

  static constexpr std::size_t channels = traits::model_channel_count_v<Model>;

  using first_channel = std::tuple_element_t<0, channels_tuple>;

  using value_type = typename first_channel::value_type;

  template <std::size_t I>
  using channel_value_t = typename std::tuple_element_t<I, channels_tuple>::value_type;

  using storage_type = traits::channels_storage_t<channels_tuple>;

  storage_type data{};

 public:
  constexpr basic_color() = default;

  template <typename... Args, typename = std::enable_if_t<sizeof...(Args) == channels>>
  constexpr explicit basic_color(Args... args) : data{make_storage(std::make_index_sequence<channels>{}, args...)} {}

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

  /**
   * @brief Lexicographic ordering on channel values (strict weak ordering).
   *
   * Enables @c std::map and sorting. The first differing channel (channel
   * order) decides. NaN-containing channels have unspecified ordering — the
   * usual IEEE caveat; do not use such colors as ordered-map keys.
   */
  constexpr bool operator<(const basic_color& other) const noexcept { return less_one<0>(other); }

 private:
  // Helper for exact equality comparison
  template <std::size_t... Is>
  constexpr bool equal_impl(const basic_color& other, std::index_sequence<Is...>) const noexcept {
    return ((get_index<Is>() == other.get_index<Is>()) && ...);
  }

  // Lexicographic less: at each index either the channel decides, or equality
  // defers to the next index; no channels left to differ means "not less".
  template <std::size_t I>
  constexpr bool less_one(const basic_color& other) const noexcept {
    if constexpr (I >= channels) {
      return false;
    } else if (get_index<I>() != other.get_index<I>()) {
      return get_index<I>() < other.get_index<I>();
    } else {
      return less_one<I + 1>(other);
    }
  }

  template <std::size_t... Is, typename... Args>
  constexpr static storage_type make_storage(std::index_sequence<Is...>, Args... args) {
    auto tp = std::make_tuple(args...);
    return storage_type{static_cast<value_type>(static_cast<channel_value_t<Is>>(std::get<Is>(tp)))...};
  }
};

/** @brief Alias for @ref basic_color. */
template <typename Model>
using basic_color_t = basic_color<Model>;

namespace factory_details {

/** Clamps one argument into Channel's nominal range (rounding for integral channels). */
template <typename Channel, typename Arg>
constexpr typename Channel::value_type clamp_to_channel(Arg v) {
  const float x = std::clamp(static_cast<float>(v), static_cast<float>(Channel::min), static_cast<float>(Channel::max));
  if constexpr (std::is_integral_v<typename Channel::value_type>) {
    return static_cast<typename Channel::value_type>(std::round(x));
  } else {
    return static_cast<typename Channel::value_type>(x);
  }
}

/** True if v lies within Channel's nominal [min, max] (compared before any narrowing). */
template <typename Channel, typename Arg>
constexpr bool in_channel_range(Arg v) {
  return v >= Channel::min && v <= Channel::max;
}

template <typename Color, std::size_t... Is, typename... Args>
constexpr Color clamped_impl(std::index_sequence<Is...>, Args... args) {
  return Color{clamp_to_channel<std::tuple_element_t<Is, typename Color::channels_tuple>>(args)...};
}

template <typename Color, std::size_t... Is, typename... Args>
constexpr Color checked_impl(std::index_sequence<Is...>, Args... args) {
  const bool valid = (in_channel_range<std::tuple_element_t<Is, typename Color::channels_tuple>>(args) && ...);
  if (!valid) throw std::out_of_range("colorcpp: channel value out of range");
  return Color{static_cast<typename Color::value_type>(args)...};
}

}  // namespace factory_details

/**
 * @brief Construct @p Color with every channel clamped into its nominal [min, max].
 *
 * The opt-in counterpart to value-type construction: use it when an untrusted or
 * computed value must not leave the model's nominal range. Integral channels round
 * to nearest (matching @c from_value in the conversion layer).
 *
 * @tparam Color Target color type (e.g. @c rgbf_t).
 * @param args One argument per channel, in channel order.
 * @return Color with clamped channel values.
 */
template <typename Color, typename... Args>
constexpr Color clamped(Args... args) {
  static_assert(sizeof...(Args) == Color::channels, "colorcpp: clamped() needs one argument per channel");
  return factory_details::clamped_impl<Color>(std::make_index_sequence<Color::channels>{}, args...);
}

/**
 * @brief Construct @p Color validating every channel against its nominal [min, max].
 *
 * Throws @c std::out_of_range on the first out-of-range argument (checked before any
 * narrowing conversion, so e.g. 300 into a byte channel is rejected rather than
 * wrapped). The exception-based opt-in boundary for callers that can use exceptions;
 * @ref clamped is the always-defined alternative.
 *
 * @tparam Color Target color type (e.g. @c rgbf_t).
 * @param args One argument per channel, in channel order.
 * @return Color with the given channel values.
 * @throws std::out_of_range if any channel value is outside its nominal range.
 */
template <typename Color, typename... Args>
constexpr Color make_checked(Args... args) {
  static_assert(sizeof...(Args) == Color::channels, "colorcpp: make_checked() needs one argument per channel");
  return factory_details::checked_impl<Color>(std::make_index_sequence<Color::channels>{}, args...);
}

/**
 * @brief Access a channel by tag on any color type that exposes @c template get<Tag>().
 * @tparam Tag Channel tag type (e.g. rgb::channel::r_tag).
 * @note Forwarding-reference parameter covers lvalue, const, and rvalue colors uniformly
 * (structured bindings may pass the hidden copy as an rvalue).
 */
template <typename Tag, typename Color>
constexpr decltype(auto) get(Color&& c) {
  return c.template get<Tag>();
}

/**
 * @brief Access channel @p I (0-based) on @ref colorcpp::core::basic_color.
 * @tparam I Channel index less than @ref colorcpp::traits::model_channel_count_v for the model.
 */
template <std::size_t I, typename Color>
constexpr decltype(auto) get(Color&& c) {
  return c.template get_index<I>();
}

}  // namespace colorcpp::core

namespace std {

template <typename Model>
struct tuple_size<colorcpp::core::basic_color<Model>>
    : std::integral_constant<std::size_t, colorcpp::traits::model_channel_count_v<Model>> {};

// tuple_element<I>::type matches channel I descriptor; storage is std::array<value_type,N>.
template <std::size_t I, typename Model>
struct tuple_element<I, colorcpp::core::basic_color<Model>> {
  using channels = typename colorcpp::traits::model_traits<Model>::channels_type;
  using type = typename std::tuple_element_t<I, channels>::value_type;
};

}  // namespace std

namespace colorcpp::core::detail {

/** @brief Channel-wise hash: equal colors produce equal hashes (std::hash contract). */
template <typename Model, std::size_t... Is>
std::size_t hash_channels(const basic_color<Model>& c, std::index_sequence<Is...>) noexcept {
  // All channels share one scalar type (std::array storage requirement).
  using T = typename basic_color<Model>::value_type;
  std::size_t seed = 0;
  // boost-style combine; channel order is part of the contract.
  ((seed ^= std::hash<T>{}(static_cast<T>(c.template get_index<Is>())) + 0x9e3779b9 + (seed << 6) + (seed >> 2)),
   ...);
  return seed;
}

}  // namespace colorcpp::core::detail

namespace std {

/**
 * @brief std::hash for @ref colorcpp::core::basic_color — enables @c std::unordered_map.
 *
 * Combines the per-channel std::hash values in channel order. Float channels
 * inherit the standard-library contract (+0.0 and -0.0 hash equally). NaN
 * channel values hash like other values but never compare equal, so NaN colors
 * cannot be found again by key lookup — do not use them as hash-map keys.
 */
template <typename Model>
struct hash<colorcpp::core::basic_color<Model>> {
  std::size_t operator()(const colorcpp::core::basic_color<Model>& c) const noexcept {
    return colorcpp::core::detail::hash_channels(c, std::make_index_sequence<colorcpp::traits::model_channel_count_v<Model>>{});
  }
};

}  // namespace std

/**
 * @brief Enables structured bindings for a color family class (e.g. @c basic_rgba).
 *
 * The family's channel count and per-element value type follow from its @ref
 * colorcpp::traits::model_traits. Invoke once per family at global scope:
 * @code
 * COLORCPP_ENABLE_STRUCTURED_BINDINGS(colorcpp::core::basic_rgba)
 * @endcode
 * (@c std supplies the cv-qualified @c tuple_size/@c tuple_element variants generically.)
 */
#define COLORCPP_ENABLE_STRUCTURED_BINDINGS(Family)                                               \
  namespace std {                                                                                 \
  template <typename Model>                                                                       \
  struct tuple_size<Family<Model>>                                                                \
      : std::integral_constant<std::size_t, ::colorcpp::traits::model_channel_count_v<Model>> {}; \
  template <std::size_t I, typename Model>                                                        \
  struct tuple_element<I, Family<Model>> {                                                        \
    using channels = typename ::colorcpp::traits::model_traits<Model>::channels_type;             \
    using type = typename std::tuple_element_t<I, channels>::value_type;                          \
  };                                                                                              \
  }

/**
 * @brief Declares @c std::hash for a color family class (e.g. @c basic_rgba), enabling
 *        @c std::unordered_map with color keys.
 *
 * A partial specialization on the family template — the base-class specialization on
 * @ref basic_color does not match derived family types. Channel-wise combine, equal to
 * the @c basic_color version (equal colors hash equally). Invoke once per family at
 * global scope, next to its @c COLORCPP_ENABLE_STRUCTURED_BINDINGS:
 * @code
 * COLORCPP_ENABLE_HASH(colorcpp::core::basic_rgba)
 * @endcode
 */
#define COLORCPP_ENABLE_HASH(Family)                                                                \
  namespace std {                                                                                   \
  template <typename Model>                                                                         \
  struct hash<Family<Model>> {                                                                      \
    std::size_t operator()(const Family<Model>& c) const noexcept {                                 \
      return ::colorcpp::core::detail::hash_channels(                                               \
          c, std::make_index_sequence<::colorcpp::traits::model_channel_count_v<Model>>{});         \
    }                                                                                               \
  };                                                                                                \
  }

/**
 * @brief Declares a named accessor pair (mutable + const) for one channel of a family class.
 *
 * Expands to @c Name() / @c Name() const overloads guarded by
 * @ref colorcpp::traits::has_channel_tag_v, forwarding to @ref colorcpp::core::basic_color::get.
 * A model whose channels lack @p Tag simply does not declare the accessor
 * (e.g. @c a() on non-alpha models). Invoke inside a family struct whose
 * template parameter is named @c Model:
 * @code
 * COLORCPP_CHANNEL_ACCESSOR(rgb::channel::r_tag, r)
 * @endcode
 *
 * @param Tag Channel tag type as resolved from the @c colorcpp::core namespace (e.g. @c rgb::channel::r_tag).
 * @param Name Accessor function name to declare.
 */
#define COLORCPP_CHANNEL_ACCESSOR(Tag, Name)                                                                \
  template <typename M = Model, typename = std::enable_if_t<::colorcpp::traits::has_channel_tag_v<M, Tag>>> \
  constexpr auto& Name() {                                                                                  \
    return this->template get<Tag>();                                                                       \
  }                                                                                                         \
  template <typename M = Model, typename = std::enable_if_t<::colorcpp::traits::has_channel_tag_v<M, Tag>>> \
  constexpr const auto& Name() const {                                                                      \
    return this->template get<Tag>();                                                                       \
  }

/**
 * @brief Declares stream insertion/extraction for a family class, forwarding to the
 * format-agnostic operators on the @c basic_color base (see @ref colorcpp::io).
 *
 * Invoke once per family at namespace scope inside @c colorcpp::core (the header
 * must already include @c colorcpp/core/io.hpp):
 * @code
 * COLORCPP_STREAM_OPERATORS(colorcpp::core::basic_rgba)
 * @endcode
 *
 * @param Family Family class template, fully qualified (e.g. @c colorcpp::core::basic_rgba).
 */
#define COLORCPP_STREAM_OPERATORS(Family)                                                                     \
  template <typename Model>                                                                                   \
  std::ostream& operator<<(std::ostream& os, const Family<Model>& c) {                                        \
    return ::colorcpp::core::io::operator<<(os, static_cast<const ::colorcpp::core::basic_color<Model>&>(c)); \
  }                                                                                                           \
  template <typename Model>                                                                                   \
  std::istream& operator>>(std::istream& is, Family<Model>& c) {                                              \
    return ::colorcpp::core::io::operator>>(is, static_cast<::colorcpp::core::basic_color<Model>&>(c));       \
  }
