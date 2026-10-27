/**
 * @file conversion.hpp
 * @brief @ref colorcpp::operations::conversion::color_cast — typed conversion between all registered color models.
 *
 * Uses a registration-based system with compile-time graph routing.
 * New color spaces can be added externally using COLORCPP_REGISTER_CONVERSION
 * or COLORCPP_REGISTER_CONVERSION_WEIGHTED without modifying core files.
 *
 * Routing uses graph-discovered registered edges and multi-hop shortest paths.
 * Unsupported pairs trigger a compile-time @c static_assert with a clear message;
 * register conversion using COLORCPP_REGISTER_CONVERSION or
 * COLORCPP_REGISTER_CONVERSION_WEIGHTED to extend.
 */

#pragma once

#include <colorcpp/operations/conversion/cast_impl.hpp>
#include <colorcpp/operations/conversion/debug.hpp>
#include <colorcpp/operations/conversion/detail.hpp>
#include <colorcpp/operations/conversion/functions/index.hpp>
#include <colorcpp/operations/conversion/registry.hpp>

namespace colorcpp::operations::conversion {

/**
 * @brief Convert @p src to color type @p To using the library's conversion graph.
 * @tparam To Destination @ref colorcpp::core::basic_color specialization (or compatible type).
 * @tparam From Source color type.
 * @param src Input color.
 * @return Color in @p To after conversion (may clamp to channel ranges per destination model).
 * @note Routes via graph-discovered direct or multi-hop paths only.
 * @note If no path exists, compilation fails with @c static_assert listing unsupported conversion.
 *       Register the conversion using COLORCPP_REGISTER_CONVERSION (or the weighted variant)
 *       to add support and influence route preference.
 */
template <typename To, typename From>
constexpr To color_cast(const From& src) {
  To result = color_cast_impl<To, From>::convert(src);
  // Straight alpha rides outside the route: minimal-cost paths cross 3-channel
  // transit nodes (XYZ / Oklab / CIELAB) that cannot carry it, so the source
  // alpha is restored on the result. Conversion functions never transform
  // alpha, so this is exact for every route.
  if constexpr (!std::is_same_v<From, To> && details::has_alpha_v<From> && details::has_alpha_v<To>) {
    result.template get_index<3>() = details::from_unit<To, 3>(details::to_unit<From, 3>(src.template get_index<3>()));
  }
  return result;
}

}  // namespace colorcpp::operations::conversion
