/**
 * @file conversion.hpp
 * @brief @ref colorcpp::operations::conversion::color_cast — typed conversion between all registered color models.
 *
 * Uses a registration-based system with hub routing. New color spaces can be added
 * externally using COLORCPP_REGISTER_CONVERSION without modifying core files.
 *
 * Routing uses per-model hubs (e.g. float sRGB, linear sRGB, XYZ) when no direct edge exists.
 * Unsupported pairs trigger a compile-time @c static_assert with a clear message;
 * register conversion using COLORCPP_REGISTER_CONVERSION to extend.
 */

#pragma once

#include <colorcpp/operations/conversion/cast_impl.hpp>
#include <colorcpp/operations/conversion/color_space_registry.hpp>
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
 * @note Routes via registered conversions or hub spaces (Linear RGB, XYZ, OkLab).
 * @note If no path exists, compilation fails with @c static_assert listing unsupported conversion.
 *       Register the conversion using COLORCPP_REGISTER_CONVERSION to add support.
 */
template <typename To, typename From>
constexpr To color_cast(const From& src) {
  return color_cast_impl<To, From>::convert(src);
}

}  // namespace colorcpp::operations::conversion
