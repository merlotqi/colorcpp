/**
 * @file smooth_path.hpp
 * @brief Piecewise interpolation across multiple control points.
 */

#pragma once

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <utility>

#include <colorcpp/operations/interpolate/catmull_rom.hpp>
#include <colorcpp/operations/interpolate/cubic.hpp>
#include <colorcpp/operations/interpolate/lerp.hpp>
#include <colorcpp/operations/interpolate/monotonic_spline.hpp>

namespace colorcpp::operations::interpolate {

/** @brief Interpolator used by @ref lerp_path across adjacent control-point segments. */
enum class path_algorithm {
  linear,
  cubic_hermite,
  catmull_rom,
  monotonic_spline,
  spline = monotonic_spline
};

namespace details {

template <typename Container>
using path_color_t = std::decay_t<decltype(*std::begin(std::declval<const Container&>()))>;

template <typename Container>
decltype(auto) path_color_at(const Container& colors, std::size_t index) {
  auto it = std::begin(colors);
  using difference_type = typename std::iterator_traits<decltype(it)>::difference_type;
  std::advance(it, static_cast<difference_type>(index));
  return *it;
}

template <typename Container>
auto lerp_path_impl(const Container& colors, float t, path_algorithm algorithm, cr_mode catmull_mode, float tension)
    -> path_color_t<Container> {
  using Color = path_color_t<Container>;

  const auto n = static_cast<std::size_t>(std::size(colors));
  if (n == 0) return Color{};
  if (n == 1) return path_color_at(colors, 0);

  t = std::clamp(t, 0.0f, 1.0f);
  if (t <= 0.0f) return path_color_at(colors, 0);
  if (t >= 1.0f) return path_color_at(colors, n - 1);

  const float scaled = t * static_cast<float>(n - 1);
  const auto segment = std::min(static_cast<std::size_t>(scaled), n - 2);
  const float local_t = scaled - static_cast<float>(segment);

  const auto& p0 = path_color_at(colors, segment == 0 ? 0 : segment - 1);
  const auto& p1 = path_color_at(colors, segment);
  const auto& p2 = path_color_at(colors, segment + 1);
  const auto& p3 = path_color_at(colors, segment + 2 < n ? segment + 2 : n - 1);

  switch (algorithm) {
    case path_algorithm::linear:
      return lerp(p1, p2, local_t);
    case path_algorithm::cubic_hermite:
      return lerp_cubic_hermite(p0, p1, p2, p3, local_t, tension);
    case path_algorithm::catmull_rom:
      return lerp_catmull_rom(p0, p1, p2, p3, local_t, catmull_mode);
    case path_algorithm::monotonic_spline:
    default:
      return lerp_monotonic_spline(p0, p1, p2, p3, local_t);
  }
}

}  // namespace details

/**
 * @brief Piecewise interpolation through multiple control points on the unit interval.
 * @param colors Control points. Endpoints are repeated as anchors for the first and last segment.
 * @param t Interpolation factor in [0, 1] (clamped).
 * @param algorithm Path interpolation algorithm (default: monotonic spline).
 * @param catmull_mode Parameterization used when @p algorithm is @ref path_algorithm::catmull_rom.
 * @param tension Tension used when @p algorithm is @ref path_algorithm::cubic_hermite.
 */
template <typename Container>
auto lerp_path(const Container& colors, float t, path_algorithm algorithm = path_algorithm::monotonic_spline,
               cr_mode catmull_mode = cr_mode::centripetal, float tension = 0.0f)
    -> details::path_color_t<Container> {
  return details::lerp_path_impl(colors, t, algorithm, catmull_mode, tension);
}

/** @brief Initializer-list overload of @ref lerp_path. */
template <typename Color>
Color lerp_path(std::initializer_list<Color> colors, float t,
                path_algorithm algorithm = path_algorithm::monotonic_spline,
                cr_mode catmull_mode = cr_mode::centripetal, float tension = 0.0f) {
  return details::lerp_path_impl(colors, t, algorithm, catmull_mode, tension);
}

}  // namespace colorcpp::operations::interpolate
