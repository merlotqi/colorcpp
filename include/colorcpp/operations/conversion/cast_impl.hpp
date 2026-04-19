/**
 * @file cast_impl.hpp
 * @brief color_cast routing: registered edges first, then hub recursion.
 */

#pragma once

#include <colorcpp/operations/conversion/detail.hpp>
#include <colorcpp/operations/conversion/functions/index.hpp>
#include <colorcpp/operations/conversion/registry.hpp>
#include <colorcpp/operations/conversion/traits.hpp>
#include <type_traits>

namespace colorcpp::operations::conversion {

template <typename To, typename From>
struct color_cast_impl;

/**
 * Single dispatch entry for @c color_cast. Priority (document order):
 * 1. Identity
 * 2. Registered direct conversion (@c conversion_registry)
 * 3. Route via the shorter of the source/destination hub paths
 * 4. Fall back to the remaining feasible hub path
 * 5. Compile-time error
 */
template <typename To, typename From>
struct conversion_dispatch {
  static constexpr To apply(const From& src) {
    if constexpr (std::is_same_v<From, To>) {
      return src;
    } else if constexpr (has_registered_conversion_v<From, To>) {
      return apply_registered_conversion<From, To>(src);
    } else if constexpr (route_cost_via_from_hub<From, To>() < route_cost_via_to_hub<From, To>()) {
      using Hub = hub_color_t<From>;
      return color_cast_impl<To, Hub>::convert(color_cast_impl<Hub, From>::convert(src));
    } else if constexpr (can_route_via_to_hub<From, To>()) {
      using Hub = hub_color_t<To>;
      return color_cast_impl<To, Hub>::convert(color_cast_impl<Hub, From>::convert(src));
    } else if constexpr (can_route_via_from_hub<From, To>()) {
      using Hub = hub_color_t<From>;
      return color_cast_impl<To, Hub>::convert(color_cast_impl<Hub, From>::convert(src));
    } else {
      static_assert(sizeof(From) == 0,
                    "colorcpp: unsupported conversion path. "
                    "Register a direct edge with COLORCPP_REGISTER_CONVERSION or define color_traits::hub_type.");
      return To{};
    }
  }
};

template <typename To, typename From>
struct color_cast_impl {
  static constexpr To convert(const From& src) { return conversion_dispatch<To, From>::apply(src); }
};

}  // namespace colorcpp::operations::conversion
