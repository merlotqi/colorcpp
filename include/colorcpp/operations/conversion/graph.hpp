/**
 * @file graph.hpp
 * @brief Compile-time conversion graph routing system.
 *
 * The graph layer discovers shortest paths across registered direct conversions
 * without changing the existing registration macros or hub fallback logic.
 */

#pragma once

#include <array>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/core/cmyk.hpp>
#include <colorcpp/core/display_p3.hpp>
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/hsv.hpp>
#include <colorcpp/core/hwb.hpp>
#include <colorcpp/core/linear_rgb.hpp>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion/functions/index.hpp>
#include <colorcpp/operations/conversion/registry.hpp>
#include <cstddef>
#include <limits>
#include <type_traits>

namespace colorcpp::operations::conversion::graph {

/**
 * @brief Compile-time infinity value for path costs.
 */
constexpr std::size_t inf = std::numeric_limits<std::size_t>::max() / 2;

/**
 * @brief Sentinel used for missing node indices.
 */
constexpr std::size_t invalid_index = std::numeric_limits<std::size_t>::max();

/**
 * @brief Compile-time edge representation.
 *
 * @tparam From Source node type
 * @tparam To Destination node type
 * @tparam Cost Edge weight (default 1)
 */
template <typename From, typename To, std::size_t Cost = 1>
struct edge {
  using from_type = From;
  using to_type = To;
  static constexpr std::size_t cost = Cost;
};

/**
 * @brief Compile-time node list.
 *
 * @tparam Nodes Variadic list of color types
 */
template <typename... Nodes>
struct node_list {
  static constexpr std::size_t size = sizeof...(Nodes);
};

/**
 * @brief Compile-time edge list.
 *
 * @tparam Edges Variadic list of edge types
 */
template <typename... Edges>
struct edge_list {
  static constexpr std::size_t size = sizeof...(Edges);
};

template <typename... Lists>
struct concat_edge_lists;

template <>
struct concat_edge_lists<> {
  using type = edge_list<>;
};

template <typename... Edges>
struct concat_edge_lists<edge_list<Edges...>> {
  using type = edge_list<Edges...>;
};

template <typename... Left, typename... Right, typename... Rest>
struct concat_edge_lists<edge_list<Left...>, edge_list<Right...>, Rest...> {
  using type = typename concat_edge_lists<edge_list<Left..., Right...>, Rest...>::type;
};

template <typename T, typename... Types>
constexpr std::size_t find_index_in_pack() {
  constexpr bool matches[] = {std::is_same_v<T, Types>...};
  for (std::size_t i = 0; i < sizeof...(Types); ++i) {
    if (matches[i]) return i;
  }
  return invalid_index;
}

template <std::size_t I, typename... Types>
struct type_at_pack;

template <typename First, typename... Rest>
struct type_at_pack<0, First, Rest...> {
  using type = First;
};

template <std::size_t I, typename First, typename... Rest>
struct type_at_pack<I, First, Rest...> {
  static_assert(I < sizeof...(Rest) + 1, "colorcpp: graph node index out of bounds");
  using type = typename type_at_pack<I - 1, Rest...>::type;
};

template <typename Nodes, typename T>
struct contains_node;

template <typename T, typename... Nodes>
struct contains_node<node_list<Nodes...>, T> : std::bool_constant<(std::is_same_v<T, Nodes> || ...)> {};

template <typename Nodes, typename T>
inline constexpr bool contains_node_v = contains_node<Nodes, T>::value;

template <typename Nodes, typename T>
struct index_of_node;

template <typename T, typename... Nodes>
struct index_of_node<node_list<Nodes...>, T> : std::integral_constant<std::size_t, find_index_in_pack<T, Nodes...>()> {
  static_assert(find_index_in_pack<T, Nodes...>() != invalid_index, "colorcpp: graph node type not found");
};

template <typename Nodes, typename T>
inline constexpr std::size_t index_of_node_v = index_of_node<Nodes, T>::value;

template <typename Nodes, std::size_t I>
struct node_at;

template <std::size_t I, typename... Nodes>
struct node_at<node_list<Nodes...>, I> {
  using type = typename type_at_pack<I, Nodes...>::type;
};

template <typename Nodes, std::size_t I>
using node_at_t = typename node_at<Nodes, I>::type;

/**
 * @brief Built-in color graph nodes known to the library.
 *
 * Registered conversions between these node types are collected automatically.
 */
using builtin_color_nodes =
    node_list<core::rgb8_t, core::rgba8_t, core::rgbf_t, core::rgbaf_t, core::linear_rgbf_t, core::linear_rgbaf_t,
              core::xyz_t, core::cielab_t, core::cielch_t, core::oklab_t, core::oklch_t, core::hsl_float_t,
              core::hsla_float_t, core::hsv_float_t, core::hsva_float_t, core::hwb_float_t, core::hwba_float_t,
              core::cmyk8_t, core::cmyk_float_t, core::display_p3f_t, core::display_p3af_t, core::linear_display_p3f_t,
              core::linear_display_p3af_t>;

/**
 * @brief Collect all direct registered edges among a known node set.
 */
template <typename Nodes>
struct collect_edges;

template <typename From, typename Nodes>
struct collect_from_edges;

template <typename From, typename... Nodes>
struct collect_from_edges<From, node_list<Nodes...>> {
  using type = typename concat_edge_lists<
      std::conditional_t<has_registered_conversion_v<From, Nodes>, edge_list<edge<From, Nodes>>, edge_list<>>...>::type;
};

template <typename... Nodes>
struct collect_edges<node_list<Nodes...>> {
  using type = typename concat_edge_lists<typename collect_from_edges<Nodes, node_list<Nodes...>>::type...>::type;
};

using builtin_edges = typename collect_edges<builtin_color_nodes>::type;

template <typename Nodes, typename Edges>
struct adjacency_matrix;

template <typename... Nodes, typename... Edges>
struct adjacency_matrix<node_list<Nodes...>, edge_list<Edges...>> {
  static constexpr std::size_t node_count = sizeof...(Nodes);

