/**
 * @file registry.hpp
 * @brief Compile-time conversion registry for external registration.
 *
 * This file provides a registration system that allows:
 * 1. External registration of color space conversions
 * 2. Compile-time route discovery
 * 3. Zero runtime overhead
 */

#pragma once

#include <cstddef>

namespace colorcpp::operations::conversion {

/**
 * @brief Conversion registry - stores conversion functions at compile time.
 * @tparam From Source color type.
 * @tparam To Destination color type.
 *
 * Specialize this template to register a conversion.
 */
template <typename From, typename To>
struct conversion_registry {
  static constexpr bool is_registered = false;
  static constexpr auto convert = nullptr;
  static constexpr std::size_t cost = 1;
};

/**
 * @brief Helper to check if a conversion is registered.
 */
template <typename From, typename To>
inline constexpr bool has_registered_conversion_v = conversion_registry<From, To>::is_registered;

/**
 * @brief Cost metadata for a registered conversion edge.
 *
 * Registered edges default to cost 1 unless a weighted registration macro
 * overrides the value.
 */
template <typename From, typename To>
inline constexpr std::size_t registered_conversion_cost_v = conversion_registry<From, To>::cost;

/**
 * @brief Apply a registered conversion.
 */
template <typename From, typename To>
constexpr To apply_registered_conversion(const From& src) {
  return conversion_registry<From, To>::convert(src);
}

/**
 * @brief Macro to register a conversion function.
 *
 * Usage:
 *   COLORCPP_REGISTER_CONVERSION(source_type, dest_type, conversion_function)
 *
 * Example:
 *   COLORCPP_REGISTER_CONVERSION(hsl_float_t, rgbf_t, details::hsl_to_rgb<rgbf_t>)
 */
#define COLORCPP_REGISTER_CONVERSION_WEIGHTED(From, To, Func, Cost) \
  template <>                                                       \
  struct conversion_registry<From, To> {                            \
    static constexpr bool is_registered = true;                     \
    using convert_fn = To (*)(const From&);                         \
    static constexpr convert_fn convert = &Func;                    \
    static constexpr std::size_t cost = Cost;                       \
  };

/**
 * @brief Macro to register a conversion function with default cost 1.
 */
#define COLORCPP_REGISTER_CONVERSION(From, To, Func) COLORCPP_REGISTER_CONVERSION_WEIGHTED(From, To, Func, 1)

/**
 * @brief Macro to register a bidirectional conversion.
 *
 * Usage:
 *   COLORCPP_REGISTER_CONVERSION_BIDIR(type_a, type_b, func_a_to_b, func_b_to_a)
 */
#define COLORCPP_REGISTER_CONVERSION_BIDIR_WEIGHTED(TypeA, TypeB, FuncAToB, FuncBToA, CostAToB, CostBToA) \
  COLORCPP_REGISTER_CONVERSION_WEIGHTED(TypeA, TypeB, FuncAToB, CostAToB);                                \
  COLORCPP_REGISTER_CONVERSION_WEIGHTED(TypeB, TypeA, FuncBToA, CostBToA)

/**
 * @brief Macro to register a bidirectional conversion with default cost 1 in both directions.
 */
#define COLORCPP_REGISTER_CONVERSION_BIDIR(TypeA, TypeB, FuncAToB, FuncBToA) \
  COLORCPP_REGISTER_CONVERSION_BIDIR_WEIGHTED(TypeA, TypeB, FuncAToB, FuncBToA, 1, 1)

}  // namespace colorcpp::operations::conversion
