/**
 * @file debug.hpp
 * @brief Debug utilities for color space conversion.
 *
 * This file provides utilities for inspecting conversion paths and
 * debugging color space routing at compile time.
 */

#pragma once

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
#include <colorcpp/operations/conversion/graph.hpp>
#include <colorcpp/operations/conversion/registry.hpp>
#include <colorcpp/operations/conversion/traits.hpp>
#include <string_view>
#include <type_traits>

namespace colorcpp::operations::conversion {

/**
 * @brief Compile-time color type name trait.
 *
 * Specialize this for each color type to provide a human-readable name.
 * If not specialized, returns "<unknown>".
 *
 * @tparam T Color type.
 */
template <typename T>
struct color_type_name {
  static constexpr std::string_view value = "<unknown>";
};

// Specializations for common color types
template <>
struct color_type_name<core::rgbf_t> {
  static constexpr std::string_view value = "rgbf";
};

template <>
struct color_type_name<core::rgbaf_t> {
  static constexpr std::string_view value = "rgbaf";
};

template <>
struct color_type_name<core::rgb8_t> {
  static constexpr std::string_view value = "rgb8";
};

template <>
struct color_type_name<core::rgba8_t> {
  static constexpr std::string_view value = "rgba8";
};

template <>
struct color_type_name<core::linear_rgbf_t> {
  static constexpr std::string_view value = "linear_rgbf";
};

template <>
struct color_type_name<core::linear_rgbaf_t> {
  static constexpr std::string_view value = "linear_rgbaf";
};

template <>
struct color_type_name<core::xyz_t> {
  static constexpr std::string_view value = "xyz";
};

template <>
struct color_type_name<core::cielab_t> {
  static constexpr std::string_view value = "cielab";
};

template <>
struct color_type_name<core::cielch_t> {
  static constexpr std::string_view value = "cielch";
};

template <>
struct color_type_name<core::oklab_t> {
  static constexpr std::string_view value = "oklab";
};

template <>
struct color_type_name<core::oklch_t> {
  static constexpr std::string_view value = "oklch";
};

template <>
struct color_type_name<core::hsl_float_t> {
  static constexpr std::string_view value = "hsl";
};

template <>
struct color_type_name<core::hsla_float_t> {
  static constexpr std::string_view value = "hsla";
};

template <>
struct color_type_name<core::hsv_float_t> {
  static constexpr std::string_view value = "hsv";
};

template <>
struct color_type_name<core::hsva_float_t> {
  static constexpr std::string_view value = "hsva";
};

template <>
struct color_type_name<core::hwb_float_t> {
  static constexpr std::string_view value = "hwb";
};

template <>
struct color_type_name<core::hwba_float_t> {
  static constexpr std::string_view value = "hwba";
};

template <>
struct color_type_name<core::cmyk8_t> {
  static constexpr std::string_view value = "cmyk8";
};

template <>
struct color_type_name<core::cmyk_float_t> {
  static constexpr std::string_view value = "cmyk_float";
};

template <>
struct color_type_name<core::display_p3f_t> {
  static constexpr std::string_view value = "display_p3";
};

template <>
struct color_type_name<core::display_p3af_t> {
  static constexpr std::string_view value = "display_p3a";
};

/**
 * @brief Helper to get color type name.
 * @tparam T Color type.
 */
template <typename T>
inline constexpr std::string_view color_type_name_v = color_type_name<T>::value;

/**
 * @brief Compile-time conversion path representation.
 *
 * This structure can be used to inspect the conversion path
 * that will be taken at compile time.
 *
 * @tparam From Source color type.
 * @tparam To Destination color type.
 */
template <typename From, typename To>
struct conversion_path_info {
  using from_type = From;
  using to_type = To;

  static constexpr bool is_identity = std::is_same_v<From, To>;
  static constexpr bool has_direct_conversion = has_registered_conversion_v<From, To>;
  static constexpr std::size_t graph_cost = graph::minimal_conversion_cost<From, To>();
  static constexpr bool can_route_via_graph = graph_cost != graph::inf;
  static constexpr bool can_route_via_from_hub = ::colorcpp::operations::conversion::can_route_via_from_hub<From, To>();
  static constexpr bool can_route_via_to_hub = ::colorcpp::operations::conversion::can_route_via_to_hub<From, To>();
  static constexpr bool is_possible =
      is_identity || has_direct_conversion || can_route_via_graph || can_route_via_from_hub || can_route_via_to_hub;
};

/**
 * @brief Check if a conversion is possible at compile time.
 *
 * This is a user-friendly wrapper around conversion_path_info.
 *
 * @tparam From Source color type.
 * @tparam To Destination color type.
 * @return true if conversion is possible.
 */
template <typename From, typename To>
constexpr bool can_convert() {
  return conversion_path_info<From, To>::is_possible;
}

/**
 * @brief Get the hub type for a color type.
 * @tparam Color Color type.
 * @return Hub type or void if no hub.
 */
template <typename Color>
using get_hub_t = hub_color_t<Color>;

/**
 * @brief Print conversion path description (for debugging).
 *
 * This function is intended for compile-time debugging. Use with
 * static_assert or constexpr contexts to verify conversion paths.
 *
 * @tparam From Source color type.
 * @tparam To Destination color type.
 * @return true if conversion is possible.
 *
 * Example:
 * @code
 * static_assert(colorcpp::operations::conversion::verify_path<hsl_t, oklab_t>(),
 *               "HSL to OkLab conversion should be possible");
 * @endcode
 */
template <typename From, typename To>
constexpr bool verify_path() {
  return conversion_path_info<From, To>::is_possible;
}

}  // namespace colorcpp::operations::conversion