  template <typename Edge>
  static constexpr void add_edge(std::array<std::size_t, node_count * node_count>& matrix) {
    constexpr std::size_t from = index_of_node_v<node_list<Nodes...>, typename Edge::from_type>;
    constexpr std::size_t to = index_of_node_v<node_list<Nodes...>, typename Edge::to_type>;
    matrix[from * node_count + to] = Edge::cost;
  }

  static constexpr auto build() {
    std::array<std::size_t, node_count * node_count> matrix{};
    for (std::size_t row = 0; row < node_count; ++row) {
      for (std::size_t col = 0; col < node_count; ++col) {
        matrix[row * node_count + col] = (row == col) ? 0 : inf;
      }
    }
    (add_edge<Edges>(matrix), ...);
    return matrix;
  }

  inline static constexpr auto value = build();
};

template <std::size_t NodeCount>
struct search_state {
  std::array<std::size_t, NodeCount> dist{};
  std::array<std::size_t, NodeCount> prev{};
};

template <typename Nodes, typename Edges, typename Source, typename Target, std::size_t MaxNodes = 32>
struct shortest_path_for_nodes {
 private:
  static constexpr std::size_t node_count = Nodes::size;
  static_assert(node_count <= MaxNodes, "colorcpp: graph node count exceeds shortest_path MaxNodes");

  static constexpr std::size_t source_index = index_of_node_v<Nodes, Source>;
  static constexpr std::size_t target_index = index_of_node_v<Nodes, Target>;

  static constexpr auto adjacency = adjacency_matrix<Nodes, Edges>::value;

  static constexpr search_state<node_count> run_dijkstra() {
    search_state<node_count> state{};
    std::array<bool, node_count> visited{};

    for (std::size_t i = 0; i < node_count; ++i) {
      state.dist[i] = inf;
      state.prev[i] = invalid_index;
      visited[i] = false;
    }
    state.dist[source_index] = 0;

    for (std::size_t step = 0; step < node_count; ++step) {
      std::size_t current = invalid_index;
      std::size_t best = inf;
      for (std::size_t i = 0; i < node_count; ++i) {
        if (!visited[i] && state.dist[i] < best) {
          current = i;
          best = state.dist[i];
        }
      }

      if (current == invalid_index || current == target_index) break;

      visited[current] = true;
      for (std::size_t neighbor = 0; neighbor < node_count; ++neighbor) {
        const std::size_t edge_cost = adjacency[current * node_count + neighbor];
        if (edge_cost == inf || visited[neighbor] || state.dist[current] == inf) continue;

        const std::size_t next_cost = state.dist[current] + edge_cost;
        if (next_cost < state.dist[neighbor]) {
          state.dist[neighbor] = next_cost;
          state.prev[neighbor] = current;
        }
      }
    }

    return state;
  }

 public:
  inline static constexpr auto state = run_dijkstra();

  static constexpr std::size_t cost() { return state.dist[target_index]; }

  static constexpr bool exists() { return cost() != inf; }

  static constexpr std::size_t first_step_index() {
    if constexpr (std::is_same_v<Source, Target>) {
      return source_index;
    } else {
      if (!exists()) return invalid_index;

      std::size_t current = target_index;
      std::size_t parent = state.prev[current];
      while (parent != invalid_index && parent != source_index) {
        current = parent;
        parent = state.prev[current];
      }
      return parent == source_index ? current : invalid_index;
    }
  }
};

/**
 * @brief Public shortest-path wrapper over the built-in node set.
 *
 * @tparam Edges Complete edge list to search
 * @tparam Source Source color type
 * @tparam Target Target color type
 * @tparam MaxNodes Safety bound on the known node count
 */
template <typename Edges, typename Source, typename Target, std::size_t MaxNodes = 32>
struct shortest_path : shortest_path_for_nodes<builtin_color_nodes, Edges, Source, Target, MaxNodes> {};

/**
 * @brief Get the minimal graph cost between two color types.
 *
 * Returns only graph-discovered paths over registered direct edges. Hub routing
 * is intentionally excluded here so callers can choose it as an explicit fallback.
 */
template <typename From, typename To>
constexpr std::size_t minimal_conversion_cost() {
  if constexpr (!contains_node_v<builtin_color_nodes, From> || !contains_node_v<builtin_color_nodes, To>) {
    return inf;
  } else {
    return shortest_path<builtin_edges, From, To>::cost();
  }
}

/**
 * @brief Check if a graph-based path exists between two color types.
 *
 * @tparam From Source color type
 * @tparam To Destination color type
 * @return true if a valid graph path exists
 */
template <typename From, typename To>
constexpr bool has_graph_path() {
  return minimal_conversion_cost<From, To>() != inf;
}

template <typename From, typename To, bool HasPath = has_graph_path<From, To>()>
struct next_hop_impl {
  using type = void;
};

template <typename From, typename To>
struct next_hop_impl<From, To, true> {
  using type = node_at_t<builtin_color_nodes, shortest_path<builtin_edges, From, To>::first_step_index()>;
};

/**
 * @brief First hop type on the minimal built-in graph route.
 *
 * Resolves to @c void when no graph route is available.
 */
template <typename From, typename To>
using next_hop_t = typename next_hop_impl<From, To>::type;

}  // namespace colorcpp::operations::conversion::graph
