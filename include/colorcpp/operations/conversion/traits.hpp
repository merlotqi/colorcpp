/**
 * @file traits.hpp
 * @brief Color space traits metadata used alongside graph-driven public routing.
 *
 * Direct conversions between concrete color types are registered with
 * @c COLORCPP_REGISTER_CONVERSION in @c functions/index.hpp (see @c registry.hpp).
 * Public conversion dispatch happens through @c color_cast and the compile-time graph
 * in @c graph.hpp; this header provides per-model metadata and compatibility helpers.
 */

#pragma once

#include <colorcpp/operations/conversion/registry.hpp>
#include <cstddef>
#include <limits>
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
 * @brief Alias for extract_model<T>::type.
 * @tparam T Color type to extract model tag from.
 */
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

/**
 * @brief Helper variable template for has_hub.
 * @tparam Traits Traits type to check for hub presence.
 */
template <typename Traits>
inline constexpr bool has_hub_v = has_hub<Traits>::value;

/**
 * @brief Check if a color model has an associated hub defined.
 * @tparam ModelTag Color space model tag to check.
 * @return True if this model has a valid conversion hub configured.
 */
template <typename ModelTag>
inline constexpr bool has_color_traits_v = !std::is_same_v<typename color_traits<ModelTag>::hub_type, void>;

/**
 * @brief Helper to get the hub color type from a color type.
 */
template <typename Color>
using hub_color_t = typename color_traits<extract_model_t<Color>>::hub_type;

/**
 * @brief Get the compatibility hub metadata for a given color type.
 * @tparam Color Source color type to retrieve hub for.
 * @note This alias exposes the metadata used by compatibility helpers and internal
 *       route-cost utilities. Public conversion dispatch remains graph-driven through
 *       @c color_cast and @c graph.hpp.
 */
template <typename Color>
using hub_color_t = typename color_traits<extract_model_t<Color>>::hub_type;

/**
 * @brief Check if two color types use the same underlying color model.
 * @tparam A First color type
 * @tparam B Second color type
 * @return True if both types share the same model tag, even with different channel types or precision.
 */
template <typename A, typename B>
inline constexpr bool same_model_v = std::is_same_v<extract_model_t<A>, extract_model_t<B>>;

/**
 * @brief Sentinel value indicating no valid conversion route exists.
 *
 * Chosen to be safely less than maximum value to allow arithmetic operations
 * without overflow during route cost calculation.
 */
inline constexpr std::size_t invalid_route_cost = std::numeric_limits<std::size_t>::max() / 4;

/**
 * @brief Calculate a compatibility route cost between two color types.
 *
 * This helper predates the graph router and estimates a route cost by following
 * registered direct conversions plus configured hub metadata. It remains useful
 * for compatibility checks, but the public conversion contract is determined by
 * the compile-time graph in @c graph.hpp.
 *
 * @tparam From Source color type
 * @tparam To Destination color type
 * @tparam max_depth Maximum allowed number of intermediate hops
 * @return Total route cost, or invalid_route_cost if no path exists within depth limit.
 */
template <typename From, typename To, std::size_t max_depth = 8>
constexpr std::size_t best_route_cost();

/**
 * @brief Calculate route cost by first converting through the source type's hub.
 *
 * Route pattern: From → [From Hub] → ... → To
 *
 * @tparam From Source color type
 * @tparam To Destination color type
 * @tparam max_depth Remaining search depth
 * @return Route cost or invalid_route_cost
 */
template <typename From, typename To, std::size_t max_depth = 8>
constexpr std::size_t route_cost_via_from_hub() {
  if constexpr (max_depth == 0 || !has_hub_v<color_traits<extract_model_t<From>>>) {
    return invalid_route_cost;
  } else {
    using Hub = hub_color_t<From>;
    if constexpr (std::is_same_v<Hub, From>) {
      return invalid_route_cost;
    } else {
      constexpr std::size_t sub = best_route_cost<Hub, To, max_depth - 1>();
      if constexpr (sub == invalid_route_cost) {
        return invalid_route_cost;
      } else {
        return 1 + sub;
      }
    }
  }
}

/**
 * @brief Calculate route cost by last converting through the destination type's hub.
 *
 * Route pattern: From → ... → [To Hub] → To
 *
 * @tparam From Source color type
 * @tparam To Destination color type
 * @tparam max_depth Remaining search depth
 * @return Route cost or invalid_route_cost
 */
template <typename From, typename To, std::size_t max_depth = 8>
constexpr std::size_t route_cost_via_to_hub() {
  if constexpr (max_depth == 0 || !has_hub_v<color_traits<extract_model_t<To>>>) {
    return invalid_route_cost;
  } else {
    using Hub = hub_color_t<To>;
    if constexpr (std::is_same_v<Hub, To>) {
      return invalid_route_cost;
    } else {
      constexpr std::size_t sub = best_route_cost<From, Hub, max_depth - 1>();
      if constexpr (sub == invalid_route_cost) {
        return invalid_route_cost;
      } else {
        return 1 + sub;
      }
    }
  }
}

/**
 * @brief Recursive implementation of the compatibility route-cost calculation.
 *
 * Search strategy for this helper:
 * 1. Exact type match → cost 0
 * 2. Direct registered conversion → cost 1
 * 3. Recursively try source-hub and destination-hub metadata paths
 * 4. Select the lower compatibility cost
 *
 * Public dispatch does not use this routine as its contract surface; graph routing
 * through @c graph.hpp determines the path reported by @c color_cast.
 *
 * @tparam From Source color type
 * @tparam To Destination color type
 * @tparam max_depth Remaining allowed recursion depth
 * @return Minimal route cost found
 */
template <typename From, typename To, std::size_t max_depth>
constexpr std::size_t best_route_cost() {
  if constexpr (std::is_same_v<From, To>) {
    return 0;
  } else if constexpr (has_registered_conversion_v<From, To>) {
    return 1;
  } else if constexpr (max_depth == 0) {
    return invalid_route_cost;
  } else {
    constexpr std::size_t via_from = route_cost_via_from_hub<From, To, max_depth>();
    constexpr std::size_t via_to = route_cost_via_to_hub<From, To, max_depth>();
    return via_from < via_to ? via_from : via_to;
  }
}

/**
 * @brief Check if a route is possible within the conversion graph (limited depth).
 *
 * This performs a compile-time check to verify that a conversion path
 * exists through the registered conversion graph while respecting the
 * hub-recursion depth budget. It checks up to max_depth hops.
 *
 * @tparam From Source color type.
 * @tparam To Destination color type.
 * @tparam max_depth Maximum number of hub hops to check (default: 4).
 */
template <typename From, typename To, std::size_t max_depth = 4>
constexpr bool is_route_possible_via_hubs() {
  return best_route_cost<From, To, max_depth>() != invalid_route_cost;
}

/**
 * @brief Check if we can route From -> Hub -> To.
 *
 * Returns true if the source-hub-first branch can reach @p To within the
 * default recursion budget.
 */
template <typename From, typename To>
constexpr bool can_route_via_from_hub() {
  return route_cost_via_from_hub<From, To>() != invalid_route_cost;
}

/**
 * @brief Check if we can route From -> Hub -> To via To's hub.
 *
 * Returns true if the destination-hub-first branch can reach @p From within
 * the default recursion budget.
 */
template <typename From, typename To>
constexpr bool can_route_via_to_hub() {
  return route_cost_via_to_hub<From, To>() != invalid_route_cost;
}

}  // namespace colorcpp::operations::conversion
