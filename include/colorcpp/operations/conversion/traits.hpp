/**
 * @file traits.hpp
 * @brief Color space traits for hub-based routing.
 *
 * Direct conversions between concrete color types are registered with
 * @c COLORCPP_REGISTER_CONVERSION in @c functions/index.hpp (see @c registry.hpp).
 * This header only supplies per-model @c hub_type for automatic multi-hop routing.
 */

#pragma once

#include <type_traits>

namespace colorcpp::operations::conversion {

/**
 * @brief Extracts the model tag from a color type.
 * @tparam Color A basic_color specialization.
 */
template <typename T>
struct extract_model;

template <template <typename...> class Color, typename Model, typename... Args>
struct extract_model<Color<Model, Args...>> {
  using type = Model;
};

template <typename T>
using extract_model_t = typename extract_model<T>::type;

/**
 * @brief Base color_traits template. Each color space must specialize this.
 * @tparam ModelTag The model tag of the color space.
 *
 * Specializations must define:
 * - hub_type: The default routing hub (e.g. float sRGB, linear RGB, XYZ), or @c void if none.
 */
template <typename ModelTag>
struct color_traits {
  using hub_type = void;
};

/**
 * @brief Helper to check if a traits type has a valid hub.
 */
template <typename Traits>
struct has_hub : std::bool_constant<!std::is_same_v<typename Traits::hub_type, void>> {};

template <typename Traits>
inline constexpr bool has_hub_v = has_hub<Traits>::value;

/**
 * @brief Helper to check if a model has a @c color_traits specialization with a hub.
 */
template <typename ModelTag>
inline constexpr bool has_color_traits_v = !std::is_same_v<typename color_traits<ModelTag>::hub_type, void>;

/**
 * @brief Helper to get the hub color type from a color type.
 */
template <typename Color>
using hub_color_t = typename color_traits<extract_model_t<Color>>::hub_type;

/**
 * @brief Check if two color types share the same model tag.
 */
template <typename A, typename B>
inline constexpr bool same_model_v = std::is_same_v<extract_model_t<A>, extract_model_t<B>>;

/**
 * @brief Check if a route is possible via hub chain (limited depth).
 *
 * This performs a compile-time check to verify that a conversion path
 * exists through the hub chain. It checks up to max_depth hops.
 *
 * @note This only checks hub-based routing, not registered direct conversions.
 * @tparam From Source color type.
 * @tparam To Destination color type.
 * @tparam max_depth Maximum number of hub hops to check (default: 4).
 */
template <typename From, typename To, std::size_t max_depth = 4>
constexpr bool is_route_possible_via_hubs() {
  if constexpr (std::is_same_v<From, To>) {
    return true;
  } else if constexpr (max_depth == 0) {
    return false;
  } else if constexpr (has_hub_v<color_traits<extract_model_t<From>>>) {
    using Hub = hub_color_t<From>;
    if constexpr (std::is_same_v<Hub, To>) {
      return true;
    } else if constexpr (!std::is_same_v<Hub, From>) {
      return is_route_possible_via_hubs<Hub, To, max_depth - 1>();
    }
    return false;
  }
  return false;
}

/**
 * @brief Check if we can route From -> Hub -> To.
 *
 * Returns true if:
 * 1. From has a valid hub
 * 2. The hub is not the same as To
 *
 * @note This function only checks hub existence and identity.
 *       Actual path feasibility is determined during recursive dispatch.
 */
template <typename From, typename To>
constexpr bool can_route_via_from_hub() {
  if constexpr (!has_hub_v<color_traits<extract_model_t<From>>>) {
    return false;
  } else {
    using Hub = hub_color_t<From>;
    return !std::is_same_v<Hub, To>;
  }
}

/**
 * @brief Check if we can route From -> Hub -> To via To's hub.
 *
 * Returns true if:
 * 1. To has a valid hub
 * 2. The hub is not the same as From
 *
 * @note This function only checks hub existence and identity.
 *       Actual path feasibility is determined during recursive dispatch.
 */
template <typename From, typename To>
constexpr bool can_route_via_to_hub() {
  if constexpr (!has_hub_v<color_traits<extract_model_t<To>>>) {
    return false;
  } else {
    using Hub = hub_color_t<To>;
    return !std::is_same_v<From, Hub>;
  }
}

}  // namespace colorcpp::operations::conversion
