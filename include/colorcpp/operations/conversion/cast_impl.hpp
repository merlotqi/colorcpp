/**
 * @file cast_impl.hpp
 * @brief color_cast routing: graph-only dispatch over registered conversion edges.
 */

#pragma once

#include <colorcpp/operations/conversion/detail.hpp>
#include <colorcpp/operations/conversion/functions/index.hpp>
#include <colorcpp/operations/conversion/graph.hpp>
#include <colorcpp/operations/conversion/registry.hpp>
#include <type_traits>

namespace colorcpp::operations::conversion {

template <typename To, typename From>
struct color_cast_impl;

template <typename To, typename From>
struct graph_conversion_dispatch {
  static constexpr To apply(const From& src) {
    if constexpr (has_registered_conversion_v<From, To>) {
      return apply_registered_conversion<From, To>(src);
    } else {
      using Next = graph::next_hop_t<From, To>;
      static_assert(!std::is_same_v<Next, void>, "colorcpp: graph route requested without a valid next hop");
      return color_cast_impl<To, Next>::convert(color_cast_impl<Next, From>::convert(src));
    }
  }
};

/**
 * Single dispatch entry for @c color_cast. Priority (document order):
 * 1. Identity
 * 2. Graph-discovered route (direct registered edge or multi-hop shortest path)
 * 3. Compile-time error
 */
template <typename To, typename From>
struct conversion_dispatch {
  static constexpr To apply(const From& src) {
    if constexpr (std::is_same_v<From, To>) {
      return src;
    } else if constexpr (graph::has_graph_path<From, To>()) {
      return graph_conversion_dispatch<To, From>::apply(src);
    } else {
      static_assert(sizeof(From) == 0,
                    "colorcpp: unsupported conversion path in graph routing. "
                    "Register a direct edge with COLORCPP_REGISTER_CONVERSION; "
                    "for multi-hop routes involving external types, also add nodes via "
                    "colorcpp::operations::conversion::graph::additional_color_nodes.");
      return To{};
    }
  }
};

template <typename To, typename From>
struct color_cast_impl {
  static constexpr To convert(const From& src) { return conversion_dispatch<To, From>::apply(src); }
};

}  // namespace colorcpp::operations::conversion
